<?hh

namespace Test\GroovyCarrot\Event;

use PHPUnit\Framework\TestCase;

class EventSpec extends TestCase
{
    /**
     * @test
     */
    public function it_should_be_told_if_stopping_propagation_is_unsafe(): void
    {
        $event = new EventStub();

        $event->setStoppingPropagationIsUnsafe();

        $this->assertTrue($event->isStoppingPropagationUnsafe());
    }

    /**
     * @test
     */
    public function it_should_know_if_event_propagation_is_stopped(): void
    {
        $event = new EventStub();

        $event->stopPropagation();

        $this->assertTrue($event->isPropagationStopped());
    }
}