#!/bin/bash
# Quick test to verify compilation works

cd "$(dirname "$0")"
mkdir -p build

echo "Step 1: Compile server.c"
gcc -Wall -Wextra -g -I.. -pthread -c ../server.c -o build/server.o 2>&1
if [ $? -ne 0 ]; then
    echo "ERROR: Failed to compile server.c"
    exit 1
fi
echo "OK"

echo "Step 2: Compile endpoint.c"
gcc -Wall -Wextra -g -I.. -pthread -c ../endpoint.c -o build/endpoint.o 2>&1
if [ $? -ne 0 ]; then
    echo "ERROR: Failed to compile endpoint.c"
    exit 1
fi
echo "OK"

echo "Step 3: Compile http.c"
gcc -Wall -Wextra -g -I.. -pthread -c ../http.c -o build/http.o 2>&1
if [ $? -ne 0 ]; then
    echo "ERROR: Failed to compile http.c"
    exit 1
fi
echo "OK"

echo "Step 4: Compile test_http_endpoints"
gcc -Wall -Wextra -g -I.. -pthread test_http_endpoints.c build/server.o build/endpoint.o build/http.o -o build/test_http_endpoints -pthread 2>&1
if [ $? -ne 0 ]; then
    echo "ERROR: Failed to compile test_http_endpoints"
    exit 1
fi
echo "OK"

echo ""
echo "SUCCESS: All compilation steps completed!"
echo "Run with: ./build/test_http_endpoints"

