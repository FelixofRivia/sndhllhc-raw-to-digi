add_executable(check_raw_read check_raw_read.cxx ${CMAKE_SOURCE_DIR}/sistrip_io/SiStripIOSource.cxx)
target_link_libraries(check_raw_read PRIVATE ${ROOT_LIBRARIES})
target_include_directories(check_raw_read PRIVATE ${ROOT_INCLUDE_DIRS} ${CMAKE_SOURCE_DIR}/sistrip_io)

ROOT_GENERATE_DICTIONARY(SiStripIODict_check_raw_read ${CMAKE_SOURCE_DIR}/sistrip_io/SiStripIOHeaders.h LINKDEF ${CMAKE_SOURCE_DIR}/sistrip_io/SiStripIOLinkDef.hxx MODULE check_raw_read)

add_custom_target(  
  copy_test_data  
  COMMAND ${CMAKE_COMMAND} -E copy_directory  
    ${CMAKE_SOURCE_DIR}/tests/data/  
    ${CMAKE_CURRENT_BINARY_DIR}/data/  
  COMMENT "Copying data files for tests"  
)  

add_dependencies(check_raw_read copy_test_data)

add_test(NAME check_raw_read COMMAND $<TARGET_FILE:check_raw_read> ${CMAKE_CURRENT_BINARY_DIR}/data/run_1000779_raw.root ${CMAKE_CURRENT_BINARY_DIR}/data/run_1000779_raw_dump_from_cmssw.csv)