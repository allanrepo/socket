#include <iostream>
#include <sys/types.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <string.h>
#include <string>
 

#define PORT 54000

 
int main()
{
	/* ---------------------------------------------------------------------------------------------------------
	create a socket that we'll use to listen to an incoming connection request.
	-	AF_INET means we are using IPv4 protocols as communication domain. I don't really know what this means...
	-	SOCK_STREAM is set as type but i have no clue what this means...
	-	returns this socket's file descriptor if successful
	--------------------------------------------------------------------------------------------------------- */
	int fdSocketToListenTo = socket(AF_INET, SOCK_STREAM, 0);
    	if (fdSocketToListenTo == -1)
	{
		std::cerr << "Can't create a socket! Quitting" << std::endl;
		return -1;
	}

	/* ---------------------------------------------------------------------------------------------------------
	we now have a listening socket, but it does not have an address. now one can really connect to it because
	it has not identity. we'll do it here by binding it to a port and IP address where it can listen to.
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
	--------------------------------------------------------------------------------------------------------- */
	sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_port = htons(54000);
	inet_pton(AF_INET, "0.0.0.0", &addr.sin_addr);

	/* ---------------------------------------------------------------------------------------------------------
	
	--------------------------------------------------------------------------------------------------------- */
	bind(fdSocketToListenTo, (sockaddr*)&hint, sizeof(hint));
 
    // Tell Winsock the socket is for listening
    listen(fdSocketToListenTo, SOMAXCONN);
 
    // Wait for a connection
    sockaddr_in client;
    socklen_t clientSize = sizeof(client);
 
    int clientSocket = accept(listening, (sockaddr*)&client, &clientSize);
 
    char host[NI_MAXHOST];      // Client's remote name
    char service[NI_MAXSERV];   // Service (i.e. port) the client is connect on
 
    memset(host, 0, NI_MAXHOST); // same as memset(host, 0, NI_MAXHOST);
    memset(service, 0, NI_MAXSERV);
 
    if (getnameinfo((sockaddr*)&client, sizeof(client), host, NI_MAXHOST, service, NI_MAXSERV, 0) == 0)
    {
        cout << host << " connected on port " << service << endl;
    }
    else
    {
        inet_ntop(AF_INET, &client.sin_addr, host, NI_MAXHOST);
        cout << host << " _connected on port " << ntohs(client.sin_port) << endl;
    }
 
    // Close listening socket
    //close(listening);
 
    // While loop: accept and echo message back to client
    char buf[4096];
 
    while (true)
    {
        memset(buf, 0, 4096);
 
        // Wait for client to send data
        int bytesReceived = recv(clientSocket, buf, 4096, 0);
        if (bytesReceived == -1)
        {
            cerr << "Error in recv(). Quitting" << endl;
            break;
        }
 
        if (bytesReceived == 0)
        {
            cout << "Client disconnected " << endl;
		continue;
            break;
        }
 
        cout << string(buf, 0, bytesReceived) << endl;
 
        // Echo message back to client
        send(clientSocket, buf, bytesReceived + 1, 0);
    }
 
    // Close the socket
    close(clientSocket);
 
    return 0;
}
