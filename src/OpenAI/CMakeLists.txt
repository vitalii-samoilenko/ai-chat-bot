cmake_minimum_required(VERSION 3.28.2)

project(OpenAI VERSION 0.1 LANGUAGES CXX)

add_library(OpenAI INTERFACE)
target_include_directories(OpenAI INTERFACE include)
target_link_libraries(OpenAI
  INTERFACE
    OpenSSL::Crypto
    OpenSSL::SSL
    Boost::asio
    Boost::beast
    Boost::json
    Boost::url
    EBoost
)
target_compile_features(OpenAI INTERFACE cxx_std_11)