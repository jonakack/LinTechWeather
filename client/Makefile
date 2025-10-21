CC := gcc
INCLUDE := -I../libs -Isrc
CFLAGS := -Wall -Wextra -MMD -MP -std=c11 $(INCLUDE)
LIBS_DIR := ../libs
BUILD_DIR := build

SRCS := main.c src/cities.c src/list.c
LIB_SOURCES := $(wildcard $(LIBS_DIR)/*.c)
OBJS := $(SRCS:%.c=$(BUILD_DIR)/%.o)
LIB_OBJS := $(LIB_SOURCES:$(LIBS_DIR)/%.c=$(BUILD_DIR)/libs_%.o)

TARGET := weather_client

# Default target
all: $(TARGET)

# Build target with dependencies
$(TARGET): $(OBJS) $(LIB_OBJS)
	$(CC) -o $@ $^

# Rule for object files - create build directory if needed
$(BUILD_DIR)/%.o: %.c | $(BUILD_DIR) $(BUILD_DIR)/src
	$(CC) $(CFLAGS) -c $< -o $@

# Rule for library object files
$(BUILD_DIR)/libs_%.o: $(LIBS_DIR)/%.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Create build directories
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(BUILD_DIR)/src: | $(BUILD_DIR)
	mkdir -p $(BUILD_DIR)/src

clean:
	rm -rf $(BUILD_DIR) $(TARGET)

run: $(TARGET)
	./$(TARGET)

.PHONY: all run clean