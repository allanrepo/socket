#include <iostream>
#include <sys/types.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <string.h>
#include <string>
#include <sstream>
#include <sys/select.h>

/* ---------------------------------------------------------------------------------------------------------
DESCRIPTION
this application demonstrates a TCP server application that listens and accept any client application
that wishes to connect to it. it can accept multiple clients connected simultaneously.
connected clients can send text message to this server and it will broadcast the message to other
clients connected to it. It uses select() to allow multiple client connection and manage their messages.

COMPILE
g++ demoTcpServerMultiConnect.cpp -o <exe>

HOW TO USE
<exe> <port>
- <port> is optional. uses #PORT by default
- compile demoTcpClient.cpp and run its binary as compliment to this software.
--------------------------------------------------------------------------------------------------------- */

// default listenting port used if not specified in command line 
#define PORT 54000

// it can receive up to a specific number of bytes of message. the size is specified below
#define MAXBYTEMESSAGE 4096

 
int main(int argc, char **argv)
{
	/* ---------------------------------------------------------------------------------------------------------
	set port based on command line argument if there's one. otherwise set to default
	--------------------------------------------------------------------------------------------------------- */
	unsigned short port;
	if (argc > 1)
	{
		std::stringstream ss(argv[1]);
		ss >> port;
	}
	else port = PORT;
	std::cout << "Listening to port: " << port << std::endl;

	/* ---------------------------------------------------------------------------------------------------------
	create a socket that we'll use to listen to an incoming connection request.
	-	AF_INET means we are using IPv4 protocols as communication domain. I don't really know what this means...
	-	SOCK_STREAM is set as type. it means the sockect uses TCP or connection oriented protocol
	-	returns this socket's file descriptor if successful
	--------------------------------------------------------------------------------------------------------- */
	int fdSocketToListenTo = socket(AF_INET, SOCK_STREAM, 0);
    	if (fdSocketToListenTo == -1)
	{
		std::cerr << "Can't create a socket! Quitting" << std::endl;
		return -1;
	}

	/* ---------------------------------------------------------------------------------------------------------
	we now have a listening socket, but it does not have an address. no one can really connect to it because
	it has not identity. we'll do it here by binding it to a port address. this port address in the server
	where this application can listen to incoming connection request
	-	AF_INET is set as family as we created socket based on that.
	-	using htons() when setting port number to ensure that this number will be set by compiler to match 
		whichever system this code is compiled into. hton stands for 'host to network short'. we do this 
		because certain systems arrange values > 255 bits (such as short (16 bits) or long (32 bits)), in 
		different manner - little/big endian for linux and unix systems. hton() lets you handle bit 
		arrangement as the code is compiled so you don't have to worry about it.
	-	we're passing our IP address as string. inet_pton() will be used to convert this string into 
		in_addr struct which contains the IP address structure broken down into series or bytes. 
		-	address family must be set to either AF_INET which takes 'x.x.x.x' format or
			AF_INET6 which can take 'x.x.x.x.x.x.x.x' 
		- 	inet_pton - 'pointer to string to number'
		-	set our IP address. our IP address is usually set dynamically so we'll have to keep 
			changing this value to what the current IP address is, and that's going to be tedious.
			to set this dynamically, we set it as '0.0.0.0'
	--------------------------------------------------------------------------------------------------------- */
	sockaddr_in addrServer;
	addrServer.sin_family = AF_INET;
	addrServer.sin_port = htons(port);
	inet_pton(AF_INET, "0.0.0.0", &addrServer.sin_addr);
	if (bind(fdSocketToListenTo, (sockaddr*)&addrServer, sizeof(addrServer)) == -1)
	{
		std::cerr << "ERROR: Failed to bind to port: " << port << "!" << std::endl;
		return -1;
	}

	/* ---------------------------------------------------------------------------------------------------------
	we now tell the system that we're listening for any incoming connection request in this socket 	
	-	the 2nd argument sets the max number of connection requests that an queue while they wait for 
		current connection is processed. this allows any incoming connection request that comes while
		this server is busy processing current connection to queue up and wait rather than getting 
		instantly refused. our course, if the queue is full, the client requesting connection will 
		receive an error and be refused connection.
		-	SOMAXCONN is constant that defines maximum possible queue count. value varies from
			system to system		
	--------------------------------------------------------------------------------------------------------- */
	if (listen(fdSocketToListenTo, SOMAXCONN) == -1)
	{
		std::cout << "ERROR: Something went wrong during listening!" << std::endl;
		return -1;
	}

	/* ---------------------------------------------------------------------------------------------------------
	create fd_set variable and store the file descriptors for listening port. the max fd value is required
	by select() so must get it
	--------------------------------------------------------------------------------------------------------- */
	fd_set fds;
	FD_ZERO(&fds);
	FD_SET(fdSocketToListenTo, &fds);

	// create variable to hold max file descriptor value. needed for select() later
	int nMaxFD = fdSocketToListenTo;

	/* ---------------------------------------------------------------------------------------------------------
	enter into app loop and monitor all sockets' file descriptor if changes happen to any of them.
	if there's an incoming connection request, listening port's file descriptor is set. connection to 
	server will be handled accordingly. 
	if incoming message request occurs, the sending client's file descriptor is set. handle it accordingly.
	--------------------------------------------------------------------------------------------------------- */
	while(1)
	{
		// we'll be blocking here and wait for any file descriptor ready to be read.
		fd_set ready = fds;
		int n = select(nMaxFD + 1, &ready, NULL, NULL, NULL);

		// check all file descriptors and find the one that is ready
		for (int i = 0; i <= nMaxFD; i++)
		{
			// only if this is the socket ready for reading
			if (!FD_ISSET(i, &ready)) continue;
		
			// is it the listening socket?
			if (i == fdSocketToListenTo)
			{
				// try to allow the incoming client to connect. if successful, the port info of
				// the client will be stored in sockaddr_in structure
			 	sockaddr_in addrClient;
			    	socklen_t nAddrClient = sizeof(addrClient); 
			    	int fdClientSocket = accept(fdSocketToListenTo, (sockaddr*)&addrClient, &nAddrClient);
				if (fdClientSocket == -1)
				{
					std::cout << "ERROR: Something went wrong during accept()!" << std::endl;
					continue;
				}			
				// if connection is successful, let's add this client's file descriptor to our fd_set
				FD_SET(fdClientSocket, &fds);
				nMaxFD = fdClientSocket > nMaxFD? fdClientSocket : nMaxFD;
				std::cout << "New client [" <<  fdClientSocket << "] connected at port: " << ntohs(addrClient.sin_port) << std::endl;
			}
			// it must be an incomming message from one of the clients
			else
			{
				// read the message
				char buf[MAXBYTEMESSAGE];
				int nBytesReceived = recv(i, buf, MAXBYTEMESSAGE, 0);

				// this client is disconnected. 
				if (nBytesReceived <= 0)
				{
					close(i);	
					FD_CLR(i, &fds);
					std::cout << "Client " << i << " has left the server." << std::endl;
					continue;
				}

				std::cout << "Incoming from [" << i << "] " << "'" << std::string(buf, 0, nBytesReceived) << "'"  << std::endl;				

				// broadcast this message to all clients
				for (int j = 0; j <= nMaxFD; j++)
				{
					// skip the listening socket
					if (j == fdSocketToListenTo) continue;

					// skip the same client that send this message
					if (j == i) continue;
					
					// send it
					std::stringstream msg;
					msg << "[" << i << "]" << "'" << std::string(buf, 0, nBytesReceived) << "'";	
					send(j, msg.str().c_str(), msg.str().length(), 0);
				}
			}
		}
	}

	/* ---------------------------------------------------------------------------------------------------------
	close the listening socket before you go
	--------------------------------------------------------------------------------------------------------- */
	close(fdSocketToListenTo);

	return 0;
}
