cmake_minimum_required(VERSION 3.0)

project(FuturePromise)
file(GLOB FuturePromise_SRC
    "*.h"
    "*.cpp"
    "*.inl"
)
set(FuturePromise_DIR ${CMAKE_CURRENT_SOURCE_DIR} CACHE STRING "FuturePromise include directories")
set(CMAKE_CXX_STANDARD 14)

find_package(Threads)

add_library(${PROJECT_NAME} ${FuturePromise_SRC})
target_include_directories(${PROJECT_NAME} PRIVATE .)

add_subdirectory(Tests)
