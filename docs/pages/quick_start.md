# Quick Start

This guide covers the basics of getting started with **libopenpresso**.

## Prerequisites

- **Linux OS** with userspace headers
- **C++23** compatible compiler (GCC 13+, Clang 16+)
- **CMake** 3.25+
- **Conan** 2.0+ (optional but recommended)

---

## Installation

### Option 1: Conan (Recommended)

Install libopenpresso using Conan for automatic dependency management:

```bash
conan remote add openpresso https://conan.cloudsmith.io/openpresso/@LIBOPENPRESSO_REPO_CHANNEL@
```

Add to your `conanfile.txt`:

```ini
[requires]
libopenpresso/@LIBOPENPRESSO_VERSION@

[generators]
CMakeDeps
CMakeToolchain
```

Then install:
```bash
conan install . --build=missing
```

### Option 2: CMake FetchContent (Simplest)

For quick prototyping without external package manager, use `FetchContent` in your `CMakeLists.txt`:

```cmake
include(FetchContent)

FetchContent_Declare(
    libopenpresso
    GIT_REPOSITORY https://github.com/openpresso/libopenpresso.git
    GIT_TAG        main # Or specific tag
)

FetchContent_MakeAvailable(libopenpresso)

add_executable(my_espresso_machine main.cpp)
target_link_libraries(my_espresso_machine PRIVATE libopenpresso)
```

---

## Basic Usage

Let's build a simple system to pour water through the group head
with **controlled pressure**. We will configure:
1.  **AC Mains Zero Cross Sensor**: To synchronize pump driver on-off cycles with AC mains.
2.  **Pump Driver**: A vibration pump controlled by pulses with triac or solid-state relay.
3.  **Pressure Sensor**: To read actual pressure.
4.  **Pressure Controller**: To maintain target pressure.
5.  **Valve**: A solenoid 3-way valve that opens water flow from the boiler to the group head.

### 1. Configure Components

Let's first define a function that makes a `DeviceConfig` with all
the components listed above.

```cpp
#include <libopenpresso/libopenpresso.hpp>

libopenpresso::DeviceConfig makeConfig()
{
    using namespace libopenpresso;

    static const unix_dev_addr_t i2cBus = "/dev/i2c-0";
    static const unix_dev_addr_t gpioChip = "/dev/gpiochip0";

    DeviceConfig config;
    config.components["ac_sensor"] = AcZeroCrossSensorConfig{
        .signalPin = {.chip = gpioChip, .pin = 17}
    };

    config.components["pump_driver"] = PulseControlledDeviceConfig{
        .pulsePin = {.chip = gpioChip, .pin = 27}
        .acZeroCrossSensor = "ac_sensor"
    };

    config.components["pressure_sensor"] = Ads1115PressureSensorConfig{
        .addr = {.bus = i2cBus, .dev = 0x48},
        .signalPin = {.chip = gpioChip, .pin = 4}
    };

    config.components["pressure_ctrl"] = PulsePressureControllerConfig{
        .pulseController = "pump_driver",
        .sensor = "pressure_sensor" 
    };

    config.components["group_valve"] = LogicalOutputPinConfig{
        .addr = {.chip = gpioChip, .pin = 22},
        .initState = false,
        .inverted = false,
    };

    return config;
}
```

### 2. Initialize Core

The `getCore` function creates all components and wires their dependencies automatically.

```cpp
    auto core = libopenpresso::getCore(makeConfig());
```

### 3. Perform brew

Now we can use the high-level interfaces to control the machine.
Let's demonstrate how to pump water while targeting specific pressure and
print actual pressure to standard output.

```cpp
void testBrew(const std::shared_ptr<libopenpresso::interfaces::LibopenpressoCore>& core)
{
    // Get interfaces
    auto pressureCtrl = core->getPressureController("pressure_ctrl");
    auto pressureSensor = core->getPressureSensor("pressure_sensor");
    auto valve = core->getLogicalOutput("group_valve");

    std::cout << "Starting Brew Cycle..." << std::endl;
    auto printPressure = [](libopenpresso::millbars_t pressure) {
        std::cout << std::format("\rPressure: {:3.3f} bars", 0.001f * pressure) << std::flush;
    };
    //read pressure using getter method
    printPressure(pressureSensor->getPressure());
    //subscribe to pressure change updates using callback
    auto cbDesr = pressureSensor->registerCallback(printPressure);

    //  Open the group valve
    valve->activate();
    valve->setState(true);

    // Start Pump at 3 Bar (assume we're doing preinfusion)
    pressureCtrl->setTargetPressure(3'000); 
    pressureCtrl->activate();

    std::this_thread::sleep_for(5s);

    // We can adjust target pressure while controller is active
    pressureCtrl->setTargetPressure(9'000);

    std::this_thread::sleep_for(10s);

    // Stop
    pressureCtrl->deactivate();
    valve->deactivate(); // Valve will return to the init state when deactivated
    pressureSensor->unregisterCallback(cbDesr);

    std::cout << std::endl << "Brew Finished." << std::endl;
}
```

---

## Advanced Usage

Real espresso machines actually require much more complex configurations involving:
- PID Temperature Control with Feed-forward
- Flow Rate Controllers
- Brew Profiling
- Steam Management

An example below demonstrates the next automated flow:
1. Activate boiler preheat
2. Print actual temperature while heating
3. Start execution of predefined multi-step brew profile when target temperature is reached
4. Stop brewing at 36 grams of coffee in the cup.
5. Switch into steam mode when brew is finished
6. Print actual boiler temperature while steaming

```cpp
void advancedBrew(const std::shared_ptr<libopenpresso::interfaces::LibopenpressoCore>& core) {
    using namespace libopenpresso;
    using namespace libopenpresso::brew_step_targets;
    using namespace libopenpresso::brew_step_advance_conditions;
    using namespace std::chrono_literals;

    auto brewProfiler = core->getBrewProfiler("profiler");
    auto temperatureSensor = core->getTemperatureSensor("boiler_temp");
    auto brewTemperatureController = core->getTemperatureController("brewTempCtrl");
    auto steamer = core->getTemperatureController("steamer");

    brewProfiler->setSteps({
        { ConstantPressure{ 1'500 }, OnWeight{ 1'000 } },
        { ConstantFlow { 2'500 }, OnStepTime{ 3s } },
        { ConstantPressure{ 0 }, OnStepTime{ 4s } },
        { ConstantPressure{ 3'000 }, OnStepTime{ 2s } },
        { ConstantPressure{ 6'000 }, OnStepTime{ 2s } },
        { ConstantPressure{ 9'000 }, Never{} }
    });

    brewProfiler->setAutoStopCondition(OnWeight{ 36'000 });

    std::atomic<bool> brewFinished = false;

    brewProfiler->registerStepChangeCallback([&brewFinished](auto step) {
        if (std::holds_alternative<interfaces::BrewProfiler::stopped_flag_t>(step)) {
            brewFinished = true;
            brewFinished.notify_one();
        } else {
            std::cout << "Advanced to step " << std::get<0>(step) << "\n";
        }
    });

    std::atomic<bool> brewTemperatureReached = false;

    auto temperatureCallback = [&brewTemperatureReached](millidegrees_t temperature) {
        if(!brewTemperatureReached.load() && temperature >= 95'000) {
            brewTemperatureReached = true;
            brewTemperatureReached.notify_one();
        }
        else {
            std::cout << std::format("\rTemperature: {:3.3f} °C    ", 0.001f * temperature) << std::flush;
        }
    };

    std::cout << "Preheating..." << std::endl;
    auto temperatureDescr = temperatureSensor->registerCallback(temperatureCallback);

    brewTemperatureController->setTargetTemperature(95'000);
    brewTemperatureController->activate();
    brewTemperatureReached.wait(false);
    temperatureSensor->unregisterCallback(temperatureDescr);

    std::cout << std::endl << "Starting brew profile..." << std::endl;
    brewProfiler->activate();
    brewFinished.wait(false);
    brewProfiler->deactivate();
    brewTemperatureController->deactivate(); // steam controller will take care about boiler temperature
    std::cout << "Brew finished. Switching to Steam Mode" << std::endl;

    temperatureSensor->registerCallback([](millidegrees_t temperature) {
        std::cout << std::format("\rTemperature: {:3.3f} °C    ", 0.001f * temperature) << std::flush;
    });
    
    steamer->setTargetTemperature(155'000);
    steamer->activate();
}
```

See the [Device Config Guide](device_config.md) for how to build a machine configuration for just demonstrated usage.

---

## What's Next?

- **[Installation Guide](installation.md)**: Detailed installation options.
- **[Device Config Guide](device_config.md)**: Detailed description of the component configurations.
- **\ref libopenpresso::interfaces "Interfaces API reference"**: List of all available interfaces with detailed methods description.
- **[Usage Guide](usage.md)**: API reference for controlling the machine.
- **[PID Tuning](pid_tuning.md)**: How to tune your temperature controllers.
