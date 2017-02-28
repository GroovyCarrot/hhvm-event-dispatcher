<?hh // strict

namespace GroovyCarrot\Event\Dispatch;

use GroovyCarrot\Event\{
    Event,
    EventListening,
    EventDispatching
};

final class SynchronousTask<TEvent as Event> extends Task<TEvent>
{
    private array<int, array<EventListening<TEvent>>> $listeners = [];
    private ?ImmVector<EventListening<TEvent>> $sortedListeners;

    private Vector<EventListening<TEvent>> $stoppedPropagationlisteners = Vector {};

    public function addSubtask(EventListening<TEvent> $eventListener, int $priority = 0): this
    {
        $this->listeners[$priority][] = $eventListener;
        $this->sortedListeners = null;
        return $this;
    }

    public function removeSubtask(EventListening<TEvent> $eventListener): void
    {
        foreach ($this->listeners as $priority => $listeners) {
            if (false !== ($key = array_search($eventListener, $listeners, true))) {
                unset($listeners[$key]);
            }
        }
    }

    public function addPropagationStoppedSubtask(EventListening<TEvent> $eventListener): this
    {
        $this->stoppedPropagationlisteners->add($eventListener);
        return $this;
    }

    public async function dispatch(TEvent $event): Awaitable<void>
    {
        foreach ($this->getSubtasks() as $listener) {
            await $listener->handleEvent($event);

            if ($event->isPropagationStopped()) {
                await $this->propagationStopped($event);
                break;
            }
        }
    }

    public function getSubtasks(): ImmVector<EventListening<TEvent>>
    {
        if ($this->sortedListeners === null) {
            return $this->sortListeners();
        }

        return $this->sortedListeners;
    }

    private async function propagationStopped(TEvent $event): Awaitable<void>
    {
        $dispatch = [];
        foreach ($this->stoppedPropagationlisteners as $listener) {
            $dispatch[] = $listener->handleEvent($event);
        }

        await \HH\Asio\m($dispatch);
    }

    private function sortListeners(): ImmVector<EventListening<TEvent>>
    {
        ksort($this->listeners, SORT_NUMERIC);

        $sortedListeners = Vector {};
        foreach ($this->listeners as $priority => $eventListeners) {
            $sortedListeners->addAll($eventListeners);
        }

        return $this->sortedListeners = $sortedListeners->immutable();
    }
}
