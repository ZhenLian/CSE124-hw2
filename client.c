#include <stdio.h>
#include "client.h"

int invoke_client(char * server_ip_str, char * server_port_str,
				  enum Operation op, char * filename)
{
	fprintf(stderr, "Connecting to %s:%s to %d %s\n",
					server_ip_str, server_port_str,
					op, filename);

	return 0;  // 0 on success, non-zero on error
}
