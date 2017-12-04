cmake_minimum_required (VERSION 2.8.11)
project (REFPROPTests)

set(CMAKE_CXX_STANDARD 11)
include_directories("${CMAKE_SOURCE_DIR}/externals/Eigen")
include_directories("${CMAKE_SOURCE_DIR}/externals/Catch/single_include")
include_directories("${CMAKE_SOURCE_DIR}/externals/REFPROP-headers/")

add_executable(main main.cpp)
if ("${CMAKE_SYSTEM_NAME}" MATCHES "Linux")
    target_link_libraries(main ${CMAKE_DL_LIBS})
endif()
