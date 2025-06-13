cmake_minimum_required(VERSION 3.25.1)

project(third_party VERSION 0.1)

find_package(OpenSSL)
if(NOT OpenSSL_FOUND)
    set(OPENSSL_SOURCE_DIR ${PROJECT_SOURCE_DIR}/openssl)
    set(OPENSSL_BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR}/openssl)
    set(OPENSSL_INSTALL_DIR ${CMAKE_SOURCE_DIR}/install/OpenSSL)
    set(OPENSSL_SSL_DIR ${OPENSSL_INSTALL_DIR}/ssl)
    if(WIN32)
        set(CONFIGURE_COMMAND perl ${OPENSSL_SOURCE_DIR}/Configure -static)
        if(MSVC)
            execute_process(COMMAND "C:/Program Files (x86)/Microsoft Visual Studio/Installer/vswhere.exe"
                -format value -property installationPath
                OUTPUT_VARIABLE VS_INSTALLATION_PATH
                OUTPUT_STRIP_TRAILING_WHITESPACE
                COMMAND_ERROR_IS_FATAL ANY
            )
            set(MAKE_COMMAND ${VS_INSTALLATION_PATH}/VC/Auxiliary/Build/vcvarsall.bat x64 && nmake)
        else()
            message(FATAL_ERROR "Compiler not supported")
        endif()
        link_libraries(ws2_32.lib gdi32.lib advapi32.lib crypt32.lib user32.lib)
    else()
        set(CONFIGURE_COMMAND ${OPENSSL_SOURCE_DIR}/Configure)
        set(MAKE_COMMAND make)
    endif()

    if(NOT EXISTS ${OPENSSL_SOURCE_DIR}/Configure)
        execute_process(COMMAND git submodule update
                --init --recursive third_party/openssl
            WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
            COMMAND_ERROR_IS_FATAL ANY
        )
    endif()
    if(NOT EXISTS ${OPENSSL_BINARY_DIR}/makefile)
        make_directory(${OPENSSL_BINARY_DIR})
        execute_process(COMMAND ${CONFIGURE_COMMAND}
                --prefix=${OPENSSL_INSTALL_DIR} --openssldir=${OPENSSL_SSL_DIR} no-apps no-docs
            WORKING_DIRECTORY ${OPENSSL_BINARY_DIR}
            COMMAND_ERROR_IS_FATAL ANY
        )
    endif()
    if(NOT EXISTS ${OPENSSL_BINARY_DIR}/OpenSSLConfig.cmake)
        execute_process(COMMAND ${MAKE_COMMAND}
            WORKING_DIRECTORY ${OPENSSL_BINARY_DIR}
            COMMAND_ERROR_IS_FATAL ANY
        )
    endif()
    if(NOT EXISTS ${OPENSSL_INSTALL_DIR})
        execute_process(COMMAND ${MAKE_COMMAND} install
            WORKING_DIRECTORY ${OPENSSL_BINARY_DIR}
            COMMAND_ERROR_IS_FATAL ANY
        )
    endif()
    set(OPENSSL_ROOT_DIR ${OPENSSL_INSTALL_DIR})
endif()

find_package(Boost CONFIG)
if(NOT Boost_FOUND)
    if(NOT EXISTS ${PROJECT_SOURCE_DIR}/boost/CMakeLists.txt)
        execute_process(COMMAND git submodule update
                --init --recursive third_party/boost
            WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
            COMMAND_ERROR_IS_FATAL ANY
        )
    endif()
    add_subdirectory(boost)
endif()
