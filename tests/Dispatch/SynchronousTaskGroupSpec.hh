<?hh // partial

namespace Tests\GroovyCarrot\Event\Dispatch;

use GroovyCarrot\Event\EventHandling;
use GroovyCarrot\Event\Dispatch\SynchronousTaskGroup;
use Mockery as m;

class SynchronousTaskGroupSpec extends TaskGroupSpec
{
    public function __construct()
    {
        parent::__construct();

        $this->group = SynchronousTaskGroup::newGroup();
    }

    /**
     * @test
     */
    public function it_should_propagate_an_event_to_tasks_synchronously(): void
    {
        $self = $this;

        $order = new Order(Vector{'T-shirt', 'Jeans'});
        $event = new OrderPlacedEvent($order);
        $step = 0;

        $first = m::mock(EventHandling::class);
        $first->shouldReceive('handleEvent')
            ->times(1)
            ->andReturnUsing(
                async function (OrderPlacedEvent $passedEvent): Awaitable<void> use ($self, $event, &$step)
                {
                    $this->assertEquals($event, $passedEvent);
                    $this->assertEquals($event->order, $passedEvent->order);
                    // Create a delay, if this task was asynchronous then the other
                    // tasks would continue.
                    await \HH\Asio\usleep(200000);
                    $self->assertEquals(0, $step);
                    $step = 1;
                }
            );

        $second = m::mock(EventHandling::class);
        $second->shouldReceive('handleEvent')
            ->times(1)
            ->andReturnUsing(
                async function (OrderPlacedEvent $passedEvent): Awaitable<void> use ($self, $event, &$step)
                {
                    $this->assertEquals($event, $passedEvent);
                    $this->assertEquals($event->order, $passedEvent->order);
                    $self->assertEquals(1, $step);
                    $step = 2;
                }
            );

        $third = m::mock(EventHandling::class);
        $third->shouldReceive('handleEvent')
            ->times(1)
            ->andReturnUsing(
                async function (OrderPlacedEvent $passedEvent): Awaitable<void> use ($self, $event, &$step)
                {
                    $this->assertEquals($event, $passedEvent);
                    $this->assertEquals($event->order, $passedEvent->order);
                    $self->assertEquals(2, $step);
                    $step = 3;
                }
            );

        $group = SynchronousTaskGroup::newGroup()
            ->addTask($first, 0)
            ->addTask($third, 2)
            ->addTask($second, 1);

        \HH\Asio\join($group->handleEvent($event));

        $this->assertEquals(3, $step);
    }
}
