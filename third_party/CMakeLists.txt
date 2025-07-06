cmake_minimum_required(VERSION 3.25.1)

project(third_party VERSION 0.1)

find_package(OpenSSL)
if(NOT OpenSSL_FOUND)
    set(OPENSSL_SOURCE_DIR ${PROJECT_SOURCE_DIR}/openssl)
    set(OPENSSL_BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR}/openssl)
    set(OPENSSL_INSTALL_DIR ${CMAKE_SOURCE_DIR}/install/OpenSSL)

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
                --prefix=${OPENSSL_INSTALL_DIR} --openssldir=${OPENSSL_INSTALL_DIR}/ssl no-apps no-docs
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

find_package(gRPC CONFIG)
if(NOT gRPC_FOUND)
    set(GRPC_SOURCE_DIR ${PROJECT_SOURCE_DIR}/grpc)
    set(GRPC_BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR}/grpc)
    set(GRPC_INSTALL_DIR ${CMAKE_SOURCE_DIR}/install/gRPC)

    if(NOT EXISTS ${GRPC_SOURCE_DIR}/CMakeLists.txt)
        execute_process(COMMAND git submodule update
                --init --recursive third_party/grpc
            WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
            COMMAND_ERROR_IS_FATAL ANY
        )
    endif()
    if(NOT EXISTS ${GRPC_BINARY_DIR})
        make_directory(${GRPC_BINARY_DIR})
        execute_process(COMMAND cmake
                -DCMAKE_POLICY_VERSION_MINIMUM=3.5
                -DCMAKE_INSTALL_PREFIX=${GRPC_INSTALL_DIR}
                -Dprotobuf_BUILD_TESTS=OFF
                -B . -S ${GRPC_SOURCE_DIR}
            WORKING_DIRECTORY ${GRPC_BINARY_DIR}
            COMMAND_ERROR_IS_FATAL ANY
        )
    endif()
    if(NOT EXISTS ${GRPC_BINARY_DIR}/Release)
        execute_process(COMMAND cmake
                --build . --config Release
            WORKING_DIRECTORY ${GRPC_BINARY_DIR}
            COMMAND_ERROR_IS_FATAL ANY
        )
    endif()
    if(NOT EXISTS ${GRPC_INSTALL_DIR})
        execute_process(COMMAND cmake
                --install . --config Release
            WORKING_DIRECTORY ${GRPC_BINARY_DIR}
            COMMAND_ERROR_IS_FATAL ANY
        )
    endif()
    set(gRPC_DIR ${GRPC_INSTALL_DIR}/lib/cmake/grpc)
    set(absl_DIR ${GRPC_INSTALL_DIR}/lib/cmake/absl)
    set(utf8_range_DIR ${GRPC_INSTALL_DIR}/lib/cmake/utf8_range)
    set(Protobuf_DIR ${GRPC_INSTALL_DIR}/lib/cmake/protobuf)
endif()

find_package(opentelemetry-cpp CONFIG)
if(NOT opentelemetry-cpp_FOUND)
    if(NOT EXISTS ${PROJECT_SOURCE_DIR}/opentelemetry-cpp/CMakeLists.txt)
        execute_process(COMMAND git submodule update
                --init --recursive third_party/opentelemetry-cpp
            WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
            COMMAND_ERROR_IS_FATAL ANY
        )
    endif()
    set(BUILD_TESTING OFF)
    set(WITH_FUNC_TESTS OFF)
    set(WITH_BENCHMARK OFF)
    set(WITH_EXAMPLES OFF)
    set(WITH_ETW OFF)
    set(WITH_ABI_VERSION_1 OFF)
    set(WITH_ABI_VERSION_2 ON)
    set(WITH_OTLP_GRPC ON)
    add_subdirectory(opentelemetry-cpp)
endif()

find_package(SQLite)
if(NOT SQLite_FOUND)
    set(SQLITE_SOURCE_DIR ${PROJECT_SOURCE_DIR}/sqlite)
    set(SQLITE_BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR}/sqlite)
    set(SQLITE_INSTALL_DIR ${CMAKE_SOURCE_DIR}/install/SQLite)
    if(WIN32)
        set(CONFIGURE_COMMAND cmake -E touch config.log)
        if(MSVC)
            execute_process(COMMAND "C:/Program Files (x86)/Microsoft Visual Studio/Installer/vswhere.exe"
                -format value -property installationPath
                OUTPUT_VARIABLE VS_INSTALLATION_PATH
                OUTPUT_STRIP_TRAILING_WHITESPACE
                COMMAND_ERROR_IS_FATAL ANY
            )
            set(MAKE_COMMAND ${VS_INSTALLATION_PATH}/VC/Auxiliary/Build/vcvarsall.bat x64 && nmake /F Makefile.msc)
        else()
            message(FATAL_ERROR "Compiler not supported")
        endif()
    else()
        set(CONFIGURE_COMMAND ${SQLITE_SOURCE_DIR}/configure)
        set(MAKE_COMMAND make)
    endif()

    if(NOT EXISTS ${SQLITE_SOURCE_DIR}/configure)
        execute_process(COMMAND git submodule update
                --init --recursive third_party/sqlite
            WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
            COMMAND_ERROR_IS_FATAL ANY
        )
    endif()
    if(NOT EXISTS ${SQLITE_BINARY_DIR}/config.log)
        file(COPY ${SQLITE_SOURCE_DIR} DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
        execute_process(COMMAND ${CONFIGURE_COMMAND}
            WORKING_DIRECTORY ${SQLITE_BINARY_DIR}
            COMMAND_ERROR_IS_FATAL ANY
        )
    endif()
    if(NOT EXISTS ${SQLITE_BINARY_DIR}/sqlite3.h)
        execute_process(COMMAND ${MAKE_COMMAND}
            WORKING_DIRECTORY ${SQLITE_BINARY_DIR}
            COMMAND_ERROR_IS_FATAL ANY
        )
    endif()
    if(NOT EXISTS ${SQLITE_INSTALL_DIR})
        file(COPY ${SQLITE_BINARY_DIR}/sqlite3.h DESTINATION ${SQLITE_INSTALL_DIR}/include)
        file(COPY ${SQLITE_BINARY_DIR}/libsqlite3.lib DESTINATION ${SQLITE_INSTALL_DIR}/lib)
    endif()
    add_library(sqlite INTERFACE)
    add_library(SQLite ALIAS sqlite)
    target_include_directories(sqlite INTERFACE ${SQLITE_INSTALL_DIR}/include)
    target_link_libraries(sqlite INTERFACE ${SQLITE_INSTALL_DIR}/lib/libsqlite3.lib)
endif()

add_subdirectory(eboost)
