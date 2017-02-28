<?hh // strict

namespace GroovyCarrot\Event;

interface EventListening<Tevent as Event>
{
    public function handleEvent(Tevent $event): Awaitable<void>;
}
