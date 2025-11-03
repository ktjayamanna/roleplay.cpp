#ifndef HTTP_H
#define HTTP_H

#include <stddef.h>

#define HTTP_OK 200
#define HTTP_NOT_FOUND 404

typedef struct {
    int status_code;
    char* body;
    int body_length;
} HttpResponse;

HttpResponse* http_build_response(int status_code, const char* body);
HttpResponse* http_build_binary_response(int status_code, const void* body,
                                        size_t body_length, const char* content_type);
void http_parse_request(const char* request, char* method, char* path);

const char* http_get_header(const char* request, const char* header_name);
int http_get_content_length(const char* request);
void http_get_content_type(const char* request, char* content_type, size_t max_len);
const char* http_find_body(const char* request);

#endif
