<?hh // partial

namespace Tests\GroovyCarrot\Event\Dispatch;

use GroovyCarrot\Event\Dispatch\TaskList;
use GroovyCarrot\Event\EventHandling;
use Mockery as m;
use PHPUnit\Framework\TestCase;

class TaskListSpec extends TestCase
{
    /**
     * @test
     */
    public function it_should_collect_tasks_with_keys(): void
    {
        $taskList = new TaskList();
        $expectedTask = m::mock(EventHandling::class);
        $expectedTaskName = 'test_task';

        $taskList->setTask($expectedTaskName, $expectedTask);
        $this->assertEquals($expectedTask, $taskList->getTask($expectedTaskName));
        $this->assertEquals(ImmMap{$expectedTaskName => $expectedTask}, $taskList->getTasks());
    }
}
