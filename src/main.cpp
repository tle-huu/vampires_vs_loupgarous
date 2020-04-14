#include <stdlib.h>     /* atoi */
#include <string>
#include <iostream>

#include "Client.h"

#include <Windows.h>
#include <process.h>

// False to launch two clients
static constexpr bool SINGLE_PLAYER = true;

// Client default parameters
static const std::string DEFAULT_HOST = "127.0.0.1";
static const int DEFAULT_PORT = 5555;
static const std::string DEFAULT_PSEUDO = "Towelie";

// Maximum depth of the game tree to explore
static constexpr int16_t DEFAULT_MAX_DEPTH = 4;

// Method to generate actions
static constexpr int DEFAULT_ACTION_METHOD = 4;

// Maximum number of friendly groups
static constexpr int16_t DEFAULT_MAX_GROUPS = 3;

struct ClientData
{
	std::string host = DEFAULT_HOST;
	int port = DEFAULT_PORT;
	std::string pseudo = DEFAULT_PSEUDO;
	int16_t max_depth = DEFAULT_MAX_DEPTH;
	int action_method = DEFAULT_ACTION_METHOD;
	int16_t max_groups = DEFAULT_MAX_GROUPS;
};

void create_client(void* data)
{
	// Create client
	ClientData *client_data = (ClientData*) data;
	Client client(client_data->host, client_data->port, client_data->pseudo, client_data->max_depth, client_data->action_method, client_data->max_groups);
	client.connect();
	client.send_name();
	client.start();
}

int main(int argc, char const *argv[])
{
	// Get parameters
	if (argc > 7)
	{
		std::cerr << "usage: .\Twilight <host = " << DEFAULT_HOST 
			<< "> <port = " << DEFAULT_PORT 
			<< "> <pseudo = " << DEFAULT_PSEUDO 
			<< "> <max_depth = " << DEFAULT_MAX_DEPTH 
			<< "> <action_method = " << DEFAULT_ACTION_METHOD 
			<< "> <max_groups = " << DEFAULT_MAX_GROUPS
			<< ">" << std::endl;
		return 1;
	}

	std::string host = DEFAULT_HOST;
	int port = DEFAULT_PORT;
	std::string pseudo = DEFAULT_PSEUDO;
	int16_t max_depth = DEFAULT_MAX_DEPTH;
	int action_method = DEFAULT_ACTION_METHOD;
	int16_t max_groups = DEFAULT_MAX_GROUPS;

	if (argc > 1)
	{
		host = argv[1];
		if (argc > 2)
		{
			port = atoi(argv[2]);
			if (argc > 3)
			{
				pseudo = argv[3];
				if (argc > 4)
				{
					max_depth = atoi(argv[4]);
					if (argc > 5)
					{
						action_method = atoi(argv[5]);
						if (argc > 6)
						{
							max_groups = atoi(argv[6]);
						}
					}
				}
			}
		}
	}

	// Create client data
	ClientData client_data = { host, port, pseudo, max_depth, action_method, max_groups };
	ClientData client_data2 = { host, port, pseudo + "2", max_depth, action_method, max_groups };
	
	// Create client(s)
	if (SINGLE_PLAYER)
	{
		create_client(&client_data);
	}
	else
	{
		HANDLE myhandleA, myhandleB;
		myhandleA = (HANDLE)_beginthread(&create_client, 0, &client_data);
		Sleep(500);
		myhandleB = (HANDLE)_beginthread(&create_client, 0, &client_data2);
		WaitForSingleObject(myhandleA, INFINITE);
		WaitForSingleObject(myhandleB, INFINITE);
	}

	return 0;
}
