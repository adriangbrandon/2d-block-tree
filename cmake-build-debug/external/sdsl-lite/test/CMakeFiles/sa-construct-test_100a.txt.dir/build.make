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

# Utility rule file for sa-construct-test_100a.txt.

# Include the progress variables for this target.
include external/sdsl-lite/test/CMakeFiles/sa-construct-test_100a.txt.dir/progress.make

external/sdsl-lite/test/CMakeFiles/sa-construct-test_100a.txt: ../external/sdsl-lite/test/test_cases/100a.txt
external/sdsl-lite/test/CMakeFiles/sa-construct-test_100a.txt: ../external/sdsl-lite/test/tmp
external/sdsl-lite/test/CMakeFiles/sa-construct-test_100a.txt: external/sdsl-lite/test/sa_construct_test
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --blue --bold --progress-dir=/Users/adrian/CLionProjects/2d-block-tree/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Execute sa-construct-test on 100a.txt."
	/Users/adrian/CLionProjects/2d-block-tree/cmake-build-debug/external/sdsl-lite/test/sa_construct_test /Users/adrian/CLionProjects/2d-block-tree/external/sdsl-lite/test/test_cases/100a.txt /Users/adrian/CLionProjects/2d-block-tree/external/sdsl-lite/test/tmp/sa_construct_test_100a.txt /Users/adrian/CLionProjects/2d-block-tree/external/sdsl-lite/test/tmp

sa-construct-test_100a.txt: external/sdsl-lite/test/CMakeFiles/sa-construct-test_100a.txt
sa-construct-test_100a.txt: external/sdsl-lite/test/CMakeFiles/sa-construct-test_100a.txt.dir/build.make

.PHONY : sa-construct-test_100a.txt

# Rule to build all files generated by this target.
external/sdsl-lite/test/CMakeFiles/sa-construct-test_100a.txt.dir/build: sa-construct-test_100a.txt

.PHONY : external/sdsl-lite/test/CMakeFiles/sa-construct-test_100a.txt.dir/build

external/sdsl-lite/test/CMakeFiles/sa-construct-test_100a.txt.dir/clean:
	cd /Users/adrian/CLionProjects/2d-block-tree/cmake-build-debug/external/sdsl-lite/test && $(CMAKE_COMMAND) -P CMakeFiles/sa-construct-test_100a.txt.dir/cmake_clean.cmake
.PHONY : external/sdsl-lite/test/CMakeFiles/sa-construct-test_100a.txt.dir/clean

external/sdsl-lite/test/CMakeFiles/sa-construct-test_100a.txt.dir/depend:
	cd /Users/adrian/CLionProjects/2d-block-tree/cmake-build-debug && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /Users/adrian/CLionProjects/2d-block-tree /Users/adrian/CLionProjects/2d-block-tree/external/sdsl-lite/test /Users/adrian/CLionProjects/2d-block-tree/cmake-build-debug /Users/adrian/CLionProjects/2d-block-tree/cmake-build-debug/external/sdsl-lite/test /Users/adrian/CLionProjects/2d-block-tree/cmake-build-debug/external/sdsl-lite/test/CMakeFiles/sa-construct-test_100a.txt.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : external/sdsl-lite/test/CMakeFiles/sa-construct-test_100a.txt.dir/depend

