all: demoTcpClient demoTcpServerMultiConnect socket guiClient

socket : ./socket.cpp
	g++ -c socket.cpp -I.

demoTcpClient : ./demoTcpClient.cpp
	g++ demoTcpClient.cpp -o clientSC

demoTcpServerMultiConnect : ./demoTcpServerMultiConnect.cpp
	g++ demoTcpServerMultiConnect.cpp -o serverMC

guiClient : ./guiClient.cpp
	g++ -I. `wx-config --cppflags` `wx-config --libs` guiClient.cpp socket.o -o gc

