cmake_minimum_required(VERSION 3.25.1)

project(boost VERSION 0.1)

block()
  set(CONFIG_SOURCE_DIR ${PROJECT_SOURCE_DIR}/config)
  set(CONFIG_BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR}/config)
  set(CONFIG_INSTALL_DIR ${CMAKE_INSTALL_PREFIX}/lib/cmake/boost/config)
  if(NOT EXISTS ${CONFIG_SOURCE_DIR}/CMakeLists.txt)
    execute_process(COMMAND git submodule update
        --init third_party/boost/config
      WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
      COMMAND_ERROR_IS_FATAL ANY
    )
  endif()
  if(NOT EXISTS ${CONFIG_BINARY_DIR})
    make_directory(${CONFIG_BINARY_DIR})
    execute_process(COMMAND cmake
        -DCMAKE_CXX_STANDARD=${CMAKE_CXX_STANDARD}
        -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
        -DCMAKE_INSTALL_PREFIX=${CMAKE_INSTALL_PREFIX}
        -B . -S ${CONFIG_SOURCE_DIR}
      WORKING_DIRECTORY ${CONFIG_BINARY_DIR}
      COMMAND_ERROR_IS_FATAL ANY
    )
  endif()
  if(NOT EXISTS ${CONFIG_BINARY_DIR}/${PLATFORM_PREFIX}/${CMAKE_BUILD_TYPE})
    execute_process(COMMAND cmake
        --build . --config ${CMAKE_BUILD_TYPE}
      WORKING_DIRECTORY ${CONFIG_BINARY_DIR}
      COMMAND_ERROR_IS_FATAL ANY
    )
  endif()
  if(NOT EXISTS ${CONFIG_INSTALL_DIR})
    execute_process(COMMAND cmake
        --install . --config ${CMAKE_BUILD_TYPE} --prefix ${CMAKE_INSTALL_PREFIX}
      WORKING_DIRECTORY ${CONFIG_BINARY_DIR}
      COMMAND_ERROR_IS_FATAL ANY
    )
  endif()
endblock()
