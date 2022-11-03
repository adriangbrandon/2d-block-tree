cmake_minimum_required(VERSION 3.9)
set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/CMakeModules")
include(AppendCompilerFlags)

project(2d-block-tree)
set(PROJECT_VENDOR "Adrian Gomez Brandon")
set(CMAKE_PROJECT_NAME "2d-block-tree")
set(PROJECT_CONTACT "adrian.gbrandon@udc.es")


#Change to "Debug" when you need it
set(CMAKE_BUILD_TYPE "Release")

# C++11 compiler Check
if(NOT CMAKE_CXX_COMPILER_VERSION) # work around for cmake versions smaller than 2.8.10
    execute_process(COMMAND ${CMAKE_CXX_COMPILER} -dumpversion OUTPUT_VARIABLE CMAKE_CXX_COMPILER_VERSION)
endif()
if(CMAKE_CXX_COMPILER MATCHES ".*clang" OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    set(CMAKE_COMPILER_IS_CLANGXX 1)
endif()
if( (CMAKE_COMPILER_IS_GNUCXX AND ${CMAKE_CXX_COMPILER_VERSION} VERSION_LESS 4.7) OR
(CMAKE_COMPILER_IS_CLANGXX AND ${CMAKE_CXX_COMPILER_VERSION} VERSION_LESS 3.2))
    message(FATAL_ERROR "Your C++ compiler does not support C++11. Please install g++ 4.7 (or greater) or clang 3.2 (or greater)")
else()
    message(STATUS "Compiler is recent enough to support C++11.")
endif()
if( CMAKE_COMPILER_IS_GNUCXX )
    append_cxx_compiler_flags("-std=c++11 -Wall -Wextra " "GCC" CMAKE_CXX_FLAGS)
    append_cxx_compiler_flags("-msse4.2 -O3 -ffast-math -funroll-loops -fno-omit-frame-pointer -g" "GCC" CMAKE_CXX_FLAGS_RELEASE)
else()
    append_cxx_compiler_flags("-std=c++11" "CLANG" CMAKE_CXX_FLAGS)
    append_cxx_compiler_flags("-stdlib=libc++" "CLANG" CMAKE_CXX_FLAGS)
    append_cxx_compiler_flags("-msse4.2 -O3 -ffast-math -funroll-loops -DNDEBUG" "CLANG" CMAKE_CXX_FLAGS_RELEASE)
endif()
include(CheckSSE)
FindSSE ()
if( SSE4_2_FOUND )
    if( CMAKE_COMPILER_IS_GNUCXX )
        append_cxx_compiler_flags("-msse4.2" "GCC" CMAKE_CXX_FLAGS)
    else()
        append_cxx_compiler_flags("-msse4.2" "CLANG" CMAKE_CXX_FLAGS)
    endif()
    message(STATUS "CPU does support SSE4.2.")
else()
    message(STATUS "CPU does NOT support SSE4.2")
endif()

add_subdirectory(external/sdsl-lite)

include_directories("${PROJECT_SOURCE_DIR}/external/googletest/include"
        ${CMAKE_HOME_DIRECTORY}/include
        ${CMAKE_BINARY_DIR}/external/sdsl-lite/include
        ${CMAKE_BINARY_DIR}/external/sdsl-lite/external/libdivsufsort/include/
        ${CMAKE_HOME_DIRECTORY}/external/googletest/include
        ${CMAKE_HOME_DIRECTORY}/external/sdsl-lite/external/FastPFor/headers/
        ${STXXL_INCLUDE_DIRS}
        )

find_package(OpenMP)

add_executable(build_webgraph src/build_webgraph.cpp)
target_link_libraries(build_webgraph sdsl divsufsort divsufsort64)

add_executable(build_webgraph_comp_leaves src/build_webgraph_comp_leaves.cpp)
target_link_libraries(build_webgraph_comp_leaves sdsl divsufsort divsufsort64)

add_executable(build_raster src/build_raster.cpp)
target_link_libraries(build_raster sdsl divsufsort divsufsort64)

add_executable(build_k2tree src/build_k2tree.cpp)
target_link_libraries(build_k2tree sdsl divsufsort divsufsort64)

add_executable(build_k2tree_v1 src/build_k2tree_v1.cpp)
target_link_libraries(build_k2tree_v1 sdsl divsufsort divsufsort64)

add_executable(times src/times.cpp)
target_link_libraries(times sdsl divsufsort divsufsort64)

add_executable(times_direct src/times_direct.cpp)
target_link_libraries(times_direct sdsl divsufsort divsufsort64)

add_executable(times_direct_cl src/times_direct_cl.cpp)
target_link_libraries(times_direct_cl sdsl divsufsort divsufsort64)

add_executable(times_k2_direct src/times_k2_direct.cpp)
target_link_libraries(times_k2_direct sdsl divsufsort divsufsort64)

add_executable(times_access src/times_access.cpp)
target_link_libraries(times_access sdsl divsufsort divsufsort64)

add_executable(times_access_cl src/times_access_cl.cpp)
target_link_libraries(times_access_cl sdsl divsufsort divsufsort64)

add_executable(times_k2_access src/times_k2_access.cpp)
target_link_libraries(times_k2_access sdsl divsufsort divsufsort64)

add_executable(times_regions src/times_regions.cpp)
target_link_libraries(times_regions sdsl divsufsort divsufsort64)

add_executable(times_regions_cl src/times_regions_cl.cpp)
target_link_libraries(times_regions_cl sdsl divsufsort divsufsort64)

add_executable(times_k2_regions src/times_k2_regions.cpp)
target_link_libraries(times_k2_regions sdsl divsufsort divsufsort64)

add_executable(times_reverse src/times_reverse.cpp)
target_link_libraries(times_reverse sdsl divsufsort divsufsort64)

add_executable(times_reverse_cl src/times_reverse_cl.cpp)
target_link_libraries(times_reverse_cl sdsl divsufsort divsufsort64)

add_executable(times_k2_reverse src/times_k2_reverse.cpp)
target_link_libraries(times_k2_reverse sdsl divsufsort divsufsort64)

add_executable(size src/size.cpp)
target_link_libraries(size sdsl divsufsort divsufsort64)

add_executable(size_cl src/size_cl.cpp)
target_link_libraries(size_cl sdsl divsufsort divsufsort64)

add_executable(build_raster_comp_leaves src/build_raster_comp_leaves.cpp )
target_link_libraries(build_raster_comp_leaves sdsl divsufsort divsufsort64)

add_executable(times_range src/times_range.cpp )
target_link_libraries(times_range sdsl divsufsort divsufsort64)

add_executable(times_range_comp_leaves src/times_range_comp_leaves.cpp )
target_link_libraries(times_range_comp_leaves sdsl divsufsort divsufsort64)

add_executable(times_cell src/times_cell.cpp )
target_link_libraries(times_cell sdsl divsufsort divsufsort64)

add_executable(build_k2tree_raster src/build_k2tree_raster.cpp )
target_link_libraries(build_k2tree_raster sdsl divsufsort divsufsort64)

add_executable(times_k2_acum src/times_k2_acum.cpp)
target_link_libraries(times_k2_acum sdsl divsufsort divsufsort64)