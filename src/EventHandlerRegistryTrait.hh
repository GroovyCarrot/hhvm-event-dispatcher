<?hh // strict

namespace GroovyCarrot\Event;

use GroovyCarrot\Event\Dispatch\Dispatcher;
use GroovyCarrot\Event\Dispatch\TaskList;

trait EventHandlerRegistryTrait
{
    require implements EventHandlerTaskCollecting;

    private Map<string, TaskList<Event>> $taskLists = Map{};
    private Map<string, Vector<(function (TaskList<Event>): void)>> $taskListProvisioners = Map{};

    public function addTaskProvisioner<Tevent as Event>(
        classname<Tevent> $eventClass,
        (function (TaskList<Tevent>): void) $lambda
    ): void {
        if (!$this->taskListProvisioners->contains($eventClass)) {
            $this->verifyEventClass($eventClass);
            $this->taskListProvisioners->set($eventClass, Vector{});
        }

        /* HH_FIXME[4110] the type checker will enforce the type to the user when they call the function. */
        $this->taskListProvisioners->at($eventClass)->add($lambda);
    }

    public function tasksForEvent<Tevent as Event>(classname<Tevent> $eventClass): TaskList<Tevent>
    {
        if (!$this->taskLists->contains($eventClass)) {
            $this->verifyEventClass($eventClass);

            $taskList = new TaskList();
            $this->taskLists->set($eventClass, $taskList);

            if ($this->taskListProvisioners->contains($eventClass)) {
                foreach ($this->taskListProvisioners->at($eventClass) as $lambda) {
                    $lambda($taskList);
                }
            }
        }

        $taskList = $this->taskLists->at($eventClass);

        /* HH_FIXME[4110] the type checker will enforce the type to the user when they are using the object. */
        return $taskList;
    }

    private function verifyEventClass(classname<Event> $eventClass): void
    {
        if (!(new \ReflectionClass($eventClass))->isSubclassOf(Event::class)) {
            $eventSuperClass = Event::class;
            throw new \InvalidArgumentException("{$eventClass} is not a subclass of {$eventSuperClass}.");
        }
    }
}
