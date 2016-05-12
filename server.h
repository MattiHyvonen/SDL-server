#include <SDL2/SDL.h>
#include <SDL2/SDL_net.h>

#include <vector>

const int MAX_PAKETTEJA = 1000;

const unsigned short PORT = 1234;            // The port our server will listen for incoming connecions on
const unsigned short BUFFER_SIZE = 1400;             // Size of our message buffer
const unsigned short MAX_SOCKETS = 100;               // Max number of sockets
const unsigned short MAX_CLIENTS = MAX_SOCKETS - 1; // Max number of clients in our socket set (-1 because server's listening socket takes the 1st socket in the set)

void server();

struct vec2{
  float x;
  float y;
};

struct pakettiDataT {
  std::vector<vec2> pisteet;
};

class paketinKuuntelija{

	IPaddress serverIP;                  // The IP of the server (this will end up being 0.0.0.0 - which means roughly "any IP address")
	TCPsocket serverSocket;              // The server socket that clients will use to connect to us
	TCPsocket clientSocket[MAX_CLIENTS]; // An array of sockets for the clients, we don't include the server socket (it's specified separately in the line above)
	bool      socketIsFree[MAX_CLIENTS]; // An array of flags to keep track of which client sockets are free (so we know whether we can use the socket for a new client connection or not)
	char buffer[BUFFER_SIZE];            // Array of characters used to store the messages we receive
	int receivedByteCount = 0;           // A variable to keep track of how many bytes (i.e. characters) we need to read for any given incoming message i.e. the size of the incoming data
 int clientCount = 0;                 // Count of how many clients are currently connected to the server
	bool shutdownServer = false;         // Flag to control when to shut down the server
	SDLNet_SocketSet socketSet;

 std::vector<pakettiDataT> paketit;

 public:

 paketinKuuntelija();

 pakettiDataT haePaketti();
 void kuuntele();
 int setup();
 void tuhoa();
 void testi();
};
