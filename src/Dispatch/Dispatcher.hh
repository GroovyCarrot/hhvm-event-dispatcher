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

    public async function dispatch<TEvent as Event>(TEvent $event): Awaitable<Map<string, TEvent>>
    {
        $eventClass = new \ReflectionClass($event);
        $className = $eventClass->getName();

        if (!array_key_exists($className, $this->taskLists)) {
            throw new \InvalidArgumentException("No dispatch tasks exist for event {$className}.");
        }

        $tasks = Map {};
        $dispatch = [];
        foreach ($this->taskLists->at($className)->getTasks() as $taskName => $task) {
            // We clone the event, so that each task can stop propagation
            // independently if it is synchronous.
            $eventCopy = clone $event;
            $tasks->set($taskName, $eventCopy);
            $dispatch[] = $task->dispatch($eventCopy);
        }

        await \HH\Asio\m($dispatch);

        return $tasks;
    }

    public async function dispatchForTask<TEvent as Event>(TEvent $event, string $taskName): Awaitable<TEvent>
    {
        $results = await $this->dispatch($event);
        return $results->at($taskName);
    }
}
