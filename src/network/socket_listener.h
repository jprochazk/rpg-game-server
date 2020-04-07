//
// Copyright (c) 2016-2019 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/vinniefalco/CppCon2018
//

#ifndef SERVER_NETWORK_SOCKET_LISTENER_H
#define SERVER_NETWORK_SOCKET_LISTENER_H

#include "beast.h"
#include "net.h"
#include "websocket.h"

#include <spdlog/spdlog.h>
#include <memory>
#include <string>
#include <functional>

namespace network {

// Accepts incoming connections and launches the sessions
class socket_listener : public std::enable_shared_from_this<socket_listener>
{
public: // public interface
    socket_listener(
        net::io_context& ioc,
        tcp::endpoint endpoint,
        socket_manager* socket_manager);

    void run();
private: // internal
    void fail(beast::error_code ec, char const* what);
    void on_accept(beast::error_code ec, tcp::socket socket);
private: // members
    net::io_context& ioc_;
    tcp::acceptor acceptor_;
    socket_manager* smgr_;
}; // class socket_listener
    
}; // namespace network

#endif // SERVER_NETWORK_SOCKET_LISTENER_H
