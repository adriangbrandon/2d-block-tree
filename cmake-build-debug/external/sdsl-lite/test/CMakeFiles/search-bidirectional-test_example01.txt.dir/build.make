# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.14

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

# Utility rule file for search-bidirectional-test_example01.txt.

# Include the progress variables for this target.
include external/sdsl-lite/test/CMakeFiles/search-bidirectional-test_example01.txt.dir/progress.make

external/sdsl-lite/test/CMakeFiles/search-bidirectional-test_example01.txt: ../external/sdsl-lite/test/test_cases/example01.txt
external/sdsl-lite/test/CMakeFiles/search-bidirectional-test_example01.txt: ../external/sdsl-lite/test/tmp
external/sdsl-lite/test/CMakeFiles/search-bidirectional-test_example01.txt: external/sdsl-lite/test/search_bidirectional_test
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --blue --bold --progress-dir=/Users/adrian/CLionProjects/2d-block-tree/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Execute search-bidirectional-test on example01.txt."
	/Users/adrian/CLionProjects/2d-block-tree/cmake-build-debug/external/sdsl-lite/test/search_bidirectional_test /Users/adrian/CLionProjects/2d-block-tree/external/sdsl-lite/test/test_cases/example01.txt /Users/adrian/CLionProjects/2d-block-tree/external/sdsl-lite/test/tmp/search_bidirectional_test_example01.txt /Users/adrian/CLionProjects/2d-block-tree/external/sdsl-lite/test/tmp

search-bidirectional-test_example01.txt: external/sdsl-lite/test/CMakeFiles/search-bidirectional-test_example01.txt
search-bidirectional-test_example01.txt: external/sdsl-lite/test/CMakeFiles/search-bidirectional-test_example01.txt.dir/build.make

.PHONY : search-bidirectional-test_example01.txt

# Rule to build all files generated by this target.
external/sdsl-lite/test/CMakeFiles/search-bidirectional-test_example01.txt.dir/build: search-bidirectional-test_example01.txt

.PHONY : external/sdsl-lite/test/CMakeFiles/search-bidirectional-test_example01.txt.dir/build

external/sdsl-lite/test/CMakeFiles/search-bidirectional-test_example01.txt.dir/clean:
	cd /Users/adrian/CLionProjects/2d-block-tree/cmake-build-debug/external/sdsl-lite/test && $(CMAKE_COMMAND) -P CMakeFiles/search-bidirectional-test_example01.txt.dir/cmake_clean.cmake
.PHONY : external/sdsl-lite/test/CMakeFiles/search-bidirectional-test_example01.txt.dir/clean

external/sdsl-lite/test/CMakeFiles/search-bidirectional-test_example01.txt.dir/depend:
	cd /Users/adrian/CLionProjects/2d-block-tree/cmake-build-debug && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /Users/adrian/CLionProjects/2d-block-tree /Users/adrian/CLionProjects/2d-block-tree/external/sdsl-lite/test /Users/adrian/CLionProjects/2d-block-tree/cmake-build-debug /Users/adrian/CLionProjects/2d-block-tree/cmake-build-debug/external/sdsl-lite/test /Users/adrian/CLionProjects/2d-block-tree/cmake-build-debug/external/sdsl-lite/test/CMakeFiles/search-bidirectional-test_example01.txt.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : external/sdsl-lite/test/CMakeFiles/search-bidirectional-test_example01.txt.dir/depend

