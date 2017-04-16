<?hh // strict

namespace GroovyCarrot\Event;

use GroovyCarrot\Event\Dispatch\TaskList;

interface EventListenerTaskCollecting
{
    public function tasksForEvent<Tevent as Event>(classname<Tevent> $eventClass): TaskList<Tevent>;
}
