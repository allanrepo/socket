#include <iostream>
//#include <sys/types.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <string.h>
#include <sstream>

#define PORT 54000
#define IP "127.0.0.1"

int main(int argc, char **argv)
{
	/* ---------------------------------------------------------------------------------------------------------
	set port based on command line argument if there's one. otherwise set to default
	--------------------------------------------------------------------------------------------------------- */
	unsigned short port;
	if (argc > 2)
	{
		std::stringstream ss(argv[2]);
		ss >> port;
	}
	else port = PORT;

	std::stringstream ip;
	if (argc > 1)
	{
		ip << argv[1];
	}
	else ip << PORT;
	std::cout << "Connecting to IP: " << ip.str() << " port: " << port << std::endl;



	int fdClientSocket = socket(AF_INET, SOCK_STREAM, 0);	// AF_INET = IPV4, sys/socket.h
	if (fdClientSocket == -1)
	{
		std::cerr << "ERROR: Failed to create socket!" << std::endl;
		return -1;
	}

	sockaddr_in addrClient; // netdb.h
	addrClient.sin_family = AF_INET;
	addrClient.sin_port = htons(port);
	if (inet_pton(AF_INET, "127.0.0.1", &addrClient.sin_addr) <= 0)
	{
		std::cout << "ERROR: Invalid adress!" << std::endl;
		return -1;
	}

	if (connect(fdClientSocket, (sockaddr*)&addrClient, sizeof(addrClient)) < 0)
	{
		std::cout << "ERROR: No connection!" << std::endl;
		return -1;
	}

	while(1)
	{
		std::string s;
		std::cout << ">";
		std::getline(std::cin, s);		

		send(fdClientSocket, s.c_str(), s.length(), 0);
	}

	close(fdClientSocket);

//	char snd[] = "hello there i'm client!";

//	char buff[255] = "";
//	read(sock_fd, buff, 255);
//	std::cout << "buff: " << buff << std::endl; 

	return 0;
}
