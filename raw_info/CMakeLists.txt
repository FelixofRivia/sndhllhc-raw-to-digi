add_executable(raw_info_dump ${CMAKE_CURRENT_SOURCE_DIR}/src/raw_info_dump.cxx)
target_link_libraries(raw_info_dump PRIVATE ${ROOT_LIBRARIES} sistrip_io)
target_include_directories(raw_info_dump PRIVATE ${ROOT_INCLUDE_DIRS} ${CMAKE_SOURCE_DIR}/sistrip_io)

