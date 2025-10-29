# Simple Calculator using Server Library
CC = gcc
CFLAGS = -Wall -Wextra -std=c11 -g

# Server library files (from ../server/ directory)
LIB_SRCS = ../server/server.c ../server/http.c ../server/endpoint.c

# Application source
APP_SRCS = calculator.c

# Object files
LIB_OBJS = $(LIB_SRCS:.c=.o)
APP_OBJS = $(APP_SRCS:.c=.o)
ALL_OBJS = $(LIB_OBJS) $(APP_OBJS)

# Target executable
TARGET = calculator

# Build the calculator
$(TARGET): $(ALL_OBJS)
	$(CC) $(CFLAGS) -o $@ $^
	@echo "Calculator built! Run with: ./$(TARGET)"

# Compile source files to object files
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Clean build artifacts
clean:
	rm -f $(ALL_OBJS) $(TARGET)
	@echo "Clean complete!"

# Test the calculator
test: $(TARGET)
	@echo "Testing calculator endpoints..."
	@echo "Starting server in background..."
	@./$(TARGET) &
	@SERVER_PID=$$!; \
	sleep 2; \
	echo "Health check:"; \
	curl -s "http://localhost:8080/health" | head -1; \
	echo ""; \
	echo "Addition (5 + 3):"; \
	curl -s "http://localhost:8080/add?a=5&b=3" | head -1; \
	echo ""; \
	echo "Addition (100 + 200):"; \
	curl -s "http://localhost:8080/add?a=100&b=200" | head -1; \
	echo ""; \
	echo "Error test (missing parameter):"; \
	curl -s "http://localhost:8080/add?a=5" | head -1; \
	echo ""; \
	kill $$SERVER_PID 2>/dev/null || true

.PHONY: clean test
