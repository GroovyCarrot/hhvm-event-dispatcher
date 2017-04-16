<?hh // strict

namespace GroovyCarrot\Event\Dispatch;

use GroovyCarrot\Event\Event;
use GroovyCarrot\Event\EventHandling;

<<__ConsistentConstruct>>
abstract class TaskGroup<Tevent as Event> implements EventHandling<Tevent>
{
    public static function newGroup(): this
    {
        return new static();
    }

    abstract public function addTask(EventHandling<Tevent> $eventListener): this;

    abstract public function removeTask(EventHandling<Tevent> $eventListener): this;

    abstract public function getTasks(): ImmVector<EventHandling<Tevent>>;
}
