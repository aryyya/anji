/**
 * aryyya, 2016
 */

#include "http-server.h"
#include "http-client.h"
#include "http-request.h"
#include "http-response.h"
#include <arpa/inet.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <signal.h>

// -- Private: -- //

// Struct initializer.
static HttpServer zero = { 0 };

static HttpServer *http_server_to_disable_on_interrupt; // See http_server_disable_on_interrupt().
static void http_server_signal_handler(int signal);
static int http_server_accept_client(const HttpServer *http_server, HttpClient *http_client);
static int http_server_client_handler(const HttpServer *http_server, HttpClient *http_client);
static int http_server_client_recv(const HttpClient *http_client, char *raw_data, const size_t raw_data_len, ssize_t *bytes_read);
static int http_server_client_send(const HttpClient *http_client, char *raw_data, const size_t raw_data_len, ssize_t *bytes_sent);

/**
 * Signal handler that disables an HttpServer.
 * @param signal The signal.
 */
static void
http_server_signal_handler(int signal)
{
    (void) signal;
    http_server_to_disable_on_interrupt->enabled = false;
    close(http_server_to_disable_on_interrupt->socket);
}

/**
 * Block until an HttpClient connects to an HttpServer.
 * @param http_server The HttpServer.
 * @param http_client The HttpClient.
 * @return HTTP_SERVER_SUCCESS on success, otherwise HTTP_SERVER_FAILURE.
 */
static int
http_server_accept_client(const HttpServer *http_server, HttpClient *http_client)
{
    // Accept a client connection on the server socket.
    http_client->socket = accept(http_server->socket, (struct sockaddr *) &(http_client->address), &(http_client->length));

    // Return the result of the connection attempt.
    return (http_client->socket != -1) ? HTTP_SERVER_SUCCESS : HTTP_SERVER_FAILURE;
}

/**
 * Handle an HttpClient's connection request and response.
 * @param http_server The HttpServer.
 * @param http_client The HttpClient.
 * @return HTTP_SERVER_SUCCESS on success, otherwise HTTP_SERVER_FAILURE.
 */
static int
http_server_client_handler(const HttpServer *http_server, HttpClient *http_client)
{
    (void) http_server;
    (void) http_client;
    int r = 0;

    const pid_t process_id = getpid();
    fprintf(stdout,
APP_NAME ": process %d spawned to handle client\n",
            process_id);

    // -- RECEIVE REQUEST -- //

    // Read data from socket.
    char data_recv[HTTP_SERVER_REQUEST_BUFFER_SIZE + 1];
    ssize_t bytes_read = 0;
    r = http_server_client_recv(http_client, data_recv, HTTP_SERVER_REQUEST_BUFFER_SIZE, &bytes_read);
    data_recv[bytes_read] = '\0';

    if (r == HTTP_SERVER_FAILURE) {
        fprintf(stderr,
APP_NAME ": handler %d was unable to read any data from the client\n",
                process_id);
        return HTTP_SERVER_FAILURE;
    }

    // Create request object.
    HttpRequest request;
    r = http_request_new(&request, data_recv, bytes_read);

    if (r == HTTP_REQUEST_FAILURE) {
        fprintf(stderr,
APP_NAME ": handler %d was unable to parse the malformed client request\n",
                process_id);
        return HTTP_SERVER_FAILURE;
    }

    /*
    fprintf(stdout,
APP_NAME ": handler %d read the following client request:\n%s\n",
            process_id, data_recv);
     */

    // -- SEND RESPONSE -- //

    // Create response object.
    HttpResponse response;
    r = http_response_new(&response, &request);

    if (r == HTTP_RESPONSE_FAILURE) {
        fprintf(stderr,
APP_NAME ": handler %d was unable to create a response to the client request\n",
                process_id);
        return HTTP_SERVER_FAILURE;
    }

    // Put response message in a buffer.
    const ssize_t data_send_len = http_response_get_total_size(&response) + strlen("HTTP/1.1 200 OK") + 8;
    char *data_send = malloc(sizeof (char) * data_send_len);

    if (data_send == NULL) {
        fprintf(stderr,
APP_NAME ": handler %d was unable to allocate enough memory to store the requested resource\n",
                process_id);
        return HTTP_SERVER_FAILURE;
    }

    r = http_response_copy_to_buffer(&response, data_send);

    // Send data to socket.
    ssize_t bytes_sent = 0;
    r = http_server_client_send(http_client, data_send, data_send_len, &bytes_sent);

    if (r == HTTP_SERVER_FAILURE) {
        fprintf(stderr,
APP_NAME ": handler %d was unable to send all response data to the client\n",
                process_id);
        return HTTP_SERVER_FAILURE;
    }

    fprintf(stdout,
APP_NAME ": handler %d sent %s\n",
            process_id, response.resource.path);

    // Free resources.
    http_response_free(&response);
    if (data_send) free(data_send);

    return HTTP_SERVER_SUCCESS;
}

/**
 * Receive data from a socket and store it in memory.
 * @param http_client The HttpClient with a connected socket.
 * @param raw_data The buffer to store the data in.
 * @param raw_data_len The maximum length of the buffer.
 * @param bytes_read The number of bytes read.
 * @return HTTP_SERVER_SUCCESS on success, otherwise HTTP_SERVER_FAILURE.
 */
static int
http_server_client_recv(const HttpClient *http_client, char *raw_data, const size_t raw_data_len, ssize_t *bytes_read)
{
    *bytes_read = recv(http_client->socket, raw_data, raw_data_len, 0);
    return (*bytes_read != -1) ? HTTP_SERVER_SUCCESS : HTTP_SERVER_FAILURE;
}

/**
 * Send data to a socket.
 * @param http_client The HttpClient with a connected socket.
 * @param raw_data The buffer to send data from.
 * @param raw_data_len The number of bytes to send from the buffer.
 * @param bytes_sent The number of bytes sent.
 * @return HTTP_SERVER_SUCCESS on success, otherwise HTTP_SERVER_FAILURE.
 */
static int
http_server_client_send(const HttpClient *http_client, char *raw_data, const size_t raw_data_len, ssize_t *bytes_sent)
{
    *bytes_sent = send(http_client->socket, raw_data, raw_data_len, 0);
    return (*bytes_sent != -1) ? HTTP_SERVER_SUCCESS : HTTP_SERVER_FAILURE;
}

// -- Public: -- //

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
int
http_server_new(HttpServer *http_server, const int port_number)
{
    *http_server = zero;
    int r = 0;

    // Check port number.
    if (HTTP_SERVER_PORT_MIN > port_number || port_number > HTTP_SERVER_PORT_MAX) return HTTP_SERVER_PORT_RANGE_ERROR;
    http_server->port = port_number;

    // Open server socket.
    http_server->socket = socket(AF_INET, SOCK_STREAM, 0);
    if (http_server->socket == -1) return HTTP_SERVER_SOCKET_ERROR;

    // Set "reuse port" option on the socket.
    int socket_option = 1;
    r = setsockopt(http_server->socket, SOL_SOCKET, SO_REUSEPORT, &socket_option, sizeof (socket_option));
    if (r == -1) return HTTP_SERVER_SOCKET_OPTION_ERROR;

    // Set up server address configuration.
    http_server->address.sin_family         = AF_INET;
    http_server->address.sin_addr.s_addr    = INADDR_ANY;
    http_server->address.sin_port           = htons(port_number);

    // Bind server address to socket.
    r = bind(http_server->socket, (struct sockaddr *) &(http_server->address), sizeof (http_server->address));
    if (r == -1) return HTTP_SERVER_BIND_ERROR;

    // Listen for incoming TCP connections.
    r = listen(http_server->socket, HTTP_SERVER_LISTEN_BACKLOG);
    if (r == -1) return HTTP_SERVER_LISTEN_ERROR;

    // Enabled the server.
    http_server->enabled = true;

    return HTTP_SERVER_SUCCESS;
}

/**
 * Destroy an HttpServer.
 * @param http_server The HttpServer.
 */
void
http_server_free(HttpServer *http_server)
{
    http_server->enabled = false;
    close(http_server->socket);
}

/**
 * Start an HttpServer listening for TCP connections and handling them.
 * @param http_server The HttpServer.
 * @return HTTP_SERVER_SUCCESS on success, otherwise a fatal error likely occurred.
 */
int
http_server_listen(const HttpServer *http_server)
{
    int r = 0;
    pid_t process_id = 0, child_process_id = 0;
    HttpClient http_client;

    // -- LISTEN LOOP -- //

    // Listen for connections while the server is enabled.
    while (http_server->enabled) {

        // Display waiting status.
        fprintf(stdout,
APP_NAME ": listening for connections on port %d\n",
                http_server->port);

        // Block until a client attempts to connect.
        (void) http_client_new(&http_client);
        r = http_server_accept_client(http_server, &http_client);

        // Check connection.
        if ((r == HTTP_CLIENT_FAILURE) && http_server->enabled) {
            fprintf(stderr,
APP_NAME ": a client was unable to connect\n");
            goto cleanup; // -- GOTO CLEANUP -- //
        }

        // Spawn a child process to handle the client.
        process_id = fork();

        // Check child.
        if (process_id == -1) {
            fprintf(stderr,
APP_NAME ": unable to spawn a child process to handle a client\n");
            goto cleanup; // -- GOTO CLEANUP -- //
        }

        // Send client to child process client handler.
        else if (process_id == 0) {
            http_server_client_handler(http_server, &http_client);

            // Terminate client handler process.
            _exit(EXIT_SUCCESS);
        }

    cleanup: // -- CLEANUP -- //

        // Free unused client socket (taken over by child process).
        http_client_free(&http_client);

        // Reap zombie child processes.
        child_process_id = waitpid(-1, NULL, WNOHANG);

        // Display reclamation status.
        if (child_process_id > 0) {
            fprintf(stdout,
APP_NAME ": handler %d was killed by the server\n",
                    child_process_id);
        }
    } // -- LISTEN LOOP END -- //

    // Display disabled status.
    fprintf(stdout,
APP_NAME ": the server is not longer listening for connections\n");

    return HTTP_SERVER_SUCCESS;
}

/**
 * Register an HttpServer to disable on interception of an OS interrupt.
 * @param http_server The HttpServer.
 */
void
http_server_disable_on_interrupt(HttpServer *http_server)
{
    http_server_to_disable_on_interrupt = http_server;

    const int signals[] = { SIGINT, SIGTERM }; // The list of signals to catch.
    const size_t signals_len = sizeof (signals) / sizeof (signals[0]);

    for (size_t i = 0; i < signals_len; i++) {
        signal(signals[i], http_server_signal_handler);
    }
}
