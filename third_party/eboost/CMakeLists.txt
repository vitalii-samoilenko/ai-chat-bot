cmake_minimum_required(VERSION 3.25.1)

project(EBoost VERSION 0.1 LANGUAGES CXX)

add_library(eboost INTERFACE)
add_library(EBoost ALIAS eboost)
target_include_directories(eboost INTERFACE include)
target_link_libraries(eboost
  INTERFACE
    Boost::asio
    Boost::beast
    Boost::json
)
target_compile_features(eboost INTERFACE cxx_std_11)