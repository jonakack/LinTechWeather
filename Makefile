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
	-I src \
	-I src/Core \
	-I src/HTTP/Server \
	-I src/HTTP/Client \
	-I src/TCP/Server \
	-I src/TCP/Client \
	-I src/Weather \
	-I src/External/cJSON

# Select flags per mode (OPTIMIZE goes into CFLAGS in release; LTO linked only in release)
ifeq ($(MODE),debug)
  CFLAGS=$(CFLAGS_BASE) $(DEBUG_FLAGS)
  LDFLAGS=
else
  CFLAGS=$(CFLAGS_BASE) #$(OPTIMIZE)
  LDFLAGS=
endif

# Port configuration
PORT_FLAGS_PRODUCTION=-DHTTP_SERVER_PORT=\"10380\"
PORT_FLAGS_LOCAL=-DHTTP_SERVER_PORT=\"8080\"

# Directories
SRC_DIR=.
BUILD_DIR=build

# Find all .c files in src/ and main.c (following symlinks)
SOURCES=$(shell find -L src/ -type f -name '*.c' 2>/dev/null) main.c

# Per-target object lists - put everything in build/
SERVER_OBJECTS=$(patsubst %.c,$(BUILD_DIR)/%.o,$(SOURCES))
SERVER_LOCAL_OBJECTS=$(patsubst %.c,$(BUILD_DIR)/local/%.o,$(SOURCES))

# Executables
EXECUTABLES=server

# Default target
all: $(EXECUTABLES)
	@echo "Build complete ($(MODE))."
	@echo "(STANDARD) Use 'make run' to start server in current terminal"
	@echo "(DETACHED) Use 'make run-tmux' to start server in tmux session"

# Link rules
server: $(SERVER_OBJECTS)
	@echo "Linking $@ (production - port 10380)..."
	@$(CC) $(LDFLAGS) $^ -o $@ $(LIBS)

# Local development server (port 8080)
server-local: $(SERVER_LOCAL_OBJECTS)
	@echo "Linking $@ (development - port 8080)..."
	@$(CC) $(LDFLAGS) $^ -o server $(LIBS)

# Compile rules - everything goes in build/
$(BUILD_DIR)/%.o: %.c
	@echo "Compiling $<..."
	@mkdir -p $(dir $@)
	@$(CC) $(CFLAGS) $(PORT_FLAGS_PRODUCTION) $(INCLUDES) -c $< -o $@

# Compile rules for local development
$(BUILD_DIR)/local/%.o: %.c
	@echo "Compiling $< (local)..."
	@mkdir -p $(dir $@)
	@$(CC) $(CFLAGS) $(PORT_FLAGS_LOCAL) $(INCLUDES) -c $< -o $@

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

run-local: server-local
	@echo "Starting local development server on port 8080..."
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

.PHONY: all clean compile debug-server debug-client run run-local run-tmux kill-server server-local