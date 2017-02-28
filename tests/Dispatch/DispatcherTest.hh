<?hh // partial

namespace Tests\GroovyCarrot\Event\Dispatch;

use GroovyCarrot\Event\EventListening;
use GroovyCarrot\Event\Dispatch\Dispatcher;
use GroovyCarrot\Event\Dispatch\AsynchronousTask;
use GroovyCarrot\Event\Dispatch\SynchronousTask;
use HackPack\HackUnit\Contract\Assert;
use PHPUnit\Framework\TestCase;
use Mockery as m;

class DispatcherTest extends TestCase
{
    public function tearDown(): void
    {
        m::close();
    }

    public function testDispatchSynchronous(): void
    {
        $eventDispatcher = new Dispatcher();
        $self = $this;

        $order = new Order(Vector{'T-shirt', 'Jeans'});
        $event = new OrderPlacedEvent($order);
        $step = 0;

        $first = m::mock(EventListening::class);
        $first->shouldReceive('handleEvent')
            ->times(1)
            ->andReturnUsing(async function (OrderPlacedEvent $passedEvent): Awaitable<void> use ($self, $event, &$step) {
                // Create a delay, if this task was asynchronous then the other
                // tasks would continue.
                await \HH\Asio\usleep(200000);
                $self->assertEquals(0, $step);
                $step = 1;
            });

        $second = m::mock(EventListening::class);
        $second->shouldReceive('handleEvent')
            ->times(1)
            ->andReturnUsing(async function (OrderPlacedEvent $passedEvent): Awaitable<void> use ($self, $event, &$step) {
                $self->assertEquals(1, $step);
                $step = 2;
            });

        $third = m::mock(EventListening::class);
        $third->shouldReceive('handleEvent')
            ->times(1)
            ->andReturnUsing(async function (OrderPlacedEvent $passedEvent): Awaitable<void> use ($self, $event, &$step) {
                $self->assertEquals(2, $step);
                $step = 3;
            });

        $eventDispatcher->tasksForEvent(OrderPlacedEvent::class)
            ->setTask(
                OrderPlacedEvent::TASK_PROCESS_ORDER,
                SynchronousTask::newTask()
                    ->addSubtask($first, 0)
                    ->addSubtask($third, 2)
                    ->addSubtask($second, 1));

        \HH\Asio\join($eventDispatcher->dispatch($event));

        $this->assertEquals(3, $step);
    }

    public function testDispatchAsynchronous(): void
    {
        $eventDispatcher = new Dispatcher();
        $self = $this;

        $order = new Order(Vector{'T-shirt', 'Jeans'});
        $event = new OrderPlacedEvent($order);
        $step = 0;

        $listener = m::mock(EventListening::class);
        $listener->shouldReceive('handleEvent')
            ->times(3)
            ->andReturnUsing(async function (OrderPlacedEvent $passedEvent): Awaitable<void> use ($self, $event, &$step)
            {
                // Assert that the order passed is identical.
                $self->assertEquals($event->order, $passedEvent->order);
                // Assert that the event itself however is different.
                $self->assertTrue($event !== $passedEvent);
                // Assert that we cannot stop propagation for this event as it
                // is propagating asynchronously.
                $self->assertTrue($passedEvent->stopPropagationIsUnsafe());
                $step++;
            });

        $eventDispatcher->tasksForEvent(OrderPlacedEvent::class)
            ->setTask(
                OrderPlacedEvent::TASK_PROCESS_ORDER,
                AsynchronousTask::newTask()
                    ->addSubtask($listener)
                    ->addSubtask($listener)
                    ->addSubtask($listener)
            );

        \HH\Asio\join($eventDispatcher->dispatch($event));

        $this->assertEquals(3, $step);
    }
}
