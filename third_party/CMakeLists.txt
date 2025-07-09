cmake_minimum_required(VERSION 3.25.1)

project(third_party VERSION 0.1)

block()
  set(ZLIB_SOURCE_DIR ${PROJECT_SOURCE_DIR}/zlib)
  set(ZLIB_BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR}/zlib)
  set(ZLIB_INSTALL_DIR ${CMAKE_SOURCE_DIR}/install/ZLib)
  if(NOT EXISTS ${ZLIB_SOURCE_DIR}/CMakeLists.txt)
    execute_process(COMMAND git submodule update
        --init third_party/zlib
      WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
      COMMAND_ERROR_IS_FATAL ANY
    )
  endif()
  if(NOT EXISTS ${ZLIB_BINARY_DIR})
    make_directory(${ZLIB_BINARY_DIR})
    execute_process(COMMAND cmake
        -DCMAKE_CXX_STANDARD=17
        -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
        -DCMAKE_INSTALL_PREFIX=${ZLIB_INSTALL_DIR}
        -DZLIB_INSTALL=ON
        -DZLIB_BUILD_TESTING=OFF
        -DZLIB_BUILD_SHARED=OFF
        -DZLIB_BUILD_STATIC=ON
        -DZLIB_INSTALL_COMPAT_DLL=OFF
        -B . -S ${ZLIB_SOURCE_DIR}
      WORKING_DIRECTORY ${ZLIB_BINARY_DIR}
      COMMAND_ERROR_IS_FATAL ANY
    )
  endif()
  if(NOT EXISTS ${ZLIB_BINARY_DIR}/${CMAKE_BUILD_TYPE})
    execute_process(COMMAND cmake
        --build . --config ${CMAKE_BUILD_TYPE}
      WORKING_DIRECTORY ${ZLIB_BINARY_DIR}
      COMMAND_ERROR_IS_FATAL ANY
    )
  endif()
  if(NOT EXISTS ${ZLIB_INSTALL_DIR})
    execute_process(COMMAND cmake
        --install . --config ${CMAKE_BUILD_TYPE} --prefix ${ZLIB_INSTALL_DIR}
      WORKING_DIRECTORY ${ZLIB_BINARY_DIR}
      COMMAND_ERROR_IS_FATAL ANY
    )
  endif()
  set(ZLIB_ROOT ${ZLIB_INSTALL_DIR} PARENT_SCOPE)
  set(ZLIB_USE_STATIC_LIBS ON PARENT_SCOPE)
endblock()

block()
  set(ABSEIL_SOURCE_DIR ${PROJECT_SOURCE_DIR}/abseil)
  set(ABSEIL_BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR}/abseil)
  set(ABSEIL_INSTALL_DIR ${CMAKE_SOURCE_DIR}/install/Abseil)
  if(NOT EXISTS ${ABSEIL_SOURCE_DIR}/CMakeLists.txt)
    execute_process(COMMAND git submodule update
        --init third_party/abseil
      WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
      COMMAND_ERROR_IS_FATAL ANY
    )
  endif()
  if(NOT EXISTS ${ABSEIL_BINARY_DIR})
    make_directory(${ABSEIL_BINARY_DIR})
    execute_process(COMMAND cmake
        -DCMAKE_CXX_STANDARD=17
        -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
        -DCMAKE_INSTALL_PREFIX=${ABSEIL_INSTALL_DIR}
        -DABSL_ENABLE_INSTALL=ON
        -DABSL_MSVC_STATIC_RUNTIME=ON
        -B . -S ${ABSEIL_SOURCE_DIR}
      WORKING_DIRECTORY ${ABSEIL_BINARY_DIR}
      COMMAND_ERROR_IS_FATAL ANY
    )
  endif()
  if(NOT EXISTS ${ABSEIL_BINARY_DIR}/${CMAKE_BUILD_TYPE})
    execute_process(COMMAND cmake
        --build . --config ${CMAKE_BUILD_TYPE}
      WORKING_DIRECTORY ${ABSEIL_BINARY_DIR}
      COMMAND_ERROR_IS_FATAL ANY
    )
  endif()
  if(NOT EXISTS ${ABSEIL_INSTALL_DIR})
    execute_process(COMMAND cmake
        --install . --config ${CMAKE_BUILD_TYPE} --prefix ${ABSEIL_INSTALL_DIR}
      WORKING_DIRECTORY ${ABSEIL_BINARY_DIR}
      COMMAND_ERROR_IS_FATAL ANY
    )
  endif()
  set(absl_DIR ${ABSEIL_INSTALL_DIR}/lib/cmake/absl PARENT_SCOPE)
endblock()

block()
  set(PROTOBUF_SOURCE_DIR ${PROJECT_SOURCE_DIR}/protobuf)
  set(PROTOBUF_BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR}/protobuf)
  set(PROTOBUF_INSTALL_DIR ${CMAKE_SOURCE_DIR}/install/Protobuf)
  if(NOT EXISTS ${PROTOBUF_SOURCE_DIR}/CMakeLists.txt)
    execute_process(COMMAND git submodule update
        --init third_party/protobuf
      WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
      COMMAND_ERROR_IS_FATAL ANY
    )
  endif()
  if(NOT EXISTS ${PROTOBUF_BINARY_DIR})
    make_directory(${PROTOBUF_BINARY_DIR})
    execute_process(COMMAND cmake
        -DCMAKE_CXX_STANDARD=17
        -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
        -DCMAKE_INSTALL_PREFIX=${PROTOBUF_INSTALL_DIR}
        -Dprotobuf_INSTALL=ON
        -Dprotobuf_BUILD_TESTS=OFF
        -Dprotobuf_BUILD_PROTOBUF_BINARIES=ON
        -Dprotobuf_BUILD_PROTOC_BINARIES=OFF
        -Dprotobuf_BUILD_LIBUPB=OFF
        -Dprotobuf_DISABLE_RTTI=ON
        -Dprotobuf_BUILD_SHARED_LIBS=OFF
        -Dprotobuf_WITH_ZLIB=ON
        -DZLIB_ROOT=${ZLIB_ROOT}
        -DZLIB_USE_STATIC_LIBS=${ZLIB_USE_STATIC_LIBS}
        -Dabsl_DIR=${absl_DIR}
        -B . -S ${PROTOBUF_SOURCE_DIR}
      WORKING_DIRECTORY ${PROTOBUF_BINARY_DIR}
      COMMAND_ERROR_IS_FATAL ANY
    )
  endif()
  if(NOT EXISTS ${PROTOBUF_BINARY_DIR}/${CMAKE_BUILD_TYPE})
    execute_process(COMMAND cmake
        --build . --config ${CMAKE_BUILD_TYPE}
      WORKING_DIRECTORY ${PROTOBUF_BINARY_DIR}
      COMMAND_ERROR_IS_FATAL ANY
    )
  endif()
  if(NOT EXISTS ${PROTOBUF_INSTALL_DIR})
    execute_process(COMMAND cmake
        --install . --config ${CMAKE_BUILD_TYPE} --prefix ${PROTOBUF_INSTALL_DIR}
      WORKING_DIRECTORY ${PROTOBUF_BINARY_DIR}
      COMMAND_ERROR_IS_FATAL ANY
    )
  endif()
endblock()

block()
  set(CARES_SOURCE_DIR ${PROJECT_SOURCE_DIR}/cares)
  set(CARES_BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR}/cares)
  set(CARES_INSTALL_DIR ${CMAKE_SOURCE_DIR}/install/CARes)
  if(NOT EXISTS ${CARES_SOURCE_DIR}/CMakeLists.txt)
    execute_process(COMMAND git submodule update
        --init third_party/cares
      WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
      COMMAND_ERROR_IS_FATAL ANY
    )
  endif()
  if(NOT EXISTS ${CARES_BINARY_DIR})
    make_directory(${CARES_BINARY_DIR})
    execute_process(COMMAND cmake
        -DCMAKE_CXX_STANDARD=17
        -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
        -DCMAKE_INSTALL_PREFIX=${CARES_INSTALL_DIR}
        -DCARES_INSTALL=ON
        -DCARES_STATIC=ON
        -DCARES_SHARED=OFF
        -DCARES_BUILD_TESTS=OFF
        -DCARES_BUILD_CONTAINER_TESTS=OFF
        -DCARES_BUILD_TOOLS=OFF
        -DCARES_THREADS=ON
        -DCARES_MSVC_STATIC_RUNTIME=ON
        -B . -S ${CARES_SOURCE_DIR}
      WORKING_DIRECTORY ${CARES_BINARY_DIR}
      COMMAND_ERROR_IS_FATAL ANY
    )
  endif()
  if(NOT EXISTS ${CARES_BINARY_DIR}/${CMAKE_BUILD_TYPE})
    execute_process(COMMAND cmake
        --build . --config ${CMAKE_BUILD_TYPE}
      WORKING_DIRECTORY ${CARES_BINARY_DIR}
      COMMAND_ERROR_IS_FATAL ANY
    )
  endif()
  if(NOT EXISTS ${CARES_INSTALL_DIR})
    execute_process(COMMAND cmake
        --install . --config ${CMAKE_BUILD_TYPE} --prefix ${CARES_INSTALL_DIR}
      WORKING_DIRECTORY ${CARES_BINARY_DIR}
      COMMAND_ERROR_IS_FATAL ANY
    )
  endif()
endblock()

block()
  set(RE2_SOURCE_DIR ${PROJECT_SOURCE_DIR}/re2)
  set(RE2_BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR}/re2)
  set(RE2_INSTALL_DIR ${CMAKE_SOURCE_DIR}/install/RE2)
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
        -DCMAKE_CXX_STANDARD=17
        -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
        -DCMAKE_INSTALL_PREFIX=${CARES_INSTALL_DIR}
        -DBUILD_SHARED_LIBS=OFF
        -DRE2_BUILD_TESTING=OFF
        -Dabsl_DIR=${absl_DIR}
        -B . -S ${RE2_SOURCE_DIR}
      WORKING_DIRECTORY ${RE2_BINARY_DIR}
      COMMAND_ERROR_IS_FATAL ANY
    )
  endif()
  if(NOT EXISTS ${RE2_BINARY_DIR}/${CMAKE_BUILD_TYPE})
    execute_process(COMMAND cmake
        --build . --config ${CMAKE_BUILD_TYPE}
      WORKING_DIRECTORY ${RE2_BINARY_DIR}
      COMMAND_ERROR_IS_FATAL ANY
    )
  endif()
  if(NOT EXISTS ${RE2_INSTALL_DIR})
    execute_process(COMMAND cmake
        --install . --config ${CMAKE_BUILD_TYPE} --prefix ${RE2_INSTALL_DIR}
      WORKING_DIRECTORY ${RE2_BINARY_DIR}
      COMMAND_ERROR_IS_FATAL ANY
    )
  endif()
endblock()

block()
  set(OPENSSL_SOURCE_DIR ${PROJECT_SOURCE_DIR}/openssl)
  set(OPENSSL_BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR}/openssl)
  set(OPENSSL_INSTALL_DIR ${CMAKE_SOURCE_DIR}/install/OpenSSL)
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
  endif()
  if(NOT EXISTS ${OPENSSL_BINARY_DIR}/makefile)
    make_directory(${OPENSSL_BINARY_DIR})
    execute_process(COMMAND ${CONFIGURE_COMMAND}
        -static
        --prefix=${OPENSSL_INSTALL_DIR}
        --openssldir=${OPENSSL_INSTALL_DIR}/ssl
        no-apps
        no-docs
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
  set(OPENSSL_ROOT_DIR ${OPENSSL_INSTALL_DIR} PARENT_SCOPE)
endblock()

# block()
#   set(GRPC_SOURCE_DIR ${PROJECT_SOURCE_DIR}/grpc)
#   set(GRPC_BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR}/grpc)
#   set(GRPC_INSTALL_DIR ${CMAKE_SOURCE_DIR}/install/gRPC)
#   if(NOT EXISTS ${GRPC_SOURCE_DIR}/CMakeLists.txt)
#     execute_process(COMMAND git submodule update
#         --init third_party/grpc
#             WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
#             COMMAND_ERROR_IS_FATAL ANY
#         )
#     endif()
#     if(NOT EXISTS ${GRPC_BINARY_DIR})
#         make_directory(${GRPC_BINARY_DIR})
#         execute_process(COMMAND cmake
#                 -DCMAKE_POLICY_VERSION_MINIMUM=3.5
#                 -DCMAKE_CXX_STANDARD=17
#                 -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
#                 -DCMAKE_INSTALL_PREFIX=${GRPC_INSTALL_DIR}
#                 -DgRPC_INSTALL=ON
#                 -DgRPC_BUILD_TESTS=OFF
#                 -DgRPC_ABSL_PROVIDER=package
#                 -DgRPC_CARES_PROVIDER=package
#                 -DgRPC_PROTOBUF_PROVIDER=package
#                 -DgRPC_RE2_PROVIDER=package
#                 -DgRPC_SSL_PROVIDER=package
#                 -DgRPC_ZLIB_PROVIDER=package
#                 -B . -S ${GRPC_SOURCE_DIR}
#             WORKING_DIRECTORY ${GRPC_BINARY_DIR}
#             COMMAND_ERROR_IS_FATAL ANY
#         )
#     endif()
#     if(NOT EXISTS ${GRPC_BINARY_DIR}/${CMAKE_BUILD_TYPE})
#         execute_process(COMMAND cmake
#                 --build . --config ${CMAKE_BUILD_TYPE}
#             WORKING_DIRECTORY ${GRPC_BINARY_DIR}
#             COMMAND_ERROR_IS_FATAL ANY
#         )
#     endif()
#     if(NOT EXISTS ${GRPC_INSTALL_DIR})
#         execute_process(COMMAND cmake
#                 --install . --config ${CMAKE_BUILD_TYPE}
#             WORKING_DIRECTORY ${GRPC_BINARY_DIR}
#             COMMAND_ERROR_IS_FATAL ANY
#         )
#     endif()
#     set(gRPC_DIR ${GRPC_INSTALL_DIR}/lib/cmake/grpc)
# endblock()



#find_package(Boost CONFIG REQUIRED)
# if(NOT Boost_FOUND)
#     if(NOT EXISTS ${PROJECT_SOURCE_DIR}/boost/CMakeLists.txt)
#         execute_process(COMMAND git submodule update
#                 --init --recursive third_party/boost
#             WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
#             COMMAND_ERROR_IS_FATAL ANY
#         )
#     endif()
#     add_subdirectory(boost)
# endif()

# find_package(opentelemetry-cpp CONFIG)
# if(NOT opentelemetry-cpp_FOUND)
#     if(NOT EXISTS ${PROJECT_SOURCE_DIR}/opentelemetry-cpp/CMakeLists.txt)
#         execute_process(COMMAND git submodule update
#                 --init third_party/opentelemetry-cpp
#             WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
#             COMMAND_ERROR_IS_FATAL ANY
#         )
#     endif()
#     set(BUILD_TESTING OFF)
#     set(WITH_FUNC_TESTS OFF)
#     set(WITH_BENCHMARK OFF)
#     set(WITH_EXAMPLES OFF)
#     set(WITH_ETW OFF)
#     set(WITH_ABI_VERSION_1 OFF)
#     set(WITH_ABI_VERSION_2 ON)
#     set(WITH_OTLP_GRPC ON)
#     add_subdirectory(opentelemetry-cpp)
# endif()

# find_package(SQLite)
# if(NOT SQLite_FOUND)
#     set(SQLITE_SOURCE_DIR ${PROJECT_SOURCE_DIR}/sqlite)
#     set(SQLITE_BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR}/sqlite)
#     set(SQLITE_INSTALL_DIR ${CMAKE_SOURCE_DIR}/install/SQLite)
#     if(WIN32)
#         set(CONFIGURE_COMMAND cmake -E touch config.log)
#         if(MSVC)
#             execute_process(COMMAND "C:/Program Files (x86)/Microsoft Visual Studio/Installer/vswhere.exe"
#                 -format value -property installationPath
#                 OUTPUT_VARIABLE VS_INSTALLATION_PATH
#                 OUTPUT_STRIP_TRAILING_WHITESPACE
#                 COMMAND_ERROR_IS_FATAL ANY
#             )
#             set(MAKE_COMMAND ${VS_INSTALLATION_PATH}/VC/Auxiliary/Build/vcvarsall.bat x64 && nmake /F Makefile.msc)
#         else()
#             message(FATAL_ERROR "Compiler not supported")
#         endif()
#         set(SQLITE_LIB libsqlite3.lib)
#     else()
#         set(CONFIGURE_COMMAND ${SQLITE_SOURCE_DIR}/configure)
#         set(MAKE_COMMAND make)
#         set(SQLITE_LIB libsqlite3.a)
#     endif()
#     if(NOT EXISTS ${SQLITE_SOURCE_DIR}/configure)
#         execute_process(COMMAND git submodule update
#                 --init third_party/sqlite
#             WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
#             COMMAND_ERROR_IS_FATAL ANY
#         )
#     endif()
#     if(NOT EXISTS ${SQLITE_BINARY_DIR}/config.log)
#         file(COPY ${SQLITE_SOURCE_DIR} DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
#         execute_process(COMMAND ${CONFIGURE_COMMAND}
#             WORKING_DIRECTORY ${SQLITE_BINARY_DIR}
#             COMMAND_ERROR_IS_FATAL ANY
#         )
#     endif()
#     if(NOT EXISTS ${SQLITE_BINARY_DIR}/sqlite3.h)
#         execute_process(COMMAND ${MAKE_COMMAND}
#             WORKING_DIRECTORY ${SQLITE_BINARY_DIR}
#             COMMAND_ERROR_IS_FATAL ANY
#         )
#     endif()
#     if(NOT EXISTS ${SQLITE_INSTALL_DIR})
#         file(COPY ${SQLITE_BINARY_DIR}/sqlite3.h DESTINATION ${SQLITE_INSTALL_DIR}/include)
#         file(COPY ${SQLITE_BINARY_DIR}/${SQLITE_LIB} DESTINATION ${SQLITE_INSTALL_DIR}/lib)
#     endif()
#     add_library(sqlite INTERFACE)
#     add_library(SQLite ALIAS sqlite)
#     target_include_directories(sqlite INTERFACE ${SQLITE_INSTALL_DIR}/include)
#     target_link_libraries(sqlite INTERFACE ${SQLITE_INSTALL_DIR}/lib/${SQLITE_LIB})
# endif()

#add_subdirectory(eboost)
