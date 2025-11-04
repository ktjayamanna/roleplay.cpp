#!/bin/bash

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
cd "$SCRIPT_DIR"

echo "========================================"
echo "Open this URL in your browser:"
echo "http://localhost:8000/ws_echo_ui.html"
echo "========================================"
echo ""

python3 -m http.server 8000

