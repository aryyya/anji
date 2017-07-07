/**
 * aryyya, 2016
 */

#ifndef http_response_h
#define http_response_h

#include <stdbool.h>
#include "http-request.h"
#include "file.h"

// Return codes.
#define HTTP_RESPONSE_SUCCESS 0
#define HTTP_RESPONSE_FAILURE 1

// Configuration constants.
#define HEADER_SIZE 256

/**
 * An object that holds an http response message.
 */
typedef struct HttpResponse {

    // The File object that holds the resource.
    File resource;

    // The flag that indicates if resource is used.
    bool has_resource;

    // -- HEADERS -- //

    // Content-Type header.
    char header_content_type[HEADER_SIZE + 1];

    // Content-Length header.
    char header_content_length[HEADER_SIZE + 1];

} HttpResponse;

/**
 * Create a new HttpResponse object.
 * @param http_response The HttpResponse.
 * @param http_request The HttpRequest being responded to.
 * @return HTTP_RESPONSE_SUCCESS on success, otherwise HTTP_RESPONSE_FAILURE.
 */
int http_response_new(HttpResponse *http_response, const HttpRequest *http_request);

/**
 * Destroy an HttpResponse object.
 * @param http_response The HttpResponse.
 */
void http_response_free(HttpResponse *http_response);

/**
 * Get the total size of an HttpResponse object.
 * @param http_response The HttpResponse.
 * @return The total size in bytes.
 */
ssize_t http_response_get_total_size(const HttpResponse *http_response);

/**
 * Put the contents of an HttpResponse object into a buffer.
 * @param http_response The HttpResponse.
 * @param buffer The buffer.
 * @return HTTP_RESPONSE_SUCCESS on success, otherwise HTTP_RESPONSE_FAILURE.
 */
int http_response_copy_to_buffer(const HttpResponse *http_response, char *buffer);

#endif /* http_response_h */
