function(libopenpresso_target_compile_warnings TARGET_NAME)
    target_compile_options(${TARGET_NAME} PRIVATE -Wall -Wextra -Wpedantic)
endfunction()

function(libopenpresso_private_headers_base_dir)
    target_include_directories(libopenpresso PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
endfunction()


function(libopenpresso_sources OBJECT_NAME)
    set(options)
    set(oneValueArgs)
    set(multiValueArgs HEADERS SOURCES)

    cmake_parse_arguments(OPENPRESSO_MODULE "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    target_sources(libopenpresso PRIVATE ${OPENPRESSO_MODULE_SOURCES} ${OPENPRESSO_MODULE_HEADERS})
endfunction()

function(libopenpresso_install)
    include(GNUInstallDirs)
    install(TARGETS libopenpresso
        EXPORT libopenpressoTargets
        ARCHIVE
        FILE_SET HEADERS
        COMPONENT Development)

    install(
        EXPORT libopenpressoTargets
        NAMESPACE libopenpresso::
        FILE libopenpressoTargets.cmake
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/libopenpresso)

    include(CMakePackageConfigHelpers)
        configure_package_config_file(cmake/libopenpressoConfig.cmake.in
        "${CMAKE_CURRENT_BINARY_DIR}/libopenpressoConfig.cmake"
        INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/libopenpresso)

    write_basic_package_version_file(
        "${CMAKE_CURRENT_BINARY_DIR}/libopenpressoConfigVersion.cmake"
        COMPATIBILITY ExactVersion)

    install(FILES
        "${CMAKE_CURRENT_BINARY_DIR}/libopenpressoConfig.cmake"
        "${CMAKE_CURRENT_BINARY_DIR}/libopenpressoConfigVersion.cmake"
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/libopenpresso)
endfunction()



