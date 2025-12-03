# C++ Weather Client Makefile

# Compiler and flags
CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -g -O2
INCLUDES = 
LIBS = 

# Directories
SRC_DIR = .
BUILD_DIR = build

# Source files
SOURCES = main.cpp HTTPClient.cpp WeatherClient.cpp Cache.cpp
OBJECTS = $(SOURCES:%.cpp=$(BUILD_DIR)/%.o)

# Target executable
TARGET = weather_client

# Default target
all: $(BUILD_DIR) $(TARGET)

# Create build directory
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

# Build target executable
$(TARGET): $(OBJECTS)
	$(CXX) $(OBJECTS) -o $@ $(LIBS)
	@echo "Built $(TARGET) successfully!"

# Build object files
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

# Clean build files
clean:
	rm -rf $(BUILD_DIR)
	rm -f $(TARGET)
	@echo "Cleaned build files"

# Install (copy to parent directory)
install: $(TARGET)
	cp $(TARGET) ..
	@echo "Installed $(TARGET) to parent directory"

# Run the program
run: $(TARGET)
	./$(TARGET)

# Debug build
debug: CXXFLAGS += -DDEBUG -g3 -O0
debug: clean $(TARGET)

# Release build
release: CXXFLAGS += -DNDEBUG -O3
release: clean $(TARGET)

# Dependencies
$(BUILD_DIR)/main.o: main.cpp WeatherClient.hpp
$(BUILD_DIR)/HTTPClient.o: HTTPClient.cpp HTTPClient.hpp
$(BUILD_DIR)/WeatherClient.o: WeatherClient.cpp WeatherClient.hpp HTTPClient.hpp Cache.hpp
$(BUILD_DIR)/Cache.o: Cache.cpp Cache.hpp

# Phony targets
.PHONY: all clean install run debug release

# Help
help:
	@echo "Available targets:"
	@echo "  all      - Build the weather client (default)"
	@echo "  clean    - Remove build files"
	@echo "  install  - Copy executable to parent directory"
	@echo "  run      - Build and run the program"
	@echo "  debug    - Build with debug flags"
	@echo "  release  - Build with release optimizations"
	@echo "  help     - Show this help message"