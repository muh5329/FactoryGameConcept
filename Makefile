# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.29

# Default target executed when no arguments are given to make.
default_target: all
.PHONY : default_target

# Allow only one "make -f Makefile2" at a time, but pass parallelism.
.NOTPARALLEL:

#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:

# Disable VCS-based implicit rules.
% : %,v

# Disable VCS-based implicit rules.
% : RCS/%

# Disable VCS-based implicit rules.
% : RCS/%,v

# Disable VCS-based implicit rules.
% : SCCS/s.%

# Disable VCS-based implicit rules.
% : s.%

.SUFFIXES: .hpux_make_needs_suffix_list

# Command-line flag to silence nested $(MAKE).
$(VERBOSE)MAKESILENT = -s

#Suppress display of executed commands.
$(VERBOSE).SILENT:

# A target that is always out of date.
cmake_force:
.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = /opt/homebrew/Cellar/cmake/3.29.0/bin/cmake

# The command to remove a file.
RM = /opt/homebrew/Cellar/cmake/3.29.0/bin/cmake -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /Users/muneebhaq/Documents/projects/FactoryGameConcept

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /Users/muneebhaq/Documents/projects/FactoryGameConcept

#=============================================================================
# Targets provided globally by CMake.

# Special rule for the target edit_cache
edit_cache:
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --cyan "Running CMake cache editor..."
	/opt/homebrew/Cellar/cmake/3.29.0/bin/ccmake -S$(CMAKE_SOURCE_DIR) -B$(CMAKE_BINARY_DIR)
.PHONY : edit_cache

# Special rule for the target edit_cache
edit_cache/fast: edit_cache
.PHONY : edit_cache/fast

# Special rule for the target rebuild_cache
rebuild_cache:
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --cyan "Running CMake to regenerate build system..."
	/opt/homebrew/Cellar/cmake/3.29.0/bin/cmake --regenerate-during-build -S$(CMAKE_SOURCE_DIR) -B$(CMAKE_BINARY_DIR)
.PHONY : rebuild_cache

# Special rule for the target rebuild_cache
rebuild_cache/fast: rebuild_cache
.PHONY : rebuild_cache/fast

# The main all target
all: cmake_check_build_system
	$(CMAKE_COMMAND) -E cmake_progress_start /Users/muneebhaq/Documents/projects/FactoryGameConcept/CMakeFiles /Users/muneebhaq/Documents/projects/FactoryGameConcept//CMakeFiles/progress.marks
	$(MAKE) $(MAKESILENT) -f CMakeFiles/Makefile2 all
	$(CMAKE_COMMAND) -E cmake_progress_start /Users/muneebhaq/Documents/projects/FactoryGameConcept/CMakeFiles 0
.PHONY : all

# The main clean target
clean:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/Makefile2 clean
.PHONY : clean

# The main clean target
clean/fast: clean
.PHONY : clean/fast

# Prepare targets for installation.
preinstall: all
	$(MAKE) $(MAKESILENT) -f CMakeFiles/Makefile2 preinstall
.PHONY : preinstall

# Prepare targets for installation.
preinstall/fast:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/Makefile2 preinstall
.PHONY : preinstall/fast

# clear depends
depend:
	$(CMAKE_COMMAND) -S$(CMAKE_SOURCE_DIR) -B$(CMAKE_BINARY_DIR) --check-build-system CMakeFiles/Makefile.cmake 1
.PHONY : depend

#=============================================================================
# Target rules for targets named YourProject

# Build rule for target.
YourProject: cmake_check_build_system
	$(MAKE) $(MAKESILENT) -f CMakeFiles/Makefile2 YourProject
.PHONY : YourProject

# fast build rule for target.
YourProject/fast:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/YourProject.dir/build.make CMakeFiles/YourProject.dir/build
.PHONY : YourProject/fast

game.o: game.c.o
.PHONY : game.o

# target to build an object file
game.c.o:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/YourProject.dir/build.make CMakeFiles/YourProject.dir/game.c.o
.PHONY : game.c.o

game.i: game.c.i
.PHONY : game.i

# target to preprocess a source file
game.c.i:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/YourProject.dir/build.make CMakeFiles/YourProject.dir/game.c.i
.PHONY : game.c.i

game.s: game.c.s
.PHONY : game.s

# target to generate assembly for a file
game.c.s:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/YourProject.dir/build.make CMakeFiles/YourProject.dir/game.c.s
.PHONY : game.c.s

game2.o: game2.c.o
.PHONY : game2.o

# target to build an object file
game2.c.o:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/YourProject.dir/build.make CMakeFiles/YourProject.dir/game2.c.o
.PHONY : game2.c.o

game2.i: game2.c.i
.PHONY : game2.i

# target to preprocess a source file
game2.c.i:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/YourProject.dir/build.make CMakeFiles/YourProject.dir/game2.c.i
.PHONY : game2.c.i

game2.s: game2.c.s
.PHONY : game2.s

# target to generate assembly for a file
game2.c.s:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/YourProject.dir/build.make CMakeFiles/YourProject.dir/game2.c.s
.PHONY : game2.c.s

# Help Target
help:
	@echo "The following are some of the valid targets for this Makefile:"
	@echo "... all (the default if no target is provided)"
	@echo "... clean"
	@echo "... depend"
	@echo "... edit_cache"
	@echo "... rebuild_cache"
	@echo "... YourProject"
	@echo "... game.o"
	@echo "... game.i"
	@echo "... game.s"
	@echo "... game2.o"
	@echo "... game2.i"
	@echo "... game2.s"
.PHONY : help



#=============================================================================
# Special targets to cleanup operation of make.

# Special rule to run CMake to check the build system integrity.
# No rule that depends on this can have commands that come from listfiles
# because they might be regenerated.
cmake_check_build_system:
	$(CMAKE_COMMAND) -S$(CMAKE_SOURCE_DIR) -B$(CMAKE_BINARY_DIR) --check-build-system CMakeFiles/Makefile.cmake 0
.PHONY : cmake_check_build_system

