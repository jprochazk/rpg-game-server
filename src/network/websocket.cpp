//
// Copyright (c) 2016-2019 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/vinniefalco/CppCon2018
//

#include "websocket.h"
#include <iostream>

namespace network {

websocket::websocket(
    tcp::socket&& socket,
    socket_manager* socket_manager)
    : ws_(std::move(socket))
    , smgr_(socket_manager)
{
}

websocket::~websocket()
{
    // Remove this session from the list of active sessions
    smgr_->socket_leave(this);
}

void websocket::run()
{
    // Set suggested timeout settings for the websocket
    ws_.set_option(
        beast::websocket::stream_base::timeout::suggested(
            beast::role_type::server));

    // Set a decorator to change the Server of the handshake
    ws_.set_option(beast::websocket::stream_base::decorator(
        [](beast::websocket::response_type& res)
    {
        res.set(http::field::server,
            std::string(BOOST_BEAST_VERSION_STRING) +
            " websocket-server");
    }));

    // Accept the websocket handshake
    ws_.async_accept(
        beast::bind_front_handler(
            &websocket::on_accept,
            shared_from_this()));
}

void websocket::fail(beast::error_code ec, char const* what)
{
    // Don't report these
    if (ec == net::error::operation_aborted ||
        ec == beast::websocket::error::closed)
        return;

    spdlog::error("{}: {}", what, ec.message());
}

void websocket::on_accept(beast::error_code ec)
{
    // Handle the error, if any
    if (ec)
        return fail(ec, "accept");

    // Add this session to the list of active sessions
    smgr_->socket_join(this);

    // Read a message
    ws_.async_read(
        buffer_,
        beast::bind_front_handler(
            &websocket::on_read,
            shared_from_this()));
}

void websocket::on_read(beast::error_code ec, std::size_t)
{
    // Handle the error, if any
    if (ec)
        return fail(ec, "read");

    // Clear the buffer
    buffer_.consume(buffer_.size());

    // Read another message
    ws_.async_read(
        buffer_,
        beast::bind_front_handler(
            &websocket::on_read,
            shared_from_this()));
}

void websocket::send(std::shared_ptr<std::string const> const& ss)
{
    // Post our work to the strand, this ensures
    // that the members of `this` will not be
    // accessed concurrently.

    net::post(
        ws_.get_executor(),
        beast::bind_front_handler(
            &websocket::on_send,
            shared_from_this(),
            ss));
}

void websocket::on_send(std::shared_ptr<std::string const> const& ss)
{
    // Always add to queue
    write_queue_.push_back(ss);

    // Are we already writing?
    if (is_async_writing_)
        return;

    is_async_writing_ = true;

    // We are not currently writing, so send this immediately
    ws_.async_write(
        net::buffer(*write_queue_.front()),
        beast::bind_front_handler(
            &websocket::on_write,
            shared_from_this()));
}

void websocket::on_write(beast::error_code ec, std::size_t)
{
    // Handle the error, if any
    if (ec)
        return fail(ec, "write");

    // Remove the string from the queue
    write_queue_.erase(write_queue_.begin());

    // Send the next message if any
    if (write_queue_.empty()) {
        is_async_writing_ = false;
        return;
    }

    ws_.async_write(
        net::buffer(*write_queue_.front()),
        beast::bind_front_handler(
            &websocket::on_write,
            shared_from_this()));
}
    
}; // namespace network
