<?hh // strict

namespace GroovyCarrot\Event\Dispatch;

use GroovyCarrot\Event\Event;

final class TaskList<TEvent as Event>
{
    private Map<string, Task<TEvent>> $tasks = Map {};

    public function setTask(string $taskName, Task<TEvent> $task): void
    {
        $this->tasks->set($taskName, $task);
    }

    public function getTask(string $taskName): Task<TEvent>
    {
        if (!$this->tasks->contains($taskName)) {
            throw new \InvalidArgumentException("Queue '{$taskName}' does not exist.");
        }

        return $this->tasks->at($taskName);
    }

    public function getTasks(): ImmMap<string, Task<TEvent>>
    {
        return $this->tasks->toImmMap();
    }
}
