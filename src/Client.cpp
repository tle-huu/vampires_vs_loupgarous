#include "Client.h"

#include <cstring>          /* std::memset, std::memcpy */
#include <iostream>
#include <algorithm>
#include <chrono>

#include "Action.h"
#include "minimax.h"

int Client::connect() noexcept
{
    WSADATA WSAData;

    // Initialize Winsock
    int result = WSAStartup(MAKEWORD(2, 2), &WSAData);
    if (result != 0)
    {
        std::cerr << "WSAStartup failed with error: " << result << std::endl;
        return EXIT_FAILURE;
    }

    if ((socket_ = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        std::cerr << "Error trying to create socket" << std::endl;
        return EXIT_FAILURE;
    }

    SOCKADDR_IN sin;
    sin.sin_addr.s_addr = inet_addr(host_.data());
    sin.sin_family = AF_INET;
    sin.sin_port = htons(port_);

    if (::connect(socket_, (SOCKADDR *)&sin, sizeof(sin)) < 0)
    {
        std::cerr << "Connection Failed" << std::endl;
        return EXIT_FAILURE;
    }
    connected_ = true;
    std::cout << "Connected" << std::endl;

    return EXIT_SUCCESS;
}

int Client::start() noexcept
{
    // Check if the client is connected
	if (!connected_)
	{
		return EXIT_FAILURE;
	}
	
	// Send player's name to the server
    send_name();

    // Receive first two messages to initialize the game map
    handle_data();
    handle_data();

	// Create timers
	std::chrono::high_resolution_clock::time_point t0;
	std::chrono::high_resolution_clock::time_point t1;

    while (connected_)
    {
        // Receive data from server to update map
        handle_data();

        if (!connected_)
        {
            break;
        }

        // Send player's move to server
		t0 = std::chrono::high_resolution_clock::now();
        send_move();
		t1 = std::chrono::high_resolution_clock::now();
		double duration = std::chrono::duration<double>(t1 - t0).count();
		std::cout << "Process time: " << duration << "s" << std::endl << std::endl;
    }

    return EXIT_SUCCESS;
}

int Client::send_name() noexcept
{
	std::memset(buffer_, 0, BUFFER_MAX_SIZE);

    std::memcpy(buffer_, "NME", 3);

	std::cout << "NME" << std::endl;

    const char size = static_cast<char>(name_.size());
    std::memcpy(buffer_ + 3, &size, 1);
    std::memcpy(buffer_ + 4, name_.data(), name_.size());

	std::cout << "name size: " << name_.size() << std::endl;
	std::cout << "name: " << name_ << std::endl << std::endl;

    return send(buffer_, 3 + 1 + name_.size());
}

int Client::send_move() noexcept
{
	std::memset(buffer_, 0, BUFFER_MAX_SIZE);

	Map *map = map_->clone();
	Action action = get_best_action(map, MAX_DEPTH);
    std::vector<Move> moves = action.moves();

	std::memcpy(buffer_, "MOV", 3);

	std::cout << "MOV" << std::endl;

    const char size = static_cast<char>(moves.size());
    std::memcpy(buffer_ + 3, &size, 1);

    int index = 4;
    for (Move const& move : moves)
    {
        char start_x = static_cast<char>(move.start().x());
        std::memcpy(buffer_ + index++, &start_x, 1);

        char start_y = static_cast<char>(move.start().y());
        std::memcpy(buffer_ + index++, &start_y, 1);

        char n = static_cast<char>(move.number());
        std::memcpy(buffer_ + index++, &n, 1);

        char end_x = static_cast<char>(move.end().x());
        std::memcpy(buffer_ + index++, &end_x, 1);

        char end_y = static_cast<char>(move.end().y());
        std::memcpy(buffer_ + index++, &end_y, 1);
    }

	std::cout << action << std::endl << std::endl;

    return send(buffer_, 3 + 1 + 5 * moves.size());
}

int Client::send(char const *data, const int size) const noexcept
{
    ::send(socket_, data, size, 0);

    return EXIT_SUCCESS;
}

int Client::handle_data() noexcept
{
    std::memset(buffer_, 0, BUFFER_MAX_SIZE);

    int n_bytes = ::recv(socket_, buffer_, BUFFER_MAX_SIZE, 0);
    int bytes_read = 0;

    while(bytes_read < n_bytes)
    {
        const std::string command((char*) (buffer_ + bytes_read), 3);
        bytes_read += 3;

        std::cout << command << std::endl;

        bytes_read = HANDLERS.at(command)(bytes_read);

        std::cout << std::endl;
    }

    return EXIT_SUCCESS;
}

int Client::close() noexcept
{
    connected_ = false;

    ::closesocket(socket_);

    WSACleanup();

    return EXIT_SUCCESS;
}

int Client::handle_set(int bytes_read)
{
    // Get map dimensions
    lines_ = buffer_[bytes_read++];
    columns_ = buffer_[bytes_read++];

    std::cout << "lines: " << lines_ << ", columns: " << columns_ << std::endl;

    // Initialize map
    switch (METHOD)
    {
        // MapVectors
        case 0:
            map_ = new MapVectors(lines_, columns_);
            break;

        // MapGrid
        case 1:
            map_ = new MapGrid(lines_, columns_);
            break;

        default:
            break;
    }

    return bytes_read;
}

int Client::handle_hum(int bytes_read)
{
    // Get home number
    int n = buffer_[bytes_read++];

    std::cout << "houses: " << n << std::endl;

    while (n > 0)
    {
        // Get home position
        const int16_t x = buffer_[bytes_read++];
        const int16_t y = buffer_[bytes_read++];

        std::cout << "(" << x << "," << y << ")" << std::endl;

        // Update map
        //map_->add_human(x, y, 0);

        --n;
    }

    return bytes_read;
}

int Client::handle_hme(int bytes_read)
{
    // Get starting position
    const int16_t x = buffer_[bytes_read++];
    const int16_t y = buffer_[bytes_read++];

	std::cout << "starting point: (" << x << "," << y << ")" << std::endl;

    starting_point_ = Point(x, y);

    return bytes_read;
}

int Client::handle_map(int bytes_read)
{
    // Get number of updates
    int n = buffer_[bytes_read++];

    std::cout << "updates: " << n << std::endl;

    const int initial_bytes_read = bytes_read;
    const int initial_n = n;

    while (n > 0)
    {
        // Get update data
        const int16_t x = buffer_[bytes_read++];
        const int16_t y = buffer_[bytes_read++];
        bytes_read++;
        const int16_t v = buffer_[bytes_read++];
        bytes_read++;

        // Get gentil species
        if (v > 0 && Point(x, y) == starting_point_)
        {
            first_ = true;
			break;
        }

        --n;
    }

    n = initial_n;
    bytes_read = initial_bytes_read;

	MAX_UTILITY = 0;

    while (n > 0)
    {
        // Get update data
        const int16_t x = buffer_[bytes_read++];
        const int16_t y = buffer_[bytes_read++];
        const int16_t h = buffer_[bytes_read++];
        const int16_t v = buffer_[bytes_read++];
        const int16_t w = buffer_[bytes_read++];

        // Update map
        if (h > 0)
        {
            map_->add_human(x, y, h);

			std::cout << "H" << h;
        }
        else if (v > 0)
        {
            first_ ? map_->add_gentil(x, y, v) : map_->add_vilain(x, y, v);

			first_ ? std::cout << "G" << v : std::cout << "V" << v;
        }
        else if (w > 0)
        {
            first_ ? map_->add_vilain(x, y, w) : map_->add_gentil(x, y, w);

			first_ ? std::cout << "V" << w : std::cout << "G" << w;
        }

		MAX_UTILITY += h + (first_ ? v : w);

		std::cout << "@(" << x << "," << y << ")" << std::endl;

        --n;
    }

	MAX_UTILITY += 1;

	std::cout << std::endl << map_ << std::endl;

    return bytes_read;
}

int Client::handle_upd(int bytes_read)
{
    // Get number of updates
    int n = buffer_[bytes_read++];

	std::cout << "updates: " << n << std::endl;

    while (n > 0)
    {
        // Get update data
        const int16_t x = buffer_[bytes_read++];
        const int16_t y = buffer_[bytes_read++];
        const int16_t h = buffer_[bytes_read++];
        const int16_t v = buffer_[bytes_read++];
        const int16_t w = buffer_[bytes_read++];

        // Update map
		if (h > 0)
		{
			map_->add_human(x, y, h);

			std::cout << "H" << h;
		}
		else if (v > 0)
		{
			first_ ? map_->add_gentil(x, y, v) : map_->add_vilain(x, y, v);

			first_ ? std::cout << "G" << v : std::cout << "V" << v;
		}
		else if (w > 0)
		{
			first_ ? map_->add_vilain(x, y, w) : map_->add_gentil(x, y, w);

			first_ ? std::cout << "V" << w : std::cout << "G" << w;
		}
		else
		{
			map_->remove_group(x, y);

			std::cout << "0";
		}

		std::cout << "@(" << x << "," << y << ")" << std::endl;

        --n;
    }

	std::cout << std::endl << map_ << std::endl;

    return bytes_read;
}

int Client::handle_end(int bytes_read)
{
	first_ = false;
    close();
    return bytes_read;
}

int Client::handle_bye(int bytes_read)
{
    close();
    return bytes_read;
}
