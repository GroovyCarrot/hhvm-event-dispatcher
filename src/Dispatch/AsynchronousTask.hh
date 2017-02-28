<?hh // strict

namespace GroovyCarrot\Event\Dispatch;

use GroovyCarrot\Event\{
    Event,
    EventListening
};

final class AsynchronousTask<TEvent as Event> extends Task<TEvent>
{
    private Vector<EventListening<TEvent>> $listeners = Vector {};

    public function addSubtask(EventListening<TEvent> $eventListener): this
    {
        $this->listeners[] = $eventListener;
        return $this;
    }

    public function removeSubtask(EventListening<TEvent> $eventListener): void
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

    public function getSubtasks(): ImmVector<EventListening<TEvent>>
    {
        return $this->listeners->immutable();
    }

    public async function dispatch(TEvent $event): Awaitable<void>
    {
        $event->setStopPropagationIsUnsafe();

        $dispatch = [];
        foreach ($this->getSubtasks() as $listener) {
            $dispatch[] = $listener->handleEvent($event);
        }

        await \HH\Asio\m($dispatch);
    }
}
