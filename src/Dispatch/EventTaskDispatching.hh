<?hh // strict

namespace GroovyCarrot\Event\Dispatch;

use GroovyCarrot\Event\Event;

interface EventTaskDispatching<TEvent as Event>
{
    public function dispatch(TEvent $event): Awaitable<void>;
}
