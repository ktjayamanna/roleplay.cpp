#ifndef WS_ENDPOINT_H
#define WS_ENDPOINT_H

#include "websocket.h"

#define MAX_WS_ENDPOINTS 50

// Registered WebSocket endpoint
typedef struct {
    char path[256];
    WsHandlers handlers;
    int is_active;
} RegisteredWsEndpoint;

// Endpoint registry
extern RegisteredWsEndpoint ws_endpoint_registry[MAX_WS_ENDPOINTS];
extern int ws_endpoint_count;

// Initialization
void ws_endpoint_system_init(void);

// Registration
int ws_endpoint_register(const char* path, WsHandlers handlers);

// Lookup
RegisteredWsEndpoint* ws_endpoint_find(const char* path);
int ws_endpoint_exists(const char* path);

// Event dispatching
void ws_endpoint_dispatch_connect(const char* path, WebSocketClient* client);
void ws_endpoint_dispatch_message(const char* path, WebSocketClient* client, 
                                  const char* message, int length, int is_binary);
void ws_endpoint_dispatch_disconnect(const char* path, WebSocketClient* client);

#endif

