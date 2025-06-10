cmake_minimum_required(VERSION 3.25.1)

project(libs VERSION 0.1)

block()
    set(OPENSSL_DIR "openssl")
    set(OPENSSL_SOURCE_DIR "${PROJECT_SOURCE_DIR}/${OPENSSL_DIR}")
    set(OPENSSL_BINARY_DIR "${CMAKE_CURRENT_BINARY_DIR}/${OPENSSL_DIR}")
    if(WIN32)
        set(CONFIGURE_COMMAND "perl ${OPENSSL_SOURCE_DIR}/Configure")
        set(MAKE_COMMAND "nmake")
    else()
        set(CONFIGURE_COMMAND "${OPENSSL_SOURCE_DIR}/Configure")
        set(MAKE_COMMAND "make")
    endif()

    make_directory("${OPENSSL_BINARY_DIR}")
    add_custom_target(configure-openssl
        COMMAND ${CONFIGURE_COMMAND}
        WORKING_DIRECTORY "${OPENSSL_BINARY_DIR}"
    )
    add_custom_target(make-openssl
        COMMAND ${MAKE_COMMAND}
        WORKING_DIRECTORY "${OPENSSL_BINARY_DIR}"
        DEPENDS configure-openssl
    )
    add_custom_target(install-openssl
        COMMAND ${MAKE_COMMAND} install
        WORKING_DIRECTORY "${OPENSSL_BINARY_DIR}"
        DEPENDS make-openssl
    )
endblock()