#include <stdio.h>
#include "server.h"

int invoke_server(char * server_port_str, char * storage_directory)
{
	fprintf(stderr, "Invoking server on port %s, using dir %s\n",
			server_port_str, storage_directory);

	return 0; // 0 on success, non-zero on failure
}
