add_executable(check_raw check_raw.cxx ${CMAKE_SOURCE_DIR}/sistrip_io/SiStripIOSource.cxx)
target_link_libraries(check_raw PRIVATE ${ROOT_LIBRARIES})
target_include_directories(check_raw PRIVATE ${ROOT_INCLUDE_DIRS} ${CMAKE_SOURCE_DIR}/sistrip_io)

ROOT_GENERATE_DICTIONARY(SiStripIODict_check_raw ${CMAKE_SOURCE_DIR}/sistrip_io/SiStripIOHeaders.h LINKDEF ${CMAKE_SOURCE_DIR}/sistrip_io/SiStripIOLinkDef.hxx MODULE check_raw)

add_custom_target(  
  copy_test_data  
  COMMAND ${CMAKE_COMMAND} -E copy_directory  
    ${CMAKE_SOURCE_DIR}/tests/data/  
    ${CMAKE_CURRENT_BINARY_DIR}/data/  
  COMMENT "Copying data files for tests"  
)  

add_dependencies(check_raw copy_test_data) 