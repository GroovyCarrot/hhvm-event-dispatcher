<?hh // strict

namespace GroovyCarrot\Event;

use GroovyCarrot\Event\Dispatch\TaskList;

interface EventHandlerTaskCollecting
{
    public function tasksForEvent<Tevent as Event>(classname<Tevent> $eventClass): TaskList<Tevent>;
}
