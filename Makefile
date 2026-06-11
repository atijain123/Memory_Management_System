# Compiler settings
CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -g -Iinclude

# Directory definitions
SRC_DIR = src
BUILD_DIR = build
BIN_DIR = bin

# Find all .cpp files in src/ and its subdirectories
SRCS := $(shell find $(SRC_DIR) -name '*.cpp')

# Generate object file paths
OBJS := $(SRCS:$(SRC_DIR)/%.cpp=$(BUILD_DIR)/%.o)

# Output executable name
TARGET = $(BIN_DIR)/memsim

# Main build target
all: $(TARGET)

# Link the executable
$(TARGET): $(OBJS)
	@mkdir -p $(BIN_DIR)
	$(CXX) $(CXXFLAGS) $^ -o $@
	@echo "Build complete: $@"

# Compile source files to object files
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Clean build artifacts
clean:
	rm -rf $(BUILD_DIR) $(BIN_DIR)

.PHONY: all clean
