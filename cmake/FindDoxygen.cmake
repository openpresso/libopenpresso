include(FindPackageHandleStandardArgs)

find_program(doxygen_EXECUTABLE doxygen)
find_package_handle_standard_args(Doxygen DEFAULT_MSG doxygen_EXECUTABLE)
add_executable(doxygen::doxygen IMPORTED GLOBAL)
set_property(TARGET doxygen::doxygen PROPERTY IMPORTED_LOCATION ${doxygen_EXECUTABLE})

function(add_doxygen_target target_name)
    set(options ALL)
    set(one_value_args DOXYFILE)
    set(multi_value_args DEPENDS)

    cmake_parse_arguments(DOXYGEN_ARG "${options}" "${one_value_args}" "${multi_value_args}" ${ARGN})

    if(NOT DOXYGEN_ARG_DOXYFILE)
        message(FATAL_ERROR "add_doxygen_target: DOXYFILE is required")
        return()
    endif()

    if(NOT IS_ABSOLUTE "${DOXYGEN_ARG_DOXYFILE}")
        set(DOXYFILE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/${DOXYGEN_ARG_DOXYFILE}")
    else()
        set(DOXYFILE_PATH "${DOXYGEN_ARG_DOXYFILE}")
    endif()

    set(TARGET_KEYWORDS ${target_name})
    if(DOXYGEN_ARG_ALL)
        list(APPEND TARGET_KEYWORDS ALL)
    endif()

    set(STAMP_FILE "${CMAKE_CURRENT_BINARY_DIR}/${target_name}.stamp")

    if(DOXYGEN_ARG_DEPENDS)
        add_custom_command(
            OUTPUT ${STAMP_FILE}
            COMMAND doxygen::doxygen "${DOXYFILE_PATH}"
            COMMAND ${CMAKE_COMMAND} -E touch "${STAMP_FILE}"
            DEPENDS ${DOXYFILE_PATH} ${DOXYGEN_ARG_DEPENDS}
        )

        add_custom_target(${TARGET_KEYWORDS} 
            DEPENDS ${STAMP_FILE})
    else()
        add_custom_target(${TARGET_KEYWORDS}
            COMMAND doxygen::doxygen "${DOXYFILE_PATH}"
            COMMAND ${CMAKE_COMMAND} -E touch "${STAMP_FILE}"
        )
    endif()
    
    set_target_properties(${target_name} PROPERTIES STAMP_FILE "${STAMP_FILE}")
endfunction()