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
#include <boost/smart_ptr.hpp>
#include <memory>
#include <string>

// Forward declaration
class socket_manager;

// Accepts incoming connections and launches the sessions
class socket_listener : public boost::enable_shared_from_this<socket_listener>
{
    net::io_context& ioc_;
    tcp::acceptor acceptor_;
    boost::shared_ptr<socket_manager> socket_manager_;

    void fail(beast::error_code ec, char const* what);
    void on_accept(beast::error_code ec, tcp::socket socket);

public:
    socket_listener(
        net::io_context& ioc,
        tcp::endpoint endpoint,
        boost::shared_ptr<socket_manager> const& socket_manager);

    // Start accepting incoming connections
    void run();
}; // class socket_listener

#endif // SERVER_NETWORK_SOCKET_LISTENER_H
