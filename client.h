#ifndef CLIENT_H
#define CLIENT_H

enum Operation
{
	RETR,
	STOR
};

int invoke_client(char * server_ip_str, char * server_port_str,
				  enum Operation op, char * filename);

#endif // CLIENT_H
