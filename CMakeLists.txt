cmake_minimum_required (VERSION 2.8.11)
project (REFPROPTests)

add_subdirectory("${CMAKE_SOURCE_DIR}/externals/boost-cmake")

set(CMAKE_CXX_STANDARD 11)
include_directories("${CMAKE_SOURCE_DIR}/externals/Catch/single_include")
include_directories("${CMAKE_SOURCE_DIR}/externals/REFPROP-headers/")
include_directories("${CMAKE_SOURCE_DIR}/externals/manyso/include")
include_directories("${CMAKE_SOURCE_DIR}/externals/manyso/externals/MemoryModule")
include_directories("${CMAKE_SOURCE_DIR}/include/")

# Make a tiny static library of MemoryModule core
add_library(MemoryModuleLib STATIC "${CMAKE_SOURCE_DIR}/externals/manyso/externals/MemoryModule/MemoryModule.c")

add_executable(main main.cpp)
target_link_libraries(main MemoryModuleLib Boost::filesystem)
if ("${CMAKE_SYSTEM_NAME}" MATCHES "Linux")
    target_link_libraries(main ${CMAKE_DL_LIBS})
endif()