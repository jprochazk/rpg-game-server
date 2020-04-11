//
// Copyright (c) 2016-2019 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/vinniefalco/CppCon2018
//

#pragma once
#ifndef SERVER_NETWORK_SOCKET_LISTENER_H
#define SERVER_NETWORK_SOCKET_LISTENER_H

#include "common/SharedDefines.h"

#include "network/net.h"
#include "network/Websocket.h"

#include <spdlog/spdlog.h>

// Accepts incoming connections and launches the sessions
class SocketListener : public std::enable_shared_from_this<SocketListener>
{
public: // public interface
    SocketListener(
        net::io_context& ioc,
        tcp::endpoint endpoint,
        SocketManager* socket_manager);
    ~SocketListener();

    void Run();
private: // internal
    void Fail(beast::error_code ec, char const* what);
    void OnAccept(beast::error_code ec, tcp::socket socket);
private: // members
    net::io_context& ioc_;
    tcp::acceptor acceptor_;
    SocketManager* socketManager_;
}; // class SocketListener

#endif // SERVER_NETWORK_SOCKET_LISTENER_H
