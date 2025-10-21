CC := gcc
CFLAGS := -Wall -Wextra -MMD -MP -std=c11 -I../libs
LIBS_DIR := ../libs
BUILD_DIR := build

# Source files
LIB_SOURCES := $(wildcard $(LIBS_DIR)/*.c)
LIB_OBJS := $(LIB_SOURCES:$(LIBS_DIR)/%.c=$(BUILD_DIR)/libs_%.o)

TARGET := weather_server
SRCS := main.c
OBJS := $(SRCS:%.c=$(BUILD_DIR)/%.o)

# Default target
all: $(TARGET)

# Build target with dependencies
$(TARGET): $(OBJS) $(LIB_OBJS)
	$(CC) -o $@ $^

# Rule for object files - create build directory if needed
$(BUILD_DIR)/%.o: %.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Rule for library object files
$(BUILD_DIR)/libs_%.o: $(LIBS_DIR)/%.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Create build directory
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

clean:
	rm -rf $(BUILD_DIR) $(TARGET)

run: $(TARGET)
	./$(TARGET)

.PHONY: all run clean