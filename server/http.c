/*
 * http.c - HTTP protocol handling implementation
 * 
 * INSTRUCTIONS:
 * 1. Include necessary headers (http.h, stdio.h, stdlib.h, string.h)
 * 
 * 2. Implement http_build_response(int status_code, const char* body):
 *    - Allocate memory for the response string
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

// TODO: Implement http_build_response()

// TODO: Implement http_parse_request()

