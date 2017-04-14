<?hh // strict

namespace GroovyCarrot\Event;

use GroovyCarrot\Event\Dispatch\Dispatcher;
use GroovyCarrot\Event\Dispatch\TaskList;

trait EventListenerRegistryTrait
{
    private Map<string, TaskList<Event>> $taskLists = Map {};

    public function tasksForEvent<TEvent as Event>(classname<TEvent> $eventClass): TaskList<TEvent>
    {
        if (!$this->taskLists->contains($eventClass)) {
            $this->taskLists->set($eventClass, new TaskList());
        }

        $taskList = $this->taskLists->at($eventClass);

        /* HH_FIXME[4110] we know that this is fine here, as the type checker
           will enforce the type to the user when they are using the object. */
        return $taskList;
    }
}
