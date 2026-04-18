cmake_minimum_required(VERSION 3.23)

project(sndhllhc_raw_to_digi VERSION 0.1)

add_compile_options(-Wall -Wextra -Wpedantic)

find_package(ROOT 6.36 REQUIRED COMPONENTS Core RIO Tree ROOTDataFrame)

add_executable(raw_info_dump raw_info_dump.cxx ${CMAKE_CURRENT_SOURCE_DIR}/src/raw/SNDHLLHCRawSource.cxx)
target_link_libraries(raw_info_dump PRIVATE ${ROOT_LIBRARIES})
target_include_directories(raw_info_dump PRIVATE ${ROOT_INCLUDE_DIRS} ${CMAKE_CURRENT_SOURCE_DIR}/include/raw)

ROOT_GENERATE_DICTIONARY(SNDHLLHCDict_raw_info_dump SNDHLLHCRawHeaders.h LINKDEF ${CMAKE_CURRENT_SOURCE_DIR}/include/raw/SNDHLLHCRawLinkDef.hxx MODULE raw_info_dump)

add_executable(raw_to_digi raw_to_digi.cxx ${CMAKE_CURRENT_SOURCE_DIR}/src/raw/SNDHLLHCRawSource.cxx ${CMAKE_CURRENT_SOURCE_DIR}/src/digi/SNDHLLHCDigiSource.cxx)
target_link_libraries(raw_to_digi PRIVATE ${ROOT_LIBRARIES})
target_include_directories(raw_to_digi PRIVATE ${ROOT_INCLUDE_DIRS} ${CMAKE_CURRENT_SOURCE_DIR}/include ${CMAKE_CURRENT_SOURCE_DIR}/include/raw ${CMAKE_CURRENT_SOURCE_DIR}/include/digi)

ROOT_GENERATE_DICTIONARY(SNDHLLHCDict_raw_to_digi SNDHLLHCRawHeaders.h SNDHLLHCDigiHeaders.h LINKDEF ${CMAKE_CURRENT_SOURCE_DIR}/include/SNDHLLHCBothLinkDef.hxx MODULE raw_to_digi)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/tests)

