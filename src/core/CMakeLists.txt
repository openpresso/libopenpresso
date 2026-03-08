libopenpresso_sources(core 
  HEADERS 
    core.hpp
    components_pins.hpp
    core_private_base.hpp
  SOURCES
    core.cpp
    get_core.cpp
)

add_subdirectory(components_fabric)