/**
 * aryyya, 2016
 */

#include "http-request.h"
#include "http-methods.h"
#include <stdio.h>
#include <string.h>

// -- Private: -- //

// Struct initializer.
static HttpRequest zero = { 0 };

// -- Public: -- //

/**
 * Create a new HttpRequest.
 * @param http_request The HttpRequest.
 * @param raw_data The data that makes up the request.
 * @param raw_data_len The length of the data.
 * @return HTTP_REQUEST_SUCCESS on success, otherwise HTTP_REQUEST_FAILURE.
 */
int
http_request_new(HttpRequest *http_request, const char *raw_data, const ssize_t raw_data_len)
{
    *http_request = zero;
    int r = 0;

    http_request->raw_data = raw_data;
    http_request->raw_data_len = raw_data_len;

    char method[256], resource[256], version[256];
    r = sscanf(raw_data, "%s %s %s\r\n", method, resource, version);

    if (r != 3) {
        return HTTP_REQUEST_FAILURE;
    }

    strcpy(http_request->resource, resource);

    // Determine method.
    if /**/ (strcmp(method, "GET")  == 0) http_request->method = HTTP_METHOD_GET;
    else if (strcmp(method, "HEAD") == 0) http_request->method = HTTP_METHOD_HEAD;
    else if (strcmp(method, "POST") == 0) http_request->method = HTTP_METHOD_POST;
    else /*                            */ http_request->method = HTTP_METHOD_NONE;

    return HTTP_REQUEST_SUCCESS;
}
