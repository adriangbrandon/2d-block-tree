cmake_minimum_required(VERSION 2.8.7)
set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/CMakeModules")
include(AppendCompilerFlags)

project(2d-block-tree)
set(PROJECT_VENDOR "Adrian Gomez Brandon")
set(CMAKE_PROJECT_NAME "2d-block-tree")
set(PROJECT_CONTACT "adrian.gbrandon@udc.es")


#Change to "Debug" when you need it
set(CMAKE_BUILD_TYPE "Debug")

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



add_executable(karp_rabin_v2 src/karp_rabin_v2.cpp)
target_link_libraries(karp_rabin_v2 sdsl divsufsort divsufsort64)


#add_executable(algorithm_test test/algorithm_test.cpp)
#target_link_libraries(algorithm_test sdsl divsufsort divsufsort64)

add_executable(block_tree_test test/block_tree_test.cpp)
target_link_libraries(block_tree_test sdsl divsufsort divsufsort64)


#add_executable(block_tree_double_hybrid_skipping_block_test test/block_tree_double_hybrid_skipping_block_test.cpp)
#target_link_libraries(block_tree_double_hybrid_skipping_block_test sdsl divsufsort divsufsort64)



add_executable(hash_table src/hash_table.cpp)
target_link_libraries(hash_table sdsl divsufsort divsufsort64)

#add_executable(algorithm src/algorithm.cpp)
#target_link_libraries(algorithm sdsl divsufsort divsufsort64)

#add_executable(algorithm_v2 src/algorithm_v2.cpp)
#target_link_libraries(algorithm_v2 sdsl divsufsort divsufsort64)



add_executable(build_webgraph src/build_webgraph.cpp)
target_link_libraries(build_webgraph sdsl divsufsort divsufsort64)

add_executable(build_raster src/build_raster.cpp)
target_link_libraries(build_raster sdsl divsufsort divsufsort64)

add_executable(build_raster_log src/build_raster_log.cpp)
target_link_libraries(build_raster_log sdsl divsufsort divsufsort64)

add_executable(build_k2tree src/build_k2tree.cpp)
target_link_libraries(build_k2tree sdsl divsufsort divsufsort64)

add_executable(build_k2tree_v1 src/build_k2tree_v1.cpp)
target_link_libraries(build_k2tree_v1 sdsl divsufsort divsufsort64)

add_executable(build_k2_tree_test test/build_k2_tree_test.cpp)
target_link_libraries(build_k2_tree_test sdsl divsufsort divsufsort64)


add_executable(times src/times.cpp)
target_link_libraries(times sdsl divsufsort divsufsort64)

add_executable(times_god_level src/times_god_level.cpp)
target_link_libraries(times_god_level sdsl divsufsort divsufsort64)

add_executable(check_blocks src/check_blocks.cpp)
target_link_libraries(check_blocks sdsl divsufsort divsufsort64)

add_executable(proba test/proba.cpp)
target_link_libraries(proba sdsl divsufsort divsufsort64)

add_executable(segmentation src/segmentation.cpp)
target_link_libraries(segmentation sdsl divsufsort divsufsort64)

add_executable(test_neighbors src/test_neighbors.cpp)
target_link_libraries(test_neighbors sdsl divsufsort divsufsort64)

add_executable(times_direct src/times_direct.cpp)
target_link_libraries(times_direct sdsl divsufsort divsufsort64)

add_executable(times_k2_direct src/times_k2_direct.cpp)
target_link_libraries(times_k2_direct sdsl divsufsort divsufsort64)

add_executable(times_access src/times_access.cpp)
target_link_libraries(times_access sdsl divsufsort divsufsort64)

add_executable(times_k2_access src/times_k2_access.cpp)
target_link_libraries(times_k2_access sdsl divsufsort divsufsort64)

add_executable(times_reverse src/times_reverse.cpp)
target_link_libraries(times_reverse sdsl divsufsort divsufsort64)

add_executable(times_k2_reverse src/times_k2_reverse.cpp)
target_link_libraries(times_k2_reverse sdsl divsufsort divsufsort64)

add_executable(info_block_tree src/info_block_tree.cpp)
target_link_libraries(info_block_tree sdsl divsufsort divsufsort64)

add_executable(size src/size.cpp)
target_link_libraries(size sdsl divsufsort divsufsort64)

add_executable(experiment_files src/experiment_files.cpp )
target_link_libraries(experiment_files sdsl divsufsort divsufsort64)