<?hh // strict

namespace GroovyCarrot\Event;

use GroovyCarrot\Event\Dispatch\TaskList;

interface EventDispatching
{
    /**
     * Dispatch an event, and return a map of tasks and their finished events.
     */
    public function dispatchEvent<TEvent as Event>(TEvent $event): Awaitable<Map<string, TEvent>>;

    /**
     * Dispatch an event, and return the finished event for a particular task.
     */
    public function dispatchEventForTask<TEvent as Event>(TEvent $event, string $taskName): Awaitable<TEvent>;
}
