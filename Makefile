# to improve :
# 1. make a function that takes the source file and returns the actual path of the object file, then use it in defintion of BIN_OBJS and in the rule for linking object files


# Compiler
COMPILER = g++
FLAGS = -std=c++26 -O0
INCLUDES = -I D:\Programmig\C++\Libraries\headers -I header

SRC_DIRS = src
OBJ_DIRS = bin
EXEC_DIRS = $(OBJ_DIRS)


SRCS := $(wildcard $(SRC_DIRS)/*.cpp)
OBJS = $(patsubst $(SRC_DIRS)/%.cpp,$(OBJ_DIRS)/%.o,$(SRCS))
# BIN_OBJS = $(addprefix $(OBJ_DIRS)/, $(notdir $(OBJS)))
EXEC = wallpaper-changer.exe

print-vars:
	@echo SRCS: $(SRCS)
	@echo OBJS: $(OBJS)


# Default target
all: $(EXEC)

# Link object files to create the executable
$(EXEC): $(OBJS)
	$(COMPILER) $(FLAGS) -o $@ $(OBJS)

# Compile source files into object files
$(OBJ_DIRS)/%.o: $(SRC_DIRS)/%.cpp
	$(COMPILER) $(FLAGS) $(INCLUDES) -c $< -o $@

# Clean up build artifacts
clean:
	del $(BIN_OBJS) $(EXEC)





