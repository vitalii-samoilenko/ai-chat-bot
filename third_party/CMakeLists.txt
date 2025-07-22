cmake_minimum_required(VERSION 3.25.1)

project(third_party VERSION 0.1)

set(CMAKE_CXX_STANDARD 17)

if(CMAKE_SIZEOF_VOID_P EQUAL 4)
  set(PLATFORM_PREFIX x32)
else()
  set(PLATFORM_PREFIX x64)
endif()

block()
  set(ZLIB_SOURCE_DIR ${PROJECT_SOURCE_DIR}/zlib)
  set(ZLIB_BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR}/zlib)
  if(NOT EXISTS ${ZLIB_SOURCE_DIR}/CMakeLists.txt)
    execute_process(COMMAND git submodule update
        --init third_party/zlib
      WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
      COMMAND_ERROR_IS_FATAL ANY
    )
    execute_process(COMMAND git checkout v1.3.1
      WORKING_DIRECTORY ${ZLIB_SOURCE_DIR}
      COMMAND_ERROR_IS_FATAL ANY
    )
  endif()
  if(NOT EXISTS ${ZLIB_BINARY_DIR}/configure.done)
    execute_process(COMMAND cmake
        -DCMAKE_CXX_STANDARD=${CMAKE_CXX_STANDARD}
        -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
        -DCMAKE_INSTALL_PREFIX=${CMAKE_INSTALL_PREFIX}
        -DCMAKE_MSVC_RUNTIME_LIBRARY=${CMAKE_MSVC_RUNTIME_LIBRARY}
        -DZLIB_BUILD_EXAMPLES=OFF
        -B ${ZLIB_BINARY_DIR} -S .
      WORKING_DIRECTORY ${ZLIB_SOURCE_DIR}
      COMMAND_ERROR_IS_FATAL ANY
    )
    execute_process(COMMAND cmake
        -E touch configure.done
      WORKING_DIRECTORY ${ZLIB_BINARY_DIR}
      COMMAND_ERROR_IS_FATAL ANY
    )
  endif()
  if(NOT EXISTS ${ZLIB_BINARY_DIR}/build.done)
    execute_process(COMMAND cmake
        --build . --config ${CMAKE_BUILD_TYPE}
      WORKING_DIRECTORY ${ZLIB_BINARY_DIR}
      COMMAND_ERROR_IS_FATAL ANY
    )
    execute_process(COMMAND cmake
        -E touch build.done
      WORKING_DIRECTORY ${ZLIB_BINARY_DIR}
      COMMAND_ERROR_IS_FATAL ANY
    )
  endif()
  if(NOT EXISTS ${ZLIB_BINARY_DIR}/install.done)
    execute_process(COMMAND cmake
        --install . --config ${CMAKE_BUILD_TYPE} --prefix ${CMAKE_INSTALL_PREFIX}
      WORKING_DIRECTORY ${ZLIB_BINARY_DIR}
      COMMAND_ERROR_IS_FATAL ANY
    )
    execute_process(COMMAND cmake
        -E touch install.done
      WORKING_DIRECTORY ${ZLIB_BINARY_DIR}
      COMMAND_ERROR_IS_FATAL ANY
    )
  endif()
  set(ZLIB_USE_STATIC_LIBS ON CACHE INTERNAL "zlib use static libraries")
endblock()

block()
  set(CARES_SOURCE_DIR ${PROJECT_SOURCE_DIR}/cares)
  set(CARES_BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR}/cares)
  if(NOT EXISTS ${CARES_SOURCE_DIR}/CMakeLists.txt)
    execute_process(COMMAND git submodule update
        --init third_party/cares
      WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
      COMMAND_ERROR_IS_FATAL ANY
    )
    execute_process(COMMAND git checkout v1.34.5
      WORKING_DIRECTORY ${CARES_SOURCE_DIR}
      COMMAND_ERROR_IS_FATAL ANY
    )
  endif()
  if(NOT EXISTS ${CARES_BINARY_DIR}/configure.done)
    execute_process(COMMAND cmake
        -DCMAKE_CXX_STANDARD=${CMAKE_CXX_STANDARD}
        -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
        -DCMAKE_INSTALL_PREFIX=${CMAKE_INSTALL_PREFIX}
        -DCARES_STATIC=ON
        -DCARES_SHARED=OFF
        -DCARES_BUILD_TOOLS=OFF
        -DCARES_THREADS=OFF
        -DCARES_MSVC_STATIC_RUNTIME=ON
        -B ${CARES_BINARY_DIR} -S .
      WORKING_DIRECTORY ${CARES_SOURCE_DIR}
      COMMAND_ERROR_IS_FATAL ANY
    )
    execute_process(COMMAND cmake
        -E touch configure.done
      WORKING_DIRECTORY ${CARES_BINARY_DIR}
      COMMAND_ERROR_IS_FATAL ANY
    )
  endif()
  if(NOT EXISTS ${CARES_BINARY_DIR}/build.done)
    execute_process(COMMAND cmake
        --build . --config ${CMAKE_BUILD_TYPE}
      WORKING_DIRECTORY ${CARES_BINARY_DIR}
      COMMAND_ERROR_IS_FATAL ANY
    )
    execute_process(COMMAND cmake
        -E touch build.done
      WORKING_DIRECTORY ${CARES_BINARY_DIR}
      COMMAND_ERROR_IS_FATAL ANY
    )
  endif()
  if(NOT EXISTS ${CARES_BINARY_DIR}/install.done)
    execute_process(COMMAND cmake
        --install . --config ${CMAKE_BUILD_TYPE} --prefix ${CMAKE_INSTALL_PREFIX}
      WORKING_DIRECTORY ${CARES_BINARY_DIR}
      COMMAND_ERROR_IS_FATAL ANY
    )
    execute_process(COMMAND cmake
        -E touch install.done
      WORKING_DIRECTORY ${CARES_BINARY_DIR}
      COMMAND_ERROR_IS_FATAL ANY
    )
  endif()
endblock()

block()
  set(ABSEIL_SOURCE_DIR ${PROJECT_SOURCE_DIR}/abseil)
  set(ABSEIL_BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR}/abseil)
  if(NOT EXISTS ${ABSEIL_SOURCE_DIR}/CMakeLists.txt)
    execute_process(COMMAND git submodule update
        --init third_party/abseil
      WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
      COMMAND_ERROR_IS_FATAL ANY
    )
    execute_process(COMMAND git checkout 20250512.1
      WORKING_DIRECTORY ${ABSEIL_SOURCE_DIR}
      COMMAND_ERROR_IS_FATAL ANY
    )
  endif()
  if(NOT EXISTS ${ABSEIL_BINARY_DIR}/configure.done)
    execute_process(COMMAND cmake
        -DCMAKE_CXX_STANDARD=${CMAKE_CXX_STANDARD}
        -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
        -DCMAKE_INSTALL_PREFIX=${CMAKE_INSTALL_PREFIX}
        -DABSL_ENABLE_INSTALL=ON
        -DABSL_MSVC_STATIC_RUNTIME=ON
        -B ${ABSEIL_BINARY_DIR} -S .
      WORKING_DIRECTORY ${ABSEIL_SOURCE_DIR}
      COMMAND_ERROR_IS_FATAL ANY
    )
    execute_process(COMMAND cmake
        -E touch configure.done
      WORKING_DIRECTORY ${ABSEIL_BINARY_DIR}
      COMMAND_ERROR_IS_FATAL ANY
    )
  endif()
  if(NOT EXISTS ${ABSEIL_BINARY_DIR}/build.done)
    execute_process(COMMAND cmake
        --build . --config ${CMAKE_BUILD_TYPE}
      WORKING_DIRECTORY ${ABSEIL_BINARY_DIR}
      COMMAND_ERROR_IS_FATAL ANY
    )
    execute_process(COMMAND cmake
        -E touch build.done
      WORKING_DIRECTORY ${ABSEIL_BINARY_DIR}
      COMMAND_ERROR_IS_FATAL ANY
    )
  endif()
  if(NOT EXISTS ${ABSEIL_BINARY_DIR}/install.done)
    execute_process(COMMAND cmake
        --install . --config ${CMAKE_BUILD_TYPE} --prefix ${CMAKE_INSTALL_PREFIX}
      WORKING_DIRECTORY ${ABSEIL_BINARY_DIR}
      COMMAND_ERROR_IS_FATAL ANY
    )
    execute_process(COMMAND cmake
        -E touch install.done
      WORKING_DIRECTORY ${ABSEIL_BINARY_DIR}
      COMMAND_ERROR_IS_FATAL ANY
    )
  endif()
endblock()

block()
  set(OPENSSL_SOURCE_DIR ${PROJECT_SOURCE_DIR}/openssl)
  set(OPENSSL_BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR}/openssl)
  if(WIN32)
    set(CONFIGURE_COMMAND perl ${OPENSSL_SOURCE_DIR}/Configure)
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
        --init third_party/openssl
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        COMMAND_ERROR_IS_FATAL ANY
    )
    execute_process(COMMAND git checkout openssl-3.5.1
      WORKING_DIRECTORY ${OPENSSL_SOURCE_DIR}
      COMMAND_ERROR_IS_FATAL ANY
    )
  endif()
  if(NOT EXISTS ${OPENSSL_BINARY_DIR}/configure.done)
    if(${CMAKE_BUILD_TYPE} STREQUAL Debug)
      set(_DEBUG_SUFFIX d)
      set(_DEBUG_FLAG --debug)
    else()
      set(_DEBUG_PREFIX n)
    endif()
    if(MSVC)
      set(_MSVC_STATIC_RUNTIME /MT${_DEBUG_SUFFIX})
    endif()
    find_library(_ZLIB_LIBRARY
      REQUIRED
      NAMES zlibstatic${_DEBUG_SUFFIX} zlibstat${_DEBUG_SUFFIX} zs${_DEBUG_SUFFIX} zlib${_DEBUG_SUFFIX} z${_DEBUG_SUFFIX}
      PATHS ${CMAKE_INSTALL_PREFIX}/lib
      NO_DEFAULT_PATH
      NO_CACHE
    )
    make_directory(${OPENSSL_BINARY_DIR})
    execute_process(COMMAND ${CONFIGURE_COMMAND}
        -static
        --prefix=${CMAKE_INSTALL_PREFIX}
        --openssldir=${CMAKE_INSTALL_PREFIX}/ssl
        ${_DEBUG_FLAG}
        zlib
        --with-zlib-include=${CMAKE_INSTALL_PREFIX}/include
        --with-zlib-lib=${_ZLIB_LIBRARY}
        ${_MSVC_STATIC_RUNTIME}
        no-apps
        no-docs
        no-weak-ssl-ciphers
        no-demos
        no-h3demo
        no-tests
        no-unit-test
        no-external-tests
        no-legacy
        no-tls-deprecated-ec
      WORKING_DIRECTORY ${OPENSSL_BINARY_DIR}
      COMMAND_ERROR_IS_FATAL ANY
    )
    execute_process(COMMAND cmake
        -E touch configure.done
      WORKING_DIRECTORY ${OPENSSL_BINARY_DIR}
      COMMAND_ERROR_IS_FATAL ANY
    )
  endif()
  if(NOT EXISTS ${OPENSSL_BINARY_DIR}/build.done)
    execute_process(COMMAND ${MAKE_COMMAND}
      WORKING_DIRECTORY ${OPENSSL_BINARY_DIR}
      COMMAND_ERROR_IS_FATAL ANY
    )
    execute_process(COMMAND cmake
        -E touch build.done
      WORKING_DIRECTORY ${OPENSSL_BINARY_DIR}
      COMMAND_ERROR_IS_FATAL ANY
    )
  endif()
  if(NOT EXISTS ${OPENSSL_BINARY_DIR}/install.done)
    execute_process(COMMAND ${MAKE_COMMAND} install
      WORKING_DIRECTORY ${OPENSSL_BINARY_DIR}
      COMMAND_ERROR_IS_FATAL ANY
    )
    execute_process(COMMAND cmake
        -E touch install.done
      WORKING_DIRECTORY ${OPENSSL_BINARY_DIR}
      COMMAND_ERROR_IS_FATAL ANY
    )
  endif()
  set(OPENSSL_USE_STATIC_LIBS TRUE CACHE INTERNAL "OpenSSL use static libraries")
  set(OPENSSL_MSVC_STATIC_RT TRUE CACHE INTERNAL "OpenSSL use static MSVC runtime")
endblock()

block()
  set(RE2_SOURCE_DIR ${PROJECT_SOURCE_DIR}/re2)
  set(RE2_BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR}/re2)
  if(NOT EXISTS ${RE2_SOURCE_DIR}/CMakeLists.txt)
    execute_process(COMMAND git submodule update
        --init third_party/re2
      WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
      COMMAND_ERROR_IS_FATAL ANY
    )
    execute_process(COMMAND git checkout 2025-07-17
      WORKING_DIRECTORY ${RE2_SOURCE_DIR}
      COMMAND_ERROR_IS_FATAL ANY
    )
  endif()
  if(NOT EXISTS ${RE2_BINARY_DIR}/configure.done)
    execute_process(COMMAND cmake
        -DCMAKE_CXX_STANDARD=${CMAKE_CXX_STANDARD}
        -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
        -DCMAKE_INSTALL_PREFIX=${CMAKE_INSTALL_PREFIX}
        -DCMAKE_POLICY_DEFAULT_CMP0091=NEW
        -DCMAKE_MSVC_RUNTIME_LIBRARY=${CMAKE_MSVC_RUNTIME_LIBRARY}
        -B ${RE2_BINARY_DIR} -S .
      WORKING_DIRECTORY ${RE2_SOURCE_DIR}
      COMMAND_ERROR_IS_FATAL ANY
    )
    execute_process(COMMAND cmake
        -E touch configure.done
      WORKING_DIRECTORY ${RE2_BINARY_DIR}
      COMMAND_ERROR_IS_FATAL ANY
    )
  endif()
  if(NOT EXISTS ${RE2_BINARY_DIR}/build.done)
    execute_process(COMMAND cmake
        --build . --config ${CMAKE_BUILD_TYPE}
      WORKING_DIRECTORY ${RE2_BINARY_DIR}
      COMMAND_ERROR_IS_FATAL ANY
    )
    execute_process(COMMAND cmake
        -E touch build.done
      WORKING_DIRECTORY ${RE2_BINARY_DIR}
      COMMAND_ERROR_IS_FATAL ANY
    )
  endif()
  if(NOT EXISTS ${RE2_BINARY_DIR}/install.done)
    execute_process(COMMAND cmake
        --install . --config ${CMAKE_BUILD_TYPE} --prefix ${CMAKE_INSTALL_PREFIX}
      WORKING_DIRECTORY ${RE2_BINARY_DIR}
      COMMAND_ERROR_IS_FATAL ANY
    )
    execute_process(COMMAND cmake
        -E touch install.done
      WORKING_DIRECTORY ${RE2_BINARY_DIR}
      COMMAND_ERROR_IS_FATAL ANY
    )
  endif()
endblock()

block()
  set(PROTOBUF_SOURCE_DIR ${PROJECT_SOURCE_DIR}/protobuf)
  set(PROTOBUF_BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR}/protobuf)
  if(NOT EXISTS ${PROTOBUF_SOURCE_DIR}/CMakeLists.txt)
    execute_process(COMMAND git submodule update
        --init third_party/protobuf
      WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
      COMMAND_ERROR_IS_FATAL ANY
    )
    execute_process(COMMAND git checkout v31.0
      WORKING_DIRECTORY ${PROTOBUF_SOURCE_DIR}
      COMMAND_ERROR_IS_FATAL ANY
    )
  endif()
  if(NOT EXISTS ${PROTOBUF_BINARY_DIR}/configure.done)
    execute_process(COMMAND cmake
        -DCMAKE_CXX_STANDARD=${CMAKE_CXX_STANDARD}
        -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
        -DCMAKE_INSTALL_PREFIX=${CMAKE_INSTALL_PREFIX}
        -Dprotobuf_BUILD_TESTS=OFF
        -Dprotobuf_DISABLE_RTTI=ON
        -Dprotobuf_LOCAL_DEPENDENCIES_ONLY=ON
        -DZLIB_USE_STATIC_LIBS=${ZLIB_USE_STATIC_LIBS}
        -B ${PROTOBUF_BINARY_DIR} -S .
      WORKING_DIRECTORY ${PROTOBUF_SOURCE_DIR}
      COMMAND_ERROR_IS_FATAL ANY
    )
    execute_process(COMMAND cmake
        -E touch configure.done
      WORKING_DIRECTORY ${PROTOBUF_BINARY_DIR}
      COMMAND_ERROR_IS_FATAL ANY
    )
  endif()
  if(NOT EXISTS ${PROTOBUF_BINARY_DIR}/build.done)
    execute_process(COMMAND cmake
        --build . --config ${CMAKE_BUILD_TYPE}
      WORKING_DIRECTORY ${PROTOBUF_BINARY_DIR}
      COMMAND_ERROR_IS_FATAL ANY
    )
    execute_process(COMMAND cmake
        -E touch build.done
      WORKING_DIRECTORY ${PROTOBUF_BINARY_DIR}
      COMMAND_ERROR_IS_FATAL ANY
    )
  endif()
  if(NOT EXISTS ${PROTOBUF_BINARY_DIR}/install.done)
    execute_process(COMMAND cmake
        --install . --config ${CMAKE_BUILD_TYPE} --prefix ${CMAKE_INSTALL_PREFIX}
      WORKING_DIRECTORY ${PROTOBUF_BINARY_DIR}
      COMMAND_ERROR_IS_FATAL ANY
    )
    execute_process(COMMAND cmake
        -E touch install.done
      WORKING_DIRECTORY ${PROTOBUF_BINARY_DIR}
      COMMAND_ERROR_IS_FATAL ANY
    )
  endif()
endblock()

block()
  set(GRPC_SOURCE_DIR ${PROJECT_SOURCE_DIR}/grpc)
  set(GRPC_BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR}/grpc)
  if(NOT EXISTS ${GRPC_SOURCE_DIR}/CMakeLists.txt)
    execute_process(COMMAND git submodule update
        --init third_party/grpc
      WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
      COMMAND_ERROR_IS_FATAL ANY
    )
    execute_process(COMMAND git checkout v1.73.1
      WORKING_DIRECTORY ${GRPC_SOURCE_DIR}
      COMMAND_ERROR_IS_FATAL ANY
    )
  endif()
  if(NOT EXISTS ${GRPC_BINARY_DIR}/configure.done)
    execute_process(COMMAND cmake
        -DCMAKE_POLICY_VERSION_MINIMUM=3.5
        -DCMAKE_CXX_STANDARD=${CMAKE_CXX_STANDARD}
        -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
        -DCMAKE_INSTALL_PREFIX=${CMAKE_INSTALL_PREFIX}
        -DCMAKE_POSITION_INDEPENDENT_CODE=FALSE
        -DgRPC_DOWNLOAD_ARCHIVES=OFF
        -DgRPC_INSTALL=ON
        -DgRPC_ZLIB_PROVIDER=package
        -DgRPC_CARES_PROVIDER=package
        -DgRPC_RE2_PROVIDER=package
        -DgRPC_SSL_PROVIDER=package
        -DgRPC_PROTOBUF_PROVIDER=package
        -DgRPC_ABSL_PROVIDER=package
        -DgRPC_MSVC_STATIC_RUNTIME=ON
        -DgRPC_BUILD_GRPC_CSHARP_PLUGIN=OFF
        -DgRPC_BUILD_GRPC_NODE_PLUGIN=OFF
        -DgRPC_BUILD_GRPC_OBJECTIVE_C_PLUGIN=OFF
        -DgRPC_BUILD_GRPC_PHP_PLUGIN=OFF
        -DgRPC_BUILD_GRPC_PYTHON_PLUGIN=OFF
        -DgRPC_BUILD_GRPC_RUBY_PLUGIN=OFF
        -DZLIB_USE_STATIC_LIBS=${ZLIB_USE_STATIC_LIBS}
        -DOPENSSL_USE_STATIC_LIBS=${OPENSSL_USE_STATIC_LIBS}
        -DOPENSSL_MSVC_STATIC_RT=${OPENSSL_MSVC_STATIC_RT}
        -B ${GRPC_BINARY_DIR} -S .
      WORKING_DIRECTORY ${GRPC_SOURCE_DIR}
      COMMAND_ERROR_IS_FATAL ANY
    )
    execute_process(COMMAND cmake
        -E touch configure.done
      WORKING_DIRECTORY ${GRPC_BINARY_DIR}
      COMMAND_ERROR_IS_FATAL ANY
    )
  endif()
  if(NOT EXISTS ${GRPC_BINARY_DIR}/build.done)
    execute_process(COMMAND cmake
        --build . --config ${CMAKE_BUILD_TYPE}
      WORKING_DIRECTORY ${GRPC_BINARY_DIR}
      COMMAND_ERROR_IS_FATAL ANY
    )
    execute_process(COMMAND cmake
        -E touch build.done
      WORKING_DIRECTORY ${GRPC_BINARY_DIR}
      COMMAND_ERROR_IS_FATAL ANY
    )
  endif()
  if(NOT EXISTS ${GRPC_BINARY_DIR}/install.done)
    execute_process(COMMAND cmake
        --install . --config ${CMAKE_BUILD_TYPE} --prefix ${CMAKE_INSTALL_PREFIX}
      WORKING_DIRECTORY ${GRPC_BINARY_DIR}
      COMMAND_ERROR_IS_FATAL ANY
    )
    execute_process(COMMAND cmake
        -E touch install.done
      WORKING_DIRECTORY ${GRPC_BINARY_DIR}
      COMMAND_ERROR_IS_FATAL ANY
    )
  endif()
endblock()

block()
  set(OPENTELEMETRY_PROTO_SOURCE_DIR ${PROJECT_SOURCE_DIR}/opentelemetry-proto)
  if(NOT EXISTS ${OPENTELEMETRY_PROTO_SOURCE_DIR}/Makefile)
    execute_process(COMMAND git submodule update
        --init third_party/opentelemetry-proto
      WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
      COMMAND_ERROR_IS_FATAL ANY
    )
    execute_process(COMMAND git checkout v1.6.0
      WORKING_DIRECTORY ${OPENTELEMETRY_PROTO_SOURCE_DIR}
      COMMAND_ERROR_IS_FATAL ANY
    )
  endif()
  set(OTELCPP_PROTO_PATH ${OPENTELEMETRY_PROTO_SOURCE_DIR} CACHE INTERNAL "OpenTelemetry Proto path")
endblock()

block()
  set(OPENTELEMETRY_SOURCE_DIR ${PROJECT_SOURCE_DIR}/opentelemetry)
  set(OPENTELEMETRY_BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR}/opentelemetry)
  if(NOT EXISTS ${OPENTELEMETRY_SOURCE_DIR}/CMakeLists.txt)
    execute_process(COMMAND git submodule update
        --init third_party/opentelemetry
      WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
      COMMAND_ERROR_IS_FATAL ANY
    )
    execute_process(COMMAND git checkout v1.21.0
      WORKING_DIRECTORY ${OPENTELEMETRY_SOURCE_DIR}
      COMMAND_ERROR_IS_FATAL ANY
    )
  endif()
  if(NOT EXISTS ${OPENTELEMETRY_BINARY_DIR}/configure.done)
    execute_process(COMMAND cmake
        -DCMAKE_CXX_STANDARD=${CMAKE_CXX_STANDARD}
        -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
        -DCMAKE_INSTALL_PREFIX=${CMAKE_INSTALL_PREFIX}
        -DCMAKE_MSVC_RUNTIME_LIBRARY=${CMAKE_MSVC_RUNTIME_LIBRARY}
        -DBUILD_SHARED_LIBS=OFF
        -DWITH_ABI_VERSION_1=OFF
        -DWITH_ABI_VERSION_2=ON
        -DWITH_NO_DEPRECATED_CODE=ON
        -DOPENTELEMETRY_INSTALL=ON
        -DWITH_OTLP_GRPC=ON
        -DWITH_NO_GETENV=ON
        -DBUILD_TESTING=OFF
        -DWITH_BENCHMARK=OFF
        -DWITH_ETW=OFF
        -DWITH_EXAMPLES=OFF
        -DWITH_FUNC_TESTS=OFF
        -DZLIB_USE_STATIC_LIBS=${ZLIB_USE_STATIC_LIBS}
        -DOTELCPP_PROTO_PATH=${OTELCPP_PROTO_PATH}
        -B ${OPENTELEMETRY_BINARY_DIR} -S .
      WORKING_DIRECTORY ${OPENTELEMETRY_SOURCE_DIR}
      COMMAND_ERROR_IS_FATAL ANY
    )
    execute_process(COMMAND cmake
        -E touch configure.done
      WORKING_DIRECTORY ${OPENTELEMETRY_BINARY_DIR}
      COMMAND_ERROR_IS_FATAL ANY
    )
  endif()
  if(NOT EXISTS ${OPENTELEMETRY_BINARY_DIR}/build.done)
    execute_process(COMMAND cmake
        --build . --config ${CMAKE_BUILD_TYPE}
      WORKING_DIRECTORY ${OPENTELEMETRY_BINARY_DIR}
      COMMAND_ERROR_IS_FATAL ANY
    )
    execute_process(COMMAND cmake
        -E touch build.done
      WORKING_DIRECTORY ${OPENTELEMETRY_BINARY_DIR}
      COMMAND_ERROR_IS_FATAL ANY
    )
  endif()
  if(NOT EXISTS ${OPENTELEMETRY_BINARY_DIR}/install.done)
    execute_process(COMMAND cmake
        --install . --config ${CMAKE_BUILD_TYPE} --prefix ${CMAKE_INSTALL_PREFIX}
      WORKING_DIRECTORY ${OPENTELEMETRY_BINARY_DIR}
      COMMAND_ERROR_IS_FATAL ANY
    )
    execute_process(COMMAND cmake
        -E touch install.done
      WORKING_DIRECTORY ${OPENTELEMETRY_BINARY_DIR}
      COMMAND_ERROR_IS_FATAL ANY
    )
  endif()
endblock()

block()
  set(BOOST_SOURCE_DIR ${PROJECT_SOURCE_DIR}/boost)
  set(BOOST_BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR}/boost)
  if(NOT EXISTS ${BOOST_SOURCE_DIR}/CMakeLists.txt)
    execute_process(COMMAND git submodule update
        --init --recursive third_party/boost
      WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
      COMMAND_ERROR_IS_FATAL ANY
    )
    execute_process(COMMAND git checkout boost-1.88.0
      WORKING_DIRECTORY ${BOOST_SOURCE_DIR}
      COMMAND_ERROR_IS_FATAL ANY
    )
  endif()
  if(NOT EXISTS ${BOOST_BINARY_DIR}/configure.done)
    execute_process(COMMAND cmake
        -DCMAKE_CXX_STANDARD=${CMAKE_CXX_STANDARD}
        -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
        -DCMAKE_INSTALL_PREFIX=${CMAKE_INSTALL_PREFIX}
        -DCMAKE_MSVC_RUNTIME_LIBRARY=${CMAKE_MSVC_RUNTIME_LIBRARY}
        -DOPENSSL_USE_STATIC_LIBS=${OPENSSL_USE_STATIC_LIBS}
        -DOPENSSL_MSVC_STATIC_RT=${OPENSSL_MSVC_STATIC_RT}
        -DZLIB_USE_STATIC_LIBS=${ZLIB_USE_STATIC_LIBS}
        -B ${BOOST_BINARY_DIR} -S .
      WORKING_DIRECTORY ${BOOST_SOURCE_DIR}
      COMMAND_ERROR_IS_FATAL ANY
    )
    execute_process(COMMAND cmake
        -E touch configure.done
      WORKING_DIRECTORY ${BOOST_BINARY_DIR}
      COMMAND_ERROR_IS_FATAL ANY
    )
  endif()
  if(NOT EXISTS ${BOOST_BINARY_DIR}/build.done)
    execute_process(COMMAND cmake
        --build . --config ${CMAKE_BUILD_TYPE}
      WORKING_DIRECTORY ${BOOST_BINARY_DIR}
      COMMAND_ERROR_IS_FATAL ANY
    )
    execute_process(COMMAND cmake
        -E touch build.done
      WORKING_DIRECTORY ${BOOST_BINARY_DIR}
      COMMAND_ERROR_IS_FATAL ANY
    )
  endif()
  if(NOT EXISTS ${BOOST_BINARY_DIR}/install.done)
    execute_process(COMMAND cmake
        --install . --config ${CMAKE_BUILD_TYPE} --prefix ${CMAKE_INSTALL_PREFIX}
      WORKING_DIRECTORY ${BOOST_BINARY_DIR}
      COMMAND_ERROR_IS_FATAL ANY
    )
    execute_process(COMMAND cmake
        -E touch install.done
      WORKING_DIRECTORY ${BOOST_BINARY_DIR}
      COMMAND_ERROR_IS_FATAL ANY
    )
  endif()
endblock()

block()
  set(EBOOST_SOURCE_DIR ${PROJECT_SOURCE_DIR}/eboost)
  set(EBOOST_BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR}/eboost)
  if(NOT EXISTS ${EBOOST_BINARY_DIR}/configure.done)
    execute_process(COMMAND cmake
        -DCMAKE_CXX_STANDARD=${CMAKE_CXX_STANDARD}
        -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
        -DCMAKE_INSTALL_PREFIX=${CMAKE_INSTALL_PREFIX}
        -B ${EBOOST_BINARY_DIR} -S .
      WORKING_DIRECTORY ${EBOOST_SOURCE_DIR}
      COMMAND_ERROR_IS_FATAL ANY
    )
    execute_process(COMMAND cmake
        -E touch configure.done
      WORKING_DIRECTORY ${EBOOST_BINARY_DIR}
      COMMAND_ERROR_IS_FATAL ANY
    )
  endif()
  if(NOT EXISTS ${EBOOST_BINARY_DIR}/build.done)
    execute_process(COMMAND cmake
        --build . --config ${CMAKE_BUILD_TYPE}
      WORKING_DIRECTORY ${EBOOST_BINARY_DIR}
      COMMAND_ERROR_IS_FATAL ANY
    )
    execute_process(COMMAND cmake
        -E touch build.done
      WORKING_DIRECTORY ${EBOOST_BINARY_DIR}
      COMMAND_ERROR_IS_FATAL ANY
    )
  endif()
  if(NOT EXISTS ${EBOOST_BINARY_DIR}/install.done)
    execute_process(COMMAND cmake
        --install . --config ${CMAKE_BUILD_TYPE} --prefix ${CMAKE_INSTALL_PREFIX}
      WORKING_DIRECTORY ${EBOOST_BINARY_DIR}
      COMMAND_ERROR_IS_FATAL ANY
    )
    execute_process(COMMAND cmake
        -E touch install.done
      WORKING_DIRECTORY ${EBOOST_BINARY_DIR}
      COMMAND_ERROR_IS_FATAL ANY
    )
  endif()
endblock()

block()
  set(SQLITE_SOURCE_DIR ${PROJECT_SOURCE_DIR}/sqlite)
  set(SQLITE_BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR}/sqlite)
  set(SQLITE_INCLUDE_DIR ${CMAKE_INSTALL_PREFIX}/include)
  set(SQLITE_LIBRARY_DIR ${CMAKE_INSTALL_PREFIX}/lib)
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
        --init third_party/sqlite
      WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
      COMMAND_ERROR_IS_FATAL ANY
    )
    execute_process(COMMAND git checkout version-3.50.2
      WORKING_DIRECTORY ${SQLITE_SOURCE_DIR}
      COMMAND_ERROR_IS_FATAL ANY
    )
  endif()
  if(NOT EXISTS ${SQLITE_BINARY_DIR}/configure.done)
    file(COPY ${SQLITE_SOURCE_DIR} DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
    execute_process(COMMAND ${CONFIGURE_COMMAND}
      WORKING_DIRECTORY ${SQLITE_BINARY_DIR}
      COMMAND_ERROR_IS_FATAL ANY
    )
    execute_process(COMMAND cmake
        -E touch configure.done
      WORKING_DIRECTORY ${SQLITE_BINARY_DIR}
      COMMAND_ERROR_IS_FATAL ANY
    )
  endif()
  if(NOT EXISTS ${SQLITE_BINARY_DIR}/build.done)
    execute_process(COMMAND ${MAKE_COMMAND}
        "OPTIONS=-DSQLITE_DQS=0 -DSQLITE_THREADSAFE=0 -DSQLITE_DEFAULT_MEMSTATUS=0 -DSQLITE_DEFAULT_WAL_SYNCHRONOUS=1 -DSQLITE_LIKE_DOESNT_MATCH_BLOBS -DSQLITE_MAX_EXPR_DEPTH=0 -DSQLITE_OMIT_DEPRECATED -DSQLITE_OMIT_PROGRESS_CALLBACK -DSQLITE_OMIT_SHARED_CACHE -DSQLITE_USE_ALLOCA -DSQLITE_OMIT_AUTOINIT -DSQLITE_STRICT_SUBTYPE=1"
      WORKING_DIRECTORY ${SQLITE_BINARY_DIR}
      COMMAND_ERROR_IS_FATAL ANY
    )
    execute_process(COMMAND cmake
        -E touch build.done
      WORKING_DIRECTORY ${SQLITE_BINARY_DIR}
      COMMAND_ERROR_IS_FATAL ANY
    )
  endif()
  if(NOT EXISTS ${SQLITE_BINARY_DIR}/install.done)
    if (WIN32)
      set(_SQLITE_LIBRARY libsqlite3.lib)
    elseif(LINUX)
      set(_SQLITE_LIBRARY libsqlite3.a)
    endif()
    file(COPY ${SQLITE_BINARY_DIR}/${_SQLITE_LIBRARY}
      DESTINATION ${SQLITE_LIBRARY_DIR}
    )
    file(COPY ${SQLITE_BINARY_DIR}/sqlite3.h
      DESTINATION ${SQLITE_INCLUDE_DIR}
    )
    execute_process(COMMAND cmake
        -E touch install.done
      WORKING_DIRECTORY ${SQLITE_BINARY_DIR}
      COMMAND_ERROR_IS_FATAL ANY
    )
  endif()
  set(PC_SQLite3_INCLUDE_DIRS ${SQLITE_INCLUDE_DIR} CACHE INTERNAL "SQLite include directory")
  set(PC_SQLite3_LIBRARY_DIRS ${SQLITE_LIBRARY_DIR} CACHE INTERNAL "SQLite library directory")
endblock()

block()
  set(ESQLITE_SOURCE_DIR ${PROJECT_SOURCE_DIR}/esqlite)
  set(ESQLITE_BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR}/esqlite)
  if(NOT EXISTS ${ESQLITE_BINARY_DIR}/configure.done)
    execute_process(COMMAND cmake
        -DCMAKE_CXX_STANDARD=${CMAKE_CXX_STANDARD}
        -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
        -DCMAKE_INSTALL_PREFIX=${CMAKE_INSTALL_PREFIX}
        -B ${ESQLITE_BINARY_DIR} -S .
      WORKING_DIRECTORY ${ESQLITE_SOURCE_DIR}
      COMMAND_ERROR_IS_FATAL ANY
    )
    execute_process(COMMAND cmake
        -E touch configure.done
      WORKING_DIRECTORY ${ESQLITE_BINARY_DIR}
      COMMAND_ERROR_IS_FATAL ANY
    )
  endif()
  if(NOT EXISTS ${ESQLITE_BINARY_DIR}/build.done)
    execute_process(COMMAND cmake
        --build . --config ${CMAKE_BUILD_TYPE}
      WORKING_DIRECTORY ${ESQLITE_BINARY_DIR}
      COMMAND_ERROR_IS_FATAL ANY
    )
    execute_process(COMMAND cmake
        -E touch build.done
      WORKING_DIRECTORY ${ESQLITE_BINARY_DIR}
      COMMAND_ERROR_IS_FATAL ANY
    )
  endif()
  if(NOT EXISTS ${ESQLITE_BINARY_DIR}/install.done)
    execute_process(COMMAND cmake
        --install . --config ${CMAKE_BUILD_TYPE} --prefix ${CMAKE_INSTALL_PREFIX}
      WORKING_DIRECTORY ${ESQLITE_BINARY_DIR}
      COMMAND_ERROR_IS_FATAL ANY
    )
    execute_process(COMMAND cmake
        -E touch install.done
      WORKING_DIRECTORY ${ESQLITE_BINARY_DIR}
      COMMAND_ERROR_IS_FATAL ANY
    )
  endif()
endblock()
