//
// Copyright (c) 2016-2019 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/vinniefalco/CppCon2018
//

#include "Websocket.h"
#include "SocketManager.h"

Websocket::Websocket(
    tcp::socket&& socket,
    SocketManager* socketManager)
    : ws_(std::move(socket))
    , socketManager_(socketManager)
{
}

Websocket::~Websocket()
{
    readLock_.lock();

    // Remove this session from the list of active sessions
    socketManager_->Remove(this);
    spdlog::info("Socket closed");
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
    
    // Accept all data as binary (bytes aren't checked against valid utf-8)
    ws_.binary(true);

    // Accept the websocket handshake
    ws_.async_accept(
        beast::bind_front_handler(
            &Websocket::OnAccept,
            shared_from_this()));
}

void Websocket::Fail(beast::error_code ec, char const* what)
{
    // Don't report these
    if (ec == net::error::operation_aborted || ec == beast::websocket::error::closed)
        return;

    spdlog::error("{}: {}", what, ec.message());
}

void Websocket::OnAccept(beast::error_code ec)
{
    // Handle the error, if any
    if (ec) return Fail(ec, "accept");

    // Add this session to the list of active sessions
    socketManager_->Add(this);

    spdlog::info("Socket opened");

    // Read a message
    ws_.async_read(
        wsBuffer_,
        beast::bind_front_handler(
            &Websocket::OnRead,
            shared_from_this()));
}

bool Websocket::IsBufferEmpty() const
{
    return readBuffer_.size() == 0;
}

std::vector<ByteBuffer> Websocket::GetBuffer()
{
    std::lock_guard<std::mutex> lock(readLock_);

    // construct output buffer by moving elements from read buffer
    std::vector<ByteBuffer> buf(
        // move iterators convert values to rvalues when dereferencing the underlying iterator
        std::make_move_iterator(readBuffer_.begin()),
        std::make_move_iterator(readBuffer_.end()));
    // buffer is in undefined state because elements were moved
    // clearing it fixes this
    readBuffer_.clear();
    
    return buf;
}

void Websocket::OnRead(beast::error_code ec, std::size_t)
{
    // Handle the error, if any
    if (ec) return Fail(ec, "read");

    { // creating an inner scope causes the lock_guard to be dropped early
    std::lock_guard<std::mutex> lock(readLock_);

    std::vector<uint8_t> buf(wsBuffer_.size());
    boost::asio::buffer_copy(
        boost::asio::buffer(buf.data(), wsBuffer_.size()),
        wsBuffer_.data()
    );
    readBuffer_.push_back(ByteBuffer{std::move(buf)});
    // Clear the buffer
    wsBuffer_.consume(wsBuffer_.size());
    }

    // Read another message
    ws_.async_read(
        wsBuffer_,
        beast::bind_front_handler(
            &Websocket::OnRead,
            shared_from_this()));
}

void Websocket::Send(const ByteBuffer ss)
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

void Websocket::OnSend(const ByteBuffer ss)
{
    // Always add to queue
    writeBuffer_.push_back(ss);

    // Are we already writing?
    if (isAsyncWriting_)
        return;

    isAsyncWriting_ = true;

    // We are not currently writing, so send this immediately
    ws_.async_write(
        net::buffer(writeBuffer_.front().GetBuffer()),
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
    writeBuffer_.erase(writeBuffer_.begin());

    // Stop writing if the queue is empty
    if (writeBuffer_.empty()) {
        isAsyncWriting_ = false;
        return;
    }

    // Write another
    ws_.async_write(
        net::buffer(writeBuffer_.front().GetBuffer()),
        beast::bind_front_handler(
            &Websocket::OnWrite,
            shared_from_this()));
}
