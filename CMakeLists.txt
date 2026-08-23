cmake_minimum_required(VERSION 3.25)

project(QuantForge
    VERSION 0.1.0
    DESCRIPTION "QuantForge quantitative research and algorithmic trading platform"
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)


# QuantForge Core Library
add_library(quantforge-core

    cpp/src/core/event.cpp
    cpp/src/core/event_type.cpp
    cpp/src/market/instrument.cpp
)

target_include_directories(quantforge-core
    PUBLIC
        cpp/include
)


# QuantForge Application
add_executable(quantforge

    cpp/src/app/main.cpp
)

target_link_libraries(quantforge
    PRIVATE
        quantforge-core
)


# Testing
enable_testing()

find_package(GTest REQUIRED)

add_executable(quantforge-tests

    cpp/tests/core/event_test.cpp
    cpp/tests/market/instrument_id_test.cpp
    cpp/tests/market/instrument_test.cpp
    cpp/tests/market/price_test.cpp
    cpp/tests/market/quantity_test.cpp
)

target_link_libraries(quantforge-tests
    PRIVATE
        quantforge-core
        GTest::gtest_main
)

include(GoogleTest)

gtest_discover_tests(quantforge-tests)