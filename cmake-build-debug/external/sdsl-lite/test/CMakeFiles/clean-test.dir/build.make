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

# Utility rule file for clean-test.

# Include the progress variables for this target.
include external/sdsl-lite/test/CMakeFiles/clean-test.dir/progress.make

external/sdsl-lite/test/CMakeFiles/clean-test:
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --blue --bold --progress-dir=/Users/adrian/CLionProjects/2d-block-tree/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Remove generated and downloaded test inputs."
	cd /Users/adrian/CLionProjects/2d-block-tree/cmake-build-debug/external/sdsl-lite/test && /Applications/CLion.app/Contents/bin/cmake/mac/bin/cmake -E remove -f /Users/adrian/CLionProjects/2d-block-tree/external/sdsl-lite/test/test_cases/faust.txt /Users/adrian/CLionProjects/2d-block-tree/external/sdsl-lite/test/test_cases/zarathustra.txt /Users/adrian/CLionProjects/2d-block-tree/external/sdsl-lite/test/test_cases/moby.int /Users/adrian/CLionProjects/2d-block-tree/external/sdsl-lite/test/test_cases/int-vec.0.1.0 /Users/adrian/CLionProjects/2d-block-tree/external/sdsl-lite/test/test_cases/int-vec.1.1.0 /Users/adrian/CLionProjects/2d-block-tree/external/sdsl-lite/test/test_cases/int-vec.1000000.1.0 /Users/adrian/CLionProjects/2d-block-tree/external/sdsl-lite/test/test_cases/int-vec.1000000.1.1 /Users/adrian/CLionProjects/2d-block-tree/external/sdsl-lite/test/test_cases/int-vec.7.1.1 /Users/adrian/CLionProjects/2d-block-tree/external/sdsl-lite/test/test_cases/int-vec.8.1.0 /Users/adrian/CLionProjects/2d-block-tree/external/sdsl-lite/test/test_cases/int-vec.9.1.1 /Users/adrian/CLionProjects/2d-block-tree/external/sdsl-lite/test/test_cases/int-vec.10.1.0 /Users/adrian/CLionProjects/2d-block-tree/external/sdsl-lite/test/test_cases/int-vec.11.1.1 /Users/adrian/CLionProjects/2d-block-tree/external/sdsl-lite/test/test_cases/int-vec.12.1.0 /Users/adrian/CLionProjects/2d-block-tree/external/sdsl-lite/test/test_cases/int-vec.13.1.1 /Users/adrian/CLionProjects/2d-block-tree/external/sdsl-lite/test/test_cases/int-vec.14.1.0 /Users/adrian/CLionProjects/2d-block-tree/external/sdsl-lite/test/test_cases/int-vec.15.1.1 /Users/adrian/CLionProjects/2d-block-tree/external/sdsl-lite/test/test_cases/int-vec.8.1.r.17 /Users/adrian/CLionProjects/2d-block-tree/external/sdsl-lite/test/test_cases/int-vec.16.1.r.42 /Users/adrian/CLionProjects/2d-block-tree/external/sdsl-lite/test/test_cases/int-vec.32.1.r.111 /Users/adrian/CLionProjects/2d-block-tree/external/sdsl-lite/test/test_cases/int-vec.64.1.r.222 /Users/adrian/CLionProjects/2d-block-tree/external/sdsl-lite/test/test_cases/int-vec.128.1.r.73 /Users/adrian/CLionProjects/2d-block-tree/external/sdsl-lite/test/test_cases/int-vec.256.1.r.4887 /Users/adrian/CLionProjects/2d-block-tree/external/sdsl-lite/test/test_cases/int-vec.512.1.r.432 /Users/adrian/CLionProjects/2d-block-tree/external/sdsl-lite/test/test_cases/int-vec.1024.1.r.898 /Users/adrian/CLionProjects/2d-block-tree/external/sdsl-lite/test/test_cases/int-vec.2048.1.r.5432 /Users/adrian/CLionProjects/2d-block-tree/external/sdsl-lite/test/test_cases/int-vec.4096.1.r.793 /Users/adrian/CLionProjects/2d-block-tree/external/sdsl-lite/test/test_cases/int-vec.8192.1.r.1043 /Users/adrian/CLionProjects/2d-block-tree/external/sdsl-lite/test/test_cases/int-vec.1000000.1.r.815 /Users/adrian/CLionProjects/2d-block-tree/external/sdsl-lite/test/test_cases/bit-vec.CRAFTED-32 /Users/adrian/CLionProjects/2d-block-tree/external/sdsl-lite/test/test_cases/bit-vec.CRAFTED-SPARSE-0 /Users/adrian/CLionProjects/2d-block-tree/external/sdsl-lite/test/test_cases/bit-vec.CRAFTED-SPARSE-1 /Users/adrian/CLionProjects/2d-block-tree/external/sdsl-lite/test/test_cases/bit-vec.CRAFTED-BLOCK-0 /Users/adrian/CLionProjects/2d-block-tree/external/sdsl-lite/test/test_cases/bit-vec.CRAFTED-BLOCK-1 /Users/adrian/CLionProjects/2d-block-tree/external/sdsl-lite/test/test_cases/bit-vec.CRAFTED-MAT-SELECT /Users/adrian/CLionProjects/2d-block-tree/external/sdsl-lite/test/test_cases/int-vec-sa.100000.18.r /Users/adrian/CLionProjects/2d-block-tree/external/sdsl-lite/test/test_cases/int-vec.0.1.0 /Users/adrian/CLionProjects/2d-block-tree/external/sdsl-lite/test/test_cases/int-vec.1.1.0 /Users/adrian/CLionProjects/2d-block-tree/external/sdsl-lite/test/test_cases/int-vec.1000000.1.0 /Users/adrian/CLionProjects/2d-block-tree/external/sdsl-lite/test/test_cases/int-vec.1000000.1.1 /Users/adrian/CLionProjects/2d-block-tree/external/sdsl-lite/test/test_cases/int-vec.7.1.1 /Users/adrian/CLionProjects/2d-block-tree/external/sdsl-lite/test/test_cases/int-vec.8.1.0 /Users/adrian/CLionProjects/2d-block-tree/external/sdsl-lite/test/test_cases/int-vec.9.1.1 /Users/adrian/CLionProjects/2d-block-tree/external/sdsl-lite/test/test_cases/int-vec.10.1.0 /Users/adrian/CLionProjects/2d-block-tree/external/sdsl-lite/test/test_cases/int-vec.11.1.1 /Users/adrian/CLionProjects/2d-block-tree/external/sdsl-lite/test/test_cases/int-vec.12.1.0 /Users/adrian/CLionProjects/2d-block-tree/external/sdsl-lite/test/test_cases/int-vec.13.1.1 /Users/adrian/CLionProjects/2d-block-tree/external/sdsl-lite/test/test_cases/int-vec.14.1.0 /Users/adrian/CLionProjects/2d-block-tree/external/sdsl-lite/test/test_cases/int-vec.15.1.1 /Users/adrian/CLionProjects/2d-block-tree/external/sdsl-lite/test/test_cases/int-vec.8.1.r.17 /Users/adrian/CLionProjects/2d-block-tree/external/sdsl-lite/test/test_cases/int-vec.16.1.r.42 /Users/adrian/CLionProjects/2d-block-tree/external/sdsl-lite/test/test_cases/int-vec.32.1.r.111 /Users/adrian/CLionProjects/2d-block-tree/external/sdsl-lite/test/test_cases/int-vec.64.1.r.222 /Users/adrian/CLionProjects/2d-block-tree/external/sdsl-lite/test/test_cases/int-vec.128.1.r.73 /Users/adrian/CLionProjects/2d-block-tree/external/sdsl-lite/test/test_cases/int-vec.256.1.r.4887 /Users/adrian/CLionProjects/2d-block-tree/external/sdsl-lite/test/test_cases/int-vec.512.1.r.432 /Users/adrian/CLionProjects/2d-block-tree/external/sdsl-lite/test/test_cases/int-vec.1024.1.r.898 /Users/adrian/CLionProjects/2d-block-tree/external/sdsl-lite/test/test_cases/int-vec.2048.1.r.5432 /Users/adrian/CLionProjects/2d-block-tree/external/sdsl-lite/test/test_cases/int-vec.4096.1.r.793 /Users/adrian/CLionProjects/2d-block-tree/external/sdsl-lite/test/test_cases/int-vec.8192.1.r.1043 /Users/adrian/CLionProjects/2d-block-tree/external/sdsl-lite/test/test_cases/int-vec.1000000.1.r.815 /Users/adrian/CLionProjects/2d-block-tree/external/sdsl-lite/test/test_cases/bit-vec.CRAFTED-32 /Users/adrian/CLionProjects/2d-block-tree/external/sdsl-lite/test/test_cases/bit-vec.CRAFTED-SPARSE-0 /Users/adrian/CLionProjects/2d-block-tree/external/sdsl-lite/test/test_cases/bit-vec.CRAFTED-SPARSE-1 /Users/adrian/CLionProjects/2d-block-tree/external/sdsl-lite/test/test_cases/bit-vec.CRAFTED-BLOCK-0 /Users/adrian/CLionProjects/2d-block-tree/external/sdsl-lite/test/test_cases/bit-vec.CRAFTED-BLOCK-1 /Users/adrian/CLionProjects/2d-block-tree/external/sdsl-lite/test/test_cases/bit-vec.CRAFTED-MAT-SELECT /Users/adrian/CLionProjects/2d-block-tree/external/sdsl-lite/test/test_cases/int-vec.0.1.0 /Users/adrian/CLionProjects/2d-block-tree/external/sdsl-lite/test/test_cases/int-vec.1.64.42 /Users/adrian/CLionProjects/2d-block-tree/external/sdsl-lite/test/test_cases/int-vec.1000000.64.i /Users/adrian/CLionProjects/2d-block-tree/external/sdsl-lite/test/test_cases/int-vec.1000000.32.i.42 /Users/adrian/CLionProjects/2d-block-tree/external/sdsl-lite/test/test_cases/int-vec.1000000.64.i.17 /Users/adrian/CLionProjects/2d-block-tree/external/sdsl-lite/test/test_cases/int-vec.0.1.0 /Users/adrian/CLionProjects/2d-block-tree/external/sdsl-lite/test/test_cases/int-vec.1.1.0 /Users/adrian/CLionProjects/2d-block-tree/external/sdsl-lite/test/test_cases/int-vec.1000000.1.0 /Users/adrian/CLionProjects/2d-block-tree/external/sdsl-lite/test/test_cases/int-vec.1000000.1.1 /Users/adrian/CLionProjects/2d-block-tree/external/sdsl-lite/test/test_cases/int-vec.7.1.1 /Users/adrian/CLionProjects/2d-block-tree/external/sdsl-lite/test/test_cases/int-vec.8.1.0 /Users/adrian/CLionProjects/2d-block-tree/external/sdsl-lite/test/test_cases/int-vec.9.1.1 /Users/adrian/CLionProjects/2d-block-tree/external/sdsl-lite/test/test_cases/int-vec.10.1.0 /Users/adrian/CLionProjects/2d-block-tree/external/sdsl-lite/test/test_cases/int-vec.11.1.1 /Users/adrian/CLionProjects/2d-block-tree/external/sdsl-lite/test/test_cases/int-vec.12.1.0 /Users/adrian/CLionProjects/2d-block-tree/external/sdsl-lite/test/test_cases/int-vec.13.1.1 /Users/adrian/CLionProjects/2d-block-tree/external/sdsl-lite/test/test_cases/int-vec.14.1.0 /Users/adrian/CLionProjects/2d-block-tree/external/sdsl-lite/test/test_cases/int-vec.15.1.1 /Users/adrian/CLionProjects/2d-block-tree/external/sdsl-lite/test/test_cases/int-vec.8.1.r.17 /Users/adrian/CLionProjects/2d-block-tree/external/sdsl-lite/test/test_cases/int-vec.16.1.r.42 /Users/adrian/CLionProjects/2d-block-tree/external/sdsl-lite/test/test_cases/int-vec.32.1.r.111 /Users/adrian/CLionProjects/2d-block-tree/external/sdsl-lite/test/test_cases/int-vec.64.1.r.222 /Users/adrian/CLionProjects/2d-block-tree/external/sdsl-lite/test/test_cases/int-vec.128.1.r.73 /Users/adrian/CLionProjects/2d-block-tree/external/sdsl-lite/test/test_cases/int-vec.256.1.r.4887 /Users/adrian/CLionProjects/2d-block-tree/external/sdsl-lite/test/test_cases/int-vec.512.1.r.432 /Users/adrian/CLionProjects/2d-block-tree/external/sdsl-lite/test/test_cases/int-vec.1024.1.r.898 /Users/adrian/CLionProjects/2d-block-tree/external/sdsl-lite/test/test_cases/int-vec.2048.1.r.5432 /Users/adrian/CLionProjects/2d-block-tree/external/sdsl-lite/test/test_cases/int-vec.4096.1.r.793 /Users/adrian/CLionProjects/2d-block-tree/external/sdsl-lite/test/test_cases/int-vec.8192.1.r.1043 /Users/adrian/CLionProjects/2d-block-tree/external/sdsl-lite/test/test_cases/int-vec.1000000.1.r.815 /Users/adrian/CLionProjects/2d-block-tree/external/sdsl-lite/test/test_cases/bit-vec.CRAFTED-32 /Users/adrian/CLionProjects/2d-block-tree/external/sdsl-lite/test/test_cases/bit-vec.CRAFTED-SPARSE-0 /Users/adrian/CLionProjects/2d-block-tree/external/sdsl-lite/test/test_cases/bit-vec.CRAFTED-SPARSE-1 /Users/adrian/CLionProjects/2d-block-tree/external/sdsl-lite/test/test_cases/bit-vec.CRAFTED-BLOCK-0 /Users/adrian/CLionProjects/2d-block-tree/external/sdsl-lite/test/test_cases/bit-vec.CRAFTED-BLOCK-1 /Users/adrian/CLionProjects/2d-block-tree/external/sdsl-lite/test/test_cases/bit-vec.CRAFTED-MAT-SELECT /Users/adrian/CLionProjects/2d-block-tree/external/sdsl-lite/test/test_cases/int-vec.0.1.0 /Users/adrian/CLionProjects/2d-block-tree/external/sdsl-lite/test/test_cases/int-vec.1023.1.0 /Users/adrian/CLionProjects/2d-block-tree/external/sdsl-lite/test/test_cases/int-vec.100023.1.0 /Users/adrian/CLionProjects/2d-block-tree/external/sdsl-lite/test/test_cases/int-vec.64.2.0 /Users/adrian/CLionProjects/2d-block-tree/external/sdsl-lite/test/test_cases/int-vec.100000.18.r

clean-test: external/sdsl-lite/test/CMakeFiles/clean-test
clean-test: external/sdsl-lite/test/CMakeFiles/clean-test.dir/build.make

.PHONY : clean-test

# Rule to build all files generated by this target.
external/sdsl-lite/test/CMakeFiles/clean-test.dir/build: clean-test

.PHONY : external/sdsl-lite/test/CMakeFiles/clean-test.dir/build

external/sdsl-lite/test/CMakeFiles/clean-test.dir/clean:
	cd /Users/adrian/CLionProjects/2d-block-tree/cmake-build-debug/external/sdsl-lite/test && $(CMAKE_COMMAND) -P CMakeFiles/clean-test.dir/cmake_clean.cmake
.PHONY : external/sdsl-lite/test/CMakeFiles/clean-test.dir/clean

external/sdsl-lite/test/CMakeFiles/clean-test.dir/depend:
	cd /Users/adrian/CLionProjects/2d-block-tree/cmake-build-debug && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /Users/adrian/CLionProjects/2d-block-tree /Users/adrian/CLionProjects/2d-block-tree/external/sdsl-lite/test /Users/adrian/CLionProjects/2d-block-tree/cmake-build-debug /Users/adrian/CLionProjects/2d-block-tree/cmake-build-debug/external/sdsl-lite/test /Users/adrian/CLionProjects/2d-block-tree/cmake-build-debug/external/sdsl-lite/test/CMakeFiles/clean-test.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : external/sdsl-lite/test/CMakeFiles/clean-test.dir/depend

