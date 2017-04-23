<?hh // strict

namespace GroovyCarrot\Event\Dispatch;

use GroovyCarrot\Event\Event;
use GroovyCarrot\Event\EventDispatching;
use GroovyCarrot\Event\EventHandlerTaskCollecting;
use GroovyCarrot\Event\EventHandlerRegistryTrait;

class Dispatcher implements EventDispatching, EventHandlerTaskCollecting
{
    use EventHandlerRegistryTrait;

    private Vector<Awaitable<void>> $pendingAwaitables = Vector{};

    public async function dispatchEvent<Tevent as Event>(Tevent $event): Awaitable<Map<string, Tevent>>
    {
        $taskEvents = Map{};
        $awaitables = $this->getDispatchAwaitables($event, $taskEvents);

        await \HH\Asio\m($awaitables);

        return $taskEvents;
    }

    public async function dispatchEventForTask<Tevent as Event>(Tevent $event, string $taskName): Awaitable<Tevent>
    {
        $taskEvents = Map{};

        $awaitables = $this->getDispatchAwaitables($event, $taskEvents);
        if (!$awaitables->contains($taskName)) {
            $eventClass = get_class($event);
            throw new \InvalidArgumentException("Task '{$taskName}' not found for {$eventClass}.");
        }

        $this->pendingAwaitables->addAll($awaitables);
        await $awaitables->at($taskName);

        return $taskEvents->at($taskName);
    }

    public async function flush(): Awaitable<void>
    {
        await \HH\Asio\m($this->pendingAwaitables);
        $this->pendingAwaitables->clear();
    }

    private function getDispatchAwaitables<Tevent as Event>(
        Tevent $event,
        Map<string, Tevent> $taskEvents
    ): Map<string, Awaitable<void>>
    {
        $eventClass = get_class($event);

        $tasks = $this->tasksForEvent($eventClass)->getTasks();
        if ($tasks->isEmpty()) {
            throw new \InvalidArgumentException("No dispatch tasks exist for event {$eventClass}.");
        }

        $dispatch = Map{};
        foreach ($tasks as $taskName => $task) {
            // We clone the event, so that each task can stop propagation independently if it is synchronous.
            $eventCopy = clone $event;
            $taskEvents->set($taskName, $eventCopy);
            $dispatch->set($taskName, $task->handleEvent($eventCopy));
        }

        return $dispatch;
    }

    /**
     * Ensure that any outstanding tasks are complete before the application finishes.
     */
    public function __destruct(): void
    {
        \HH\Asio\join($this->flush());
    }
}
