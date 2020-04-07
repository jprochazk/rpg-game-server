//
// Copyright (c) 2016-2019 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/vinniefalco/CppCon2018
//

#include "socket_listener.h"
#include <iostream>

namespace network {

socket_listener::socket_listener(
    net::io_context& ioc,
    tcp::endpoint endpoint,
    socket_manager* socket_manager)   
    : ioc_(ioc)
    , acceptor_(ioc)
    , smgr_(socket_manager)
{
    spdlog::info("Starting socket listener");

    beast::error_code ec;

    // Open the acceptor
    acceptor_.open(endpoint.protocol(), ec);
    if (ec)
    {
        fail(ec, "open");
        return;
    }

    // Allow address reuse
    acceptor_.set_option(net::socket_base::reuse_address(true), ec);
    if (ec)
    {
        fail(ec, "set_option");
        return;
    }

    // Bind to the server address
    acceptor_.bind(endpoint, ec);
    if (ec)
    {
        fail(ec, "bind");
        return;
    }

    // Start listening for connections
    acceptor_.listen(
        net::socket_base::max_listen_connections, ec);
    if (ec)
    {
        fail(ec, "listen");
        return;
    }
}

void socket_listener::run()
{
    // The new connection gets its own strand
    acceptor_.async_accept(
        net::make_strand(ioc_),
        beast::bind_front_handler(
            &socket_listener::on_accept,
            shared_from_this()));
}

// Report a failure
void socket_listener::fail(beast::error_code ec, char const* what)
{
    // Don't report on canceled operations
    if (ec == net::error::operation_aborted)
        return;
    spdlog::error("{}: {}", what, ec.message());
}

// Handle a connection
void socket_listener::on_accept(beast::error_code ec, tcp::socket socket)
{
    if (ec) {
        return fail(ec, "accept");
    } else {
        // Launch a new session for this connection
        std::make_shared<websocket>(
            std::move(socket),
            smgr_
        )->run();
    }
    // The new connection gets its own strand
    acceptor_.async_accept(
        net::make_strand(ioc_),
        beast::bind_front_handler(
            &socket_listener::on_accept,
            shared_from_this()));
}
    
}; // namespace network
