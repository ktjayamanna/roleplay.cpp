/*
 * routes.h - HTTP route handlers header
 * 
 * INSTRUCTIONS:
 * 1. Add header guards
 * 2. Declare function prototypes for route handlers:
 *    - char* handle_health_check(void)
 *    - char* handle_not_found(void)
 * 
 * Each handler should return a dynamically allocated string
 * containing the HTTP response (caller must free it!)
 * 
 * LEARNING GOALS:
 * - Understand routing in web servers
 * - Learn about function pointers (for future route table)
 * - Practice modular code organization
 */

// TODO: Add header guards
#ifndef ROUTES_H
#define ROUTES_H


// TODO: Declare route handler function prototypes
char* handle_health_check(void);
char* handle_not_found(void);

// TODO: Close header guard
#endif
