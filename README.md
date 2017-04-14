# HHVM asynchronous event dispatcher

### Usage

A simple example of adding a product to a basket.

```hack
$eventDispatcher = new Dispatcher();

// Tasks must implement the interface EventListening<AddProductToBasketEvent>.
$eventDispatcher->tasksForEvent(AddProductToBasketEvent::class)
    ->setTask('add_to_basket', new AddProductToBasketTask($session->getBasket()))
    ->setTask('reserve_stock', new WarehouseReserveStockTask());

$product = new TShirt();

$events = await $eventDispatcher->dispatchEvent(new AddProductToBasketEvent($product));
```
```
class HH\Map (2) {
  public $add_to_basket =>
  class AddProductToBasketEvent#2 (1) {
    public $product =>
    class TShirt#1 {}
  }
  public $reserve_stock =>
  class AddProductToBasketEvent#3 (1) {
  public $product =>
    class TShirt#1 {}
  }
}
```

A more complex example of placing the order.

```hack
$eventDispatcher->tasksForEvent(PlaceOrderEvent::class)
    ->setTask(
        'process_order',
        // Synchronous task groups will propagate the event to the subtasks in order. Event propagation can
        // be stopped by any of the subtasks, which then triggers the propagation-stopped tasks instead.
        SynchronousTaskGroup::newGroup()
            ->addTask(new ProcessPaymentTask(), 0)
            ->addTask(new WarehouseDispatchTask(), 1)
            ->addPropagationStoppedTask(new OrderProcessFailedTask())
            ->addPropagationStoppedTask(new LogToDatabaseTask()))
    ->setTask(
        'logging',
        // Asynchronous task groups will propagate the event to the subtasks at the same time. Each subtasks
        // can modify the event, however stopping propagation is not allowed as it will be unsafe.
        AsynchronousTaskGroup::newGroup()
            ->addSubtask(new LogToDatabaseTask())
            ->addSubtask(new LogToFilesystemTask()));

$order = new Order($session->getCustomer(), $session->getBasket());
$event = new PlaceOrderEvent($order);

// Dispatch the event, and return the finished event for a particular task.
$finishedEvent = await $eventDispatcher->dispatchEventForTask($event, 'process_order');

if ($finishedEvent->isPropagationStopped()) {
    // We know if the order failed by checking whether or not the event propagation was stopped.
    // PlaceOrderEvent could implement logic to store error information for handling here.
    print "Sorry, {$finishedEvent->getFailureReason()}";
} else {
    print "Order successful!";
}
```

### Running tests

```hack
hhvm vendor/bin/phpunit --bootstrap vendor/hh_autoload.php
```
