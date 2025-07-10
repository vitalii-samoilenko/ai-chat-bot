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
  set(ZLIB_INSTALL_DIR ${CMAKE_INSTALL_PREFIX}/lib/cmake/zlib)
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
  if(NOT EXISTS ${ZLIB_BINARY_DIR})
    make_directory(${ZLIB_BINARY_DIR})
    execute_process(COMMAND cmake
        -DCMAKE_CXX_STANDARD=${CMAKE_CXX_STANDARD}
        -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
        -DCMAKE_INSTALL_PREFIX=${CMAKE_INSTALL_PREFIX}
        -DZLIB_BUILD_EXAMPLES=OFF
        -B . -S ${ZLIB_SOURCE_DIR}
      WORKING_DIRECTORY ${ZLIB_BINARY_DIR}
      COMMAND_ERROR_IS_FATAL ANY
    )
  endif()
  if(NOT EXISTS ${ZLIB_BINARY_DIR}/${PLATFORM_PREFIX}/${CMAKE_BUILD_TYPE})
    execute_process(COMMAND cmake
        --build . --config ${CMAKE_BUILD_TYPE}
      WORKING_DIRECTORY ${ZLIB_BINARY_DIR}
      COMMAND_ERROR_IS_FATAL ANY
    )
  endif()
  if(NOT EXISTS ${ZLIB_INSTALL_DIR})
    execute_process(COMMAND cmake
        --install . --config ${CMAKE_BUILD_TYPE} --prefix ${CMAKE_INSTALL_PREFIX}
      WORKING_DIRECTORY ${ZLIB_BINARY_DIR}
      COMMAND_ERROR_IS_FATAL ANY
    )
  endif()
  set(ZLIB_USE_STATIC_LIBS ON CACHE INTERNAL "zlib use static libraries")
endblock()

# block()
#   set(ABSEIL_SOURCE_DIR ${PROJECT_SOURCE_DIR}/abseil)
#   set(ABSEIL_BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR}/abseil)
#   set(ABSEIL_INSTALL_DIR ${CMAKE_INSTALL_PREFIX}/lib/cmake/absl)
#   if(NOT EXISTS ${ABSEIL_SOURCE_DIR}/CMakeLists.txt)
#     execute_process(COMMAND git submodule update
#         --init third_party/abseil
#       WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
#       COMMAND_ERROR_IS_FATAL ANY
#     )
#   endif()
#   if(NOT EXISTS ${ABSEIL_BINARY_DIR})
#     make_directory(${ABSEIL_BINARY_DIR})
#     execute_process(COMMAND cmake
#         -DCMAKE_CXX_STANDARD=${CMAKE_CXX_STANDARD}
#         -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
#         -DCMAKE_INSTALL_PREFIX=${CMAKE_INSTALL_PREFIX}
#         -DABSL_ENABLE_INSTALL=ON
#         -DABSL_MSVC_STATIC_RUNTIME=ON
#         -B . -S ${ABSEIL_SOURCE_DIR}
#       WORKING_DIRECTORY ${ABSEIL_BINARY_DIR}
#       COMMAND_ERROR_IS_FATAL ANY
#     )
#   endif()
#   if(NOT EXISTS ${ABSEIL_BINARY_DIR}/${PLATFORM_PREFIX}/${CMAKE_BUILD_TYPE})
#     execute_process(COMMAND cmake
#         --build . --config ${CMAKE_BUILD_TYPE}
#       WORKING_DIRECTORY ${ABSEIL_BINARY_DIR}
#       COMMAND_ERROR_IS_FATAL ANY
#     )
#   endif()
#   if(NOT EXISTS ${ABSEIL_INSTALL_DIR})
#     execute_process(COMMAND cmake
#         --install . --config ${CMAKE_BUILD_TYPE} --prefix ${CMAKE_INSTALL_PREFIX}
#       WORKING_DIRECTORY ${ABSEIL_BINARY_DIR}
#       COMMAND_ERROR_IS_FATAL ANY
#     )
#   endif()
# endblock()

# block()
#   set(PROTOBUF_SOURCE_DIR ${PROJECT_SOURCE_DIR}/protobuf)
#   set(PROTOBUF_BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR}/protobuf)
#   set(PROTOBUF_INSTALL_DIR ${CMAKE_INSTALL_PREFIX}/lib/cmake/protobuf)
#   if(NOT EXISTS ${PROTOBUF_SOURCE_DIR}/CMakeLists.txt)
#     execute_process(COMMAND git submodule update
#         --init third_party/protobuf
#       WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
#       COMMAND_ERROR_IS_FATAL ANY
#     )
#   endif()
#   if(NOT EXISTS ${PROTOBUF_BINARY_DIR})
#     make_directory(${PROTOBUF_BINARY_DIR})
#     execute_process(COMMAND cmake
#         -DCMAKE_CXX_STANDARD=${CMAKE_CXX_STANDARD}
#         -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
#         -DCMAKE_INSTALL_PREFIX=${CMAKE_INSTALL_PREFIX}
#         -Dprotobuf_INSTALL=ON
#         -Dprotobuf_BUILD_TESTS=OFF
#         -Dprotobuf_BUILD_PROTOBUF_BINARIES=ON
#         -Dprotobuf_BUILD_PROTOC_BINARIES=ON
#         -Dprotobuf_BUILD_LIBUPB=ON
#         -Dprotobuf_DISABLE_RTTI=ON
#         -Dprotobuf_BUILD_SHARED_LIBS=OFF
#         -Dprotobuf_MSVC_STATIC_RUNTIME=ON
#         -Dprotobuf_WITH_ZLIB=ON
#         -DZLIB_USE_STATIC_LIBS=${ZLIB_USE_STATIC_LIBS}
#         -B . -S ${PROTOBUF_SOURCE_DIR}
#       WORKING_DIRECTORY ${PROTOBUF_BINARY_DIR}
#       COMMAND_ERROR_IS_FATAL ANY
#     )
#   endif()
#   if(NOT EXISTS ${PROTOBUF_BINARY_DIR}/${PLATFORM_PREFIX}/${CMAKE_BUILD_TYPE})
#     execute_process(COMMAND cmake
#         --build . --config ${CMAKE_BUILD_TYPE}
#       WORKING_DIRECTORY ${PROTOBUF_BINARY_DIR}
#       COMMAND_ERROR_IS_FATAL ANY
#     )
#   endif()
#   if(NOT EXISTS ${PROTOBUF_INSTALL_DIR})
#     execute_process(COMMAND cmake
#         --install . --config ${CMAKE_BUILD_TYPE} --prefix ${CMAKE_INSTALL_PREFIX}
#       WORKING_DIRECTORY ${PROTOBUF_BINARY_DIR}
#       COMMAND_ERROR_IS_FATAL ANY
#     )
#   endif()
# endblock()

# block()
#   set(CARES_SOURCE_DIR ${PROJECT_SOURCE_DIR}/cares)
#   set(CARES_BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR}/cares)
#   set(CARES_INSTALL_DIR ${CMAKE_INSTALL_PREFIX}/lib/cmake/c-ares)
#   if(NOT EXISTS ${CARES_SOURCE_DIR}/CMakeLists.txt)
#     execute_process(COMMAND git submodule update
#         --init third_party/cares
#       WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
#       COMMAND_ERROR_IS_FATAL ANY
#     )
#   endif()
#   if(NOT EXISTS ${CARES_BINARY_DIR})
#     make_directory(${CARES_BINARY_DIR})
#     execute_process(COMMAND cmake
#         -DCMAKE_CXX_STANDARD=${CMAKE_CXX_STANDARD}
#         -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
#         -DCMAKE_INSTALL_PREFIX=${CMAKE_INSTALL_PREFIX}
#         -DCARES_INSTALL=ON
#         -DCARES_STATIC=ON
#         -DCARES_SHARED=OFF
#         -DCARES_BUILD_TESTS=OFF
#         -DCARES_BUILD_CONTAINER_TESTS=OFF
#         -DCARES_BUILD_TOOLS=OFF
#         -DCARES_THREADS=OFF
#         -DCARES_MSVC_STATIC_RUNTIME=ON
#         -B . -S ${CARES_SOURCE_DIR}
#       WORKING_DIRECTORY ${CARES_BINARY_DIR}
#       COMMAND_ERROR_IS_FATAL ANY
#     )
#   endif()
#   if(NOT EXISTS ${CARES_BINARY_DIR}/${PLATFORM_PREFIX}/${CMAKE_BUILD_TYPE})
#     execute_process(COMMAND cmake
#         --build . --config ${CMAKE_BUILD_TYPE}
#       WORKING_DIRECTORY ${CARES_BINARY_DIR}
#       COMMAND_ERROR_IS_FATAL ANY
#     )
#   endif()
#   if(NOT EXISTS ${CARES_INSTALL_DIR})
#     execute_process(COMMAND cmake
#         --install . --config ${CMAKE_BUILD_TYPE} --prefix ${CMAKE_INSTALL_PREFIX}
#       WORKING_DIRECTORY ${CARES_BINARY_DIR}
#       COMMAND_ERROR_IS_FATAL ANY
#     )
#   endif()
# endblock()

<<<<<<< HEAD
# block()
#   set(RE2_SOURCE_DIR ${PROJECT_SOURCE_DIR}/re2)
#   set(RE2_BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR}/re2)
#   set(RE2_INSTALL_DIR ${CMAKE_INSTALL_PREFIX}/lib/cmake/re2)
#   if(NOT EXISTS ${RE2_SOURCE_DIR}/CMakeLists.txt)
#     execute_process(COMMAND git submodule update
#         --init third_party/re2
#       WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
#       COMMAND_ERROR_IS_FATAL ANY
#     )
#   endif()
#   if(NOT EXISTS ${RE2_BINARY_DIR})
#     make_directory(${RE2_BINARY_DIR})
#     execute_process(COMMAND cmake
#         -DCMAKE_CXX_STANDARD=${CMAKE_CXX_STANDARD}
#         -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
#         -DCMAKE_INSTALL_PREFIX=${CMAKE_INSTALL_PREFIX}
#         -DRE2_BUILD_TESTING=OFF
#         -B . -S ${RE2_SOURCE_DIR}
#       WORKING_DIRECTORY ${RE2_BINARY_DIR}
#       COMMAND_ERROR_IS_FATAL ANY
#     )
#   endif()
#   if(NOT EXISTS ${RE2_BINARY_DIR}/${PLATFORM_PREFIX}/${CMAKE_BUILD_TYPE})
#     execute_process(COMMAND cmake
#         --build . --config ${CMAKE_BUILD_TYPE}
#       WORKING_DIRECTORY ${RE2_BINARY_DIR}
#       COMMAND_ERROR_IS_FATAL ANY
#     )
#   endif()
#   if(NOT EXISTS ${RE2_INSTALL_DIR})
#     execute_process(COMMAND cmake
#         --install . --config ${CMAKE_BUILD_TYPE} --prefix ${CMAKE_INSTALL_PREFIX}
#       WORKING_DIRECTORY ${RE2_BINARY_DIR}
#       COMMAND_ERROR_IS_FATAL ANY
#     )
#   endif()
# endblock()
=======
block()
  set(RE2_SOURCE_DIR ${PROJECT_SOURCE_DIR}/re2)
  set(RE2_BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR}/re2)
  set(RE2_INSTALL_DIR ${CMAKE_INSTALL_PREFIX}/lib/cmake/re2)
  if(NOT EXISTS ${RE2_SOURCE_DIR}/CMakeLists.txt)
    execute_process(COMMAND git submodule update
        --init third_party/re2
      WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
      COMMAND_ERROR_IS_FATAL ANY
    )
  endif()
  if(NOT EXISTS ${RE2_BINARY_DIR})
    make_directory(${RE2_BINARY_DIR})
    execute_process(COMMAND cmake
        -DCMAKE_CXX_STANDARD=${CMAKE_CXX_STANDARD}
        -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
        -DCMAKE_INSTALL_PREFIX=${CMAKE_INSTALL_PREFIX}
        -DRE2_BUILD_TESTING=OFF
        -B . -S ${RE2_SOURCE_DIR}
      WORKING_DIRECTORY ${RE2_BINARY_DIR}
      COMMAND_ERROR_IS_FATAL ANY
    )
  endif()
  if(NOT EXISTS ${RE2_BINARY_DIR}/${PLATFORM_PREFIX}/${CMAKE_BUILD_TYPE})
    execute_process(COMMAND cmake
        --build . --config ${CMAKE_BUILD_TYPE}
      WORKING_DIRECTORY ${RE2_BINARY_DIR}
      COMMAND_ERROR_IS_FATAL ANY
    )
  endif()
  if(NOT EXISTS ${RE2_INSTALL_DIR})
    execute_process(COMMAND cmake
        --install . --config ${CMAKE_BUILD_TYPE} --prefix ${CMAKE_INSTALL_PREFIX}
      WORKING_DIRECTORY ${RE2_BINARY_DIR}
      COMMAND_ERROR_IS_FATAL ANY
    )
  endif()
endblock()
>>>>>>> d43df738c145f0b08357e3bbca86aa4a830adf9b

# block()
#   set(OPENSSL_SOURCE_DIR ${PROJECT_SOURCE_DIR}/openssl)
#   set(OPENSSL_BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR}/openssl)
#   set(OPENSSL_INSTALL_DIR ${CMAKE_INSTALL_PREFIX}/lib/cmake/OpenSSL)
#   if(WIN32)
#     set(CONFIGURE_COMMAND perl ${OPENSSL_SOURCE_DIR}/Configure)
#     if(MSVC)
#       execute_process(COMMAND "C:/Program Files (x86)/Microsoft Visual Studio/Installer/vswhere.exe"
#           -format value -property installationPath
#         OUTPUT_VARIABLE VS_INSTALLATION_PATH
#         OUTPUT_STRIP_TRAILING_WHITESPACE
#         COMMAND_ERROR_IS_FATAL ANY
#       )
#       set(MAKE_COMMAND ${VS_INSTALLATION_PATH}/VC/Auxiliary/Build/vcvarsall.bat x64 && nmake)
#     else()
#       message(FATAL_ERROR "Compiler not supported")
#     endif()
#     link_libraries(ws2_32.lib gdi32.lib advapi32.lib crypt32.lib user32.lib)
#   else()
#     set(CONFIGURE_COMMAND ${OPENSSL_SOURCE_DIR}/Configure)
#     set(MAKE_COMMAND make)
#   endif()
#   if(NOT EXISTS ${OPENSSL_SOURCE_DIR}/Configure)
#     execute_process(COMMAND git submodule update
#         --init third_party/openssl
#         WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
#         COMMAND_ERROR_IS_FATAL ANY
#     )
#   endif()
#   if(NOT EXISTS ${OPENSSL_BINARY_DIR}/makefile)
#     make_directory(${OPENSSL_BINARY_DIR})
#     execute_process(COMMAND ${CONFIGURE_COMMAND}
#         -static
#         --prefix=${CMAKE_INSTALL_PREFIX}
#         --openssldir=${CMAKE_INSTALL_PREFIX}/ssl
#         no-apps
#         no-docs
#       WORKING_DIRECTORY ${OPENSSL_BINARY_DIR}
#       COMMAND_ERROR_IS_FATAL ANY
#     )
#   endif()
#   if(NOT EXISTS ${OPENSSL_BINARY_DIR}/OpenSSLConfig.cmake)
#     execute_process(COMMAND ${MAKE_COMMAND}
#       WORKING_DIRECTORY ${OPENSSL_BINARY_DIR}
#       COMMAND_ERROR_IS_FATAL ANY
#     )
#   endif()
#   if(NOT EXISTS ${OPENSSL_INSTALL_DIR})
#     execute_process(COMMAND ${MAKE_COMMAND} install
#       WORKING_DIRECTORY ${OPENSSL_BINARY_DIR}
#       COMMAND_ERROR_IS_FATAL ANY
#     )
#   endif()
#   set(OPENSSL_USE_STATIC_LIBS TRUE CACHE INTERNAL "OpenSSL use static libraries")
#   set(OPENSSL_MSVC_STATIC_RT TRUE CACHE INTERNAL "OpenSSL use static MSVC runtime")
# endblock()

<<<<<<< HEAD
# block()
#   set(GRPC_SOURCE_DIR ${PROJECT_SOURCE_DIR}/grpc)
#   set(GRPC_BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR}/grpc)
#   set(GRPC_INSTALL_DIR ${CMAKE_INSTALL_PREFIX}/lib/cmake/grpc)
#   if(NOT EXISTS ${GRPC_SOURCE_DIR}/CMakeLists.txt)
#     execute_process(COMMAND git submodule update
#         --init third_party/grpc
#       WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
#       COMMAND_ERROR_IS_FATAL ANY
#     )
#   endif()
#   if(NOT EXISTS ${GRPC_BINARY_DIR})
#     make_directory(${GRPC_BINARY_DIR})
#     execute_process(COMMAND cmake
#         -DCMAKE_POLICY_VERSION_MINIMUM=3.5
#         -DCMAKE_CXX_STANDARD=${CMAKE_CXX_STANDARD}
#         -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
#         -DCMAKE_INSTALL_PREFIX=${CMAKE_INSTALL_PREFIX}
#         -DgRPC_INSTALL=ON
#         -DgRPC_MSVC_STATIC_RUNTIME=ON
#         -DgRPC_BUILD_TESTS=OFF
#         -DgRPC_BUILD_CODEGEN=ON
#         -DgRPC_DOWNLOAD_ARCHIVES=OFF
#         -DgRPC_ABSL_PROVIDER=package
#         -DgRPC_CARES_PROVIDER=package
#         -DgRPC_PROTOBUF_PROVIDER=package
#         -DgRPC_RE2_PROVIDER=package
#         -DgRPC_SSL_PROVIDER=package
#         -DgRPC_ZLIB_PROVIDER=package
#         -DgRPC_OPENTELEMETRY_PROVIDER=package
#         -DgRPC_USE_PROTO_LITE=OFF
#         -DCMAKE_POSITION_INDEPENDENT_CODE=FALSE
#         -DgRPC_BUILD_GRPC_CPP_PLUGIN=ON
#         -DgRPC_BUILD_GRPC_CSHARP_PLUGIN=OFF
#         -DgRPC_BUILD_GRPC_NODE_PLUGIN=OFF
#         -DgRPC_BUILD_GRPC_OBJECTIVE_C_PLUGIN=OFF
#         -DgRPC_BUILD_GRPC_PHP_PLUGIN=OFF
#         -DgRPC_BUILD_GRPC_PYTHON_PLUGIN=OFF
#         -DgRPC_BUILD_GRPC_RUBY_PLUGIN=OFF
#         -DOPENSSL_USE_STATIC_LIBS=${OPENSSL_USE_STATIC_LIBS}
#         -DOPENSSL_MSVC_STATIC_RT=${OPENSSL_MSVC_STATIC_RT}
#         -DZLIB_USE_STATIC_LIBS=${ZLIB_USE_STATIC_LIBS}
#         -B . -S ${GRPC_SOURCE_DIR}
#       WORKING_DIRECTORY ${GRPC_BINARY_DIR}
#       COMMAND_ERROR_IS_FATAL ANY
#     )
#   endif()
#   if(NOT EXISTS ${GRPC_BINARY_DIR}/${PLATFORM_PREFIX}/${CMAKE_BUILD_TYPE})
#     execute_process(COMMAND cmake
#         --build . --config ${CMAKE_BUILD_TYPE}
#       WORKING_DIRECTORY ${GRPC_BINARY_DIR}
#       COMMAND_ERROR_IS_FATAL ANY
#     )
#   endif()
#   if(NOT EXISTS ${GRPC_INSTALL_DIR})
#     execute_process(COMMAND cmake
#         --install . --config ${CMAKE_BUILD_TYPE} --prefix ${CMAKE_INSTALL_PREFIX}
#       WORKING_DIRECTORY ${GRPC_BINARY_DIR}
#       COMMAND_ERROR_IS_FATAL ANY
#     )
#   endif()
# endblock()

# block()
#   set(OPENTELEMETRY_SOURCE_DIR ${PROJECT_SOURCE_DIR}/opentelemetry)
#   set(OPENTELEMETRY_BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR}/opentelemetry)
#   set(OPENTELEMETRY_INSTALL_DIR ${CMAKE_INSTALL_PREFIX}/lib/cmake/opentelemetry-cpp)
#   if(NOT EXISTS ${OPENTELEMETRY_SOURCE_DIR}/CMakeLists.txt)
#     execute_process(COMMAND git submodule update
#         --init third_party/opentelemetry
#       WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
#       COMMAND_ERROR_IS_FATAL ANY
#     )
#   endif()
#   if(NOT EXISTS ${OPENTELEMETRY_BINARY_DIR})
#     make_directory(${OPENTELEMETRY_BINARY_DIR})
#     execute_process(COMMAND cmake
#         -DCMAKE_CXX_STANDARD=${CMAKE_CXX_STANDARD}
#         -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
#         -DCMAKE_INSTALL_PREFIX=${CMAKE_INSTALL_PREFIX}
#         -DOPENTELEMETRY_INSTALL=ON
#         -DWITH_NO_GETENV=ON
#         -DBUILD_TESTING=OFF
#         -DWITH_FUNC_TESTS=OFF
#         -DWITH_BENCHMARK=OFF
#         -DWITH_EXAMPLES=OFF
#         -DWITH_ETW=OFF
#         -DWITH_ABI_VERSION_1=OFF
#         -DWITH_ABI_VERSION_2=ON
#         -DWITH_OTLP_GRPC=ON
#         -DWITH_NO_DEPRECATED_CODE=ON
#         -DOPENSSL_USE_STATIC_LIBS=${OPENSSL_USE_STATIC_LIBS}
#         -DOPENSSL_MSVC_STATIC_RT=${OPENSSL_MSVC_STATIC_RT}
#         -DZLIB_USE_STATIC_LIBS=${ZLIB_USE_STATIC_LIBS}
#         -B . -S ${OPENTELEMETRY_SOURCE_DIR}
#       WORKING_DIRECTORY ${OPENTELEMETRY_BINARY_DIR}
#       COMMAND_ERROR_IS_FATAL ANY
#     )
#   endif()
#   if(NOT EXISTS ${OPENTELEMETRY_BINARY_DIR}/${PLATFORM_PREFIX}/${CMAKE_BUILD_TYPE})
#     execute_process(COMMAND cmake
#         --build . --config ${CMAKE_BUILD_TYPE}
#       WORKING_DIRECTORY ${OPENTELEMETRY_BINARY_DIR}
#       COMMAND_ERROR_IS_FATAL ANY
#     )
#   endif()
#   if(NOT EXISTS ${OPENTELEMETRY_INSTALL_DIR})
#     execute_process(COMMAND cmake
#         --install . --config ${CMAKE_BUILD_TYPE} --prefix ${CMAKE_INSTALL_PREFIX}
#       WORKING_DIRECTORY ${OPENTELEMETRY_BINARY_DIR}
#       COMMAND_ERROR_IS_FATAL ANY
#     )
#   endif()
# endblock()
