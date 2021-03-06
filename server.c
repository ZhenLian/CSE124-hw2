#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <time.h>
#include <fcntl.h>

#include "server.h"

ssize_t sendall(int sockfd, char *buffer, size_t len) {
    ssize_t ret, sent = 0;
    while(sent < len) {
        ret = send(sockfd, buffer + sent, len - sent, 0);
        if(ret < 0) {
            return -1;
        }
        sent += ret;
    }
    return sent;
}

int invoke_server(char * server_port_str, char * storage_directory)
{
	fprintf(stderr, "Invoking server on port %s, using dir %s\n",
			server_port_str, storage_directory);
    int BUFSIZE = 1024;
	int listenfd = 0, connfd = 0;
	int fd = 0;
	char operation[4];
	char * file_name;
    struct sockaddr_in serv_addr;
    char buffer[BUFSIZE];

    memset(operation, '0', sizeof(operation));

    //creating a socket
    listenfd = socket(AF_INET, SOCK_STREAM, 0);

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(atoi(server_port_str));
 
    //binding socket with the address and port
    bind(listenfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)); 

    //listening
    listen(listenfd, 10);
    printf("listening...\n");
    while(1)
    {
	    //waiting for connection
        connfd = accept(listenfd, (struct sockaddr*)NULL, NULL); 
        if(connfd < 0) {
        	printf("Error: accept() failed\n");
        	return 1;
        }
        printf("accept succeed\n");
        ssize_t numBytes = recv(connfd, operation, 4, 0);
        if(numBytes < 0) {
        	printf("Error: Could not receive operation\n");
        	return 1;
        }
        printf("operation received: %s\n", operation);

        memset(buffer, '0', sizeof(buffer));

        numBytes = recv(connfd, buffer, BUFSIZE - 1, 0);
        if(numBytes < 0) {
            printf("Error: Could not receive filename\n");
            return 1;
        }
        printf("Getting filename...\n");
        char *pch = strstr(buffer, "\r");
        if(!pch) {
            printf("Error: Invalid filename\n");

            return 1;
        }
        printf("valid filename\n");
        const char* tmp = "/";
        file_name = (char*)malloc(strlen(storage_directory) + strlen(tmp) + (pch - buffer) + 1);

        strcpy(file_name, storage_directory);
        strcat(file_name, tmp);
        strncat(file_name, buffer, pch - buffer);
        printf("File path received: %s\n", file_name);

        if(strcmp(operation, "RETR") == 0) {
            FILE *input_stream = fopen(file_name, "r");
            if (!input_stream) {
                printf("cannot open file: %s", file_name);
            }
            ssize_t nread = 0;
            do {
                char buffer[BUFSIZE];
                memset(buffer, '0', BUFSIZE);
                nread = fread(buffer, 1, BUFSIZE, input_stream);
                if(nread <= 0) {
                    break;
                }
                numBytes = sendall(connfd, buffer, nread);
                if (numBytes < 0) {
                    printf("recv() failed \n");
                    return 1;
                }
                else if (numBytes == 0) {
                    break;
                }
            } while(numBytes > 0);
            fclose(input_stream);
        }
        else if(strcmp(operation, "STOR") == 0) {
	        FILE *output_stream = fopen(file_name, "w");
	        if(!output_stream) {
	        	printf("Error: Could not receive operation\n");
	        }
            char *pch2 = strstr(pch, "\r");
            fwrite(pch2 + 2, sizeof(char), numBytes - (pch2 + 2 - buffer), output_stream);
	        do {
	            memset(buffer, '0', sizeof(buffer));
		        numBytes = recv(connfd, buffer, BUFSIZE, 0);
	            if (numBytes < 0) {
	                printf("recv() failed \n");
	                return 1;
	            }
	            else if (numBytes == 0) {
	                break;
	            }
		        fwrite(buffer, sizeof(char), numBytes, output_stream);
	        } while (numBytes > 0);
		fclose(output_stream);
        }
        else {
        	printf("Error: Wrong operation: %s\n", operation);
        	return 1;
        }
        
        close(fd);

        close(connfd);
        sleep(1);
    }
    close(listenfd);
	return 0; // 0 on success, non-zero on failure
}
