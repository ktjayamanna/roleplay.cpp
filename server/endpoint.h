#ifndef ENDPOINT_H
#define ENDPOINT_H

#include "http.h"
#include "server.h"

#define MAX_ENDPOINTS 50

typedef struct {
    char path[MAX_PATH_LENGTH];
    HttpMethod method;
    EndpointHandler handler;
    int is_active;
} RegisteredEndpoint;

extern RegisteredEndpoint endpoint_registry[MAX_ENDPOINTS];
extern int endpoint_count;

void endpoint_system_init(void);
int endpoint_register(const char* path, HttpMethod method, EndpointHandler handler);
EndpointResponse* endpoint_dispatch(const char* method_str, const char* path, const char* query_string, const char* body, int body_length);
EndpointResponse* endpoint_dispatch_with_body(const char* method_str, const char* path, const char* query_string, const char* content_type, const char* body, int body_length);
void endpoint_response_free(EndpointResponse* response);

EndpointResponse* endpoint_create_response(int status_code, const char* body, const char* content_type);
const char* endpoint_get_param(const RequestContext* request, const char* param_name);
int endpoint_get_param_int(const RequestContext* request, const char* param_name, int default_value);

EndpointResponse* endpoint_json_response(int status_code, const char* json_body);
EndpointResponse* endpoint_error_response(int status_code, const char* error_message);

EndpointResponse* endpoint_binary_response(int status_code, const void* data,
                                          size_t data_length, const char* content_type);

EndpointResponse* endpoint_file_response(int status_code, const char* file_path);

#endif
