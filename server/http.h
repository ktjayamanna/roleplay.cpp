/*
 * http.h - HTTP protocol handling header
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

#endif
