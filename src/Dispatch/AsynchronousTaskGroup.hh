<?hh // strict

namespace GroovyCarrot\Event\Dispatch;

use GroovyCarrot\Event\Event;
use GroovyCarrot\Event\EventHandling;

final class AsynchronousTaskGroup<Tevent as Event> extends TaskGroup<Tevent>
{
    private Vector<EventHandling<Tevent>> $listeners = Vector{};

    public function addTask(EventHandling<Tevent> $eventListener): this
    {
        $this->listeners->add($eventListener);
        return $this;
    }

    public function removeTask(EventHandling<Tevent> $eventListener): this
    {
        $listenerKey = null;
        foreach ($this->listeners as $key => $listener) {
            if ($listener === $eventListener) {
                $listenerKey = $key;
                break;
            }
        }

        if ($listenerKey === null) {
            throw new \InvalidArgumentException('Task not found in group.');
        }

        $this->listeners->removeKey($listenerKey);
        return $this;
    }

    public function getTasks(): ImmVector<EventHandling<Tevent>>
    {
        return $this->listeners->immutable();
    }

    public async function handleEvent(Tevent $event): Awaitable<void>
    {
        $event->setStoppingPropagationIsUnsafe();

        $dispatch = [];
        foreach ($this->getTasks() as $listener) {
            $dispatch[] = $listener->handleEvent($event);
        }

        await \HH\Asio\m($dispatch);
    }
}
