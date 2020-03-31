#include <stdlib.h>     /* atoi */
#include <string>
#include <iostream>

#include "../include/Client.h"

int main(int argc, char const *argv[])
{
    // Get parameters
    if (argc > 4)
    {
        std::cout << "usage: ./bin/main <host = localhost> <port = 5555> <pseudo = Towelie>" << std::endl;
        return 1;
    }

    std::string host = "localhost";
    int port = 5555;
    std::string pseudo = "Towelie";

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
