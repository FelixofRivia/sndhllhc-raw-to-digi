add_executable(raw_to_digi ${CMAKE_CURRENT_SOURCE_DIR}/src/raw_to_digi.cxx ${CMAKE_SOURCE_DIR}/sistrip_io/SiStripIOSource.cxx)
target_link_libraries(raw_to_digi PRIVATE ${ROOT_LIBRARIES})
target_include_directories(raw_to_digi PRIVATE ${ROOT_INCLUDE_DIRS} ${CMAKE_CURRENT_SOURCE_DIR}/include ${CMAKE_SOURCE_DIR}/sistrip_io)

ROOT_GENERATE_DICTIONARY(SiStripIODict_raw_to_digi ${CMAKE_SOURCE_DIR}/sistrip_io/SiStripIOHeaders.h LINKDEF ${CMAKE_SOURCE_DIR}/sistrip_io/SiStripIOLinkDef.hxx MODULE raw_to_digi)