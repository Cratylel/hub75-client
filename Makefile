# Compiler and flags
CXX ?= g++
CXXFLAGS := -Wall -Wextra -O3 -std=c++17 -g -rdynamic -shared -fPIC
CXXFLAGS += $(if $(TEST_ENV),-DTEST_ENV)
CXXFLAGS += $(if $(LOG_LEVEL_DEBUG),-DLOG_LEVEL_DEBUG)
CXXFLAGS += $(if $(TEST_SERVER),-DTEST_SERVER)

# Directories
SRCDIR := lib
OBJDIR := build

# Source and object files
#SOURCES := $(wildcard $(SRCDIR)/*.cpp)
SOURCES := lib/matrix.cpp
SOURCES += lib/main.cpp #temp testing main file
OBJECTS := $(patsubst $(SRCDIR)/%.cpp,$(OBJDIR)/%.o,$(SOURCES))

# Output binary
TARGET ?= main_x86_64

# Default rule
#all: $(TARGET)
all: bindings-python

# Linking the final binary
$(TARGET): $(OBJECTS)
	$(CXX) $(OBJECTS) -o $@

# Compiling source files to object files
$(OBJDIR)/%.o: $(SRCDIR)/%.cpp | $(OBJDIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Ensure build directory exists
$(OBJDIR):
	mkdir -p $(OBJDIR)

# Clean rule
clean:
	rm -rf $(OBJDIR) $(TARGET) matrix$(shell python3-config --extension-suffix)

# Python bindings target
PYTHON_CONFIG = python3-config
PYBIND11_INCLUDES = $(shell python3 -m pybind11 --includes)
EXT_SUFFIX = $(shell $(PYTHON_CONFIG) --extension-suffix)

bindings-python:
	$(CXX) $(CXXFLAGS) $(PYBIND11_INCLUDES) lib/bindings.cpp lib/matrix.cpp -o matrix$(EXT_SUFFIX)

.PHONY: all clean bindings-python
