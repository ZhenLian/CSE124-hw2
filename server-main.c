#include <stdio.h>
#include <string.h>
#include <strings.h>
#include "server.h"
 
int main(int argc, char* argv[])
{
    /* Parse the command line arguments */
    if (argc != 3) {
        fprintf(stderr,
                "Usage: %s server-port storage_directory\n",
                argv[0]);
        return 1;
    }
    char * server_port_str = argv[1];
    char * storage_directory = argv[2];
 
    return invoke_server(server_port_str, storage_directory);
}
