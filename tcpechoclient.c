#include <stdio.h>
#include <stdlib.h> 
#include <string.h>
#include <netdb.h> 
#include <netinet/in.h> 
#include <sys/socket.h> 
#include <sys/types.h> 
#include <arpa/inet.h>
#include <unistd.h>

#define defaultTCPPort 7777
#define bufferSize 1024

int main(int argc, char *argv[])
{
	int port;
	if(argc<2)
	{
		printf("No port was entered. Server will use defaultTCPPort = %d\n",defaultTCPPort);
		port=defaultTCPPort;
	}
	else
	{
		port=atoi(argv[1]);
	}

	int client_fd;
	client_fd=socket(AF_INET,SOCK_STREAM,0);
	if(client_fd<0)
	{
		printf("Error. Couldn't create socket.\n");
		exit(1);
	}

	printf("Socket created successfully.\n");

	struct sockaddr_in server;
	server.sin_family=AF_INET;
	server.sin_port=htons(port);
	server.sin_addr.s_addr=htonl(INADDR_ANY);

	if(connect(client_fd,(struct sockaddr*) &server, sizeof(server))<0)
	{
		printf("Error. Couldn't connect to server.\n");
		if(client_fd)
		{
			close(client_fd);
		}
		exit(1);
	}

	printf("Successfully connected to server with port %d.\n",port);

	int dataSendCounter=0;
	int dataReceivedCounter=0;
	while(1)
	{
		char buffer[bufferSize];

		memset(buffer, '\0',bufferSize);

		printf("To server: ");
		fgets(buffer,bufferSize,stdin);
		if(strncmp(buffer,"over",4)==0)
		{
			printf("Transmission over. Closing connection with server.");
			close(client_fd);
			exit(0);
		}
		int bytesSent=0;
		int totalBytesSent=0;
		int timesSent=0;
		int messageSize=strlen(buffer)+1;
		//printf("\nSending data.\n");

		while(1)
		{
			if(totalBytesSent<messageSize)
			{
				bytesSent=send(client_fd,buffer+totalBytesSent,messageSize-totalBytesSent,0);
				if(bytesSent<=0)
				{
					printf("Error. Could not send message to server.\n");
					close(client_fd);
					exit(1);
				}
				else
				{
					//printf("To server: %s\n",buffer);
					totalBytesSent+=bytesSent;
				}
			}
			else
			{
				break;
			}
		}

		memset(buffer,'\0',bufferSize);
		int readBytes=0;
		int totalReadBytes=0;
		while(1)
		{
			fflush(stdout);
			if(totalReadBytes==messageSize)
			{
				break;
			}
			//printf("\nReceiving new data.\n");
			readBytes = recv(client_fd,buffer,bufferSize,0);
			dataReceivedCounter++;
			//printf("\nTimes received data = %d\n",dataReceivedCounter);
			if(readBytes<=0)
			{		
				printf("Error. Could not receive data from server.\n");
				close(client_fd);
				exit(1);
			}
			else
			{
				totalReadBytes+=readBytes;
			}
			printf("From server: %s",buffer);
			//printf("\nREADBYTES = %d\n",totalReadBytes);
		}
	}

	return 0;
}