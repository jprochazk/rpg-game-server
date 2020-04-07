//
// Copyright (c) 2016-2019 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/vinniefalco/CppCon2018
//

#ifndef SERVER_NETWORK_WEBSOCKET_H
#define SERVER_NETWORK_WEBSOCKET_H

#include "net.h"
#include "beast.h"

#include <spdlog/spdlog.h>

#include <cstdlib>
#include <memory>
#include <string>
#include <vector>

namespace network {
class socket_manager;

/** Represents an active WebSocket connection to the server
*/
class websocket : public std::enable_shared_from_this<websocket>
{
    beast::flat_buffer buffer_;
    beast::websocket::stream<beast::tcp_stream> ws_;
    bool is_async_writing_;
    std::vector<std::shared_ptr<std::string const>> write_queue_;
    socket_manager* smgr_;

    void fail(beast::error_code ec, char const* what);
    void on_accept(beast::error_code ec);
    void on_read(beast::error_code ec, std::size_t bytes_transferred);
    void on_write(beast::error_code ec, std::size_t bytes_transferred);

public:
    websocket(
        tcp::socket&& socket,
        socket_manager* socket_manager);

    ~websocket();

    void run();

    // Send a message
    void send(std::shared_ptr<std::string const> const& ss);

private:
    void on_send(std::shared_ptr<std::string const> const& ss);
}; // class websocket

class socket_manager {
public:
    virtual void socket_join(websocket* session) = 0;
    virtual void socket_leave(websocket* session) = 0;

}; // class socket_manager
    
}; // namespace network

#endif // SERVER_NETWORK_WEBSOCKET_H
