ROOT_GENERATE_DICTIONARY(SiStripIO ${CMAKE_SOURCE_DIR}/sistrip_io/SiStripIOHeaders.h LINKDEF ${CMAKE_SOURCE_DIR}/sistrip_io/SiStripIOLinkDef.hxx)

add_library(sistrip_io SHARED ${CMAKE_SOURCE_DIR}/sistrip_io/SiStripIOSource.cxx SiStripIO.cxx)
target_link_libraries(sistrip_io PRIVATE ROOT::Core)
target_include_directories(sistrip_io PRIVATE ${ROOT_INCLUDE_DIRS} ${CMAKE_SOURCE_DIR}/sistrip_io)