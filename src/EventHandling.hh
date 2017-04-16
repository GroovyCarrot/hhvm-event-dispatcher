<?hh // strict

namespace GroovyCarrot\Event;

interface EventHandling<Tevent as Event>
{
    public function handleEvent(Tevent $event): Awaitable<void>;
}
