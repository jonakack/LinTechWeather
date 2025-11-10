# Variables
CC=gcc
OPTIMIZE=-ffunction-sections -fdata-sections -O2 -flto -Wno-unused-result -fno-strict-aliasing
DEBUG_FLAGS=-g -O0 -Wfatal-errors -Werror
LIBS=-luuid -lcurl -pthread -lm -lbsd
INCLUDES = 

# Build mode: release (default) or debug
MODE ?= release

# Base warnings and #includes
CFLAGS_BASE=-Wall -Wno-psabi -Werror \
	-I libs \
	-I libs/HTTPServer \
	-I libs/TCP \
	-I libs/Utils \
	-I libs/WeatherServer

# Select flags per mode (OPTIMIZE goes into CFLAGS in release; LTO linked only in release)
ifeq ($(MODE),debug)
  CFLAGS=$(CFLAGS_BASE) $(DEBUG_FLAGS)
  LDFLAGS=
else
  CFLAGS=$(CFLAGS_BASE) #$(OPTIMIZE)
  LDFLAGS=
endif

# Directories
SRC_DIR=.
BUILD_DIR=build

# Find all .c files (following symlinks)
SOURCES=$(shell find -L $(SRC_DIR) -type f -name '*.c')

# Per-target object lists - put everything in build/
SERVER_OBJECTS=$(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/%.o,$(SOURCES))

# Executables
EXECUTABLES=server

# Default target
all: $(EXECUTABLES)
	@echo "Build complete ($(MODE))."

# Link rules
server: $(SERVER_OBJECTS)
	@echo "Linking $@..."
	@$(CC) $(LDFLAGS) $^ -o $@ $(LIBS)

# Compile rules - everything goes in build/
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	@echo "Compiling $<..."
	@mkdir -p $(dir $@)
	@$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

# Specific file compilation
FILE=
compile:
	@echo "Compiling $(FILE)..."
	@mkdir -p $(BUILD_DIR)/$(dir $(FILE))
	$(CC) $(CFLAGS) $(INCLUDES) -c $(FILE) -o $(BUILD_DIR)/$(FILE:.c=.o)

# Run targets
run: server
	@echo "Starting server..."
	@./server

# Run server and detach it automatically
run-tmux: server
	@echo "Starting server in detached tmux session..."
	@tmux new-session -d -s weather-server './server'
	@echo "Server started in tmux session 'weather-server'"
	@echo "Use 'tmux attach -t weather-server' to attach"
	@echo "Use 'make kill-server' to stop"

# Kill the tmux server session
kill-server:
	@echo "Stopping server..."
	@tmux kill-session -t weather-server 2>/dev/null || echo "Server was not running"

# Clean
clean:
	@echo "Cleaning up..."
	@rm -rf $(BUILD_DIR) $(EXECUTABLES)

.PHONY: all clean compile debug-server debug-client run run-tmux kill-server