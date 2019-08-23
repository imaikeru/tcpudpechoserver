#include <stdio.h>
#include <stdlib.h> 
#include <string.h>
#include <netdb.h> 
#include <netinet/in.h> 
#include <sys/socket.h> 
#include <sys/types.h> 
#include <arpa/inet.h>
#include <unistd.h>

#define defaultUDPPort 7778
#define bufferSize 1024

int main(int argc, char *argv[])
{
	int port;
	if(argc<3)
	{
		printf("No port was entered. Server will use defaultUDPPort = %d\n",defaultUDPPort);
		port=defaultUDPPort;
	}
	else
	{
		port=atoi(argv[2]);
	}
	char buffer[bufferSize];

	int server_fd = socket(AF_INET,SOCK_DGRAM,0);
	struct sockaddr_in server,client;
	if(server_fd<0)
	{
		if(server_fd)
		{
			close(server_fd);
		}
		printf("Error. Couldn't create socket on server.\n");
		exit(1);
	}

	server.sin_family = AF_INET;
	server.sin_port = htons(port);
	server.sin_addr.s_addr=htonl(INADDR_ANY);

	int reuseOn=1;
	if(setsockopt(server_fd,SOL_SOCKET,SO_REUSEADDR,&reuseOn,sizeof(reuseOn)) < 0)
	{
		printf("Error. setsockopt(SO_REUSEADDR) failed.\n");
		fflush(stdout);
		exit(1);
	}

	if(bind(server_fd,(struct sockaddr *) &server, sizeof(server))<0)
	{
		printf("Error. Binding datagram socket server_fd to port %d failed.\n",port);
		if(server_fd)
		{
			close(server_fd);
		}
		exit(1);
	}

	while(1)
	{
		socklen_t client_len = sizeof(client);

		while(1)
		{
			int readBytes = recvfrom(server_fd,buffer,bufferSize,0,(struct sockaddr *)&client,&client_len);
			if(readBytes<=0)
			{
				printf("Error. Could not read data from client.\n");
				break;
			}
			buffer[readBytes]='\0';
			char * clientAddress = inet_ntoa(client.sin_addr);
			int clientPort = ntohs(client.sin_port);
			printf("From client(%s:%d): %s",clientAddress,clientPort,buffer);
			int totalBytesSent = 0;
			int errorless = 1;
			while(1)
			{
				if(totalBytesSent==readBytes)
				{
					break;
				}
				int sentBytes = sendto(server_fd,buffer+totalBytesSent,readBytes-totalBytesSent,0,(struct sockaddr *)&client, client_len);
				if(sentBytes<=0)
				{
					printf("Error. Could not send data to client.\n");
					errorless=0;
					break;
				}
				else
				{
					printf("To client(%s:%d): %s",clientAddress,clientPort,buffer);
					totalBytesSent+=readBytes;
				}
			}
			if(!errorless)
			{
				break;
			}
		}
	}
	if(server_fd)
	{
		close(server_fd);
	}

	return 0;
}