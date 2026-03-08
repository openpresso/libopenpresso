# Device Configuration {#device_config}

This guide covers how to create a \ref libopenpresso::DeviceConfig "DeviceConfig" —
the blueprint that describes your espresso machine hardware to libopenpresso.  
Once configured, you pass it to \ref libopenpresso::getCore() "libopenpresso::getCore()" 
to obtain a fully initialized \ref libopenpresso::interfaces::LibopenpressoCore "Core" instance.

> For future after initialization, see [API Usage Guide](usage.md).

---

## Overview

The basic workflow is:

1. Define your hardware components (sensors, controllers, I/O) as config structs
2. Put them into a \ref libopenpresso::components_map_t "components map", each under a unique **label**
3. Assemble the final \ref libopenpresso::DeviceConfig "DeviceConfig" struct
4. Call \ref libopenpresso::getCore() "libopenpresso::getCore(config)" — the Core creates all components on demand

```cpp
#include <libopenpresso/libopenpresso.hpp>

libopenpresso::DeviceConfig config;
config.components["my_sensor"] = libopenpresso::Ads1115PressureSensorConfig{...};
config.components["my_controller"] = libopenpresso::PulsePressureControllerConfig{
    .pulseController = "my_pump",   // reference another component by label
    .sensor = "my_sensor",          // reference another component by label
};
// ...

auto core = libopenpresso::getCore(config);
auto pressure = core->getPressureSensor("my_sensor");
```

---

## The Components Map

\ref libopenpresso::components_map_t "components_map_t" is an `std::unordered_map<component_label_t, component_config_t>`.

Each entry associates a **unique string label** with a **component configuration** struct.  
Labels serve a dual purpose:

- **Lookup key** — retrieve the component from the Core via `core->getXxx("label")`
- **Dependency reference** — other components reference dependencies by their label

The \ref libopenpresso::component_config_t "component_config_t" variant holds any supported config 
type — see the API reference for the full list.

Most components are **created lazily** — the Core instantiates a component the first time 
you request it, then caches it for subsequent accesses.

---

## Understanding Dependencies

Many components depend on other components. Dependencies are expressed by 
including the **label** of the dependency in the config struct. The Core 
automatically resolves and creates dependencies when a component is first accessed.
If any field has a type of \ref libopenpresso::component_label_t "component_label_t",
there are two possible options and a comment in API reference will let you know that:
- This field should point to some specific component config. 
For example see how \ref libopenpresso::VirtualWeightSensorConfig::pumpFlowSensor 
"VirtualWeightSensorConfig.pumpFlowSensor" field points to 
\ref libopenpresso::VibroPumpFlowSensor "VibroPumpFlowSensor" 

or

- This field should point to any config whose component implements some interface.
For example see how \ref libopenpresso::PulsePressureControllerConfig::sensor 
"PulsePressureControllerConfig.sensor" field points to any component that implements
\ref libopenpresso::interfaces::PressureSensor "PressureSensor" interface.
In this case you will see `Supported implementation configs` section at the interfaces 
API reference page.

For example, a temperature controller needs a power controller and a sensor:

```cpp
// 1. The sensor (no dependencies)
config.components["temp_sensor"] = libopenpresso::Max31856TemperatureSensorConfig {...};

// 2. The power chain: AC sensor -> pulse device -> power controller
config.components["ac_sensor"]   = libopenpresso::AcZeroCrossSensorConfig {...};
config.components["heater"]      = libopenpresso::PulseControlledDeviceConfig {
    .pulsePin = {...},
    .acZeroCrossSensor = "ac_sensor",     // References #2
};
config.components["heater_pwr"]  = libopenpresso::PulsePowerControllerConfig {
    .pulseControlledDevice = "heater",    // References #3
    .dutyCycle = 64,
};

// 3. The controller (references both chains)
config.components["temp_ctrl"]   = libopenpresso::TemperaturePidControllerConfig {
    .powerController = "heater_pwr",      // References power chain
    .sensor = "temp_sensor",              // References sensor
    ...
};
```

@note The order in which you insert entries into the components map does **not** matter.
The Core resolves dependencies at access time, not at config construction time.

---

## Component overview

Here we will show which components config you probably should start with.
For detailed description of all the fields of each component and their dependency 
components see \ref component_config.hpp "full API reference".

### High-Level Automation

These are the most aggregating components — they depend on multiple controllers 
and sensors to orchestrate complete machine operations.

#### BrewProfiler

This is the main component you should configure to pull espresso shots
with automatically executed user-defined brewing profiles. Temperature 
isn't controlled by this component, though [temperature controller](#temperature-controller-config) 
should be configured and created separately. 

This component can be skipped if only classic fixed-pressure brewing routing
is required. Start with [pressure controller config](#pressure-controller-config) in this case.

```cpp
config.components["brew_profiler"] = libopenpresso::BrewProfilerConfig {
    .updatePeriod = 50ms,
    .pressureController = "pressure_ctrl",  
    .flowController     = "flow_ctrl",
    .valveController    = "group_valve",
    .weightSensor       = "scale",
};
```

Retrieved via: `core->getBrewProfiler("brew_profiler")`

For brew step definitions, see \ref libopenpresso::brew_step_targets and 
\ref libopenpresso::brew_step_advance_conditions in \ref brew_steps_data.hpp.
If the device doesn't have a real weight sensor in the drip tray, 
\ref libopenpresso::VirtualWeightSensorConfig "virtual weight sensor" can be used. 

#### SteamControllerConfig

This component manages steam heating with automatic boiler refilling to compensate water loss.
It also allows to use two different PID controllers with different tuning for idle state and
active steaming.
If there is no need for boiler refill during steaming and single temperature controller
can be used, this component can be skipped.

```cpp
config.components["steam_ctrl"] = libopenpresso::SteamControllerConfig {
    .steamTemperature = 155'000,             // 155°C in millidegrees
    .pressureThreshold = 2'500,              // Disable refill above ~2.5 bar
    .temperatureThreshold = 145'000,         // Disable refill below 145°C
    .refillFlow = 500,                       // Refill at 500 mg/s
    .refillUpdatePeriod = 250ms,
    .preheatController           = "steam_preheat_temp_ctrl",  
    .steamingTemperatureController = "steam_active_temp_ctrl",
    .temperatureSensor           = "temp_sensor",             
    .pressureSensor              = "press_sensor",          
    .flowRateController          = "virt_flow_ctrl",
};
```

Retrieved via: `core->getSteamController("steam_ctrl")`

---

### Controllers

Controllers implement closed-loop regulation of physical parameters.

#### TemperaturePidControllerConfig {#temperature-controller-config}

PID-based temperature controller with feedforward compensation for cold water input.
Flow counter is highly recommended for espresso brewing scenarios to achieve 
better thermal stability, but usually not needed for steaming. 
Take note that all the feedforward-related PID settings should be set to 0 if no flow counter provided.
You can create several controllers with different PID tuning for various scenarios, like
brewing, steam preheat and active steaming. But if several temperature controllers share one 
underlying `powerController`, only one of them can be active at the same time.

```cpp
config.components["temp_ctrl"] = libopenpresso::TemperaturePidControllerConfig {
    .powerController = "heater_pwr",       
    .sensor          = "temp_sensor",      
    .pidSettings = {
        .p = 0.07f, .d = 0.25f,
        .dTermRelax = 0.025f, .dFilterTime = 750ms,
        .i = 0.0025f, .iTermRelax = 1.0f,
        .f = 0.04f, .w = 0.002f, .wDecay = 0.02f,
    },
    .enablePidStateDump = true,           
    .flowCounter = "flow_sensor",         
};
```

Retrieved via: `core->getTemperatureController("temp_ctrl")`

The \ref libopenpresso::PidSettings "PidSettings" struct contains tuning coefficients 
for the PID + feedforward control loop. For detailed explanation of each coefficient 
and tuning methodology, see the [PID Tuning Guide](pid_tuning.md).

#### PulsePressureControllerConfig {#pressure-controller-config}

Simple comparator-based pressure controller for pulse-controlled pumps.
Use it to run pump at fixed pressure if you don't use brew profiler.

```cpp
config.components["pressure_ctrl"] = libopenpresso::PulsePressureControllerConfig {
    .pulseController = "pump",            
    .sensor          = "press_sensor",    
};
```

Retrieved via: `core->getPressureController("pressure_ctrl")`

---

### Sensors

Sensors can be used to display graphs or instantaneous values
on the UI, indicate machine status with LEDs on front panel or
log brewing process to database. They are also used by controllers
to compare actual parameter value against set target.

#### Pressure: Ads1115PressureSensorConfig

16-bit I2C ADC reading a pressure transducer.

```cpp
config.components["press_sensor"] = libopenpresso::Ads1115PressureSensorConfig {
    .addr      = {.bus = "/dev/i2c-1", .dev = 0x48},
    .signalPin = {.chip = "/dev/gpiochip0", .pin = 4}, 
};
```

Retrieved via: `core->getPressureSensor("press_sensor")`

#### Temperature Sensors

Two thermocouple amplifier ICs are supported:

**Max6675TemperatureSensorConfig** (SPI, K-type only):
```cpp
config.components["temp_sensor"] = libopenpresso::Max6675TemperatureSensorConfig {
    .spiDev = "/dev/spidev0.0",
    .watchdogMinValidValue = 5'000,
    .watchdogMaxValidValue = 165'000
};
```

**Max31856TemperatureSensorConfig** (SPI, multiple thermocouple types):
```cpp
config.components["temp_sensor"] = libopenpresso::Max31856TemperatureSensorConfig {
    .spiDev    = "/dev/spidev0.0",
    .signalPin = {.chip = "/dev/gpiochip0", .pin = 25},
    .watchdogMinValidValue = 5'000,
    .watchdogMaxValidValue = 165'000
};
```

Both retrieved via: `core->getTemperatureSensor("temp_sensor")`

#### Weight Sensors

**Nau7802WeightSensorConfig** — physical load cell via I2C:
```cpp
config.components["scale"] = libopenpresso::Nau7802WeightSensorConfig {
    .addr                  = {.bus = "/dev/i2c-1", .dev = 0x2a},
    .signalPin             = {.chip = "/dev/gpiochip0", .pin = 12},
    .scale                 = 27762,      
    .flowRateSmoothingTime = 600ms,
};
```

**VirtualWeightSensorConfig** — estimated from pump flow data:
```cpp
config.components["vrt_weight"] = libopenpresso::VirtualWeightSensorConfig {
    .pumpFlowSensor        = "flow_sensor",   // -> VibroPumpFlowSensor
    .flowRateSmoothingTime = 150ms,
};
```

Both retrieved via: `core->getWeightSensor("label")`

---

### Logical Inputs & Outputs

#### LogicalInputPinConfig

Debounced digital input for buttons and switches.

```cpp
config.components["brew_btn"] = libopenpresso::LogicalInputPinConfig {
    .addr           = {.chip = "/dev/gpiochip0", .pin = 19},
    .debouncePeriod = 10ms,
    .pull           = libopenpresso::PinPull::PullUp,
    .inverted       = false,            
};
```

Retrieved via: `core->getLogicalInput("brew_btn")`

#### LogicalOutputPinConfig

Digital output for LEDs, relays, solenoid valves.

```cpp
config.components["power_led"] = libopenpresso::LogicalOutputPinConfig {
    .addr      = {.chip = "/dev/gpiochip0", .pin = 21},
    .initState = false,               
    .inverted  = false,
};
```

Retrieved via: `core->getLogicalOutput("power_led")`

---

## Watchdog Configuration

The \ref libopenpresso::WatchdogConfig "WatchdogConfig" is an **optional global setting** 
on \ref libopenpresso::DeviceConfig "DeviceConfig" — it is not a part of the components map.

```cpp
libopenpresso::DeviceConfig config;
config.components = {...};
config.watchdog = libopenpresso::WatchdogConfig {
    .watchdogDev = "/dev/watchdog0",
    .timeout     = 1s,      
};
```

The watchdog triggers a hardware reset if the system becomes unresponsive. 
The Core validates that the actual hardware timeout does not exceed requested `timeout` — 
if it does, an exception is thrown during initialization.

@note Despite this field is optional, leaving it unset is **not recommended for production**.

---

## Complete Example

Minimal configuration with support of the automated brew profiling,
brew stop by weight and advanced steam control:

```cpp
#include <libopenpresso/libopenpresso.hpp>

using namespace std::chrono_literals;

// Hardware addresses
static const libopenpresso::unix_dev_addr_t i2cBus   = "/dev/i2c-1";
static const libopenpresso::unix_dev_addr_t gpioChip = "/dev/gpiochip0";

libopenpresso::DeviceConfig config;
config.components = {
    // --- Sensors ---
    { "press_sensor", libopenpresso::Ads1115PressureSensorConfig {
        .addr = {.bus = i2cBus, .dev = 0x48},
        .signalPin = {.chip = gpioChip, .pin = 4},
    }},

    { "temp_sensor", libopenpresso::Max31856TemperatureSensorConfig {
        .spiDev = "/dev/spidev0.0",
        .signalPin = {.chip = gpioChip, .pin = 25},
        .watchdogMinValidValue = 5'000,
        .watchdogMaxValidValue = 165'000
    }},

    { "ac_sensor", libopenpresso::AcZeroCrossSensorConfig {
        .signalPin = {.chip = gpioChip, .pin = 17},
    }},

    { "weight_sensor", libopenpresso::Nau7802WeightSensorConfig {
        .addr = {.bus = i2cBus, .dev = 0x2a}, 
        .signalPin = {.chip = gpioChip, .pin = 12},
        .scale = 27762,
        .flowRateSmoothingTime = 600ms,
    }},

    { "brew_btn", libopenpresso::LogicalInputPinConfig {
        .addr = {.chip = gpioChip, .pin = 19},
        .debouncePeriod = 10ms,
        .pull = libopenpresso::PinPull::PullUp,
        .inverted = true,
    }},

    { "internal_flow", libopenpresso::VibroPumpFlowSensor {
        .pressureSensor = "press_sensor",
        .pumpPulseController = "pump",
        .volumePerPulse = 205'000,
        .pumpStallPressure = 18'500,
    }},

    // --- Pump power controller ---

    { "pump", libopenpresso::PulseControlledDeviceConfig {
        .pulsePin = {.chip = gpioChip, .pin = 27},
        .acZeroCrossSensor = "ac_sensor",
    }},

    { "pump_pwr", libopenpresso::PulsePowerControllerConfig {
        .pulseControlledDevice = "pump",
        .dutyCycle = 16,
    }},

    // --- Heater power controller ---

    { "heater", libopenpresso::PulseControlledDeviceConfig {
        .pulsePin = {.chip = gpioChip, .pin = 23},
        .acZeroCrossSensor = "ac_sensor",
    }},

    { "heater_pwr", libopenpresso::PulsePowerControllerConfig {
        .pulseControlledDevice = "heater",
        .dutyCycle = 64,
    }},

    // --- Per-parameter controllers  ---

    { "pressure_ctrl", libopenpresso::PulsePressureControllerConfig {
        .pulseController = "pump",
        .sensor = "press_sensor",
    }},

    { "temp_ctrl", libopenpresso::TemperaturePidControllerConfig {
        .powerController = "heater_pwr",
        .sensor = "temp_sensor",
        .pidSettings = {
            .p = 0.07f, .d = 0.25f,
            .dTermRelax = 0.025f, .dFilterTime = 750ms,
            .i = 0.0025f, .iTermRelax = 1.0f,
            .f = 0.04f, .w = 0.002f, .wDecay = 0.02f,
        },
        .enablePidStateDump = false,
        .flowCounter = "internal_flow"
    }},

    { "internal_flow_controller", libopenpresso::VibroPumpFlowController {
        .pumpFlowSensor = "internal_flow",
    }},

    { "output_flow_controller", libopenpresso::IntegralFlowRateControllerConfig {
        .powerController = "pump_pwr",
        .sensor = "weight_sensor",
        .feedbackCoef = 0.00005f,
    }},

    { "valve", libopenpresso::LogicalOutputPinConfig {
        .addr = {.chip = gpioChip, .pin = 21},
        .initState = false,
    }},

    // --- Automation controllers ---

    { "brew_controller", libopenpresso::BrewProfilerConfig {
        .updatePeriod = 50ms,
        .pressureController = "pressure_ctrl",
        .flowController = "output_flow_controller",
        .valveController = "valve",
        .weightSensor = "weight_sensor",
    }},

    { "steam_controller", libopenpresso::SteamControllerConfig {
        .steamTemperature = 155'000,
        .pressureThreshold = 2'500, .temperatureThreshold = 145'000,
        .refillFlow = 500, .refillUpdatePeriod = 250ms,
        .preheatController = "temp_ctrl", .steamingTemperatureController = "temp_ctrl",
        .temperatureSensor = "temp_sensor",
        .pressureSensor = "press_sensor",
        .flowRateController = "internal_flow_controller",
    }},
};

config.watchdog = libopenpresso::WatchdogConfig {
    .watchdogDev = "/dev/watchdog0",
    .timeout = 1s,
};

// Initialize the Core
auto core = libopenpresso::getCore(config);
```

---

## See Also

- [API Usage Guide](usage.md) — working with Core APIs after initialization
- [PID Tuning Guide](pid_tuning.md) — detailed PID coefficient tuning for temperature controllers
- [Internal flow sensor tuning guide](flow_sensor_tuning.md) — finding coefficients to correctly calculate a flow from vibro-pump outlet
- \ref libopenpresso::component_config_t "component_config_t" — full variant type listing
- \ref libopenpresso::interfaces::LibopenpressoCore "LibopenpressoCore" — all component getter methods
