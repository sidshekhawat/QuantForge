cmake_minimum_required(VERSION 3.25)

project(QuantForge
    VERSION 0.1.0
    DESCRIPTION "QuantForge quantitative research and algorithmic trading platform"
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)


# QuantForge Core Library
add_library(quantforge-core

    cpp/src/core/event.cpp
    cpp/src/core/event_type.cpp
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