#include <stdio.h>
#include <string.h>
#include <strings.h>
#include "client.h"
 
int main(int argc, char* argv[])
{
    /* Parse the command line arguments */
    if (argc != 5) {
        fprintf(stderr,
                "Usage: %s server-ip server-port [retr|stor] filename\n",
                argv[0]);
        return 1;
    }
    char * server_ip_str = argv[1];
    char * server_port_str = argv[2];
 
    char * opstr = argv[3];
    enum Operation op;
    if (!strncasecmp(opstr, "retr", strlen("retr"))) {
        op = RETR;
    } else if (!strncasecmp(opstr, "stor", strlen("stor"))) {
        op = STOR;
    } else {
        fprintf(stderr, "%s is not either retr or stor\n", opstr);
        return 1;
    }
 
    char * fn = argv[4];
 
    return invoke_client(server_ip_str, server_port_str, op, fn);
}
