#pragma once

#include <functional>
#include <string>
#include <unordered_map>
#include <vector>

static constexpr int BUFFER_MAX_SIZE = 10;

class Client
{

public:

    Client(int const port
        , std::string const & host_ip
        , std::string && player_name) : port_(port), host_ip_(host_ip), name_(player_name)
    {
    }

    int connect() noexcept;

    int send(uint8_t const * data, int size) const noexcept;

    int send_name() const noexcept;

    int start() noexcept
    {
        std::memset(buffer_, 0, BUFFER_MAX_SIZE);

        while (connected_)
        {
            handle_data();
        }

        return EXIT_SUCCESS;
    }
private:

    int handle_data() noexcept;

    std::string get_op_code() noexcept;

    uint8_t get_payload_length(std::string const & op_code) noexcept;
    int refill_buffer();

    int handle_set(int payload_length);
    int handle_hum(int payload_length);
    int handle_hme(int payload_length);
    int handle_upd(int payload_length);
    int handle_map(int payload_length);
    int handle_end(int payload_length);
    int handle_bye(int payload_length);

    inline int close() noexcept
    {
        connected_ = false;
        return EXIT_SUCCESS;
    }

private:

    using HANDLER_TYPE = std::function<int(int)>;

    const std::unordered_map<std::string, HANDLER_TYPE> HANDLERS = {
            {"SET", [this](int p) -> int {return handle_set(p);}},
            {"HUM", [this](int p) -> int {return handle_hum(p);}},
            {"HME", [this](int p) -> int {return handle_hme(p);}},
            {"UPD", [this](int p) -> int {return handle_upd(p);}},
            {"MAP", [this](int p) -> int {return handle_map(p);}},
            {"END", [this](int p) -> int {return handle_end(p);}},
            {"BYE", [this](int p) -> int {return handle_bye(p);}},
    };


    std::string const host_ip_;
    int         const port_;
    std::string const name_;

    int connected_ = false;
    int socket_;

    uint8_t buffer_[BUFFER_MAX_SIZE];
    int buffer_size_ = 0;
    int index_ = 0;

    std::vector<std::vector<uint8_t>> map_;

    bool map_initialized_ = false;


};