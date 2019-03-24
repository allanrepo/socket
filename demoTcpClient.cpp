#include <iostream>
#include <unistd.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <string.h>
#include <sstream>
#include <stdio.h>

/* ---------------------------------------------------------------------------------------------------------
DESCRIPTION
this application demonstrates a TCP client application that on launch, will attempt to connect to server
with specified IP and port. it can send message to server where the server will broadcast the message
to other connected clients. simultaneously, it can receive messages broadcasted by server. it uses
select() to allow stdin and send() to work simultaneously.

COMPILE
g++ demoTclClient.cpp -o <exe>

HOW TO USE
<exe> <ip> <port>
- <port> is optional. uses #PORT by default
- <ip> is optional. uses #IP by default
- compile demoTcpServerMultiConnect.cpp and run its binary as compliment to this software.
--------------------------------------------------------------------------------------------------------- */

#define IP "127.0.0.1"

// default listenting port used if not specified in command line 
#define PORT 54000

// it can receive up to a specific number of bytes of message. the size is specified below
#define MAXBYTEMESSAGE 4096


int main(int argc, char **argv)
{
	/* ---------------------------------------------------------------------------------------------------------
	set port and ip based on command line argument if there's one. otherwise set to default
	--------------------------------------------------------------------------------------------------------- */
	unsigned short port;
	if (argc > 2)
	{
		std::stringstream ss(argv[2]);
		ss >> port;
	}
	else port = PORT;

	std::stringstream ip;
	ip << ( argc > 1? argv[1] : IP );

	std::cout << "Connecting to IP: " << ip.str() << " port: " << port << std::endl;

	/* ---------------------------------------------------------------------------------------------------------
	create a socket that we'll use to listen to an incoming connection request.
	-	AF_INET means we are using IPv4 protocols as communication domain. 
	-	SOCK_STREAM is set as type. it means the socket uses TCP or connection oriented protocol
	-	returns this socket's file descriptor if successful
	--------------------------------------------------------------------------------------------------------- */
	int fdClientSocket = socket(AF_INET, SOCK_STREAM, 0);	
	if (fdClientSocket == -1)
	{
		std::cerr << "ERROR: Failed to create socket!" << std::endl;
		return -1;
	}

	/* ---------------------------------------------------------------------------------------------------------
	setup port info for the server we want to connect to and try to connect to it
	--------------------------------------------------------------------------------------------------------- */
	sockaddr_in addrClient; 
	addrClient.sin_family = AF_INET;
	addrClient.sin_port = htons(port);
	if (inet_pton(AF_INET, ip.str().c_str(), &addrClient.sin_addr) <= 0)
	{
		std::cout << "ERROR: Invalid adress!" << std::endl;
		return -1;
	}

	if (connect(fdClientSocket, (sockaddr*)&addrClient, sizeof(addrClient)) < 0)
	{
		std::cout << "ERROR: No connection!" << std::endl;
		return -1;
	}
	std::cout << "Successfully connected to server at port: " << ntohs(addrClient.sin_port) << std::endl;

	/* ---------------------------------------------------------------------------------------------------------
	create fd_set variable and store the file descriptors for listening port. the max fd value is required
	by select() so must get it
	--------------------------------------------------------------------------------------------------------- */
	fd_set fds;
	FD_ZERO(&fds);
	FD_SET(fileno(stdin), &fds);
	FD_SET(fdClientSocket, &fds);

	// create variable to hold max file descriptor value. needed for select() later
	int nMaxFD = fdClientSocket;

	/* ---------------------------------------------------------------------------------------------------------
	enter into app loop and monitor all sockets' file descriptor if changes happen to any of them.
	if there's an incoming connection request, listening port's file descriptor is set. connection to 
	server will be handled accordingly. 
	if incoming message request occurs, the sending client's file descriptor is set. handle it accordingly.
	--------------------------------------------------------------------------------------------------------- */
	bool go = true;
	while(go)
	{
		// we'll be blocking here and wait for any file descriptor ready to be read.
		fd_set ready = fds;
#if 0
		int n = select(nMaxFD + 1, &ready, NULL, NULL, NULL);
#else
		struct timeval to;
		to.tv_sec = 0;
		to.tv_usec = 1000000; // 100 msec

		int n = select(nMaxFD + 1, &ready, NULL, NULL, &to);
		//std::cout << "time out" << std::endl;
		if (n == 0) continue;
#endif
		// check all file descriptors and find the one that is ready
		for (int i = 0; i <= nMaxFD; i++)
		{
			// only if this is the socket ready for reading
			if (!FD_ISSET(i, &ready)) continue;

			// message is being typed on stdin. let's get it and send to server
			if (i == fileno(stdin))
			{
				std::string s;
				std::getline(std::cin, s);		
				send(fdClientSocket, s.c_str(), s.length(), 0);	
				continue;
			}
			// message is received from server. must be a broadcast message. let's print it.
			else
			{
				char buf[MAXBYTEMESSAGE];
				int nBytesReceived = recv(i, buf, MAXBYTEMESSAGE, 0);

				// this client is exiting
				if (nBytesReceived <= 0)
				{
					close(i);	
					FD_CLR(i, &fds);
					std::cout << "Server stopped. Quitting..." << std::endl;
					go = false;
					break;
				}

				std::cout << std::string(buf, 0, nBytesReceived)  << std::endl;				

			}
		}
	}
	close(fdClientSocket);

	return 0;
}
