#include <stdlib.h>     /* atoi */
#include <string>
#include <iostream>

#include "Client.h"

#include <Windows.h>
#include <process.h>

static constexpr bool SINGLE_PLAYER = false;

static const std::string DEFAULT_HOST = "127.0.0.1";
static const int DEFAULT_PORT = 5555;
static const std::string DEFAULT_PSEUDO = "Towelie";

struct ClientData
{
	std::string host = DEFAULT_HOST;
	int port = DEFAULT_PORT;
	std::string pseudo = DEFAULT_PSEUDO;
};

void createClient(void* data)
{
	// Create client
	ClientData *clientData = (ClientData*) data;
	Client client(clientData->host, clientData->port, clientData->pseudo);
	client.connect();
	client.start();
}

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

	// Create client data
	ClientData clientData = { host, port, pseudo };
	ClientData clientData2 = { host, port, pseudo + "2" };
	
	// Create client(s)
	if (SINGLE_PLAYER)
	{
		createClient(&clientData);
	}
	else
	{
		HANDLE myhandleA, myhandleB;
		myhandleA = (HANDLE)_beginthread(&createClient, 0, &clientData);
		myhandleB = (HANDLE)_beginthread(&createClient, 0, &clientData2);
		WaitForSingleObject(myhandleA, INFINITE);
		WaitForSingleObject(myhandleB, INFINITE);
	}

	return 0;
}
