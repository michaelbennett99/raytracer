# Compiler settings
CXX = g++
CXXFLAGS = -Wall -std=c++23
RELEASE_FLAGS = -O3 -march=native -flto -ffast-math -DNDEBUG
DEBUG_FLAGS = -g -O0 -DDEBUG

# Directories
SRC_DIR = src
BUILD_DIR = build
RELEASE_DIR = $(BUILD_DIR)/release
DEBUG_DIR = $(BUILD_DIR)/debug

# Source files
SRCS = $(SRC_DIR)/main.cpp
RELEASE_OBJS = $(SRCS:$(SRC_DIR)/%.cpp=$(RELEASE_DIR)/%.o)
DEBUG_OBJS = $(SRCS:$(SRC_DIR)/%.cpp=$(DEBUG_DIR)/%.o)
TARGET = raytracer

# Default target
all: release

# Release target
release: CXXFLAGS += $(RELEASE_FLAGS)
release: $(RELEASE_DIR)/$(TARGET)

# Debug target
debug: CXXFLAGS += $(DEBUG_FLAGS)
debug: $(DEBUG_DIR)/$(TARGET)

# Create build directories
$(RELEASE_DIR):
	mkdir -p $(RELEASE_DIR)

$(DEBUG_DIR):
	mkdir -p $(DEBUG_DIR)

# Compile source files to object files (release)
$(RELEASE_DIR)/%.o: $(SRC_DIR)/%.cpp | $(RELEASE_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Compile source files to object files (debug)
$(DEBUG_DIR)/%.o: $(SRC_DIR)/%.cpp | $(DEBUG_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Link object files to create executable (release)
$(RELEASE_DIR)/$(TARGET): $(RELEASE_OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^

# Clean build files
clean:
	rm -rf $(BUILD_DIR)

.PHONY: all clean release debug
