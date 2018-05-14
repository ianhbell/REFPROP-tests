cmake_minimum_required (VERSION 3.0)
project (REFPROPTests)

add_subdirectory("${CMAKE_SOURCE_DIR}/externals/boost-cmake")

set(CMAKE_CXX_STANDARD 11)
include_directories("${CMAKE_SOURCE_DIR}/externals/Catch/single_include")
include_directories("${CMAKE_SOURCE_DIR}/externals/REFPROP-headers/")
include_directories("${CMAKE_SOURCE_DIR}/externals/manyso/include")
include_directories("${CMAKE_SOURCE_DIR}/externals/manyso/externals/MemoryModule")
include_directories("${CMAKE_SOURCE_DIR}/include/")

# Turn on more warnings in VS and turn off secure deprecation warnings
if(MSVC)
    add_definitions(-W4)
    add_definitions(-D_CRT_SECURE_NO_WARNINGS)

    if (NOT CMAKE_SIZEOF_VOID_P EQUAL 8)
        add_definitions(-DIS_32_BIT)
    endif ()
endif()

# Make a tiny static library of MemoryModule core (only for windows)
if (WIN32)
    add_library(MemoryModuleLib STATIC "${CMAKE_SOURCE_DIR}/externals/manyso/externals/MemoryModule/MemoryModule.c")
endif()

# The main EXE
add_executable(main main.cpp transport.cpp GERG2008.cpp XLS.cpp)
# The required dependencies
target_link_libraries(main Boost::filesystem)
# For windows, the MemoryModule too
if (WIN32)
    target_link_libraries(main MemoryModuleLib)
endif()
if ("${CMAKE_SYSTEM_NAME}" MATCHES "Linux")
    target_link_libraries(main ${CMAKE_DL_LIBS})
endif()