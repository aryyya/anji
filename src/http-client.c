/**
 * aryyya, 2016
 */

#include "http-client.h"
#include <unistd.h>

// -- Private: -- //

// Struct initializer.
static HttpClient zero = { 0 };

// -- Public: -- //

/**
 * Create a new HttpClient.
 * @param http_client The HttpClient.
 * @return HTTP_CLIENT_SUCCESS on success, otherwise HTTP_CLIENT_FAILURE.
 * The HttpClient should eventually be destroyed via http_client_free().
 */
int
http_client_new(HttpClient *http_client)
{
    *http_client = zero;

    // From the accept (2) man page:
    // The address_len is a value-result parameter; it should initially contain the amount of space pointed to by
    // address; on return it will contain the actual length (in bytes) of the address returned.
    http_client->length = sizeof (http_client->address);

    return HTTP_CLIENT_SUCCESS;
}

/**
 * Destroy an HttpClient.
 * @param http_client The HttpClient.
 */
void
http_client_free(HttpClient *http_client)
{
    (void) close(http_client->socket);
}
