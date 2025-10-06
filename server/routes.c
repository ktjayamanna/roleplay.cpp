/*
 * routes.c - HTTP route handlers implementation
 * 
 * INSTRUCTIONS:
 * 1. Include necessary headers (routes.h, http.h, stdio.h, stdlib.h, string.h)
 * 
 * 2. Implement handle_health_check():
 *    - Create a JSON response body: {"status": "ok"}
 *    - Call http_build_response() with HTTP_OK (200) and the body
 *    - Return the response string
 * 
 * 3. Implement handle_not_found():
 *    - Create a JSON response body: {"error": "Not Found"}
 *    - Call http_build_response() with HTTP_NOT_FOUND (404) and the body
 *    - Return the response string
 * 
 * LEARNING GOALS:
 * - Understand RESTful API design
 * - Practice JSON formatting (manual for now)
 * - Learn about HTTP status codes and their meanings
 */

// TODO: Add your includes here
#include "routes.h"
#include "http.h"
#include <stdio.h>
#include <stdlib.h>

// TODO: Implement handle_health_check()

char* handle_health_check(void) {
    const char* body = "{\"status\": \"ok\"}";
    HttpResponse* response = http_build_response(HTTP_OK, body);
    return response->body;
}

// TODO: Implement handle_not_found()

char* handle_not_found(void) {
    const char* body = "{\"error\": \"Not Found\"}";
    HttpResponse* response = http_build_response(HTTP_NOT_FOUND, body);
    return response->body;
}

