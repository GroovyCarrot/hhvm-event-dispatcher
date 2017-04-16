<?hh // strict

namespace GroovyCarrot\Event\Dispatch;

use GroovyCarrot\Event\Event;
use GroovyCarrot\Event\EventDispatching;
use GroovyCarrot\Event\EventListenerTaskCollecting;
use GroovyCarrot\Event\EventListenerRegistryTrait;

class Dispatcher implements EventDispatching, EventListenerTaskCollecting
{
    use EventListenerRegistryTrait;

    public async function dispatchEvent<Tevent as Event>(Tevent $event): Awaitable<Map<string, Tevent>>
    {
        $eventClass = get_class($event);

        $tasks = $this->tasksForEvent($eventClass)->getTasks();
        if ($tasks->isEmpty()) {
            throw new \InvalidArgumentException("No dispatch tasks exist for event {$eventClass}.");
        }

        $taskEvents = Map {};
        $dispatch = [];
        foreach ($tasks as $taskName => $task) {
            // We clone the event, so that each task can stop propagation independently if it is synchronous.
            $eventCopy = clone $event;
            $taskEvents->set($taskName, $eventCopy);
            $dispatch[] = $task->handleEvent($eventCopy);
        }

        await \HH\Asio\m($dispatch);

        return $taskEvents;
    }

    public async function dispatchEventForTask<Tevent as Event>(Tevent $event, string $taskName): Awaitable<Tevent>
    {
        $results = await $this->dispatchEvent($event);
        if (!$results->contains($taskName)) {
            throw new \InvalidArgumentException("Result for task '{$taskName}' not found.");
        }

        return $results->at($taskName);
    }
}
