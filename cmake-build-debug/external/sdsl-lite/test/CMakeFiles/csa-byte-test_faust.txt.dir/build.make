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

# Utility rule file for csa-byte-test_faust.txt.

# Include the progress variables for this target.
include external/sdsl-lite/test/CMakeFiles/csa-byte-test_faust.txt.dir/progress.make

external/sdsl-lite/test/CMakeFiles/csa-byte-test_faust.txt: ../external/sdsl-lite/test/test_cases/faust.txt
external/sdsl-lite/test/CMakeFiles/csa-byte-test_faust.txt: ../external/sdsl-lite/test/tmp
external/sdsl-lite/test/CMakeFiles/csa-byte-test_faust.txt: external/sdsl-lite/test/csa_byte_test
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --blue --bold --progress-dir=/Users/adrian/CLionProjects/2d-block-tree/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Execute csa-byte-test on faust.txt."
	/Users/adrian/CLionProjects/2d-block-tree/cmake-build-debug/external/sdsl-lite/test/csa_byte_test /Users/adrian/CLionProjects/2d-block-tree/external/sdsl-lite/test/test_cases/faust.txt /Users/adrian/CLionProjects/2d-block-tree/external/sdsl-lite/test/tmp/csa_byte_test_faust.txt /Users/adrian/CLionProjects/2d-block-tree/external/sdsl-lite/test/tmp

../external/sdsl-lite/test/test_cases/faust.txt:
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --blue --bold --progress-dir=/Users/adrian/CLionProjects/2d-block-tree/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Checking or downloading test case faust.txt."
	cd /Users/adrian/CLionProjects/2d-block-tree/external/sdsl-lite/test && /Applications/CLion.app/Contents/bin/cmake/mac/bin/cmake -Ddownload_url=http://algo2.iti.kit.edu/gog/sdsl/faust.txt.tar.gz -Dtest_case_dir=/Users/adrian/CLionProjects/2d-block-tree/external/sdsl-lite/test/test_cases -P /Users/adrian/CLionProjects/2d-block-tree/external/sdsl-lite/test/download.cmake

csa-byte-test_faust.txt: external/sdsl-lite/test/CMakeFiles/csa-byte-test_faust.txt
csa-byte-test_faust.txt: ../external/sdsl-lite/test/test_cases/faust.txt
csa-byte-test_faust.txt: external/sdsl-lite/test/CMakeFiles/csa-byte-test_faust.txt.dir/build.make

.PHONY : csa-byte-test_faust.txt

# Rule to build all files generated by this target.
external/sdsl-lite/test/CMakeFiles/csa-byte-test_faust.txt.dir/build: csa-byte-test_faust.txt

.PHONY : external/sdsl-lite/test/CMakeFiles/csa-byte-test_faust.txt.dir/build

external/sdsl-lite/test/CMakeFiles/csa-byte-test_faust.txt.dir/clean:
	cd /Users/adrian/CLionProjects/2d-block-tree/cmake-build-debug/external/sdsl-lite/test && $(CMAKE_COMMAND) -P CMakeFiles/csa-byte-test_faust.txt.dir/cmake_clean.cmake
.PHONY : external/sdsl-lite/test/CMakeFiles/csa-byte-test_faust.txt.dir/clean

external/sdsl-lite/test/CMakeFiles/csa-byte-test_faust.txt.dir/depend:
	cd /Users/adrian/CLionProjects/2d-block-tree/cmake-build-debug && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /Users/adrian/CLionProjects/2d-block-tree /Users/adrian/CLionProjects/2d-block-tree/external/sdsl-lite/test /Users/adrian/CLionProjects/2d-block-tree/cmake-build-debug /Users/adrian/CLionProjects/2d-block-tree/cmake-build-debug/external/sdsl-lite/test /Users/adrian/CLionProjects/2d-block-tree/cmake-build-debug/external/sdsl-lite/test/CMakeFiles/csa-byte-test_faust.txt.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : external/sdsl-lite/test/CMakeFiles/csa-byte-test_faust.txt.dir/depend
