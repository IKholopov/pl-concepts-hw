cmake_minimum_required(VERSION 3.0)

enable_testing()
find_package(GTest REQUIRED)
include_directories(${GTEST_INCLUDE_DIRS})

file(GLOB FuturePromiseTest_SRC
    "*.h"
    "*.cpp"
)
set(CMAKE_CXX_STANDARD 14)

add_executable(${PROJECT_NAME}Test ${FuturePromiseTest_SRC})
target_include_directories(${PROJECT_NAME}Test PRIVATE ${FuturePromise_DIR} .)
target_link_libraries(${PROJECT_NAME}Test ${GTEST_BOTH_LIBRARIES} Threads::Threads ${PROJECT_NAME} )
