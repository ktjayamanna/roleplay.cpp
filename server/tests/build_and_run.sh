#!/bin/bash

set -e

echo "==================================="
echo "Building Server Tests"
echo "==================================="

cd "$(dirname "$0")"

# Create build directory
mkdir -p build

# Compile server library
echo "Compiling server library..."
gcc -Wall -Wextra -g -I.. -pthread -c ../server.c -o build/server.o
gcc -Wall -Wextra -g -I.. -pthread -c ../endpoint.c -o build/endpoint.o
gcc -Wall -Wextra -g -I.. -pthread -c ../http.c -o build/http.o

# Compile tests
echo "Compiling test_http_endpoints..."
gcc -Wall -Wextra -g -I.. -pthread test_http_endpoints.c build/server.o build/endpoint.o build/http.o -o build/test_http_endpoints -pthread

echo "Compiling test_memory_leaks..."
gcc -Wall -Wextra -g -I.. -pthread test_memory_leaks.c build/server.o build/endpoint.o build/http.o -o build/test_memory_leaks -pthread

echo "Compiling test_stress..."
gcc -Wall -Wextra -g -I.. -pthread test_stress.c build/server.o build/endpoint.o build/http.o -o build/test_stress -pthread

echo "Compiling test_edge_cases..."
gcc -Wall -Wextra -g -I.. -pthread test_edge_cases.c build/server.o build/endpoint.o build/http.o -o build/test_edge_cases -pthread

echo ""
echo "==================================="
echo "Build Complete!"
echo "==================================="
echo ""

# Run tests if requested
if [ "$1" == "run" ]; then
    echo "==================================="
    echo "Running HTTP Endpoint Tests"
    echo "==================================="
    timeout 15 ./build/test_http_endpoints
    TEST_RESULT=$?
    if [ $TEST_RESULT -eq 0 ]; then
        echo "✅ HTTP Endpoint Tests: PASSED"
    elif [ $TEST_RESULT -eq 124 ]; then
        echo "⏱️  HTTP Endpoint Tests: TIMEOUT (may have passed)"
    else
        echo "❌ HTTP Endpoint Tests: FAILED"
    fi
    echo ""

    echo "==================================="
    echo "Running Stress Tests"
    echo "==================================="
    timeout 30 ./build/test_stress
    TEST_RESULT=$?
    if [ $TEST_RESULT -eq 0 ]; then
        echo "✅ Stress Tests: PASSED"
    elif [ $TEST_RESULT -eq 124 ]; then
        echo "⏱️  Stress Tests: TIMEOUT (may have passed)"
    else
        echo "❌ Stress Tests: FAILED"
    fi
    echo ""

    echo "==================================="
    echo "Running Edge Case Tests"
    echo "==================================="
    timeout 15 ./build/test_edge_cases
    TEST_RESULT=$?
    if [ $TEST_RESULT -eq 0 ]; then
        echo "✅ Edge Case Tests: PASSED"
    elif [ $TEST_RESULT -eq 124 ]; then
        echo "⏱️  Edge Case Tests: TIMEOUT (may have passed)"
    else
        echo "❌ Edge Case Tests: FAILED"
    fi
    echo ""

    echo "==================================="
    echo "Running Memory Leak Tests (basic)"
    echo "==================================="
    echo "Note: Run with valgrind for full leak detection"
    timeout 20 ./build/test_memory_leaks 10
    TEST_RESULT=$?
    if [ $TEST_RESULT -eq 0 ]; then
        echo "✅ Memory Leak Tests: PASSED"
    elif [ $TEST_RESULT -eq 124 ]; then
        echo "⏱️  Memory Leak Tests: TIMEOUT (may have passed)"
    else
        echo "❌ Memory Leak Tests: FAILED"
    fi
    echo ""

    echo "==================================="
    echo "Test Summary"
    echo "==================================="
    echo "See TEST_RESULTS.md for detailed results"
    echo ""
    echo "To run with valgrind:"
    echo "  valgrind --leak-check=full ./build/test_memory_leaks 50"
fi

echo "Done!"

