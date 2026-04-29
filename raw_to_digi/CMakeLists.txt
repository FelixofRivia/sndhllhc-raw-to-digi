add_executable(raw_to_digi ${CMAKE_CURRENT_SOURCE_DIR}/src/raw_to_digi.cxx ${CMAKE_CURRENT_SOURCE_DIR}/src/SiStripFEDBuffer.cxx)
target_link_libraries(raw_to_digi PRIVATE ${ROOT_LIBRARIES} sistrip_io)
target_include_directories(raw_to_digi PRIVATE ${ROOT_INCLUDE_DIRS} ${CMAKE_CURRENT_SOURCE_DIR}/include ${CMAKE_SOURCE_DIR}/sistrip_io)