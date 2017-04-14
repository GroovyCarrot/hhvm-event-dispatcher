<?hh // strict

namespace GroovyCarrot\Event\Dispatch;

use GroovyCarrot\Event\Event;
use GroovyCarrot\Event\EventListening;

<<__ConsistentConstruct>>
abstract class TaskGroup<TEvent as Event> implements EventListening<TEvent>
{
    public static function newGroup(): this
    {
        return new static();
    }

    abstract public function addTask(EventListening<TEvent> $eventListener): this;

    abstract public function removeTask(EventListening<TEvent> $eventListener): void;

    abstract public function getTasks(): ImmVector<EventListening<TEvent>>;
}
