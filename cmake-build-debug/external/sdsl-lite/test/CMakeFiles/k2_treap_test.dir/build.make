# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.12

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:


#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:


# Remove some rules from gmake that .SUFFIXES does not remove.
SUFFIXES =

.SUFFIXES: .hpux_make_needs_suffix_list


# Suppress display of executed commands.
$(VERBOSE).SILENT:


# A target that is always out of date.
cmake_force:

.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = /Applications/CLion.app/Contents/bin/cmake/mac/bin/cmake

# The command to remove a file.
RM = /Applications/CLion.app/Contents/bin/cmake/mac/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /Users/adrian/CLionProjects/2d-block-tree

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /Users/adrian/CLionProjects/2d-block-tree/cmake-build-debug

# Include any dependencies generated for this target.
include external/sdsl-lite/test/CMakeFiles/k2_treap_test.dir/depend.make

# Include the progress variables for this target.
include external/sdsl-lite/test/CMakeFiles/k2_treap_test.dir/progress.make

# Include the compile flags for this target's objects.
include external/sdsl-lite/test/CMakeFiles/k2_treap_test.dir/flags.make

external/sdsl-lite/test/CMakeFiles/k2_treap_test.dir/k2_treap_test.cpp.o: external/sdsl-lite/test/CMakeFiles/k2_treap_test.dir/flags.make
external/sdsl-lite/test/CMakeFiles/k2_treap_test.dir/k2_treap_test.cpp.o: ../external/sdsl-lite/test/k2_treap_test.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/Users/adrian/CLionProjects/2d-block-tree/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object external/sdsl-lite/test/CMakeFiles/k2_treap_test.dir/k2_treap_test.cpp.o"
	cd /Users/adrian/CLionProjects/2d-block-tree/cmake-build-debug/external/sdsl-lite/test && /Library/Developer/CommandLineTools/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/k2_treap_test.dir/k2_treap_test.cpp.o -c /Users/adrian/CLionProjects/2d-block-tree/external/sdsl-lite/test/k2_treap_test.cpp

external/sdsl-lite/test/CMakeFiles/k2_treap_test.dir/k2_treap_test.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/k2_treap_test.dir/k2_treap_test.cpp.i"
	cd /Users/adrian/CLionProjects/2d-block-tree/cmake-build-debug/external/sdsl-lite/test && /Library/Developer/CommandLineTools/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /Users/adrian/CLionProjects/2d-block-tree/external/sdsl-lite/test/k2_treap_test.cpp > CMakeFiles/k2_treap_test.dir/k2_treap_test.cpp.i

external/sdsl-lite/test/CMakeFiles/k2_treap_test.dir/k2_treap_test.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/k2_treap_test.dir/k2_treap_test.cpp.s"
	cd /Users/adrian/CLionProjects/2d-block-tree/cmake-build-debug/external/sdsl-lite/test && /Library/Developer/CommandLineTools/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /Users/adrian/CLionProjects/2d-block-tree/external/sdsl-lite/test/k2_treap_test.cpp -o CMakeFiles/k2_treap_test.dir/k2_treap_test.cpp.s

# Object files for target k2_treap_test
k2_treap_test_OBJECTS = \
"CMakeFiles/k2_treap_test.dir/k2_treap_test.cpp.o"

# External object files for target k2_treap_test
k2_treap_test_EXTERNAL_OBJECTS =

external/sdsl-lite/test/k2_treap_test: external/sdsl-lite/test/CMakeFiles/k2_treap_test.dir/k2_treap_test.cpp.o
external/sdsl-lite/test/k2_treap_test: external/sdsl-lite/test/CMakeFiles/k2_treap_test.dir/build.make
external/sdsl-lite/test/k2_treap_test: external/sdsl-lite/lib/libsdsl.a
external/sdsl-lite/test/k2_treap_test: external/sdsl-lite/external/googletest/googletest/libgtest.a
external/sdsl-lite/test/k2_treap_test: external/sdsl-lite/external/libdivsufsort/lib/libdivsufsort.a
external/sdsl-lite/test/k2_treap_test: external/sdsl-lite/external/libdivsufsort/lib/libdivsufsort64.a
external/sdsl-lite/test/k2_treap_test: external/sdsl-lite/test/CMakeFiles/k2_treap_test.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/Users/adrian/CLionProjects/2d-block-tree/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable k2_treap_test"
	cd /Users/adrian/CLionProjects/2d-block-tree/cmake-build-debug/external/sdsl-lite/test && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/k2_treap_test.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
external/sdsl-lite/test/CMakeFiles/k2_treap_test.dir/build: external/sdsl-lite/test/k2_treap_test

.PHONY : external/sdsl-lite/test/CMakeFiles/k2_treap_test.dir/build

external/sdsl-lite/test/CMakeFiles/k2_treap_test.dir/clean:
	cd /Users/adrian/CLionProjects/2d-block-tree/cmake-build-debug/external/sdsl-lite/test && $(CMAKE_COMMAND) -P CMakeFiles/k2_treap_test.dir/cmake_clean.cmake
.PHONY : external/sdsl-lite/test/CMakeFiles/k2_treap_test.dir/clean

external/sdsl-lite/test/CMakeFiles/k2_treap_test.dir/depend:
	cd /Users/adrian/CLionProjects/2d-block-tree/cmake-build-debug && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /Users/adrian/CLionProjects/2d-block-tree /Users/adrian/CLionProjects/2d-block-tree/external/sdsl-lite/test /Users/adrian/CLionProjects/2d-block-tree/cmake-build-debug /Users/adrian/CLionProjects/2d-block-tree/cmake-build-debug/external/sdsl-lite/test /Users/adrian/CLionProjects/2d-block-tree/cmake-build-debug/external/sdsl-lite/test/CMakeFiles/k2_treap_test.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : external/sdsl-lite/test/CMakeFiles/k2_treap_test.dir/depend

