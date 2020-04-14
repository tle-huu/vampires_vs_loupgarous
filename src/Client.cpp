#include "Client.h"

#include <cstring>          /* std::memset, std::memcpy */
#include <iostream>
#include <algorithm>

#include "Action.h"
#include "minimax.h"

int Client::connect()
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

int Client::send_name()
{
// Check if the client is connected
if (!connected_)
{
	return EXIT_FAILURE;
}

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

int Client::start()
{
	// Check if the client is connected to the game server
	if (!connected_)
	{
		return EXIT_FAILURE;
	}

	while (connected_)
	{
		// Reset attributes
		running_ = true;
		turns_ = 0;
		first_ = false;
		current_max_vilain_groups_ = 1;

		// Receive first two messages to initialize the map
		handle_data();
		handle_data();

		while (running_)
		{
			// Receive data from server to update map
			handle_data();

			// Update turns counter
			++turns_;

			// Check if the game is not ended
			if (!running_)
			{
				break;
			}

			// Update max vilain groups
			set_max_vilain_groups();

			// Send player's move to server
			std::cout << "TURN " << turns_ << std::endl << std::endl;
			send_move();
			std::cout << "Process time: " << chrono_ << std::endl << std::endl;
			//Sleep(1000); // in order to see the moves for debugging
		}

		// Check to restart a game
		if (!connected_)
		{
			break;
		}
	}

	return EXIT_SUCCESS;
}

void Client::set_max_vilain_groups()
{
	// Update current max vilain groups
	current_max_vilain_groups_ = max(current_max_vilain_groups_, map_->vilains().size());

	// Downgrade max vilain groups if vilain doesn't split his first group after one turn
	if (turns_ == 1 && current_max_vilain_groups_ == 1)
	{
		max_vilain_groups_ = 1;
	}

	// Update max vilain groups according to the actual max vilain groups
	else if (turns_ >= 1)
	{
		max_vilain_groups_ = current_max_vilain_groups_;
	}
}

int16_t Client::get_max_depth() const
{
	// Optimization
	int gentils = map_->gentils().size(), vilains = map_->vilains().size();

	if (map_->humans().size() == 0)
	{
		return max_depth_ + 2;
	}

	if (gentils + vilains <= 2)
	{
		return max_depth_ + 1;
	}

	if (gentils + vilains >= 5)
	{
		return max_depth_ - 1;
	}

	if (max_depth_ == 4 && max_gentil_groups_ == 3)
	{
		if (gentils == 2 && vilains == 2)
		{
			int big_groups = 0;
			for (Gentil const& gentil : map_->gentils())
			{
				if (gentil.number() >= 4)
				{
					++big_groups;
				}
			}
			for (Vilain const& vilain : map_->vilains())
			{
				if (vilain.number() >= 4)
				{
					++big_groups;
				}
			}
			if (big_groups >= 2)
			{
				return max_depth_ - 1;
			}
		}
	}

	return max_depth_;
}

int Client::send(char const *data, int size) const
{
	::send(socket_, data, size, 0);

	return EXIT_SUCCESS;
}

int Client::send_move()
{
	std::memset(buffer_, 0, BUFFER_MAX_SIZE);

	Map *map = map_->clone();
	Action action = get_best_action(map, get_max_depth(), action_method_, max_gentil_groups_, max_vilain_groups_);
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

int Client::handle_data()
{
    std::memset(buffer_, 0, BUFFER_MAX_SIZE);

    int n_bytes = ::recv(socket_, buffer_, BUFFER_MAX_SIZE, 0);
	chrono_.reset();
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

int Client::close()
{
    connected_ = false;
	running_ = false;

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
    switch (MAP_METHOD)
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
		map_ = new MapGrid(lines_, columns_);
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

        // Get gentil species (vampires always play first)
        if (v > 0 && Point(x, y) == starting_point_)
        {
            first_ = true;
			--turns_;
			break;
        }

        --n;
    }

    n = initial_n;
    bytes_read = initial_bytes_read;

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

		std::cout << "@(" << x << "," << y << ")" << std::endl;

        --n;
    }

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
	running_ = false;
	//close();
    return bytes_read;
}

int Client::handle_bye(int bytes_read)
{
    close();
    return bytes_read;
}
