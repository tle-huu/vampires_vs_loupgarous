#include <algorithm>
#include <arpa/inet.h>
#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h> 

#include "client.hpp"

static constexpr int PORT = 5555;

int Client::connect() noexcept
{
    if ((socket_ = socket(AF_INET, SOCK_STREAM, 0)) < 0) 
    { 
        std::cerr << "Error trying to create socket\n"; 
        return EXIT_FAILURE; 
    } 

    struct sockaddr_in serv_addr; 
    serv_addr.sin_family = AF_INET; 
    serv_addr.sin_port = htons(port_); 
    serv_addr.sin_addr.s_addr = inet_addr(host_ip_.data());

    if (::connect(socket_, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) 
    { 
        std::cerr << "Connection Failed \n"; 
        return EXIT_FAILURE; 
    }
    connected_ = true;
    std::cout << "Connected\n";
    return EXIT_SUCCESS;  
}

int Client::send(uint8_t const * data, int size) const noexcept
{
    ::send(socket_, data, size, 0); 
    return 0;
}

int Client::send_name() const noexcept
{

    uint8_t nme[3 + 1 + name_.size()];

    std::memcpy(nme, "NME", 3);

    uint8_t size = static_cast<uint8_t>(name_.size());
    std::memcpy(nme + 3, &size, 1);
    std::memcpy(nme + 4, name_.data(), name_.size());

    int res = send(nme, 4 + name_.size());

    return res;

}


int Client::handle_data() noexcept
{
    int res = EXIT_SUCCESS;

    if ((buffer_size_ - index_) < 5)
    {
        res = refill_buffer();
        if (res == EXIT_FAILURE)
        {
            std::cerr << "Error trying to refill the buffer\n";
            return res;
        }
    }

    std::string op_code = get_op_code();

    int payload_length = get_payload_length(op_code);

    // handling

    printf("[%s] [%u] ", op_code.data(), payload_length);

    HANDLERS.at(op_code)(payload_length);


    return EXIT_SUCCESS;
}

int Client::refill_buffer()
{

    buffer_size_ = buffer_size_ - index_;
    std::memmove(buffer_, &buffer_[index_], buffer_size_);

    index_ = 0;
    int bytes_read = ::read(socket_, &buffer_[buffer_size_], BUFFER_MAX_SIZE - buffer_size_);

    buffer_size_ += bytes_read;

    return EXIT_SUCCESS;
}

std::string Client::get_op_code() noexcept
{
    std::string op_code((char *)(buffer_ + index_), 3);

    index_ += 3;

    return op_code;
}

uint8_t Client::get_payload_length(std::string const & op_code) noexcept
{
    uint8_t payload_length = 0;
    if (op_code == "MAP" || op_code == "HUM" || op_code == "UPD")
    {
        payload_length = buffer_[index_];
        if (op_code == "HUM") payload_length *= 2;
        if (op_code == "UPD" || op_code == "MAP") payload_length *= 5;
        ++index_;
    }

    return payload_length;
}

int Client::handle_set(int payload_length)
{
    int n = buffer_[index_++];
    int m = buffer_[index_++];

    map_.resize(n);

    for (int i = 0; i < n; ++i)
    {
        map_[i] = std::vector<uint8_t>(m);
    }

    for (auto x : map_)
    {
        for ( auto y : x)
        {
            printf("%u ", y);
        }
        std::cout << '\n';
    }
    std::cout << "Mapset  inited \n";
    return EXIT_SUCCESS;

}

int Client::handle_hum(int payload_length)
{
    while (payload_length > 0)
    {
        if (buffer_size_ - index_ < 2)
        {
            refill_buffer();
        }

        uint8_t x = buffer_[index_++];
        uint8_t y = buffer_[index_++];

        std::cout << "maison " << x << " " << y << '\n';

        payload_length -= 2;
    }
    return EXIT_SUCCESS;

}

int Client::handle_hme(int payload_length)
{
    if (buffer_size_ - index_ < 2)
    {
        refill_buffer();
    }

    uint8_t x = buffer_[index_++];
    uint8_t y = buffer_[index_++];

    std::cout << "depart " << x << " " << y << '\n';

    return EXIT_SUCCESS;

}

int Client::handle_upd(int payload_length)
{
    printf("UPD\n");

    while (payload_length > 0)
    {
        if (buffer_size_ - index_ < 5)
        {
            refill_buffer();
        }

        uint8_t x = buffer_[index_++];
        uint8_t y = buffer_[index_++];
        uint8_t humans = buffer_[index_++];
        uint8_t vampires = buffer_[index_++];
        uint8_t werewolves = buffer_[index_++];


        map_[y][x] = humans + vampires + werewolves;

        payload_length -= 5;
    }
    return EXIT_SUCCESS;

}

int Client::handle_map(int payload_length)
{
    handle_upd(payload_length);
    map_initialized_ = true;
    printf("MAP\n");
    for (auto x : map_)
    {
        for ( auto y : x)
        {
            printf("%u ", y);
        }
        std::cout << '\n';
    }
    return EXIT_SUCCESS;
}

int Client::handle_end(int payload_length)
{
    std::cout << "END\n";

    close();
    return EXIT_SUCCESS;
}

int Client::handle_bye(int payload_length)
{
    std::cout << "BYE\n";

    close();

    return EXIT_SUCCESS;

}


int main(int argc, char const *argv[]) 
{
    if (argc < 2)
    {
        std::cout << "usage: ./client host \n default port is 5555\n";
        return 0;
    }

    Client client(PORT, argv[1], "Foo");

    client.connect();
    client.send_name();

    client.start();

    return 0; 
} 
