/*
 * http.h - HTTP protocol handling header
 * 
 * INSTRUCTIONS:
 * 1. Add header guards
 * 2. Define constants for HTTP status codes:
 *    - HTTP_OK (200)
 *    - HTTP_NOT_FOUND (404)
 * 3. Define a struct for HTTP responses containing:
 *    - status_code (int)
 *    - body (char pointer)
 *    - body_length (int)
 * 4. Declare function prototypes:
 *    - char* http_build_response(int status_code, const char* body)
 *    - void http_parse_request(const char* request, char* method, char* path)
 * 
 * LEARNING GOALS:
 * - Understand HTTP protocol basics
 * - Learn about string manipulation in C
 * - Practice working with pointers and dynamic memory
 */

// TODO: Add header guards
#ifndef HTTP_H
#define HTTP_H


// TODO: Define HTTP status code constants

#define HTTP_OK 200
#define HTTP_NOT_FOUND 404

// TODO: Define HttpResponse struct

typedef struct {
    int status_code;
    char* body;
    int body_length;
} HttpResponse;

// TODO: Declare function prototypes
HttpResponse* http_build_response(int status_code, const char* body);
void http_parse_request(const char* request, char* method, char* path);

// TODO: Close header guard
#endif
