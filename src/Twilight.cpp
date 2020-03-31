#include <stdlib.h>     /* atoi */
#include <string>
#include <iostream>

#include "Client.h"

static const std::string DEFAULT_HOST = "127.0.0.1";
static const int DEFAULT_PORT = 5555;
static const std::string DEFAULT_PSEUDO = "Towelie";

int main(int argc, char const *argv[])
{
	// Get parameters
	if (argc > 4)
	{
		std::cerr << "usage: ./bin/main <host = 127.0.0.1> <port = 5555> <pseudo = Towelie>" << std::endl;
		return 1;
	}

	std::string host = DEFAULT_HOST;
	int port = DEFAULT_PORT;
	std::string pseudo = DEFAULT_PSEUDO;

	if (argc > 1)
	{
		host = argv[1];
		if (argc > 2)
		{
			port = atoi(argv[2]);
			if (argc > 3)
			{
				pseudo = argv[3];
			}
		}
	}

	// Create client
	Client client(host, port, pseudo);
	client.connect();
	client.start();

	return 0;
}
