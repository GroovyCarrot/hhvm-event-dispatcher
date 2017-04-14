<?hh // strict

namespace GroovyCarrot\Event;

use GroovyCarrot\Event\Exception\StopPropagationUnsafeException;

abstract class Event
{
    private bool $stopPropagationIsUnsafe = false;
    private bool $propagationStopped = false;

    public function setStoppingPropagationIsUnsafe(): void
    {
        $this->stopPropagationIsUnsafe = true;
    }

    public function stopPropagation(): void
    {
        if ($this->isStoppingPropagationUnsafe()) {
            throw new StopPropagationUnsafeException('Stopping event propagation is unsafe as this event is not being propagated in a synchronous task.');
        }

        $this->propagationStopped = true;
    }

    public function isStoppingPropagationUnsafe(): bool
    {
        return $this->stopPropagationIsUnsafe;
    }

    public function isPropagationStopped(): bool
    {
        return $this->propagationStopped;
    }
}
