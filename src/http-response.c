/**
 * aryyya, 2016
 */

#include "http-response.h"
#include "http-request.h"
#include "http-methods.h"
#include "mime-types.h"
#include "file.h"
#include <string.h>

// -- Private: -- //

// Struct initializer.
static HttpResponse zero = { 0 };

static int http_response_get (HttpResponse *http_response, const HttpRequest *http_request);
static int http_response_post(HttpResponse *http_response, const HttpRequest *http_request);
static int http_response_head(HttpResponse *http_response, const HttpRequest *http_request);

/**
 * Construct an HttpResponse response for an HttpRequest with the GET method.
 * @param http_response The HttpResponse.
 * @param http_request The HttpRequest.
 * @return HTTP_RESPONSE_SUCCESS on success, otherwise HTTP_RESPONSE_FAILURE.
 */
static int
http_response_get(HttpResponse *http_response, const HttpRequest *http_request)
{
    int r = 0;

    // Get safe version of resource file path.
    char path_safe[FILE_PATH_SIZE + 1];
    file_fix_path(http_request->resource, path_safe);

    // Get resource file extension.
    char path_extn[FILE_PATH_SIZE + 1];
    file_get_extn(http_request->resource, path_extn);

    // Get resource mime type.
    const char *file_mime = mime_get_content_type(path_extn);

    // Load resource file.
    r = file_new(&(http_response->resource), path_safe, FILE_READ_BINARY_MODE);
    if (r == FILE_FAILURE) return HTTP_RESPONSE_FAILURE;
    http_response->has_resource = true;

    // Get file size.
    const size_t file_size = file_get_size(&(http_response->resource));

    // -- HEADERS: -- //

    // Set content-type header.
    sprintf(http_response->header_content_type, "Content-Type: %s", file_mime);

    // Set content-length header.
    sprintf(http_response->header_content_length, "Content-Length: %lu", file_size);

    return HTTP_RESPONSE_SUCCESS;
}

/**
 * Construct an HttpResponse response for an HttpRequest with the HEAD method.
 * @param http_response The HttpResponse.
 * @param http_request The HttpRequest.
 * @return HTTP_RESPONSE_SUCCESS on success, otherwise HTTP_RESPONSE_FAILURE.
 */
static int
http_response_head(HttpResponse *http_response, const HttpRequest *http_request)
{
    (void) http_response;
    (void) http_request;

    return HTTP_RESPONSE_FAILURE;
}

/**
 * Construct an HttpResponse response for an HttpRequest with the POST method.
 * @param http_response The HttpResponse.
 * @param http_request The HttpRequest.
 * @return HTTP_RESPONSE_SUCCESS on success, otherwise HTTP_RESPONSE_FAILURE.
 */
static int
http_response_post(HttpResponse *http_response, const HttpRequest *http_request)
{
    (void) http_response;
    (void) http_request;

    return HTTP_RESPONSE_FAILURE;
}

// -- Public: -- //

/**
 * Create a new HttpResponse object.
 * @param http_response The HttpResponse.
 * @param http_request The HttpRequest being responded to.
 * @return HTTP_RESPONSE_SUCCESS on success, otherwise HTTP_RESPONSE_FAILURE.
 */
int
http_response_new(HttpResponse *http_response, const HttpRequest *http_request)
{
    *http_response = zero;
    int r = 0;

    switch (http_request->method) {
        case HTTP_METHOD_GET:  r = http_response_get (http_response, http_request); break;
        case HTTP_METHOD_POST: r = http_response_head(http_response, http_request); break;
        case HTTP_METHOD_HEAD: r = http_response_post(http_response, http_request); break;
        default: break;
    }

    return r;
}

/**
 * Destroy an HttpResponse object.
 * @param http_response The HttpResponse.
 */
void
http_response_free(HttpResponse *http_response)
{
    if (http_response->has_resource) {
        file_free(&(http_response->resource));
    }
}

/**
 * Get the total size of an HttpResponse object.
 * @param http_response The HttpResponse.
 * @return The total size in bytes.
 */
ssize_t
http_response_get_total_size(const HttpResponse *http_response)
{
    ssize_t size = 0;

    size += file_get_size(&(http_response->resource));

    if (http_response->header_content_type[0])    size += strlen(http_response->header_content_type);
    if (http_response->header_content_length[0])  size += strlen(http_response->header_content_length);

    return size;
}

/**
 * Put the contents of an HttpResponse object into a buffer.
 * @param http_response The HttpResponse.
 * @param buffer The buffer.
 * @return HTTP_RESPONSE_SUCCESS on success, otherwise HTTP_RESPONSE_FAILURE.
 */
int
http_response_copy_to_buffer(const HttpResponse *http_response, char *buffer)
{
    sprintf(buffer,
"HTTP/1.1 200 OK\r\n"
"%s\r\n"
"%s\r\n"
"\r\n",
            http_response->header_content_type,
            http_response->header_content_length);

    // Copy binary data to content section.
    memcpy(buffer + strlen(buffer), file_get_data(&(http_response->resource)), file_get_size(&(http_response->resource)));

    return HTTP_RESPONSE_SUCCESS;
}
