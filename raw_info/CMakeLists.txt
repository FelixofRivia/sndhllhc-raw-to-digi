add_executable(raw_info_dump ${CMAKE_CURRENT_SOURCE_DIR}/src/raw_info_dump.cxx ${CMAKE_SOURCE_DIR}/sistrip_io/SiStripIOSource.cxx)
target_link_libraries(raw_info_dump PRIVATE ${ROOT_LIBRARIES})
target_include_directories(raw_info_dump PRIVATE ${ROOT_INCLUDE_DIRS} ${CMAKE_SOURCE_DIR}/sistrip_io)

ROOT_GENERATE_DICTIONARY(SiStripIODict_raw_info_dump ${CMAKE_SOURCE_DIR}/sistrip_io/SiStripIOHeaders.h LINKDEF ${CMAKE_SOURCE_DIR}/sistrip_io/SiStripIOLinkDef.hxx MODULE raw_info_dump)

