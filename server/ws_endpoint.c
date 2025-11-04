#define _GNU_SOURCE
#include "ws_endpoint.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

RegisteredWsEndpoint ws_endpoint_registry[MAX_WS_ENDPOINTS];
int ws_endpoint_count = 0;

void ws_endpoint_system_init(void) {
    ws_endpoint_count = 0;
    for (int i = 0; i < MAX_WS_ENDPOINTS; i++) {
        ws_endpoint_registry[i].is_active = 0;
    }
}

int ws_endpoint_register(const char* path, WsHandlers handlers) {
    if (ws_endpoint_count >= MAX_WS_ENDPOINTS) {
        fprintf(stderr, "Error: Maximum number of WebSocket endpoints reached\n");
        return -1;
    }

    int slot = -1;
    for (int i = 0; i < MAX_WS_ENDPOINTS; i++) {
        if (!ws_endpoint_registry[i].is_active) {
            slot = i;
            break;
        }
    }

    if (slot == -1) {
        fprintf(stderr, "Error: No available WebSocket endpoint slots\n");
        return -1;
    }

    strncpy(ws_endpoint_registry[slot].path, path, sizeof(ws_endpoint_registry[slot].path) - 1);
    ws_endpoint_registry[slot].path[sizeof(ws_endpoint_registry[slot].path) - 1] = '\0';
    ws_endpoint_registry[slot].handlers = handlers;
    ws_endpoint_registry[slot].is_active = 1;

    ws_endpoint_count++;
    printf("Registered WebSocket endpoint: %s\n", path);
    return 0;
}

RegisteredWsEndpoint* ws_endpoint_find(const char* path) {
    for (int i = 0; i < MAX_WS_ENDPOINTS; i++) {
        if (ws_endpoint_registry[i].is_active &&
            strcmp(ws_endpoint_registry[i].path, path) == 0) {
            return &ws_endpoint_registry[i];
        }
    }
    return NULL;
}

int ws_endpoint_exists(const char* path) {
    return ws_endpoint_find(path) != NULL;
}

void ws_endpoint_dispatch_connect(const char* path, WebSocketClient* client) {
    RegisteredWsEndpoint* endpoint = ws_endpoint_find(path);
    if (endpoint && endpoint->handlers.on_connect) {
        endpoint->handlers.on_connect(client);
    }
}

void ws_endpoint_dispatch_message(const char* path, WebSocketClient* client,
                                  const char* message, int length, int is_binary) {
    RegisteredWsEndpoint* endpoint = ws_endpoint_find(path);
    if (endpoint && endpoint->handlers.on_message) {
        endpoint->handlers.on_message(client, message, length, is_binary);
    }
}

void ws_endpoint_dispatch_disconnect(const char* path, WebSocketClient* client) {
    RegisteredWsEndpoint* endpoint = ws_endpoint_find(path);
    if (endpoint && endpoint->handlers.on_disconnect) {
        endpoint->handlers.on_disconnect(client);
    }
}

