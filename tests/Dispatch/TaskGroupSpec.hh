<?hh // partial

namespace Tests\GroovyCarrot\Event\Dispatch;

use GroovyCarrot\Event\Dispatch\TaskGroup;
use GroovyCarrot\Event\EventHandling;
use PHPUnit\Framework\TestCase;
use Mockery as m;

abstract class TaskGroupSpec extends TestCase
{
    protected TaskGroup $group;

    public function tearDown(): void
    {
        m::close();
    }

    /**
     * @test
     */
    public function it_collects_tasks_and_remove_them(): void
    {
        $task = m::mock(EventHandling::class);

        $this->group->addTask($task);
        $this->assertEquals(ImmVector{$task}, $this->group->getTasks());

        $this->group->removeTask($task);
        $this->assertTrue($this->group->getTasks()->isEmpty());
    }
}
