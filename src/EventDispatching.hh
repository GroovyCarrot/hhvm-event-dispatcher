<?hh // strict

namespace GroovyCarrot\Event;

use GroovyCarrot\Event\Dispatch\TaskList;

interface EventDispatching
{
    /**
     * Dispatch an event, and return a map of tasks and their finished events.
     */
    public function dispatchEvent<Tevent as Event>(Tevent $event): Awaitable<Map<string, Tevent>>;

    /**
     * Dispatch an event, and return the finished event for a particular task.
     */
    public function dispatchEventForTask<Tevent as Event>(Tevent $event, string $taskName): Awaitable<Tevent>;

    /**
     * Finish any unawaited tasks.
     */
    public function flush(): Awaitable<void>;
}
