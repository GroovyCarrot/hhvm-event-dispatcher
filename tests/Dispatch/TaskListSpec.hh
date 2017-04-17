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
    public function it_collects_tasks_with_keys(): void
    {
        $taskList = new TaskList();
        $expectedTask = m::mock(EventHandling::class);
        $expectedTaskName = 'test_task';

        $taskList->setTask($expectedTaskName, $expectedTask);
        $this->assertEquals($expectedTask, $taskList->getTask($expectedTaskName));
        $this->assertEquals(ImmMap{$expectedTaskName => $expectedTask}, $taskList->getTasks());
    }

    /**
     * @test
     */
    public function it_is_iterable(): void
    {
        $taskList = new TaskList();

        $tasks = ImmMap{
            1 => m::mock(EventHandling::class),
            2 => m::mock(EventHandling::class),
        };

        $taskList->setTask('task1', $tasks->at(1));
        $taskList->setTask('task2', $tasks->at(2));

        $i = 1;
        foreach ($taskList as $name => $task) {
            $this->assertEquals("task{$i}", $name);
            $this->assertTrue($tasks->at($i) === $task);
            $i++;
        }
    }
}
