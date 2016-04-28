#include <SDL2/SDL.h>
#include <SDL2/SDL_net.h>
#include <iostream>
#include <string>

using namespace std;

const unsigned short PORT = 1234; // The port we are connecting to
const unsigned short BUFFER_SIZE = 512;  // Size of our message buffer (i.e. maximum length of characters in a message)


int client(string hostname) {
	const char *host;         // Where we store the host name

	IPaddress serverIP;       // The IP we will connect to
	TCPsocket clientSocket;   // The socket to use
	string    serverName = hostname;     // The server name

	string userInput = "";    // A string to hold our user input
	int inputLength = 0;     // The length of our string in characters
	char buffer[BUFFER_SIZE]; // Array of character's we'll use to transmit our message. We get input into the userInput string for ease of use, then just copy it to this character array and send it.

	// Initialise SDL_net
	if (SDLNet_Init() < 0)
	{
		cout << "Failed to intialise SDN_net: " << SDLNet_GetError() << "\n";
		exit(-1); // Quit!
	}


	// Create the socket set with enough space to store our desired number of connections (i.e. sockets)
	SDLNet_SocketSet socketSet = SDLNet_AllocSocketSet(1);
	if (socketSet == NULL)
	{
		cout << "Failed to allocate the socket set: " << SDLNet_GetError() << "\n";
		exit(-1); // Quit!
	}
	else
	{
		cout << "Successfully allocated socket set." << endl;
	}

	// Try to resolve the host. If successful, this places the connection details in the serverIP object
	int hostResolved = SDLNet_ResolveHost(&serverIP, serverName.c_str(), PORT);

	if (hostResolved == -1)
	{
		cout << "Failed to resolve the server hostname: " << SDLNet_GetError() << "\nContinuing...\n";
	}
	else // If we successfully resolved the host then output the details
	{
		// Get our IP address in proper dot-quad format by breaking up the 32-bit unsigned host address and splitting it into an array of four 8-bit unsigned numbers...
		Uint8 * dotQuad = (Uint8*)&serverIP.host;

		//... and then outputting them cast to integers. Then read the last 16 bits of the serverIP object to get the port number
		cout << "Successfully resolved host to IP: " << (unsigned short)dotQuad[0] << "." << (unsigned short)dotQuad[1] << "." << (unsigned short)dotQuad[2] << "." << (unsigned short)dotQuad[3];
		cout << " port " << SDLNet_Read16(&serverIP.port) << endl << endl;
	}


	// Try to resolve the IP of the server, just for kicks
	if ((host = SDLNet_ResolveIP(&serverIP)) == NULL)
	{
		cout << "Failed to resolve the server IP address: " << SDLNet_GetError() << endl;
	}
	else
	{
		cout << "Successfully resolved IP to host: " << host << endl;
	}


	// Flag to keep track of when to disconnect and finish up. We initially set it so that we CANNOT connect, and only change this to false when we got an "OK" response from the server
	bool shutdownClient = true;


	// Try to open a connection to the server and quit out if we can't connect
	clientSocket = SDLNet_TCP_Open(&serverIP);
	if (!clientSocket)
	{
		cout << "Failed to open socket to server: " << SDLNet_GetError() << "\n";
		return -1;
	}
	else // If we successfully opened a connection then check for the server response to our connection
	{
		cout << "Connection okay, about to read connection status from the server..." << endl;

		// Add our socket to the socket set for polling
		SDLNet_TCP_AddSocket(socketSet, clientSocket);

		// Wait for up to five seconds for a response from the server
		// Note: If we don't check the socket set and WAIT for the response, we'll be checking before the server can respond, and it'll look as if the server sent us nothing back
		int activeSockets = SDLNet_CheckSockets(socketSet, 5000);

		cout << "There are " << activeSockets << " socket(s) with data on them at the moment." << endl;

		// Check if we got a response from the server
		int gotServerResponse = SDLNet_SocketReady(clientSocket);

		if (gotServerResponse != 0)
		{
			cout << "Got a response from the server... " << endl;
			int serverResponseByteCount = SDLNet_TCP_Recv(clientSocket, buffer, BUFFER_SIZE);

			cout << "Got the following from server: " << buffer << "(" << serverResponseByteCount << " bytes)" << endl;

			// We got an okay from the server, so we can join!
			if (strcmp(buffer, "OK") == 0)
			{
				// So set the flag to say we're not quitting out just yet
				shutdownClient = false;

				cout << "Joining server now..." << endl << endl;
			}
			else
			{
				cout << "Server is full... Terminating connection." << endl;
			}
		}
		else
		{
			cout << "No response from server..." << endl;
		}

	} // End of if we managed to open a connection to the server condition


	// Copy our user's string into our char array called "buffer"
	cout << ">";
	cin >> buffer;
	strcpy(buffer, userInput.c_str());
	cout << "\n";

	bool sendMessage = false; // Whether or not it's time to send the message (flips to true when the user presses return)

	// Send the message to the server
	if (SDLNet_TCP_Send(clientSocket, (void *)buffer, inputLength) < inputLength)
	{
		cout << "Failed to send message: " << SDLNet_GetError() << endl;
		return -1;
	}
	else
	{
		//cout << "Message sent successfully." << endl;

		// If we've asked the server to shutdown or we want out then set the flag appropriately
		if (sendMessage == true && (userInput == "quit" || userInput == "exit" || userInput == "shutdown"))
		{
			shutdownClient = true;
		}

		// Reset for the next message
		cout << endl;
		//wrotePrompt = false;
		sendMessage = false;
		userInput = "";
	}
	
	// Check our socket set for activity. Don't wait if there's nothing on the socket just continue
	int socketActive = SDLNet_CheckSockets(socketSet, 0);

	if (socketActive != 0)
	{
		// Check if we got a response from the server
		int messageFromServer = SDLNet_SocketReady(clientSocket);

		if (messageFromServer != 0)
		{
			//cout << "Got a response from the server... " << endl;
			int serverResponseByteCount = SDLNet_TCP_Recv(clientSocket, buffer, BUFFER_SIZE);

			cout << "Received: " << buffer << endl;// "(" << serverResponseByteCount << " bytes)" << endl;

			if (strcmp(buffer, "shutdown") == 0)
			{
				cout << "Server is going down. Disconnecting..." << endl;
				shutdownClient = true;
			}
		}
		else
		{
			//cout << "No response from server..." << endl;
		}

	} // End of if socket has activity check

	// Close our socket, cleanup SDL_net, reset the terminal mode and finish!
	SDLNet_TCP_Close(clientSocket);

	SDLNet_Quit();

	return 0;
}