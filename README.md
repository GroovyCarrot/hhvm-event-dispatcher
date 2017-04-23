# HHVM asynchronous event dispatcher

### Usage
A simple example of adding a product to a basket: when a product is added to a basket, a couple of things need
to happen; the first task is to add it to the basket itself stored in the current session. The other task is to
inform the warehouse to reserve the stock, to ensure the order can be placed later on. These responsibilities
should be separate in the design of an object-oriented application, which is where events come in.
```hack
$eventDispatcher = new Dispatcher();

// Tasks must implement the interface EventHandling<AddProductToBasketEvent>.
$eventDispatcher->tasksForEvent(AddProductToBasketEvent::class)
    ->setTask('add_to_basket', new AddProductToBasketTask($session->getBasket()))
    ->setTask('reserve_stock', new WarehouseReserveStockTask());

$product = new TShirt();

$events = await $eventDispatcher->dispatchEvent(new AddProductToBasketEvent($product));
```
Both tasks `add_to_basket` and `reserve_stock` are called
[asynchronously](https://docs.hhvm.com/hack/async/introduction), and each task receives a different copy of
the event. To inspect the event for each task `dispatchEvent()` returns a
`Map<string, AddProductToBasketEvent>`.
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

A more complex example of placing an order.

```hack
$eventDispatcher->tasksForEvent(PlaceOrderEvent::class)
    ->setTask(
        'process_order',
        // Synchronous task groups will propagate the event to the subtasks in order. Event propagation can
        // be stopped by any of the subtasks, which then triggers the propagation-stopped tasks instead.
        // None of these tasks will prevent other tasks outside of this group from running however.
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

// We need to finish any outstanding tasks that were not awaited. This happens automatically when the
// dispatcher is destroyed, however can be triggered manually if the application terminates a
// significant amount of time after these operations.
await $eventDispatcher->flush();
```

### Running tests
```hack
hhvm vendor/bin/phpunit
```
