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

// Need to link with Ws2_32.lib
#pragma comment (lib, "ws2_32.lib")

// Maximum depth of the game tree to explore
static constexpr int16_t MAX_DEPTH = 2;

static constexpr int BUFFER_MAX_SIZE = 1024;

class Client
{
public:

    Client(std::string const& host, const int port, std::string const& pseudo) : host_(host), port_(port), name_(pseudo), connected_(false), first_(false) {}

	virtual ~Client() { delete map_; }

    int connect() noexcept;

    int start() noexcept;

private:

    int send_name() noexcept;

    int send_move() noexcept;

    int send(char const *data, const int size) const noexcept;

    int handle_data() noexcept;

    int close() noexcept;

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

    SOCKET socket_;

    bool connected_;

    char buffer_[BUFFER_MAX_SIZE];

    int16_t lines_;
    int16_t columns_;
    Map     *map_;

    Point starting_point_;
    bool  first_;
};

#endif // Client_h_INCLUDED
