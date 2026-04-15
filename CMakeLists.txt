cmake_minimum_required(VERSION 3.23)

project(sndhllhc_raw_to_digi VERSION 0.1)

find_package(ROOT 6.36 REQUIRED COMPONENTS Core RIO Tree)

add_executable(raw_to_digi raw_to_digi.cxx ${CMAKE_CURRENT_SOURCE_DIR}/src/SNDHLLHCSource.cxx)
target_link_libraries(raw_to_digi PRIVATE ${ROOT_LIBRARIES})
target_include_directories(raw_to_digi PRIVATE ${ROOT_INCLUDE_DIRS} ${CMAKE_CURRENT_SOURCE_DIR}/include)

ROOT_GENERATE_DICTIONARY(SNDHLLHCDict SNDHLLHCHeaders.h LINKDEF include/SNDHLLHCLinkDef.hxx MODULE raw_to_digi)

