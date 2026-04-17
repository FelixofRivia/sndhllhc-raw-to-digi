#find_package(ROOT 6.36 REQUIRED COMPONENTS Core RIO Tree ROOTDataFrame)

add_executable(check_raw check_raw.cxx ${CMAKE_SOURCE_DIR}/src/SNDHLLHCSource.cxx)
target_link_libraries(check_raw PRIVATE ${ROOT_LIBRARIES})
target_include_directories(check_raw PRIVATE ${ROOT_INCLUDE_DIRS} ${CMAKE_SOURCE_DIR}/include)

ROOT_GENERATE_DICTIONARY(SNDHLLHCDict_check_raw SNDHLLHCHeaders.h LINKDEF ${CMAKE_SOURCE_DIR}/include/SNDHLLHCLinkDef.hxx MODULE check_raw)

add_custom_target(  
  copy_test_data  
  COMMAND ${CMAKE_COMMAND} -E copy_directory  
    ${CMAKE_SOURCE_DIR}/tests/data/  
    ${CMAKE_CURRENT_BINARY_DIR}/data/  
  COMMENT "Copying test data files"  
)  

add_dependencies(check_raw copy_test_data) 