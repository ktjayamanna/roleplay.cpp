/*
 * http.c - HTTP protocol handling implementation
 */

#define _GNU_SOURCE  // For strcasestr
#include "http.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>  // For strcasecmp

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

// This function builds an HTTP response with binary data
HttpResponse* http_build_binary_response(int status_code, const void* body,
                                        size_t body_length, const char* content_type) {
    // STEP 1: Get status text
      const char* status_text = "OK";
      if (status_code == 404) status_text = "Not Found";
      else if (status_code == 500) status_text = "Internal Server Error";
      // Add more status codes as needed

    // STEP 2: Build HTTP headers (without body)
      const char* header_format = "HTTP/1.1 %d %s\r\n"
                                 "Content-Type: %s\r\n"
                                 "Content-Length: %zu\r\n"
                                 "Connection: close\r\n"
                                 "\r\n";
    
      int header_length = snprintf(NULL, 0, header_format,
          status_code, status_text, content_type, body_length);

    // STEP 3: Allocate buffer for headers + body
      size_t total_length = header_length + body_length;
      char* response_buffer = malloc(total_length);
      if (!response_buffer) return NULL;

    // STEP 4: Write headers to buffer
      int written = sprintf(response_buffer, header_format,
          status_code, status_text, content_type, body_length);

    // STEP 5: Copy binary body after headers
      memcpy(response_buffer + written, body, body_length);
      // NOTE: Use memcpy, NOT strcat! Binary data may contain null bytes.

    // STEP 6: Create and return HttpResponse
      HttpResponse* response = malloc(sizeof(HttpResponse));
      response->status_code = status_code;
      response->body = response_buffer;
      response->body_length = total_length;
      return response;
}


void http_parse_request(const char* request, char* method, char* path) {
    sscanf(request, "%s %s", method, path);
}

// Find a header value in the HTTP request
const char* http_get_header(const char* request, const char* header_name) {
    static char header_value[512];
    char search_pattern[128];
    snprintf(search_pattern, sizeof(search_pattern), "%s:", header_name);

    const char* header_start = strcasestr(request, search_pattern);
    if (!header_start) {
        return NULL;
    }

    // Move past the header name and colon
    header_start += strlen(search_pattern);

    // Skip whitespace
    while (*header_start == ' ' || *header_start == '\t') {
        header_start++;
    }

    // Find end of line
    const char* header_end = strstr(header_start, "\r\n");
    if (!header_end) {
        header_end = header_start + strlen(header_start);
    }

    // Copy header value
    size_t len = header_end - header_start;
    if (len >= sizeof(header_value)) {
        len = sizeof(header_value) - 1;
    }
    strncpy(header_value, header_start, len);
    header_value[len] = '\0';

    return header_value;
}

// Get Content-Length from request
int http_get_content_length(const char* request) {
    const char* content_length = http_get_header(request, "Content-Length");
    if (content_length) {
        return atoi(content_length);
    }
    return 0;
}

// Get Content-Type from request
void http_get_content_type(const char* request, char* content_type, size_t max_len) {
    const char* ct = http_get_header(request, "Content-Type");
    if (ct) {
        strncpy(content_type, ct, max_len - 1);
        content_type[max_len - 1] = '\0';
    } else {
        content_type[0] = '\0';
    }
}

// Find the body in an HTTP request (after headers)
const char* http_find_body(const char* request) {
    const char* body_start = strstr(request, "\r\n\r\n");
    if (body_start) {
        return body_start + 4;  // Skip past \r\n\r\n
    }
    return NULL;
}

