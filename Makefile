# This makefile handles both the server- and client Makefiles
.PHONY: all server client clean run run-server run-client

# Build both server and client
all: server client

# Run Makefile in /server
server:
	@echo "Building server..."
	@$(MAKE) -C server

# Run Makefile in /client
client:
	@echo "Building client..."
	@$(MAKE) -C client

# Clean server and client
clean:
	@echo "Cleaning server..."
	@$(MAKE) -C server clean
	@echo "Cleaning client..."
	@$(MAKE) -C client clean
	@echo "Clean complete!"

# Run both (server first, then client)
run: all
	@echo "Killing any existing servers..."
	-@pkill -f weather_server 2>/dev/null
	@sleep 1
	@echo "Starting server in background..."
	cd server && ./weather_server &
	@echo "Waiting for server to start..."
	@sleep 2
	@echo "Starting client..."
	cd client && ./weather_client
	@echo "Stopping server..."
	-@pkill -f weather_server 2>/dev/null

# Run just the server
run-server: server
	@echo "Killing any existing servers..."
	-@pkill -f weather_server 2>/dev/null
	@echo "Starting server..."
	cd server && ./weather_server

# Run just the client (assumes server is already running)
run-client: client
	@echo "Starting client..."
	@cd client && ./weather_client