cmake_minimum_required(VERSION 3.30)

set(CACHE LIBOPENPRESSO_VERSION_MAJOR 0)
set(CACHE LIBOPENPRESSO_VERSION_MINOR 0)
set(CACHE LIBOPENPRESSO_VERSION_PATCH 0)
set(CACHE LIBOPENPRESSO_MIN_LOG_LEVEL info)

project(Libopenpresso VERSION ${LIBOPENPRESSO_VERSION_MAJOR}.${LIBOPENPRESSO_VERSION_MINOR}.${LIBOPENPRESSO_VERSION_PATCH})

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_COMPILE_WARNING_AS_ERROR ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_INTERPROCEDURAL_OPTIMIZATION_RELEASE ON)
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

include(libopenpresso)
include(clang-format)
include(clang-tidy)

find_package(Threads REQUIRED)
find_package(spdlog QUIET)
find_package(Boost)

add_library(libopenpresso STATIC)
add_library(libopenpresso::libopenpresso ALIAS libopenpresso)

libopenpresso_target_compile_warnings(libopenpresso)
set_target_properties(libopenpresso PROPERTIES OUTPUT_NAME libopenpresso PREFIX "")
target_link_libraries(libopenpresso PRIVATE Threads::Threads)

if(Boost_FOUND)
  target_link_libraries(libopenpresso PRIVATE $<BUILD_INTERFACE:boost::boost>)
  target_compile_definitions(libopenpresso PRIVATE BOOST_TYPE_ID)
endif()

if(spdlog_FOUND)
  target_link_libraries(libopenpresso PUBLIC spdlog::spdlog)
  target_compile_definitions(libopenpresso PRIVATE LIBOPENPRESSO_MIN_LOG_LEVEL=${LIBOPENPRESSO_MIN_LOG_LEVEL} LIBOPENPRESSO_LOGS_ENABLE)
else()
  message(WARNING "spdlog not found, logging functionality will be disabled")
endif()

add_subdirectory(include)
add_subdirectory(src)

#we don't export docs folder to conan cache
if(IS_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/docs)
  add_subdirectory(docs)
endif()

libopenpresso_install()

add_clang_format_target(clang_format_validate TARGET libopenpresso DRY_RUN)
add_clang_format_target(clang_format_apply TARGET libopenpresso)

add_clang_tidy_target(clang_tidy_validate TARGET libopenpresso ARGS --quiet -p=${CMAKE_BINARY_DIR} --warnings-as-errors=*)
add_clang_tidy_target(clang_tidy_analyze TARGET libopenpresso ARGS --quiet -p=${CMAKE_BINARY_DIR})
