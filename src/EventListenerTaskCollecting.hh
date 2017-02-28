<?hh // strict

namespace GroovyCarrot\Event;

use GroovyCarrot\Event\Dispatch\TaskList;

interface EventListenerTaskCollecting
{
    public function tasksForEvent<TEvent as Event>(classname<TEvent> $eventClass): TaskList<TEvent>;
}
