#include <iostream>
//#include <sys/types.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <string.h>
//#include <string>

int main()
{
	// create a socket 
	int sock_fd = socket(AF_INET, SOCK_STREAM, 0);	// AF_INET = IPV4, sys/socket.h
	if (sock_fd == -1)
	{
		std::cout << "ERROR: Failed to create socket!" << std::endl;
		return -1;
	}

	// bind the socket to ip/port
	sockaddr_in addr; // netdb.h
	addr.sin_family = AF_INET;
	addr.sin_port = htons(8079);
	addr.sin_addr.s_addr = INADDR_ANY;
//	inet_pton(AF_INET, "0.0.0.0", &hint.sin_addr); // arpa/inet.h

	// mark the socket for listening in
	if (bind(sock_fd, (sockaddr*)&addr, sizeof(addr)) == -1)
	{
		std::cout << "ERROR: Failed to bind to IP/port!" << std::endl;
		return -2;
	}

	if (listen(sock_fd, 4) < 0)
	{
		std::cout << "ERROR: Something went wrong during listening!" << std::endl;
		return -3;
	}

	int addrlen = sizeof(addr);
	while (1)
	{
		int new_socket = accept(sock_fd, (sockaddr*)&addr, (socklen_t*)&addrlen);
		if (new_socket < 0)
		{
			std::cout << "ERROR: Something went wrong during accept!" << std::endl;
			return -4;
		}


		char buff[255] = "";
		char snd[] = "hello there i'm server!";
		read(new_socket, buff, 255);
		std::cout << "buff: " << buff << std::endl;
		send(new_socket, snd, strlen(snd), 0);
	}
	

	// accept a call

	// close the listening socket

	// whie receiveing - display message, echo message

	// close socket

	return 0;
}
