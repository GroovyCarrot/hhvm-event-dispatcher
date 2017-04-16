<?hh // strict

namespace GroovyCarrot\Event\Dispatch;

use GroovyCarrot\Event\Event;
use GroovyCarrot\Event\EventHandling;
use GroovyCarrot\Event\EventDispatching;

final class SynchronousTaskGroup<Tevent as Event> extends TaskGroup<Tevent>
{
    private array<int, array<EventHandling<Tevent>>> $listeners = [];
    private ?ImmVector<EventHandling<Tevent>> $sortedListeners;

    private Vector<EventHandling<Tevent>> $stoppedPropagationlisteners = Vector {};

    public function addTask(EventHandling<Tevent> $eventListener, int $priority = 0): this
    {
        $this->listeners[$priority][] = $eventListener;
        $this->sortedListeners = null;
        return $this;
    }

    public function removeTask(EventHandling<Tevent> $eventListener): this
    {
        $found = false;

        foreach ($this->listeners as $priority => $listeners) {
            if (false !== ($key = array_search($eventListener, $listeners, true))) {
                unset($listeners[$key]);
                $found = true;
            }
        }

        if (!$found) {
            throw new \InvalidArgumentException('Task not found in group.');
        }

        return $this;
    }

    public function addPropagationStoppedTask(EventHandling<Tevent> $eventListener): this
    {
        $this->stoppedPropagationlisteners->add($eventListener);
        return $this;
    }

    public function getTasks(): ImmVector<EventHandling<Tevent>>
    {
        if ($this->sortedListeners === null) {
            return $this->sortListeners();
        }

        return $this->sortedListeners;
    }

    public async function handleEvent(Tevent $event): Awaitable<void>
    {
        foreach ($this->getTasks() as $listener) {
            await $listener->handleEvent($event);

            if ($event->isPropagationStopped()) {
                await $this->propagationStopped($event);
                break;
            }
        }
    }

    private async function propagationStopped(Tevent $event): Awaitable<void>
    {
        $dispatch = [];
        foreach ($this->stoppedPropagationlisteners as $listener) {
            $dispatch[] = $listener->handleEvent($event);
        }

        await \HH\Asio\m($dispatch);
    }

    private function sortListeners(): ImmVector<EventHandling<Tevent>>
    {
        ksort($this->listeners, SORT_NUMERIC);

        $sortedListeners = Vector {};
        foreach ($this->listeners as $priority => $eventListeners) {
            $sortedListeners->addAll($eventListeners);
        }

        return $this->sortedListeners = $sortedListeners->immutable();
    }
}
