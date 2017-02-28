<?hh // strict

namespace GroovyCarrot\Event\Dispatch;

use GroovyCarrot\Event\{
    Event,
    EventListening
};

<<__ConsistentConstruct>>
abstract class Task<TEvent as Event> implements EventTaskDispatching<TEvent>
{
    public static function newTask(): this
    {
        return new static();
    }

    abstract public function addSubtask(EventListening<TEvent> $eventListener): this;

    abstract public function removeSubtask(EventListening<TEvent> $eventListener): void;

    abstract public function getSubtasks(): ImmVector<EventListening<TEvent>>;
}
