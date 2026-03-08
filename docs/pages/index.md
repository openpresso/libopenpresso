# Libopenpresso

> @doxyconfig PROJECT_BRIEF

## Project overview

Libopenpresso is a C++ library that provides high abstraction-level set of interfaces
for espresso-brewing and steam control.  
It allows to create a software for espresso machine that focuses on user-side logic
while the library takes care about interaction with hardware and handles run-loop 
specific tasks.  
The library is designed to be modular and agnostic about espresso machine architecture.
So you can use it to automate single-boiler home-use-only Gaggia Classic, or any dual-boiler
multi-group-head monster for bussiness.  
Libopenpresso can be easily extended with additional modules to add another implementation of
already exising interfaces, as well as with completely new 
interfaces and their implementations to extend existing functionality with new features.

## Features 

- 🌡️ Temperature PID-control
- ⏲ Pressure PDM-control
- ⚖️ Weigh and flow-rate sensors
- 📈 Extraction with pressure and flow-rate profiling
- 💨 Steam controller with in-process boiler refill
- 🚦 Front-panel LED output and push-buttons input
- 🐶 Watchdog for system health-check control

## Getting started

To start using libopenpresso it's recommended to read
a [quick start guide](quick_start.md) to get an overall understanding
of the libopenpresso usage workflow, and then read more detailed 
tutorials in the following order:

1. [Installation guide](installation.md)
2. [Creation of the device config](device_config.md)
3. [API Usage Guide](usage.md)

Also see advanced topics:
- [PID tuning guide](pid_tuning.md)
- [Internal flow sensor tuning guide](flow_sensor_tuning.md)
- [Cross-compiling guide](crosscompiling.md)