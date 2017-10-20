#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/sendfile.h>
#include <libgen.h>

#include "client.h"

int invoke_client(char * server_ip_str, char * server_port_str,
				  enum Operation op, char * filename)
{
	fprintf(stderr, "Connecting to %s:%s to %d %s\n",
					server_ip_str, server_port_str,
					op, filename);
    int BUFSIZE = 1024;
    int sockfd = 0;
    int fd = 0;
    struct sockaddr_in serv_addr;
    char * message;

	//Creating a socket
    if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("\n Error : Could not create socket \n");
        return 1;
    }

    memset(&serv_addr, '0', sizeof(serv_addr));

    //Initializing the address family.
    serv_addr.sin_family = AF_INET;

    //Adding the corresponding port
    serv_addr.sin_port = htons(atoi(server_port_str));

    //Adding the IP address of the server
    if(inet_pton(AF_INET, server_ip_str, &serv_addr.sin_addr)<=0) {
        printf("\n inet_pton error occured\n");
        return 1;
    }

    //Connecting to the server corresponding to the ip, port.
    if(connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
       printf("Error : Connect Failed \n");
       return 1;
    }
    else {
        printf("Connection to server successful\n");
    }
    ssize_t numBytes = 0;

    if(op == RETR) {
        const char *tmp1 = "RETR";
        const char *tmp2 = "\r\n";
        message = (char *)malloc(strlen(tmp1) + strlen(filename) + strlen(tmp2) + 1);
        strcpy(message, tmp1);
        strcat(message, filename);
        strcat(message, tmp2);
        numBytes = send(sockfd, message, strlen(message), 0);
        if(numBytes < 0) {
            printf("send() failed\n");
            return 1;
        }
        FILE *output_stream = fopen(filename, "w");
        // Retrieve the file
        do {
            char buffer[BUFSIZE];
            memset(buffer, '0', sizeof(buffer));
            numBytes = recv(sockfd, buffer, BUFSIZE - 1, 0);
            if (numBytes < 0) {
                printf("recv() failed \n");
                return 1;
            }
            else if (numBytes == 0) {
                break;
            }
            buffer[numBytes] = '\0';    // Terminate the string!
            fputs(buffer, output_stream); 
        } while (numBytes > 0);
    	fclose(output_stream);
    }
    else if(op == STOR) {
        const char *tmp1 = "STOR";
        const char *tmp2 = "\r\n";
        char *file = basename(filename);
	printf("file name: %s\n", file);
	message = (char *)malloc(strlen(tmp1) + strlen(file) + strlen(tmp2) + 1);
        strcpy(message, tmp1);
        strcat(message, file);
        strcat(message, tmp2);
        numBytes = send(sockfd, message, strlen(message), 0);
        if(numBytes < 0) {
            printf("send() failed\n");
            return 1;
        }
        fd = open(filename, O_RDONLY);
        if(!fd) {
            printf("Error: Could not receive operation\n");
        }
        printf("client: file opened\n");
	struct stat stat_buf;
        fstat(fd, &stat_buf);
        if(sendfile(sockfd, fd, NULL, stat_buf.st_size) != stat_buf.st_size) {
            printf("Error: fail to send file: %s\n", filename);
            return 1;
        }
    }
    else {
        printf("Invalid operation\n");
        return 1;
    }
    close(sockfd);
	return 0;  // 0 on success, non-zero on error
}
