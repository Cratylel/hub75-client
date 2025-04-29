# Makefile

# Set up the compiler and flags
CXX ?= c++
CXXFLAGS = -Wall -Wextra -O3 -std=c++17 -g -rdynamic -fPIC
LIB_PATH = ../../lib
LIB_SRC = $(LIB_PATH)/matrix.cpp
TARGET_DIR = .

# Find all .cpp files in the current directory
SRCS := $(wildcard *.cpp)
# Extract the base names of the source files (without extension)
BINS := $(patsubst %.cpp, $(TARGET_DIR)/%, $(SRCS))

# Default target is 'all'
all: $(BINS)

# Rule to create each binary
$(TARGET_DIR)/%: %.cpp $(LIB_SRC)
	@mkdir -p $(TARGET_DIR)
	$(CXX) $(CXXFLAGS) $< $(LIB_SRC) -o $@

# Clean up the generated binaries
clean:
	rm -rf $(BINS)

# Phony targets
.PHONY: all clean
