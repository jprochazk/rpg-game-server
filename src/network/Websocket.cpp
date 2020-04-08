//
// Copyright (c) 2016-2019 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/vinniefalco/CppCon2018
//

#include "Websocket.h"
#include <iostream>

Websocket::Websocket(
    tcp::socket&& socket,
    SocketManager* socketManager)
    : ws_(std::move(socket))
    , socketManager_(socketManager)
{
}

Websocket::~Websocket()
{
    // Remove this session from the list of active sessions
    socketManager_->Remove(this);
}

void Websocket::Run()
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
            &Websocket::OnAccept,
            shared_from_this()));
}

void Websocket::Fail(beast::error_code ec, char const* what)
{
    // Don't report these
    if (ec == net::error::operation_aborted ||
        ec == beast::websocket::error::closed)
        return;

    spdlog::error("{}: {}", what, ec.message());
}

void Websocket::OnAccept(beast::error_code ec)
{
    // Handle the error, if any
    if (ec)
        return Fail(ec, "accept");

    // Add this session to the list of active sessions
    socketManager_->Add(this);

    // Read a message
    ws_.async_read(
        buffer_,
        beast::bind_front_handler(
            &Websocket::OnRead,
            shared_from_this()));
}

void Websocket::OnRead(beast::error_code ec, std::size_t)
{
    // Handle the error, if any
    if (ec)
        return Fail(ec, "read");

    // Clear the buffer
    buffer_.consume(buffer_.size());

    // Read another message
    ws_.async_read(
        buffer_,
        beast::bind_front_handler(
            &Websocket::OnRead,
            shared_from_this()));
}

void Websocket::Send(std::shared_ptr<std::string const> const& ss)
{
    // Post our work to the strand, this ensures
    // that the members of `this` will not be
    // accessed concurrently.

    net::post(
        ws_.get_executor(),
        beast::bind_front_handler(
            &Websocket::OnSend,
            shared_from_this(),
            ss));
}

void Websocket::OnSend(std::shared_ptr<std::string const> const& ss)
{
    // Always add to queue
    writeQueue_.push_back(ss);

    // Are we already writing?
    if (isAsyncWriting_)
        return;

    isAsyncWriting_ = true;

    // We are not currently writing, so send this immediately
    ws_.async_write(
        net::buffer(*writeQueue_.front()),
        beast::bind_front_handler(
            &Websocket::OnWrite,
            shared_from_this()));
}

void Websocket::OnWrite(beast::error_code ec, std::size_t)
{
    // Handle the error, if any
    if (ec)
        return Fail(ec, "write");

    // Remove the string from the queue
    writeQueue_.erase(writeQueue_.begin());

    // Send the next message if any
    if (writeQueue_.empty()) {
        isAsyncWriting_ = false;
        return;
    }

    ws_.async_write(
        net::buffer(*writeQueue_.front()),
        beast::bind_front_handler(
            &Websocket::OnWrite,
            shared_from_this()));
}
