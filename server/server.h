#ifndef SERVER_H
#define SERVER_H

#include <stddef.h>

#define MAX_PARAM_LENGTH 128
#define MAX_PARAMS 10
#define MAX_PATH_LENGTH 256

typedef enum {
    HTTP_METHOD_GET,
    HTTP_METHOD_POST,
    HTTP_METHOD_PUT,
    HTTP_METHOD_DELETE
} HttpMethod;

typedef struct {
    char name[MAX_PARAM_LENGTH];
    char value[MAX_PARAM_LENGTH];
} RequestParam;

typedef struct RequestContext {
    HttpMethod method;
    char path[MAX_PATH_LENGTH];
    char* body;
    int body_length;
    RequestParam params[MAX_PARAMS];
    int param_count;
    char content_type[128];
} RequestContext;

typedef struct EndpointResponse {
    int status_code;
    void* body;
    char* content_type;
    size_t body_length;
} EndpointResponse;

typedef EndpointResponse* (*EndpointHandler)(const RequestContext* request);

int server_init(int port);
int server_start(void);
void server_stop(void);

int server_register_handler(const char* path, const char* method, EndpointHandler handler);

const char* request_get_param(const RequestContext* request, const char* param_name);
int request_get_param_int(const RequestContext* request, const char* param_name, int default_value);
const char* request_get_body(const RequestContext* request);

EndpointResponse* response_json(int status_code, const char* json_body);
EndpointResponse* response_text(int status_code, const char* text_body);
EndpointResponse* response_error(int status_code, const char* error_message);

typedef struct {
    char filename[256];
    char content_type[128];
    const char* data;
    size_t size;
} UploadedFile;

int parse_multipart_file(const RequestContext* request, UploadedFile* file);

#define SERVER_GET(path, handler) server_register_handler(path, "GET", handler)
#define SERVER_POST(path, handler) server_register_handler(path, "POST", handler)

#endif
