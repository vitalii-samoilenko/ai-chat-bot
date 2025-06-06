cmake_minimum_required(VERSION 3.28.2)

project(OpenAI VERSION 0.1 LANGUAGES CXX)

add_library(openai INTERFACE)
add_library(OpenAI ALIAS openai)
target_include_directories(openai INTERFACE include)
target_link_libraries(openai
  INTERFACE
    Boost::url
    EBoost
)
target_compile_features(openai INTERFACE cxx_std_11)