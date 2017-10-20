#ifndef SERVER_H
#define SERVER_H

enum Operation
{
	RETR,
	STOR
};

int invoke_server(char * server_port_str, char * storage_directory);

#endif // SERVER_H
