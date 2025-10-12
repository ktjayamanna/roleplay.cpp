#!/bin/bash

# Test script to trigger server endpoints for debugging
# Run this while your server is running in the debugger

echo "Testing server endpoints..."
echo "Make sure your server is running in the debugger first!"
echo ""

# Wait a moment for server to start
sleep 2

echo "1. Testing health endpoint..."
curl -i http://localhost:8080/health
echo ""
echo ""

echo "2. Testing root endpoint..."
curl -i http://localhost:8080/
echo ""
echo ""

echo "3. Testing unknown endpoint..."
curl -i http://localhost:8080/unknown
echo ""
echo ""

echo "Debug test complete!"
