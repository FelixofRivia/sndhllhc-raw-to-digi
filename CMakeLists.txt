cmake_minimum_required(VERSION 3.23)

project(sndhllhc_raw_to_digi VERSION 0.1)

#find_package(ROOT CONFIG REQUIRED)

add_executable(raw_to_digi raw_to_digi.cxx)
#target_link_libraries(raw_to_digi PUBLIC ROOT::Physics)
