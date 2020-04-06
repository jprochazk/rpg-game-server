//
// Copyright (c) 2016-2019 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/vinniefalco/CppCon2018
//

#ifndef SERVER_NETWORK_WEBSOCKET_SESSION_H
#define SERVER_NETWORK_WEBSOCKET_SESSION_H

#include "net.h"
#include "beast.h"
#include "socket_manager.h"

#include <cstdlib>
#include <memory>
#include <string>
#include <vector>

// Forward declaration
class socket_manager;

/** Represents an active WebSocket connection to the server
*/
class websocket_session : public boost::enable_shared_from_this<websocket_session>
{
    beast::flat_buffer buffer_;
    websocket::stream<beast::tcp_stream> ws_;
    boost::shared_ptr<socket_manager> state_;
    std::vector<boost::shared_ptr<std::string const>> queue_;

    bool is_async_writing_;

    void fail(beast::error_code ec, char const* what);
    void on_accept(beast::error_code ec);
    void on_read(beast::error_code ec, std::size_t bytes_transferred);
    void on_write(beast::error_code ec, std::size_t bytes_transferred);

public:
    websocket_session(
        tcp::socket&& socket,
        boost::shared_ptr<socket_manager> const& state);

    ~websocket_session();

    void run();

    // Send a message
    void send(boost::shared_ptr<std::string const> const& ss);

private:
    void on_send(boost::shared_ptr<std::string const> const& ss);
}; // class websocket_session

#endif // SERVER_NETWORK_WEBSOCKET_SESSION_H
