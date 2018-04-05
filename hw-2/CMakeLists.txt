cmake_minimum_required(VERSION 3.0)

project(Exceptions)
file(GLOB Exceptions_SRC
    "*.h"
    "*.cpp"
)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "-fno-exceptions")

add_executable(${PROJECT_NAME} ${Exceptions_SRC})
target_include_directories(${PROJECT_NAME} PRIVATE .)
