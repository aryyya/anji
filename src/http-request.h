/**
 * aryyya, 2016
 */

#ifndef http_request_h
#define http_request_h

#include <stdlib.h>
#include <sys/socket.h>

// Return codes.
#define HTTP_REQUEST_SUCCESS 0
#define HTTP_REQUEST_FAILURE 1

// Configuration constants.
#define HTTP_REQUEST_RESOURCE_SIZE 256

/**
 * An object that holds an http request message.
 */
typedef struct HttpRequest {

    // The raw request data.
    const char *raw_data;

    // The raw request data length.
    ssize_t raw_data_len;

    // The HTTP method.
    int method;

    // The requested resource path.
    char resource[HTTP_REQUEST_RESOURCE_SIZE + 1];

} HttpRequest;

/**
 * Create a new HttpRequest.
 * @param http_request The HttpRequest.
 * @param raw_data The data that makes up the request.
 * @param raw_data_len The length of the data.
 * @return HTTP_REQUEST_SUCCESS on success, otherwise HTTP_REQUEST_FAILURE.
 */
int http_request_new(HttpRequest *http_request, const char *raw_data, const ssize_t raw_data_len);

#endif /* http_request_h */
