add_subdirectory(interfaces)

target_sources(libopenpresso PUBLIC FILE_SET HEADERS
FILES
    libopenpresso.hpp
    types.hpp
    config.hpp
    component_config.hpp
    i2c_info.hpp
    pin_info.hpp
    pid_settings.hpp
    brew_steps_data.hpp
    exception.hpp
)