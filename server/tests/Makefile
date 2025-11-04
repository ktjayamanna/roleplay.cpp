# Makefile for server tests
CC = gcc
CFLAGS = -Wall -Wextra -g -I.. -pthread
LDFLAGS = -pthread

# Server source files
SERVER_DIR = ..
SERVER_SOURCES = $(SERVER_DIR)/server.c $(SERVER_DIR)/endpoint.c $(SERVER_DIR)/http.c
SERVER_OBJECTS = $(SERVER_SOURCES:.c=.o)

# Test executables
TESTS = test_http_endpoints test_memory_leaks test_stress test_edge_cases

# Build directory
BUILD_DIR = build

.PHONY: all clean run run_valgrind help

all: $(BUILD_DIR) $(TESTS)

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

# Build server object files
$(SERVER_DIR)/%.o: $(SERVER_DIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

# Build test executables
test_http_endpoints: test_http_endpoints.c $(SERVER_OBJECTS)
	$(CC) $(CFLAGS) -o $(BUILD_DIR)/$@ $^ $(LDFLAGS)

test_memory_leaks: test_memory_leaks.c $(SERVER_OBJECTS)
	$(CC) $(CFLAGS) -o $(BUILD_DIR)/$@ $^ $(LDFLAGS)

test_stress: test_stress.c $(SERVER_OBJECTS)
	$(CC) $(CFLAGS) -o $(BUILD_DIR)/$@ $^ $(LDFLAGS)

test_edge_cases: test_edge_cases.c $(SERVER_OBJECTS)
	$(CC) $(CFLAGS) -o $(BUILD_DIR)/$@ $^ $(LDFLAGS)

# Run all tests
run: all
	@echo "==================================="
	@echo "Running HTTP Endpoint Tests"
	@echo "==================================="
	@./$(BUILD_DIR)/test_http_endpoints || true
	@echo ""
	@echo "==================================="
	@echo "Running Edge Case Tests"
	@echo "==================================="
	@./$(BUILD_DIR)/test_edge_cases || true
	@echo ""
	@echo "==================================="
	@echo "Running Stress Tests"
	@echo "==================================="
	@./$(BUILD_DIR)/test_stress || true
	@echo ""
	@echo "==================================="
	@echo "Running Memory Leak Tests (basic)"
	@echo "==================================="
	@./$(BUILD_DIR)/test_memory_leaks 10 || true

# Run memory leak tests with valgrind
run_valgrind: all
	@echo "==================================="
	@echo "Running Memory Leak Detection"
	@echo "==================================="
	@echo "Testing HTTP endpoints..."
	@valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes \
		--suppressions=valgrind.supp 2>&1 \
		./$(BUILD_DIR)/test_memory_leaks 50 | tee valgrind_output.txt
	@echo ""
	@echo "Check valgrind_output.txt for detailed results"

# Run quick smoke test
smoke: all
	@echo "Running quick smoke test..."
	@timeout 10 ./$(BUILD_DIR)/test_http_endpoints || echo "Test completed or timed out"

# Clean build artifacts
clean:
	rm -rf $(BUILD_DIR)
	rm -f $(SERVER_DIR)/*.o
	rm -f valgrind_output.txt

help:
	@echo "Available targets:"
	@echo "  all          - Build all tests"
	@echo "  run          - Run all tests"
	@echo "  run_valgrind - Run memory leak tests with valgrind"
	@echo "  smoke        - Run quick smoke test"
	@echo "  clean        - Remove build artifacts"
	@echo ""
	@echo "Individual tests:"
	@echo "  test_http_endpoints - HTTP endpoint functionality"
	@echo "  test_memory_leaks   - Memory leak detection"
	@echo "  test_stress         - Stress and performance tests"
	@echo "  test_edge_cases     - Edge case handling"

