#include <socket.h>


CClient::CClient()
{

}

CClient::~CClient()
{

}

bool CClient::connect(const std::string& ip, unsigned short port )
{
	/* ---------------------------------------------------------------------------------------------------------
	create a socket that we'll use to listen to an incoming connection request.
	-	AF_INET means we are using IPv4 protocols as communication domain. 
	-	SOCK_STREAM is set as type. it means the socket uses TCP or connection oriented protocol
	-	returns this socket's file descriptor if successful
	--------------------------------------------------------------------------------------------------------- */
	m_fdSocket = socket(AF_INET, SOCK_STREAM, 0);	
	if (m_fdSocket == -1)
	{
		std::cerr << "ERROR: Failed to create socket!" << std::endl;
		return false;
	}

	/* ---------------------------------------------------------------------------------------------------------
	setup port info for the server we want to connect to and try to connect to it
	--------------------------------------------------------------------------------------------------------- */
	sockaddr_in addrClient; 
	addrClient.sin_family = AF_INET;
	addrClient.sin_port = htons(port);
	if (inet_pton(AF_INET, ip.c_str(), &addrClient.sin_addr) <= 0)
	{
		std::cout << "ERROR: Invalid adress!" << std::endl;
		return false;
	}

	if (::connect(m_fdSocket, (sockaddr*)&addrClient, sizeof(addrClient)) < 0)
	{
		std::cout << "ERROR: No connection!" << std::endl;
		return false;
	}
	std::cout << "Successfully connected to server at port: " << ntohs(addrClient.sin_port) << std::endl;

	/* ---------------------------------------------------------------------------------------------------------
	create fd_set variable and store the file descriptors for listening port. the max fd value is required
	by select() so must get it
	--------------------------------------------------------------------------------------------------------- */
	FD_ZERO(&m_fds);
	FD_SET(fileno(stdin), &m_fds);
	FD_SET(m_fdSocket, &m_fds);

	// create variable to hold max file descriptor value. needed for select() later
	m_nMaxFD = m_fdSocket;

	return true;
}

bool CClient::start(unsigned long ms)
{
	/* ---------------------------------------------------------------------------------------------------------
	enter into app loop and monitor all sockets' file descriptor if changes happen to any of them.
	if there's an incoming connection request, listening port's file descriptor is set. connection to 
	server will be handled accordingly. 
	if incoming message request occurs, the sending client's file descriptor is set. handle it accordingly.
	--------------------------------------------------------------------------------------------------------- */
	bool go = true;
	while(go)
	{
		struct timeval to;
		to.tv_sec = (unsigned long) ms / 1000; // convert to second
		to.tv_usec = (ms % 1000) * 1000; // convert to usec

		// if timeout is not specified, we'll be blocking here and wait for any file descriptor ready to be read.
		fd_set ready = m_fds;
		if (to.tv_sec == 0 && to.tv_usec == 0)
		{
			int n = select(m_nMaxFD + 1, &ready, NULL, NULL, NULL);
		}
		// otherwise wait for timeout
		else
		{	
			//std::cout << "time out" << std::endl;
			int n = select(m_nMaxFD + 1, &ready, NULL, NULL, &to);
			if (n == 0) continue;
		}

		// check all file descriptors and find the one that is ready
		for (int i = 0; i <= m_nMaxFD; i++)
		{
			// only if this is the socket ready for reading
			if (!FD_ISSET(i, &ready)) continue;

			// message is being typed on stdin. let's get it and send to server
			if (i == fileno(stdin))
			{
				std::string s;
				std::getline(std::cin, s);		
				::send(m_fdSocket, s.c_str(), s.length(), 0);	
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
					FD_CLR(i, &m_fds);
					std::cout << "Server stopped. Quitting..." << std::endl;
					go = false;
					break;
				}

				std::cout << std::string(buf, 0, nBytesReceived)  << std::endl;			
			}
		}
	}
}

bool CClient::read(std::string& out, unsigned long ms)
{
	/* ---------------------------------------------------------------------------------------------------------
	enter into app loop and monitor all sockets' file descriptor if changes happen to any of them.
	if there's an incoming connection request, listening port's file descriptor is set. connection to 
	server will be handled accordingly. 
	if incoming message request occurs, the sending client's file descriptor is set. handle it accordingly.
	--------------------------------------------------------------------------------------------------------- */

	struct timeval to;
	to.tv_sec = (unsigned long) ms / 1000; // convert to second
	to.tv_usec = (ms % 1000) * 1000; // convert to usec

	// if timeout is not specified, we'll be blocking here and wait for any file descriptor ready to be read.
	fd_set ready = m_fds;
	if (to.tv_sec == 0 && to.tv_usec == 0)
	{
		int n = select(m_nMaxFD + 1, &ready, NULL, NULL, NULL);
	}
	// otherwise wait for timeout
	else
	{	
		int n = select(m_nMaxFD + 1, &ready, NULL, NULL, &to);
		if (n == 0) return true;
	}

	// check all file descriptors and find the one that is ready
	for (int i = 0; i <= m_nMaxFD; i++)
	{
		// only if this is the socket ready for reading
		if (!FD_ISSET(i, &ready)) continue;

		// message is being typed on stdin. let's get it and send to server
		if (i == fileno(stdin))
		{
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
				FD_CLR(i, &m_fds);
				std::cout << "Server stopped. Quitting..." << std::endl;
				disconnect();
				return false;
			}
			out = std::string(buf, 0, nBytesReceived);
		}
	}
	return true;
}

bool CClient::send(const std::string& in)
{
	::send(m_fdSocket, in.c_str(), in.length(), 0);
	return true;
}

void CClient::disconnect()
{
	close(m_fdSocket);
	FD_ZERO(&m_fds);
}


CServer::CServer()
{

}

CServer::~CServer()
{

}


