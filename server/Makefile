# Makefile for C Web Server
# This helps you compile and run your server easily

# Compiler and flags
CC = gcc
CFLAGS = -Wall -Wextra -std=c11 -g
LDFLAGS = 

# Target executable
TARGET = webserver

# Source files
SRCS = main.c server.c http.c routes.c

# Object files (automatically generated from SRCS)
OBJS = $(SRCS:.c=.o)

# Default target - builds the server
all: $(TARGET)

# Link object files to create executable
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS) $(LDFLAGS)
	@echo "Build complete! Run with: ./$(TARGET)"

# Compile .c files to .o files
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Run the server
run: $(TARGET)
	./$(TARGET)

# Clean build artifacts
clean:
	rm -f $(OBJS) $(TARGET)
	@echo "Clean complete!"

# Test the health check endpoint (server must be running)
test:
	@echo "Testing health check endpoint..."
	@curl -i http://localhost:8080/health || echo "Server not running or request failed"

# Debug with gdb
debug: $(TARGET)
	gdb ./$(TARGET)

# Check for memory leaks with valgrind
valgrind: $(TARGET)
	valgrind --leak-check=full --show-leak-kinds=all ./$(TARGET)

# Phony targets (not actual files)
.PHONY: all run clean test debug valgrind

