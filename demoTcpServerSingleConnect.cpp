#include <iostream>
#include <sys/types.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <string.h>
#include <string>
#include <sstream>

/* ---------------------------------------------------------------------------------------------------------
DESCRIPTION
this application demonstrates a TCP server application that listens and accept any client application
that wishes to connect to it.
it uses the port below as listening port */
#define PORT 4000
/*
it creates a listening port and waits for a client to request connection. 
once it receives and accept a connection from a client, it stops listening and then wait for 
message (in bytes) from client and prints it out. the max number of bytes the message can accept is */
#define MAXBYTEMESSAGE 256
/*
it will keep printing out any message sent for by the client until it receives 'quit'.

optionally, it will print the port information of the client that connects to it using getnameinfo().
you can turn this off by setting this to false */
#define GETNAMEINFO true
 
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
	-	AF_INET means we are using IPv4 protocols as communication domain. 
	-	SOCK_STREAM is set as type. it means the sockect uses TCP or connection oriented protocol
	-	returns this socket's file descriptor if successful
	--------------------------------------------------------------------------------------------------------- */
	int fdSocketToListenTo = socket(AF_INET, SOCK_DGRAM, 0);
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

/*
	if (listen(fdSocketToListenTo, SOMAXCONN) == -1)
	{
		std::cout << "ERROR: Something went wrong during listening!" << std::endl;
		return -1;
	}
 */
	/* ---------------------------------------------------------------------------------------------------------
	at this point, if any client makes a connection request to this server, we'll be able to accept that with
	the use of accept(). accept() blocks this server application and remains here until it receives connection
	request
	-	1st argument specifies which sockect we are listening to, in this case it's the socket we set 
		this server as its listening socket
	-	2nd argument will be the socket address structure where the information (port, IP) of the 
		incoming client that is trying to connect to will be stored.
	-	accept() returns the socket of the client that connects to this server
	--------------------------------------------------------------------------------------------------------- */
 	sockaddr_in addrClient;
    	socklen_t nAddrClient = sizeof(addrClient); 
    	int fdClientSocket = accept(fdSocketToListenTo, (sockaddr*)&addrClient, &nAddrClient);
	if (fdClientSocket == -1)
	{
		std::cout << "ERROR: Something went wrong during accept()!" << std::endl;
		return -1;
	}
 
	/* ---------------------------------------------------------------------------------------------------------
	the next codes are optional. what it tries to do is get print the port information of both the server and
	client
	--------------------------------------------------------------------------------------------------------- */
#if GETNAMEINFO
	char client[NI_MAXHOST]; // Client's remote name
    	char server[NI_MAXSERV]; // Service (i.e. port) the client is connect on
 
	memset(client, 0, NI_MAXHOST); // same as memset(host, 0, NI_MAXHOST);
    	memset(server, 0, NI_MAXSERV);

	if (getnameinfo((sockaddr*)&addrClient, sizeof(addrClient), client, NI_MAXHOST, server, NI_MAXSERV, 0) == 0)
	{
		std::cout << client << " is connected on port " << server << " (via getnameinfo())" << std::endl;
	}
	else
	{
		inet_ntop(AF_INET, &addrClient.sin_addr, client, NI_MAXHOST);
		std::cout << client << " is connected on port " << ntohs(addrClient.sin_port) << " (via inet_ntop)" << std::endl;
	}
 #endif
	/* ---------------------------------------------------------------------------------------------------------
	we don't need the listening port now as we already have a client connected to us
	--------------------------------------------------------------------------------------------------------- */
	close(fdSocketToListenTo);
 
	/* ---------------------------------------------------------------------------------------------------------
	keep looping now while we wait for client to send message
	--------------------------------------------------------------------------------------------------------- */
	char buf[MAXBYTEMESSAGE];
	while (true)
	{
		memset(buf, 0, MAXBYTEMESSAGE);

		// recv() blocks the server and waits for client to send data
		int nBytesReceived = recv(fdClientSocket, buf, MAXBYTEMESSAGE, 0);
		if (nBytesReceived == -1)
		{
		    std::cerr << "Error in recv(). Quitting" << std::endl;
		    break;
		}

		if (nBytesReceived == 0)
		{
		    std::cout << "Client disconnected " << std::endl;
		    break;
		}

		std::string s(buf, 0, nBytesReceived);
		if (s.compare(0, 4, "quit", 4) == 0) break;

		std::cout << "'" << std::string(buf, 0, nBytesReceived) << "'"  << std::endl;

		// Echo message back to client
		//send(clientSocket, buf, bytesReceived + 1, 0);
	}

	// close the client's socket. we're done.
	close(fdClientSocket);

	return 0;
}
