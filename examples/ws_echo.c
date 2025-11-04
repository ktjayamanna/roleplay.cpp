/**
 * WebSocket Echo Server Example
 *
 * Demonstrates WebSocket support - echoes back any text or binary messages.
 * This is a complete, self-contained example in a single C file.
 *
 * Build: make APP_NAME=ws_echo
 * Run:   ./build/ws_echo.exe
 *
 * Test with GUI:
 *   1. Start the server: ./build/ws_echo.exe
 *   2. Open ws_echo_utils/index.html in your browser
 *   3. Click "Connect" and start sending messages
 *
 * Test from browser console:
 *   const ws = new WebSocket('ws://localhost:8080/echo');
 *   ws.onmessage = (e) => console.log('Received:', e.data);
 *   ws.send('Hello WebSocket!');
 */

#include "../server/server.h"
#include <stdio.h>

void handle_ws_connect(WebSocketClient* client) {
    printf("✓ Client %d connected to %s\n", client->id, client->path);
    ws_send_text(client, "Welcome to the echo server!");
}

void handle_ws_message(WebSocketClient* client, const char* message, int length, int is_binary) {
    if (is_binary) {
        printf("📦 Client %d sent %d bytes of binary data\n", client->id, length);
        ws_send_binary(client, message, length);
    } else {
        printf("💬 Client %d: %s\n", client->id, message);
        ws_send_text(client, message);
    }
}

void handle_ws_disconnect(WebSocketClient* client) {
    printf("✗ Client %d disconnected\n", client->id);
}

int main() {
    printf("WebSocket Echo Server\n");
    printf("=====================\n\n");

    if (server_init(8080) != 0) {
        fprintf(stderr, "Failed to initialize server\n");
        return 1;
    }

    SERVER_WS("/echo", handle_ws_message, handle_ws_connect, handle_ws_disconnect);

    printf("Server ready on port 8080\n");
    printf("Test: const ws = new WebSocket('ws://localhost:8080/echo');\n");
    printf("      ws.onmessage = (e) => console.log(e.data);\n");
    printf("      ws.send('Hello!');\n\n");

    server_start();
    return 0;
}
