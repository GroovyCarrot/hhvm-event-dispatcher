<?hh // partial

namespace Tests\GroovyCarrot\Event\Dispatch;

use GroovyCarrot\Event\EventHandling;
use GroovyCarrot\Event\Dispatch\Dispatcher;
use GroovyCarrot\Event\Dispatch\AsynchronousTaskGroup;
use GroovyCarrot\Event\Dispatch\SynchronousTaskGroup;
use GroovyCarrot\Event\Dispatch\TaskList;
use HackPack\HackUnit\Contract\Assert;
use PHPUnit\Framework\TestCase;
use Mockery as m;

class DispatcherSpec extends TestCase
{
    public function tearDown(): void
    {
        m::close();
    }

    /**
     * @test
     */
    public function it_creates_task_lists_only_for_event_classes(): void
    {
        $dispatcher = new Dispatcher();

        $taskList = $dispatcher->tasksForEvent(OrderPlacedEvent::class);
        $this->assertTrue($taskList instanceof TaskList);

        $this->expectException(\InvalidArgumentException::class);

        /* HH_FIXME[4110] Ignore this typechecker error to test runtime. */
        $taskList = $dispatcher->tasksForEvent(self::class);
    }

    /**
     * @test
     */
    public function it_collects_task_list_provisioners_for_lazy_loading(): void
    {
        $dispatcher = new Dispatcher();
        $expectedTaskName = 'test';
        $expectedTask = null;

        $dispatcher->addTaskProvisioner(
            OrderPlacedEvent::class,
            function (TaskList $taskList) use ($expectedTaskName, &$expectedTask) {
                $expectedTask = m::mock(EventHandling::class);
                $taskList->setTask($expectedTaskName, $expectedTask);
            }
        );

        $this->assertNull($expectedTask);

        $taskList = $dispatcher->tasksForEvent(OrderPlacedEvent::class);

        $this->assertNotNull($expectedTask);
        $this->assertTrue($taskList instanceof TaskList);
        $this->assertEquals($expectedTask, $taskList->getTask($expectedTaskName));
    }

    /**
     * @test
     */
    public function it_dipatches_events_to_handlers(): void
    {
        $dispatcher = new Dispatcher();
        $eventHandler = m::mock(EventHandling::class);
        $event = new OrderPlacedEvent(new Order(Vector{'Item 1'}));

        $eventHandler->shouldReceive('handleEvent')
            ->once()
            ->andReturnUsing(
                async function (OrderPlacedEvent $passedEvent): Awaitable<void> use ($event) {
                    $this->assertEquals($event, $passedEvent);
                }
            );

        $dispatcher->tasksForEvent(OrderPlacedEvent::class)
            ->setTask('test', $eventHandler);

        $events = \HH\Asio\join($dispatcher->dispatchEvent($event));

        $this->assertEquals($event, $events->at('test'));
    }

    /**
     * @test
     */
    public function it_dipatches_events_to_handlers_asynchronously_and_returns_the_event_for_the_desired_task(): void
    {
        $dispatcher = new Dispatcher();
        $eventHandler1 = m::mock(EventHandling::class);
        $eventHandler2 = m::mock(EventHandling::class);

        $event = new OrderPlacedEvent(new Order(Vector{'Item 1'}));
        $step2Finished = false;

        $eventHandler1->shouldReceive('handleEvent')
            ->once()
            ->andReturnUsing(
                async function (OrderPlacedEvent $passedEvent): Awaitable<void> use ($event) {
                    $this->assertEquals($event, $passedEvent);
                }
            );

        $eventHandler2->shouldReceive('handleEvent')
            ->once()
            ->andReturnUsing(
                async function (OrderPlacedEvent $passedEvent): Awaitable<void> use ($event, &$step2Finished) {
                    $this->assertEquals($event, $passedEvent);
                    // Reason to not finish executing immediately.
                    await \HH\Asio\usleep(1);
                    $step2Finished = true;
                }
            );

        $dispatcher->tasksForEvent(OrderPlacedEvent::class)
            ->setTask('first', $eventHandler1)
            ->setTask('second', $eventHandler2);

        $resultEvent = \HH\Asio\join($dispatcher->dispatchEventForTask($event, 'first'));
        $this->assertEquals($event, $resultEvent);

        $this->assertFalse($step2Finished);

        // Flush the dispatcher to finish outstanding tasks.
        \HH\Asio\join($dispatcher->flush());

        $this->assertTrue($step2Finished);
    }

    /**
     * @test
     */
    public function it_flushes_unawaited_events_upon_destruction(): void
    {
        $dispatcher = new Dispatcher();
        $eventHandler1 = m::mock(EventHandling::class);
        $eventHandler2 = m::mock(EventHandling::class);

        $event = new OrderPlacedEvent(new Order(Vector{'Item 1'}));
        $step2Finished = false;

        $eventHandler1->shouldReceive('handleEvent')
            ->once()
            ->andReturnUsing(
                async function (OrderPlacedEvent $passedEvent): Awaitable<void> use ($event) {
                    $this->assertEquals($event, $passedEvent);
                }
            );

        $eventHandler2->shouldReceive('handleEvent')
            ->once()
            ->andReturnUsing(
                async function (OrderPlacedEvent $passedEvent): Awaitable<void> use ($event, &$step2Finished) {
                    $this->assertEquals($event, $passedEvent);
                    // Reason to not finish executing immediately.
                    await \HH\Asio\usleep(1);
                    $step2Finished = true;
                }
            );

        $dispatcher->tasksForEvent(OrderPlacedEvent::class)
            ->setTask('first', $eventHandler1)
            ->setTask('second', $eventHandler2);

        // Wait on the first task.
        \HH\Asio\join($dispatcher->dispatchEventForTask($event, 'first'));

        $this->assertFalse($step2Finished);

        $dispatcher = null;

        // Ensure that the second task finished before destroying the dispatcher.
        $this->assertTrue($step2Finished);
    }
}
