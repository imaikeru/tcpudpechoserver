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
	if(argc<2)
	{
		printf("No port was entered. Server will use defaultPort = %d\n",defaultUDPPort);
		port=defaultUDPPort;
	}
	else
	{
		port=atoi(argv[1]);
	}
	char buffer[bufferSize];

	int client_fd = socket(AF_INET,SOCK_DGRAM,0);
	if(client_fd<0)
	{
		if(client_fd)
		{
			close(client_fd);
		}
		printf("Error. Couldn't create socket on server.\n");
		exit(1);
	}

	struct sockaddr_in server;
	server.sin_family = AF_INET;
	server.sin_port = htons(port);
	server.sin_addr.s_addr=htonl(INADDR_ANY);

	socklen_t server_len = sizeof(server);

	while(1)
	{
		printf("To server: ");
		fgets(buffer,bufferSize,stdin);
		if(strncmp(buffer,"over",4)==0)
		{
			printf("Transmission over. Closing connection with server.");
			close(client_fd);
			exit(0);
		}
		int totalBytesSent=0;
		int messageSize = strlen(buffer)+1;
		while(1)
		{
			//printf("To server: %s\n",buffer);
			if(totalBytesSent<messageSize)
			{
				int bytesSent=sendto(client_fd,buffer+totalBytesSent,messageSize-totalBytesSent,0,(struct sockaddr *)&server,server_len);
				if(bytesSent<=0)
				{
					printf("Error. Could not send message to server.\n");
					close(client_fd);
					exit(1);
				}
				else
				{
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
			readBytes = recvfrom(client_fd,buffer,bufferSize,0,(struct sockaddr *)&server, &server_len);
			//fprintf(fp2,&buffer[0]);
			//fflush(fp2);
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