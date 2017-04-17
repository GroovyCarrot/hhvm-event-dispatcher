<?hh // strict

namespace GroovyCarrot\Event\Dispatch;

use GroovyCarrot\Event\Event;
use GroovyCarrot\Event\EventHandling;

// @todo remove this when this is a part of HHVM.
use HH\KeyedTraversable;
use HH\KeyedIterator;
interface KeyedIteratorAggregate<+Tk, +Tv> extends KeyedTraversable<Tk, Tv>, \IteratorAggregate<Tv>
{
    public function getIterator(): KeyedIterator<Tk, Tv>;
}

final class TaskList<Tevent as Event> implements KeyedIteratorAggregate<string, EventHandling<Tevent>>
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

    public function getIterator(): KeyedIterator<string, EventHandling<Tevent>>
    {
        return $this->getTasks()->getIterator();
    }
}
