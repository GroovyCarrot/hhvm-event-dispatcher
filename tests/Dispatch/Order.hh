<?hh // strict

namespace Tests\GroovyCarrot\Event\Dispatch;

class Order
{
    public function __construct(
        public Vector<string> $items,
    ) {
    }
}
