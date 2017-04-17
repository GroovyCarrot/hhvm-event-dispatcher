<?hh // strict

namespace GroovyCarrot\Event\Dispatch;

use GroovyCarrot\Event\Event;
use GroovyCarrot\Event\EventHandling;

final class TaskList<Tevent as Event>
{
    private Map<string, EventHandling<Tevent>> $tasks = Map{};

    public function setTask(string $taskName, EventHandling<Tevent> $task): this
    {
        $this->tasks->set($taskName, $task);
        return $this;
    }

    public function getTask(string $taskName): EventHandling<Tevent>
    {
        if (!$this->tasks->contains($taskName)) {
            throw new \InvalidArgumentException("Task '{$taskName}' does not exist.");
        }

        return $this->tasks->at($taskName);
    }

    public function getTasks(): ImmMap<string, EventHandling<Tevent>>
    {
        return $this->tasks->toImmMap();
    }
}
