<?hh // partial

namespace Tests\GroovyCarrot\Event\Dispatch;

use GroovyCarrot\Event\EventHandling;
use GroovyCarrot\Event\Dispatch\AsynchronousTaskGroup;
use Mockery as m;

class AsynchronousTaskGroupSpec extends TaskGroupSpec
{
    public function __construct()
    {
        parent::__construct();

        $this->group = AsynchronousTaskGroup::newGroup();
    }

    /**
     * @test
     */
    public function it_propagates_an_event_to_tasks_asynchronously(): void
    {
        $self = $this;

        $order = new Order(Vector{'T-shirt', 'Jeans'});
        $event = new OrderPlacedEvent($order);
        $step = 0;

        $listener = m::mock(EventHandling::class);
        $listener->shouldReceive('handleEvent')
            ->times(3)
            ->andReturnUsing(
                async function (OrderPlacedEvent $passedEvent): Awaitable<void> use ($self, $event, &$step) {
                    // Assert that the order passed is identical.
                    $self->assertEquals($event->order, $passedEvent->order);
                    // Assert that we cannot stop propagation for this event as it
                    // is propagating asynchronously.
                    $self->assertTrue($passedEvent->isStoppingPropagationUnsafe());
                    $step++;
                }
            );

        $group = AsynchronousTaskGroup::newGroup()
            ->addTask($listener)
            ->addTask($listener)
            ->addTask($listener);

        \HH\Asio\join($group->handleEvent($event));

        $this->assertEquals(3, $step);
    }
}
