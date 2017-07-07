/**
 * aryyya, 2016
 */

#ifndef http_client_h
#define http_client_h

#include <netinet/in.h>

// Return codes.
#define HTTP_CLIENT_SUCCESS 0
#define HTTP_CLIENT_FAILURE 1

/**
 * An object that holds the state of an http client.
 */
typedef struct HttpClient {

    // Client socket file descriptor.
    int socket;

    // Client address configuration.
    struct sockaddr_in address;

    // Client address length.
    socklen_t length;

} HttpClient;

/**
 * Create a new HttpClient.
 * @param http_client The HttpClient.
 * @return HTTP_CLIENT_SUCCESS on success, otherwise HTTP_CLIENT_FAILURE.
 * The HttpClient should eventually be destroyed via http_client_free().
 */
int http_client_new(HttpClient *http_client);

/**
 * Destroy an HttpClient.
 * @param http_client The HttpClient.
 */
void http_client_free(HttpClient *http_client);

#endif /* http_client_h */
