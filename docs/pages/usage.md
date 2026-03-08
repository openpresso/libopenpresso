# API Usage

This guide provides an in-depth look at the Libopenpresso API architecture, focusing on component lifetime management, thread safety, and the controller activation model. It's assumed that you already read
the [quick start guide](quick_start.md) and are familiar with the overall libopenpresso usage workflow.

## LibopenpressoCore & Component Access

\ref libopenpresso::interfaces::LibopenpressoCore "LibopenpressoCore" serves as the central factory and access point for all other components.

### Lazy Factory Pattern
Most top-level components (sensors, controllers) are created on-demand when you first request them using the `get...` methods (e.g., \ref libopenpresso::interfaces::LibopenpressoCore::getTemperatureSensor "getTemperatureSensor"). 
- **Initialization**: The first call initializes the component and its dependencies.
- **Caching**: Subsequent calls with the same label return the same cached instance (Singleton-per-label).
- **Exceptions**: If a requested label is missing from the configuration, doesn't match to requested interface or there is a hardware initialization error, methods will throw `libopenpresso::Exception`.

> [!NOTE]
> While high-level components are lazy, some shared low-level resources (like GPIO event handlers and communication buses) are initialized immediately when \ref libopenpresso::interfaces::LibopenpressoCore "LibopenpressoCore" is constructed.

## Lifetime Management

### Automatic Cleanup
All components are returned as `std::shared_ptr` and clean up happens automatically when
the last shared pointer is destroyed. *Remember that if `LibopenpressoCore`
wasn't destroyed, it also holds one strong reference to each of the requested components*.

### Controllers deactivation and sensors callbacks unregister
Automatic clean up also applies to controllers in active state and sensors with registered callbacks, they will be deactivated and then destroyed. **Deactivation methods can potentially raise an exception** if returning hardware into the default state fails, therefore for production use it's recommended to **call them explicitly before getting into the destructor**, or **have watchdog setup** just in case some deactivation method raises an exception inside the destructor.

### Dependencies
Components hold strong references to their dependencies. For example, a `PidTemperatureController` holds a reference to its `TemperatureSensor` and `PowerController`. Even if you drop your reference to the sensor, it will stay alive as long as the controller needs it.

### Safe Core Destruction
You can safely destroy the `LibopenpressoCore` instance once you have requested all the components your application needs. The components will remain valid because they hold their own dependencies.

### Callback Resource Safety
- **Dangling pointers**: When you register a callback with access by reference to any object with externally managed lifetime, it's your responsibility to keep this object alive until you explicitly unregister the callback or the component firing the callback is destroyed.
- **Shared pointers deadlocks**: You should never store shared pointer to the sensor inside the callback
registered for this sensor, as it will lead to dead lock and sensor will never be destroyed. Given that components can have internal dependencies, the rule of thumb is to avoid holding shared pointers to libopenpresso components inside callbacks.

## Thread Safety

### LibopenpressoCore
Access to `LibopenpressoCore` factory methods (`get...`) is **NOT** thread-safe. It should be
protected from concurrent calls from different threads.

### Sensors
Sensor interfaces **ARE** thread-safe. You can safely call getters and callback operation methods from multiple threads simultaneously. Avoid frequent calls to `registerCallback` and
`unregisterCallback` as they use internal synchronization primitives and will also lock processing of already
registered callbacks inside the event handler thread.

### Controllers
Controller interfaces are **NOT** thread-safe.
- **Single Owner**: Controllers are designed to be managed by a single control logic thread.
- **Synchronization**: If you need to `activate`, `deactivate`, or `setTarget...` from multiple threads, you must use external locking.

## Controller Architecture

### Activation Model
Controllers have an explicit active/inactive state managed via `activate()` and `deactivate()`.
- **Activate**: Starts the control loop, claims hardware resources, and enables outputs.
- **Deactivate**: Stops control, releases hardware resources, and puts outputs into a safe state (usually off).

### Unique Write Access
An essential feature of the architecture is that active controllers claim **exclusive write access** to their underlying hardware.
- **Exclusive Claim**: When `activate()` is called, the controller attempts to claim its dependencies (e.g., a pump or heater driver).
- **Conflict**: If another controller is already active and using the same underlying hardware, `activate()` will throw a `libopenpresso::Exception`.

### Hardware Sharing
This exclusive access model allows multiple high-level controllers to share the same physical hardware safely, provided they are not active simultaneously.

**Example**:
- Both **Pressure Controller** and **Flow Rate Controller** uses the same pump driver component.
- You can switch between them by strictly deactivating one before activating the other. The underlying pump resource is released by the first and claimed by the second.