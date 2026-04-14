cmake_minimum_required(VERSION 3.23)

project(sndhllhc_raw_to_digi VERSION 0.1)

add_executable(raw_to_digi)
target_sources(raw_to_digi PRIVATE raw_to_digi.cxx)

find_package(ROOT 6.36 REQUIRED COMPONENTS Core)
target_include_directories(raw_to_digi PRIVATE ${ROOT_INCLUDE_DIRS} ${CMAKE_CURRENT_SOURCE_DIR}/include)

target_link_libraries(raw_to_digi ${ROOT_LIBRARIES})


