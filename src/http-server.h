/**
 * aryyya, 2016
 */

#ifndef http_server_h
#define http_server_h

#include "http-client.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdbool.h>

// Default server application name.
#define APP_NAME "anji"

// Configuration constants.
#define HTTP_SERVER_PORT_MIN                1
#define HTTP_SERVER_PORT_MAX                65535
#define HTTP_SERVER_LISTEN_BACKLOG          5
#define HTTP_SERVER_REQUEST_BUFFER_SIZE     8192
#define HTTP_SERVER_RESPONSE_BUFFER_SIZE    2000000

// Return codes.
#define HTTP_SERVER_SUCCESS                 0
#define HTTP_SERVER_FAILURE                 1
#define HTTP_SERVER_PORT_RANGE_ERROR        2
#define HTTP_SERVER_SOCKET_ERROR            3
#define HTTP_SERVER_SOCKET_OPTION_ERROR     4
#define HTTP_SERVER_BIND_ERROR              5
#define HTTP_SERVER_LISTEN_ERROR            6

/**
 * An object that holds the state of an http server.
 */
typedef struct HttpServer {

    // Server socket file descriptor.
    int socket;

    // Server address configuration.
    struct sockaddr_in address;

    // Server port number.
    int port;

    // Server status.
    bool enabled;

} HttpServer;

/**
 * Create a new HttpServer.
 * @param http_server The HttpServer.
 * @param port_number The port number.
 * @return HTTP_SERVER_SUCCESS on success, otherwise:
 *      HTTP_SERVER_PORT_RANGE_ERROR: port_number is out of HTTP_SERVER_PORT_MIN and HTTP_SERVER_PORT_MAX range.
 *      HTTP_SERVER_SOCKET_ERROR: There was an error opening a socket.
 *      HTTP_SERVER_SOCKET_OPTION_ERROR: There was an error setting a socket option.
 *      HTTP_SERVER_BIND_ERROR: There was an error binding the address to the socket.
 *      HTTP_SERVER_LISTEN_ERROR: There was an error listening for TCP connections.
 * The HttpServer should eventually be destroyed via http_server_free().
 */
int http_server_new(HttpServer *http_server, const int port_number);

/**
 * Destroy an HttpServer.
 * @param http_server The HttpServer.
 */
void http_server_free(HttpServer *http_server);

/**
 * Start an HttpServer listening for TCP connections and handling them.
 * @param http_server The HttpServer.
 * @return HTTP_SERVER_SUCCESS on success, otherwise a fatal error likely occurred.
 */
int http_server_listen(const HttpServer *http_server);

/**
 * Register an HttpServer to disable on interception of an OS interrupt.
 * @param http_server The HttpServer.
 */
void http_server_disable_on_interrupt(HttpServer *http_server);

#endif /* http_server_h */
