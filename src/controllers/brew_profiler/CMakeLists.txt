libopenpresso_sources(brew_profiler
    HEADERS
        brew_profiler_impl.hpp
        const_param_executor.hpp
        sensor_condition_checker.hpp
        ureachable_condition_checker.hpp
        shared_time_condition_checker.hpp
        time_condition_checker.hpp
    SOURCES brew_profiler_impl.cpp)