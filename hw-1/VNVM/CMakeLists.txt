cmake_minimum_required(VERSION 3.0)

project(VNVM)
file(GLOB VNVM_SRC
    "*.h"
    "*.cpp"
    "Framework/*.h"
)

file(GLOB VNASM_SRC
    "Assemble/*.h"
    "Assemble/*.cpp"
)

file(GLOB VNDASM_SRC
    "Disassemble/*.h"
    "Disassemble/*.cpp"
)

file(GLOB Vn_Sources
    "*.vn"
)

foreach(vn_src ${Vn_Sources})
    get_filename_component(vn_name ${vn_src} NAME)
    configure_file(${vn_src} ${CMAKE_CURRENT_BINARY_DIR}/${vn_name})
endforeach(vn_src)

set(VNASM_NAME VNAsm)
set(VNDASM_NAME VNDasm)
set(CMAKE_CXX_STANDARD 17)

add_executable(${PROJECT_NAME} ${VNVM_SRC})
target_include_directories(${PROJECT_NAME} PRIVATE . ./Framework)

add_executable(${VNASM_NAME} ${VNASM_SRC})
target_include_directories(${VNASM_NAME} PRIVATE ./Assemble ./Framework)

add_executable(${VNDASM_NAME} ${VNDASM_SRC})
target_include_directories(${VNDASM_NAME} PRIVATE . ./Disassemble ./Framework)
