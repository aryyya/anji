/**
 * aryyya, 2016
 */

#include <stdio.h>
#include <stdlib.h>
#include "http-server.h"

#undef  APP_NAME
#define APP_NAME "anji"

int
main(int argc, char *argv[])
{
    // Check command-line arguments.
    if (argc < 2) {
        fprintf(stderr,
APP_NAME ": not enough arguments\n"
"usage: " APP_NAME " [port-number]\n");
        exit(EXIT_FAILURE);
    }
    int r = 0, port_num = atoi(argv[1]);

    // Start server on the given port.
    HttpServer http_server;
    r = http_server_new(&http_server, port_num);

    // Check that the server started properly.
    if (r != HTTP_SERVER_SUCCESS) {
        const char *error_msg = "\0";
        switch (r) {
            case HTTP_SERVER_PORT_RANGE_ERROR:     error_msg = "port must be between 1 and 65535";                break;
            case HTTP_SERVER_SOCKET_ERROR:         error_msg = "failed to open a socket";                         break;
            case HTTP_SERVER_SOCKET_OPTION_ERROR:  error_msg = "failed to set a socket option";                   break;
            case HTTP_SERVER_BIND_ERROR:           error_msg = "failed to bind to the socket (permissions?)";     break;
            case HTTP_SERVER_LISTEN_ERROR:         error_msg = "failed to listen for incoming connections";       break;
            default:                               error_msg = "something inexplicable happened";                 break;
        }
        fprintf(stdout,
APP_NAME ": %s\n",
                error_msg);
        exit(EXIT_FAILURE);
    }

    // Set the server to disable if a signal event is caught.
    http_server_disable_on_interrupt(&http_server);

    // Start listening for connections.
    r = http_server_listen(&http_server);

    // Free resources.
    http_server_free(&http_server);

    return EXIT_SUCCESS;
}