<?hh // strict

namespace Tests\GroovyCarrot\Event\Dispatch;

use GroovyCarrot\Event\Event;

class OrderPlacedEvent extends Event
{
    const string TASK_PROCESS_ORDER = 'order.placed.process';

    public function __construct(
        public Order $order,
    )
    {}
}
