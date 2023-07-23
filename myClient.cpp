#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <netdb.h>
#include <sys/uio.h>
#include <sys/time.h>
#include <sys/wait.h>
#include <fcntl.h>

#include <iostream>
#include <string>
#include <fstream>

int main(int argc, char*argv[])
{
	//for IP address and port number
	if(argc != 3) {
		std::cerr << "Also enter IP address and port number while running the program.\n";
		exit(0);
	}

	//take IP address and port number
	char* serverIP = argv[1];
	int port = atoi(argv[2]);

	//getting ready for creating a socket
	struct hostent* host = gethostbyname(serverIP);
	sockaddr_in sendSockAddr;
	bzero((char*)&sendSockAddr, sizeof(sendSockAddr));
	sendSockAddr.sin_family = AF_INET; //for IPv4
	sendSockAddr.sin_addr.s_addr = inet_addr(inet_ntoa(*(struct in_addr*)*host->h_addr_list)); //to set destination server parameters
	sendSockAddr.sin_port = htons(port);

	//creating socket for our client
	int sockClient = socket(AF_INET, SOCK_STREAM, 0);

	//connecting to server
	int status = connect(sockClient, (sockaddr*)&sendSockAddr, sizeof(sendSockAddr));
	if(status < 0) {
		std::cout << "Error connecting to the server!\n";
		exit(0);
	}
	std::cout << "Conected to the server!\n";

	//measuring session time
	struct timeval start, end;
	gettimeofday(&start, NULL);

	//measuring amount of sent bytes
	int bytesRead = 0;
	int bytesWritten = 0;

	//creating buffer for message exchange
	char buff[1247];

	//starting the main part of session
	std::cout << "Enter request\n";
	while(1) 
	{
		std::cout << "> ";
		std::string request;
		std::getline(std::cin, request);
		memset(&buff, 0, sizeof(buff)); //clearing the buffer
		strcpy(buff, request.c_str()); //c_str() is called to obtain pointer on request
		if(!strcmp(buff, "exit")) {
			send(sockClient, (char*)&buff, strlen(buff), 0);
			break;
		}
		bytesWritten += send(sockClient, (char*)&buff, strlen(buff), 0);
		memset(&buff, 0, sizeof(buff)); // clearing the buffer
		bytesRead += recv(sockClient, (char*)&buff, sizeof(buff), 0);
		if(!strcmp(buff, "exit")) {
			std::cout << "Session terminated by server\n";
			break;
		}
		std::cout << "Server: " << buff << std::endl;
	}

	//closing sockets
	gettimeofday(&end, NULL);
	close(sockClient);
	std::cout << std::endl;
	std::cout << "*********Report**********\n";
	std::cout << "Bytes written: " << bytesWritten << std::endl;
	std::cout << "Bytes read: " << bytesRead << std::endl;
	std::cout << "Session duration: " << (end.tv_sec - start.tv_sec) << " seconds " << std::endl;
	std::cout << "Connection closed...\n";
	return 0;

}
