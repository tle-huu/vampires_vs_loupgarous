#ifndef Client_h_INCLUDED
#define Client_h_INCLUDED

#include <stdint.h>         /* int16_t */
#include <functional>
#include <string>
#include <unordered_map>
#include <vector>

#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <winsock2.h>

#include "Point.h"
#include "Map.h"
#include "Chrono.h"

// Need to link with Ws2_32.lib
#pragma comment (lib, "ws2_32.lib")

// Maximum buffer size
static constexpr int BUFFER_MAX_SIZE = 1024;

// Expected maximum number of vilain groups at the start of the game
static constexpr int16_t DEFAULT_MAX_VILAIN_GROUPS = 2;

class Client
{
public:

    Client(std::string const& host, int port, std::string const& pseudo, int16_t max_depth, int action_method, int16_t max_groups)
		: host_(host), port_(port), name_(pseudo), max_depth_(max_depth), action_method_(action_method), 
		max_gentil_groups_(max_groups), max_vilain_groups_(DEFAULT_MAX_VILAIN_GROUPS), connected_(false) {}

	virtual ~Client() { delete map_; }

    int connect();

	int send_name();

    int start();
	int start_manual();


private:

    void set_max_vilain_groups();

	int16_t get_max_depth() const;

    int send(char const *data, int size) const;

	int send_move();
	int send_move_manual();

    int handle_data();

    int close();

    int handle_set(int bytes_read);
    int handle_hum(int bytes_read);
    int handle_hme(int bytes_read);
    int handle_upd(int bytes_read);
    int handle_map(int bytes_read);
    int handle_end(int bytes_read);
    int handle_bye(int bytes_read);

    using HANDLER_TYPE = std::function<int(int)>;

    const std::unordered_map<std::string, HANDLER_TYPE> HANDLERS = {
        {"SET", [this](int b) -> int { return handle_set(b); }},
        {"HUM", [this](int b) -> int { return handle_hum(b); }},
        {"HME", [this](int b) -> int { return handle_hme(b); }},
        {"UPD", [this](int b) -> int { return handle_upd(b); }},
        {"MAP", [this](int b) -> int { return handle_map(b); }},
        {"END", [this](int b) -> int { return handle_end(b); }},
        {"BYE", [this](int b) -> int { return handle_bye(b); }}
    };

    const std::string host_;
    const int         port_;
    const std::string name_;

	const int16_t max_depth_;
	const int     action_method_;
	const int16_t max_gentil_groups_;
	int16_t       max_vilain_groups_;

	Chrono chrono_;

    SOCKET socket_;

    bool connected_;
	bool running_;

    char buffer_[BUFFER_MAX_SIZE];

    int16_t lines_;
    int16_t columns_;
    Map     *map_;
	int16_t turns_; // turns played by vilain
    Point   starting_point_;
    bool    first_;
	int16_t current_max_vilain_groups_;
};

#endif // Client_h_INCLUDED
