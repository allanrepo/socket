design philosophy
- 	to develop a client/host software that sends and receive text data with each other (aka IRC chat)
-	multiple clients can connect to host, connection limit TBD
-	application uses GUI as interface
-	both client and host displays general text exchange list with time stamp and user 
-	both client and host has a single line text box where it can enter text message it wants to send to specific
	user or broadcast to everyone
-	option for the host to kick or block a user from connecting
-	text message list has size limit. older messages are removed to be replaced by new ones. size is TBD. 
-	text message has a size limit. size is TBD.

interface
-	uses wxWidget as API for GUI
-	client
	-	multi-line text box that displays list of text messages from all users
	-	single-line text box that accepts entries to be broadcast 
	-	list box that contains list of users connected to the host
	-	text box that accepts entry to set IP address and port of the host to connect.
		- 	is disabled when connected
	-	button to connect to host
		-	turn into 'disconnect' if already connected.
-	host
	-	multi-line text box that displays list of text messages from all users
	-	single-line text box that accepts entries to be broadcast or send to user
	-	list box that contains list of users connected to the host
	-	read-only text box that displays the IP address and port
	- 	button to kick a user selected in the list
	-	button to kick and ban user selected in the list


1st version
-	config file
	- xml format
	- categorized in config setups and an active config
-	no GUI, runs on terminal
-	host
	-	IP and port are set in xml config file
	-	when running, can enter text to broadcast and be received by client/s
	-	multiple clients can connect
	-	any text message sent by client is displayed in terminal
		-	they are also broadcast into all other clients
-	client
	-	connects to host with IP and port set in xml config file
	-	when running, can enter text to broadcast and be received by host and other client/s

2nd version
-	host
	-	prints IP of user that connects
	-	can execute command that lists connected users' IP
	-	can execute command that shows how many users are connected
-	client
	-	

3rd version
-	GUI
	-	implement GUI for client and host
	-	text box for IP and port for both client/host
	-	list box for messages sent by host and all clients
	-	text box for sending messages
	-	connect button for client
	-	kick/ban button for host
-	client
	-	can execute command that pings host and returns latency

	
	
	

	

	



