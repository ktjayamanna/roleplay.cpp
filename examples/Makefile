# Examples using Server Library
CC = gcc
CFLAGS = -Wall -Wextra -std=c11 -g

# Build directory for all intermediate and output files
BUILD_DIR = build

# Server library files (from ../server/ directory)
LIB_SRCS = ../server/server.c ../server/http.c ../server/endpoint.c
LIB_OBJS = $(patsubst ../server/%.c,$(BUILD_DIR)/%.o,$(LIB_SRCS))

# WebSocket library files (optional - only for WebSocket apps)
WS_LIB_SRCS = ../server/websocket.c ../server/ws_endpoint.c
WS_LIB_OBJS = $(patsubst ../server/%.c,$(BUILD_DIR)/%.o,$(WS_LIB_SRCS))

# WebSocket apps that need additional libraries
WS_APPS = ws_echo

# Determine if current app needs WebSocket support
ifeq ($(filter $(APP_NAME),$(WS_APPS)),$(APP_NAME))
    ALL_LIB_OBJS = $(LIB_OBJS) $(WS_LIB_OBJS)
    LDFLAGS = -lssl -lcrypto -lpthread
    CFLAGS += -DENABLE_WEBSOCKET
else
    ALL_LIB_OBJS = $(LIB_OBJS)
    LDFLAGS =
endif

# ============================================================================
# APP CONFIGURATION - Change APP_NAME to build different apps
# ============================================================================
APP_NAME ?= ws_echo

# Application source and output
APP_SRC = $(APP_NAME).c
APP_OBJ = $(BUILD_DIR)/$(APP_NAME).o
APP_EXE = $(BUILD_DIR)/$(APP_NAME).exe

# ============================================================================
# BUILD RULES
# ============================================================================

# Default target - build the specified app
all: $(APP_EXE)
	@echo "✓ Built $(APP_NAME).exe successfully!"
	@echo "  Location: $(BUILD_DIR)/$(APP_NAME).exe"
	@echo "  Run with: make run"

# Create build directory
$(BUILD_DIR):
	@mkdir -p $(BUILD_DIR)

# Build the application executable
$(APP_EXE): $(APP_OBJ) $(ALL_LIB_OBJS) | $(BUILD_DIR)
	$(CC) $(CFLAGS) -o $@ $(APP_OBJ) $(ALL_LIB_OBJS) $(LDFLAGS)

# Compile application source
$(BUILD_DIR)/%.o: %.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Compile server library sources
$(BUILD_DIR)/%.o: ../server/%.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

# ============================================================================
# RUN TARGET
# ============================================================================

# Run the built application
run: $(APP_EXE)
	@make kill
	@echo "Running $(APP_NAME).exe..."
	@$(APP_EXE)

# ============================================================================
# CLEAN TARGETS
# ============================================================================

# Clean all build artifacts
clean:
	rm -rf $(BUILD_DIR)
	@echo "✓ Clean complete! (removed $(BUILD_DIR)/)"

# Kill any processes using port 8888
kill:
	@echo "Killing processes on port 8888..."
	@fuser -k 8888/tcp 2>/dev/null || echo "No processes found on port 8888"
	@sleep 1
	@echo "✓ Port 8888 freed"

# ============================================================================
# TEST TARGETS
# ============================================================================

# Test the calculator
test: $(APP_EXE)
	@if [ "$(APP_NAME)" = "calculator" ]; then \
		echo "Testing calculator endpoints..."; \
		echo "Starting server in background..."; \
		make kill; \
		$(APP_EXE) & \
		SERVER_PID=$$!; \
		sleep 2; \
		echo "Addition (5 + 3):"; \
		curl -s "http://localhost:8888/add?a=5&b=3" | head -1; \
		echo ""; \
		echo "Addition (100 + 200):"; \
		curl -s "http://localhost:8888/add?a=100&b=200" | head -1; \
		echo ""; \
		echo "Error test (missing parameter):"; \
		curl -s "http://localhost:8888/add?a=5" | head -1; \
		echo ""; \
		kill $$SERVER_PID 2>/dev/null || true; \
	else \
		echo "No tests defined for $(APP_NAME)"; \
	fi

# ============================================================================
# HELP
# ============================================================================

help:
	@echo "Usage:"
	@echo "  make APP_NAME=<name>     - Build specific app (default: calculator)"
	@echo "  make run                 - Run the built application"
	@echo "  make test                - Run tests for the application"
	@echo "  make clean               - Remove all build artifacts"
	@echo "  make kill                - Kill processes on port 8888"
	@echo ""
	@echo "Examples:"
	@echo "  make APP_NAME=calculator - Build calculator.exe"
	@echo "  make APP_NAME=sound_gen  - Build sound_gen.exe"
	@echo "  make run                 - Run the current app"
	@echo "  make kill                - Free up port 8888"
	@echo ""
	@echo "Current app: $(APP_NAME)"
	@echo "Output: $(BUILD_DIR)/$(APP_NAME).exe"

.PHONY: all run clean test help kill
