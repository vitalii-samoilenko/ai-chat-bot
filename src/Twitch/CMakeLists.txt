cmake_minimum_required(VERSION 3.28.2)

project(Twitch VERSION 0.1 LANGUAGES CXX)

add_library(twitch INTERFACE)
add_library(Twitch ALIAS twitch)
target_include_directories(twitch INTERFACE include)
target_link_libraries(twitch
  INTERFACE
    Boost::url
    Boost::xpressive
    EBoost
)
target_compile_features(twitch INTERFACE cxx_std_11)