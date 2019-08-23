#include <stdio.h>
#include <stdlib.h> 
#include <string.h>
#include <netdb.h> 
#include <netinet/in.h> 
#include <sys/socket.h> 
#include <sys/types.h> 
#include <arpa/inet.h>
#include <unistd.h>

#define bufferSize 1024
#define defaultTCPPort 7777
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
	char buffer[bufferSize];

	int server_fd, client_fd, checker;
	struct sockaddr_in server, client;

	pid_t child_pid;

	// server initialization
	// if any error occurs here 
	// we need to fail and exit
	server_fd = socket(AF_INET,SOCK_STREAM,0);
	if(server_fd < 0)
	{
		printf("Couldn't create socket.\n");
		fflush(stdout);
		exit(1);
	}
	memset(&server,'\0',sizeof(server));
	server.sin_family=AF_INET;
	server.sin_port=htons(port);
	server.sin_addr.s_addr = htonl(INADDR_ANY);

	int reuseOn=1;
	if(setsockopt(server_fd,SOL_SOCKET,SO_REUSEADDR,&reuseOn,sizeof(reuseOn)) < 0)
	{
		printf("Error. setsockopt(SO_REUSEADDR) failed.\n");
		fflush(stdout);
		exit(1);
	}

	if(bind(server_fd,(struct sockaddr *) &server, sizeof(server))<0)
	{
		printf("Error. Binding server_fd to port %d failed. Some other process is probably using port %d\n",port,port);
		fflush(stdout);
		exit(1);
	}

	checker = listen(server_fd,128);
	if(checker<0)
	{
		printf("Error. Listening on socket failed.\n");
		fflush(stdout);
		exit(1);
	}

	printf("Echo server is listening on port %d.\n",port);
	fflush(stdout);

	// main server loop
	// any errors in the main process are ignored
	int dataSendCounter=0;
	int dataReceivedCounter=0;
	while(1)
	{
		int receivedBytesFromClient = 0;
		int sentBytesToClient=0;
		socklen_t client_len = sizeof(client);
		memset(&client,'\0',client_len);
		client_fd = accept(server_fd,(struct sockaddr*) &client,&client_len);
		if(client_fd<0)
		{
			printf("Error. Failed to establish new connection.");
			fflush(stdout);
			continue;
		}
		if((child_pid=fork())==0)
		{
			printf("Accepted connection from %s:%d" , inet_ntoa(client.sin_addr),ntohs(client.sin_port));
			// free parent resource (server_fd)
			close(server_fd);
			int hasConnectionWithClient=1;
			while(1)
			{
				printf("\nReceiving new data.\n");
				int readBytes = recv(client_fd,buffer,bufferSize,0);
				dataReceivedCounter++;
				printf("\nTimes received data = %d \n",dataReceivedCounter);
				if(readBytes<=0)
				{
					printf("Error. Connection with %s:%d lost. Could not read message. readBytes = %d \n",inet_ntoa(client.sin_addr),ntohs(client.sin_port),readBytes);
					fflush(stdout);
					break;
				}
				receivedBytesFromClient+=readBytes;
				printf("\nREADBYTES = %d\n",readBytes);
				buffer[readBytes]='\0';
				char * clientAddress=inet_ntoa(client.sin_addr);
				int clientPort=ntohs(client.sin_port);
				printf("Message sent from client(%s:%d): %s ",inet_ntoa(client.sin_addr),ntohs(client.sin_port),buffer);
				fflush(stdout);
				int timesSent=0;
				int bytesSent=0;
				int totalBytesSent=0;
				while(1)
				{
					bytesSent=totalBytesSent;
					if(totalBytesSent<readBytes)
					{
						int sentBytes=send(client_fd,buffer+totalBytesSent,readBytes-totalBytesSent,0);
						if(sentBytes>0)
						{
							totalBytesSent+=sentBytes;
							sentBytesToClient+=sentBytes;
							timesSent++;
							printf("\nSending data.\n");
							printf("Sending message to client(%s:%d): %s",inet_ntoa(client.sin_addr),ntohs(client.sin_port),buffer);
						}
						else
						{
							printf("Error. Connection with %s:%d lost. Could not send message.",inet_ntoa(client.sin_addr),ntohs(client.sin_port));
							fflush(stdout);
							hasConnectionWithClient=0;
							break;
						}
					}
					else
					{
						break;
					}
				}
				if(!hasConnectionWithClient)
				{
					break;
				}
			}
			if(client_fd)
			{
				close(client_fd);
			}
			printf("Sent bytes = %d\n",sentBytesToClient);
			printf("Received bytes = %d\n",receivedBytesFromClient);
			exit(0);
		}
		// parent closes its client file descriptor
		// all the work is done by the child process
		// parent process just forks itself
		if(child_pid>0)
		{
			if(client_fd)
			{
				close(client_fd);
			}
		}
		// error from fork is ignored
		// because we might not have enough system resources (e.g. RAM)
		if(child_pid<0)
		{
			printf("Error. Could not fork myself.");
			if(client_fd)
			{
				close(client_fd);
			}
			continue;
		}
	}
	// close(server_fd);
	return 0;
}