cmake_minimum_required(VERSION 3.23)

project(sndhllhc_raw_to_digi VERSION 0.1)

add_compile_options(-Wall -Wextra -Wpedantic)

find_package(ROOT 6.36 REQUIRED COMPONENTS Core RIO Tree ROOTDataFrame)

enable_testing()

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

add_subdirectory(${CMAKE_SOURCE_DIR}/sistrip_io)
add_subdirectory(${CMAKE_SOURCE_DIR}/raw_info)
add_subdirectory(${CMAKE_SOURCE_DIR}/raw_to_digi)
add_subdirectory(${CMAKE_SOURCE_DIR}/tests)

