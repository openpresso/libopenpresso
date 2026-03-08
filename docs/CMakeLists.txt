find_package(Doxygen)
if(NOT Doxygen_FOUND)
  message(WARNING "Doxygen not found, skip docs")
  return()
endif()

include(FetchContent)
FetchContent_Declare(doxygen-awesome-css
  URL https://github.com/jothepro/doxygen-awesome-css/archive/refs/heads/main.zip)
FetchContent_MakeAvailable(doxygen-awesome-css)
FetchContent_GetProperties(doxygen-awesome-css SOURCE_DIR AWESOME_CSS_DIR)

set(CACHE LIBOPENPRESSO_VERSION "0.0.0-unknown")
set(CACHE LIBOPENPRESSO_REPO_CHANNEL "testing")

set(DOXY_PAGES 
  index.md
  quick_start.md
  installation.md
  device_config.md
  usage.md
  pid_tuning.md
  flow_sensor_tuning.md
  crosscompiling.md
)

foreach(DOCFILE ${DOXY_PAGES})
  configure_file(pages/${DOCFILE} ${CMAKE_CURRENT_BINARY_DIR}/${DOCFILE} @ONLY)
endforeach()

list(TRANSFORM DOXY_PAGES PREPEND "${CMAKE_CURRENT_BINARY_DIR}/")
list(JOIN DOXY_PAGES " " DOXY_PAGES_STR)
  
set(DOXFILE_OUT ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile)
set(DOXYGEN_INPUT "${PROJECT_SOURCE_DIR}/include ${DOXY_PAGES_STR}")
set(DOXYGEN_HTML_DIR ${CMAKE_CURRENT_BINARY_DIR}/html)
configure_file(Doxyfile.in ${DOXFILE_OUT} @ONLY)

get_target_property(libopenpresso_HEADERS libopenpresso HEADER_SET)

add_doxygen_target(docs 
  DOXYFILE ${DOXFILE_OUT} 
  DEPENDS 
    ${libopenpresso_HEADERS} 
    ${DOXY_PAGES} 
    ${CMAKE_CURRENT_SOURCE_DIR}/header.html 
    ${CMAKE_CURRENT_SOURCE_DIR}/custom.css
    ${CMAKE_CURRENT_SOURCE_DIR}/logo-light.svg
    ${CMAKE_CURRENT_SOURCE_DIR}/logo-dark.svg
)

install(DIRECTORY ${DOXYGEN_HTML_DIR}/
  DESTINATION docs
  COMPONENT Documentation
  EXCLUDE_FROM_ALL)
