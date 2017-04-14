<?hh // strict

namespace GroovyCarrot\Event\Dispatch;

use GroovyCarrot\Event\Event;
use GroovyCarrot\Event\EventListening;

final class TaskList<TEvent as Event>
{
    private Map<string, EventListening<TEvent>> $tasks = Map {};

    public function setTask(string $taskName, EventListening<TEvent> $task): this
    {
        $this->tasks->set($taskName, $task);
        return $this;
    }

    public function getTask(string $taskName): EventListening<TEvent>
    {
        if (!$this->tasks->contains($taskName)) {
            throw new \InvalidArgumentException("Task '{$taskName}' does not exist.");
        }

        return $this->tasks->at($taskName);
    }

    public function getTasks(): ImmMap<string, EventListening<TEvent>>
    {
        return $this->tasks->toImmMap();
    }
}
