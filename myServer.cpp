#include <iostream>
#include <string>
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
#include <fstream>

int main(int argc, char*argv[]) 
{
	//for port number
	if(argc != 2) {
		std::cerr << "Also enter the port number while running the program.\n";
		exit(0);//programm terminated
	}

	//take the port number
	int port = atoi(argv[1]);

	//getting ready for creating a socket
	sockaddr_in servAddr;
	bzero((char*)&servAddr, sizeof(servAddr));
	servAddr.sin_family = AF_INET; //for IPv4
	servAddr.sin_addr.s_addr = htonl(INADDR_ANY);//to enable socket accept connections from any IP address
	servAddr.sin_port = htons(port);

	//creating the accepter socket
	int listener = socket(AF_INET, SOCK_STREAM, 0);
	if(listener < 0) {
		std::cerr << "Error establishing the server socket\n";
		exit(0);
	}	

	//binding the socket to it's local address
	int bindStatus = bind(listener, (struct sockaddr*) &servAddr, sizeof(servAddr));
	if(bindStatus < 0) {
		std::cerr << "Error binding socket to local address!\n";
		exit(0);
	}
	std::cout << "Waiting for a client to connect...\n";

	//listen up to 3 requests at a time
	listen(listener, 3);

	//to serve a client request using accept we need new address for new socket
	sockaddr_in newSockAddr;
	socklen_t newSockAddrLen = sizeof(newSockAddr);
	//now creating a new socket for new client
	int sockServer = accept(listener, (sockaddr*)&newSockAddr, &newSockAddrLen);
	if(sockServer < 0) {
		std::cerr << "Error accepting request from client!\n";
		exit(0);
	}
	std::cout << "Connected with client!\n";

	//measuring the session time
	struct timeval start, end;
	gettimeofday(&start, NULL);

	//also measuring amount of sent bytes
	int bytesRead = 0;
        int bytesWritten = 0;

	//creating a buffer for message exchange
	char buff[1247];

	//starting the main part of session
	while(1) 
	{
		std::cout << "Waiting for client response...\n";
		memset(&buff, 0, sizeof(buff)); //clear the buffer
		bytesRead += recv(sockServer, (char*)&buff, sizeof(buff), 0);
		if(!strcmp(buff, "exit")) {
			std::cout << "Session terminated by client\n";
			break;
		}
		std::cout << "Client: " << buff << std::endl;
		std::cout << "> ";
		std::string response;
		std::getline(std::cin, response);
		memset(&buff, 0, sizeof(buff)); //clear the buffer
		strcpy(buff, response.c_str()); //c_str() is called to obtain pointer on response
		if(response == "exit") {
			send(sockServer, (char*)&buff, strlen(buff), 0);
			break;
		}
		//sending the response back
		bytesWritten += send(sockServer, (char*)&buff, strlen(buff), 0);
	}

	//closing sockets
	gettimeofday(&end, NULL);
	close(sockServer);
	close(listener);
	std::cout << std::endl;
	std::cout << "**********Report**********\n";
      	std::cout << "Bytes written: " << bytesWritten << std::endl;
	std::cout << "Bytes read: " << bytesRead << std::endl;
	std::cout << "Session duration: " << (end.tv_sec - start.tv_sec) << " seconds " << std::endl;
	std::cout << "Connection closed...\n";
	return 0;	
}
