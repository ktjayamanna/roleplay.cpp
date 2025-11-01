/*
 * http.c - HTTP protocol handling implementation
 * 
 * INSTRUCTIONS:
 * 1. Include necessary headers (http.h, stdio.h, stdlib.h, string.h)
 * 
 * 2. Implement http_build_response(int status_code, const char* body):
 *    - Allocate memory for the response struct and body string
 *    - Build HTTP response with format:
 *      "HTTP/1.1 [status_code] [status_text]\r\n"
 *      "Content-Type: application/json\r\n"
 *      "Content-Length: [body_length]\r\n"
 *      "Connection: close\r\n"
 *      "\r\n"
 *      "[body]"
 *    - Use sprintf() or snprintf() to format the string
 *    - Return the allocated string (caller must free it!)
 * 
 * 3. Implement http_parse_request(const char* request, char* method, char* path):
 *    - Parse the first line of HTTP request
 *    - Extract method (GET, POST, etc.) and path
 *    - Use sscanf() or manual string parsing
 *    - Example request line: "GET /health HTTP/1.1"
 * 
 * LEARNING GOALS:
 * - Understand HTTP request/response format
 * - Practice dynamic memory allocation
 * - Learn string formatting and parsing in C
 * - Understand the importance of memory management
 */

// TODO: Add your includes here
#include "http.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


// TODO: Implement http_build_response()

HttpResponse* http_build_response(int status_code, const char* body) {
    const char* status_text = (status_code == 200) ?  "OK" : "NOT FOUND";
    int body_length = strlen(body);

    const char* format = "HTTP/1.1 %d %s\r\n"
                        "Content-Type: application/json\r\n"
                        "Content-Length: %d\r\n"
                        "Connection: close\r\n"
                        "\r\n"
                        "%s";

    int response_length = snprintf(NULL, 0, format,
        status_code, status_text, body_length, body) + 1;

    HttpResponse* response = malloc(sizeof(HttpResponse));
    response->status_code = status_code;
    response->body = malloc(response_length);
    response->body_length = response_length;

    sprintf(response->body, format,
        status_code, status_text, body_length, body);

    return response;
}

// TODO: PHASE 1 - Implement http_build_binary_response()
// This function builds an HTTP response with binary data
//
// HttpResponse* http_build_binary_response(int status_code, const void* body,
//                                         size_t body_length, const char* content_type) {
//     // STEP 1: Get status text
//     //   const char* status_text = "OK";
//     //   if (status_code == 404) status_text = "Not Found";
//     //   else if (status_code == 500) status_text = "Internal Server Error";
//     //   // Add more status codes as needed
//
//     // STEP 2: Build HTTP headers (without body)
//     //   const char* header_format = "HTTP/1.1 %d %s\r\n"
//     //                              "Content-Type: %s\r\n"
//     //                              "Content-Length: %zu\r\n"
//     //                              "Connection: close\r\n"
//     //                              "\r\n";
//     //
//     //   int header_length = snprintf(NULL, 0, header_format,
//     //       status_code, status_text, content_type, body_length);
//
//     // STEP 3: Allocate buffer for headers + body
//     //   size_t total_length = header_length + body_length;
//     //   char* response_buffer = malloc(total_length);
//     //   if (!response_buffer) return NULL;
//
//     // STEP 4: Write headers to buffer
//     //   int written = sprintf(response_buffer, header_format,
//     //       status_code, status_text, content_type, body_length);
//
//     // STEP 5: Copy binary body after headers
//     //   memcpy(response_buffer + written, body, body_length);
//     //   // NOTE: Use memcpy, NOT strcat! Binary data may contain null bytes.
//
//     // STEP 6: Create and return HttpResponse
//     //   HttpResponse* response = malloc(sizeof(HttpResponse));
//     //   response->status_code = status_code;
//     //   response->body = response_buffer;
//     //   response->body_length = total_length;
//     //   return response;
// }

// TODO: Implement http_parse_request()

void http_parse_request(const char* request, char* method, char* path) {
    sscanf(request, "%s %s", method, path);
}

