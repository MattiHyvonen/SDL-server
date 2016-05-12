#include "server.h"
#include "client.h"
#include <iostream>

int main(int argc, char* argv[]) {

	std::string hostname;

	if (argc < 2)
		hostname = "localhost";
	else
		hostname = argv[1];

	paketinKuuntelija server;

 server.setup();
	server.kuuntele();
 //server.testi();

	server.tuhoa();
	return 0;
}
