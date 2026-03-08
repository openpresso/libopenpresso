libopenpresso_sources(watchdog
    HEADERS
        watchdog_thread.hpp
        sensor_value_validator.hpp
        pin_output_validator.hpp
        ac_sensor_validator.hpp
        watchdog_manager.hpp
    SOURCES
        watchdog_thread.cpp
        sensor_value_validator.cpp
        pin_output_validator.cpp
        ac_sensor_validator.cpp
        watchdog_manager.cpp)

