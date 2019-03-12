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
	int sock_fd = socket(AF_INET, SOCK_STREAM, 0);	// AF_INET = IPV4, sys/socket.h
	if (sock_fd == -1)
	{
		std::cout << "ERROR: Failed to create socket!" << std::endl;
		return -1;
	}

	sockaddr_in addr; // netdb.h
	addr.sin_family = AF_INET;
	addr.sin_port = htons(8079);

	if (inet_pton(AF_INET, "0.0.0.0", &addr.sin_addr) <= 0)
	{
		std::cout << "ERROR: Invalid adress!" << std::endl;
		return -1;
	}

	if (connect(sock_fd, (sockaddr*)&addr, sizeof(addr)) < 0)
	{
		std::cout << "ERROR: No connection!" << std::endl;
		return -1;
	}

	char snd[] = "hello there i'm client!";
	send(sock_fd, snd, strlen(snd), 0);

	char buff[255] = "";
	read(sock_fd, buff, 255);
	std::cout << "buff: " << buff << std::endl; 

	return 0;
}
