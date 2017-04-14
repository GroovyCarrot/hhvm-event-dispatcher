<?hh // strict

namespace GroovyCarrot\Event\Dispatch;

use GroovyCarrot\Event\Event;
use GroovyCarrot\Event\EventListening;

final class AsynchronousTaskGroup<TEvent as Event> extends TaskGroup<TEvent>
{
    private Vector<EventListening<TEvent>> $listeners = Vector {};

    public function addTask(EventListening<TEvent> $eventListener): this
    {
        $this->listeners[] = $eventListener;
        return $this;
    }

    public function removeTask(EventListening<TEvent> $eventListener): void
    {
        $listenerKey = null;
        foreach ($this->listeners as $key => $listener) {
            if ($listener === $eventListener) {
                $listenerKey = $key;
                break;
            }
        }

        if ($listenerKey === null) {
            return;
        }

        $this->listeners->removeKey($listenerKey);
    }

    public function getTasks(): ImmVector<EventListening<TEvent>>
    {
        return $this->listeners->immutable();
    }

    public async function handleEvent(TEvent $event): Awaitable<void>
    {
        $event->setStoppingPropagationIsUnsafe();

        $dispatch = [];
        foreach ($this->getTasks() as $listener) {
            $dispatch[] = $listener->handleEvent($event);
        }

        await \HH\Asio\m($dispatch);
    }
}
