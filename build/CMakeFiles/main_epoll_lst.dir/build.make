# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.10

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
CMAKE_COMMAND = /usr/bin/cmake

# The command to remove a file.
RM = /usr/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/zhuyukuan/my_code/mytimer

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/zhuyukuan/my_code/mytimer/build

# Include any dependencies generated for this target.
include CMakeFiles/main_epoll_lst.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/main_epoll_lst.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/main_epoll_lst.dir/flags.make

CMakeFiles/main_epoll_lst.dir/main_epoll_lst.cpp.o: CMakeFiles/main_epoll_lst.dir/flags.make
CMakeFiles/main_epoll_lst.dir/main_epoll_lst.cpp.o: ../main_epoll_lst.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/zhuyukuan/my_code/mytimer/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/main_epoll_lst.dir/main_epoll_lst.cpp.o"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/main_epoll_lst.dir/main_epoll_lst.cpp.o -c /home/zhuyukuan/my_code/mytimer/main_epoll_lst.cpp

CMakeFiles/main_epoll_lst.dir/main_epoll_lst.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/main_epoll_lst.dir/main_epoll_lst.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/zhuyukuan/my_code/mytimer/main_epoll_lst.cpp > CMakeFiles/main_epoll_lst.dir/main_epoll_lst.cpp.i

CMakeFiles/main_epoll_lst.dir/main_epoll_lst.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/main_epoll_lst.dir/main_epoll_lst.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/zhuyukuan/my_code/mytimer/main_epoll_lst.cpp -o CMakeFiles/main_epoll_lst.dir/main_epoll_lst.cpp.s

CMakeFiles/main_epoll_lst.dir/main_epoll_lst.cpp.o.requires:

.PHONY : CMakeFiles/main_epoll_lst.dir/main_epoll_lst.cpp.o.requires

CMakeFiles/main_epoll_lst.dir/main_epoll_lst.cpp.o.provides: CMakeFiles/main_epoll_lst.dir/main_epoll_lst.cpp.o.requires
	$(MAKE) -f CMakeFiles/main_epoll_lst.dir/build.make CMakeFiles/main_epoll_lst.dir/main_epoll_lst.cpp.o.provides.build
.PHONY : CMakeFiles/main_epoll_lst.dir/main_epoll_lst.cpp.o.provides

CMakeFiles/main_epoll_lst.dir/main_epoll_lst.cpp.o.provides.build: CMakeFiles/main_epoll_lst.dir/main_epoll_lst.cpp.o


# Object files for target main_epoll_lst
main_epoll_lst_OBJECTS = \
"CMakeFiles/main_epoll_lst.dir/main_epoll_lst.cpp.o"

# External object files for target main_epoll_lst
main_epoll_lst_EXTERNAL_OBJECTS =

main_epoll_lst: CMakeFiles/main_epoll_lst.dir/main_epoll_lst.cpp.o
main_epoll_lst: CMakeFiles/main_epoll_lst.dir/build.make
main_epoll_lst: CMakeFiles/main_epoll_lst.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/zhuyukuan/my_code/mytimer/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable main_epoll_lst"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/main_epoll_lst.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/main_epoll_lst.dir/build: main_epoll_lst

.PHONY : CMakeFiles/main_epoll_lst.dir/build

CMakeFiles/main_epoll_lst.dir/requires: CMakeFiles/main_epoll_lst.dir/main_epoll_lst.cpp.o.requires

.PHONY : CMakeFiles/main_epoll_lst.dir/requires

CMakeFiles/main_epoll_lst.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/main_epoll_lst.dir/cmake_clean.cmake
.PHONY : CMakeFiles/main_epoll_lst.dir/clean

CMakeFiles/main_epoll_lst.dir/depend:
	cd /home/zhuyukuan/my_code/mytimer/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/zhuyukuan/my_code/mytimer /home/zhuyukuan/my_code/mytimer /home/zhuyukuan/my_code/mytimer/build /home/zhuyukuan/my_code/mytimer/build /home/zhuyukuan/my_code/mytimer/build/CMakeFiles/main_epoll_lst.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/main_epoll_lst.dir/depend

