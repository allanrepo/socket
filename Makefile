makeall: demoTcpClient demoTcpServerMultiConnect socket guiClient demoUdpServer demoUdpClient

LIB_PATH=/home/localuser/wxWidgets-3.0.4/cnt7Bld/lib

WXLIBS=-llibwx_gtk2u_xrc-3.0

socket : ./socket.cpp
	g++ -c socket.cpp -I.

demoTcpClient : ./demoTcpClient.cpp
	g++ demoTcpClient.cpp -o clientSC

demoTcpServerMultiConnect : ./demoTcpServerMultiConnect.cpp
	g++ demoTcpServerMultiConnect.cpp -o serverMC

guiClient : ./guiClient.cpp
	g++ -I. `wx-config --cppflags` `wx-config --libs` guiClient.cpp socket.o -o gc
#	g++  -L$(LIB_PATH) guiClient.cpp socket.o -o gc -I. -I/home/localuser/wxWidgets-3.0.4/include/wx


demoUdpServer : ./udpserver.cpp
	g++ udpserver.cpp -o serverUDP

demoUdpClient : ./udpclient.cpp
	g++ udpclient.cpp -o clientUDP

