<?hh // strict

namespace GroovyCarrot\Event\Dispatch;

use GroovyCarrot\Event\{
    Event,
    EventDispatching,
    EventListenerTaskCollecting,
    EventListenerRegistryTrait
};

class Dispatcher implements EventDispatching, EventListenerTaskCollecting
{
    use EventListenerRegistryTrait;

    public async function dispatchEvent<TEvent as Event>(TEvent $event): Awaitable<Map<string, TEvent>>
    {
        $eventClass = new \ReflectionClass($event);
        $className = $eventClass->getName();

        if (!array_key_exists($className, $this->taskLists)) {
            throw new \InvalidArgumentException("No dispatch tasks exist for event {$className}.");
        }

        $taskEvents = Map {};
        $dispatch = [];
        foreach ($this->taskLists->at($className)->getTasks() as $taskName => $task) {
            // We clone the event, so that each task can stop propagation
            // independently if it is synchronous.
            $eventCopy = clone $event;
            $taskEvents->set($taskName, $eventCopy);
            $dispatch[] = $task->handleEvent($eventCopy);
        }

        await \HH\Asio\m($dispatch);

        return $taskEvents;
    }

    public async function dispatchEventForTask<TEvent as Event>(TEvent $event, string $taskName): Awaitable<TEvent>
    {
        $results = await $this->dispatchEvent($event);
        if (!$results->contains($taskName)) {
            throw new \InvalidArgumentException("Result for task '{$taskName}' not found.");
        }

        return $results->at($taskName);
    }
}
