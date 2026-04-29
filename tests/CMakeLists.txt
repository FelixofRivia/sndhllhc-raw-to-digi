add_custom_target(  
  copy_test_data  
  COMMAND ${CMAKE_COMMAND} -E copy_directory  
    ${CMAKE_SOURCE_DIR}/tests/data/  
    ${CMAKE_CURRENT_BINARY_DIR}/data/  
  COMMENT "Copying data files for tests"  
)  

add_executable(check_raw_read check_raw_read.cxx)
target_link_libraries(check_raw_read PRIVATE ${ROOT_LIBRARIES} sistrip_io)
target_include_directories(check_raw_read PRIVATE ${ROOT_INCLUDE_DIRS} ${CMAKE_SOURCE_DIR}/sistrip_io)
add_dependencies(check_raw_read copy_test_data)
add_test(NAME check_raw_read COMMAND $<TARGET_FILE:check_raw_read> ${CMAKE_CURRENT_BINARY_DIR}/data/run_1000779_raw.root ${CMAKE_CURRENT_BINARY_DIR}/data/run_1000779_raw_dump_from_cmssw.csv)


add_executable(check_digi_conversion check_digi_conversion.cxx ${CMAKE_SOURCE_DIR}/raw_to_digi/src/SiStripFEDBuffer.cxx)
target_link_libraries(check_digi_conversion PRIVATE ${ROOT_LIBRARIES} sistrip_io)
target_include_directories(check_digi_conversion PRIVATE ${ROOT_INCLUDE_DIRS} ${CMAKE_SOURCE_DIR}/raw_to_digi/include ${CMAKE_SOURCE_DIR}/sistrip_io)
add_dependencies(check_digi_conversion copy_test_data)
add_test(NAME check_digi_conversion COMMAND $<TARGET_FILE:check_digi_conversion> ${CMAKE_CURRENT_BINARY_DIR}/data/run_1000779_raw.root ${CMAKE_CURRENT_BINARY_DIR}/data/run_1000779_raw_dump_from_cmssw.csv)