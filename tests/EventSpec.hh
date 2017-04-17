<?hh // partial

namespace Test\GroovyCarrot\Event;

use GroovyCarrot\Event\Exception\StopPropagationUnsafeException;
use PHPUnit\Framework\TestCase;

class EventSpec extends TestCase
{
    /**
     * @test
     */
    public function it_prevents_stopping_propagation_if_it_is_unsafe(): void
    {
        $event = new EventStub();

        $event->setStoppingPropagationIsUnsafe();

        $this->assertTrue($event->isStoppingPropagationUnsafe());

        $this->expectException(StopPropagationUnsafeException::class);

        $event->stopPropagation();
    }

    /**
     * @test
     */
    public function it_knows_if_event_propagation_is_stopped(): void
    {
        $event = new EventStub();

        $event->stopPropagation();

        $this->assertTrue($event->isPropagationStopped());
    }
}
