# Specify the compiler to use
CC = g++

# Specify the flags to use during compilation
# CFLAGS = -std=c++17 -Wall -O0 -g
CFLAGS = -std=c++17 -Wall -O3

# Specify the path to the project's source files
SRC_DIR = src

# Specify the search paths for source files
VPATH = $(SRC_DIR)

# Specify the path to the project's header files
INCLUDE_DIR = inc

# Specify the search paths for header files
INCLUDE_PATHS = -I $(INCLUDE_DIR)

# Specify the path to the project's object files
OBJ_DIR = obj
OUT_DIR = out
DRAW_DIR = draw

# Specify the name of the executable to generate
EXE = router

# Specify the names of the source files
SRC_FILES = $(wildcard $(SRC_DIR)/*.cpp)

# Specify the names of the object files
OBJ_FILES = $(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(SRC_FILES))

# Generate the list of header files to include
HEADER_FILES = $(patsubst $(SRC_DIR)/%.cpp,$(INCLUDE_DIR)/%.hpp,$(SRC_FILES))

.PHONY: clean clean_out 

# The default target of the makefile
all: DIRS $(EXE)
# The rule to build pre-requisite directories
DIRS: $(OBJ_DIR) $(OUT_DIR) $(DRAW_DIR)

$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

$(OUT_DIR):
	mkdir -p $(OUT_DIR)

$(DRAW_DIR):
	mkdir -p $(DRAW_DIR)

# The rule to build the executable
$(EXE): $(OBJ_FILES) $(OBJ_DIR)/main.o
	$(CC) $(CFLAGS) $(INCLUDE_PATHS) -o $@ $^

# The rule to build the object files
$(OBJ_DIR)/%.o: %.cpp
	$(CC) $(CFLAGS) $(INCLUDE_PATHS) -c -o $@ $<

$(OBJ_DIR)/main.o: main.cpp
	$(CC) $(CFLAGS) $(INCLUDE_PATHS) -c -o $@ $<



# A rule to clean up the project directory
clean:
	rm -f $(OBJ_DIR)/*.o $(EXE)
clean_out:
	rm out/* draw/*