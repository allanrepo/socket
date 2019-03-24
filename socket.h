#ifndef __SOCKET__
#define __SOCKET__

#include <iostream>
#include <unistd.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <string.h>
#include <sstream>
#include <stdio.h>

#define IP "127.0.0.1"
#define PORT 54000 // default listenting port used if not specified in command line 
#define MAXBYTEMESSAGE 4096

class CClient
{
private:
	int m_fdSocket;
	fd_set m_fds;
	int m_nMaxFD;
public:
	CClient();
	virtual ~CClient();

	bool connect(const std::string& ip = "127.0.0.1" , unsigned short port = PORT );
	bool start(unsigned long ms = 0);
	bool read(std::string& out, unsigned long ms = 0);
	bool send(const std::string& in);
	void disconnect();
};


class CServer
{
private:
public:
	CServer();
	virtual ~CServer();
};

#endif
