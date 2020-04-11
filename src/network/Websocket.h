//
// Copyright (c) 2016-2019 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/vinniefalco/CppCon2018
//

#pragma once
#ifndef SERVER_NETWORK_WEBSOCKET_H
#define SERVER_NETWORK_WEBSOCKET_H

#include "common/SharedDefines.h"
#include "common/ByteBuffer.h"
#include "network/net.h"

#include <spdlog/spdlog.h>

class SocketManager;
class Websocket;

/** Represents an active WebSocket connection to the server
*/
class Websocket : public std::enable_shared_from_this<Websocket>
{
public:
    Websocket(
        tcp::socket&& socket,
        SocketManager* socketManager);

    ~Websocket();

    void Run();
    void Send(const std::string ss);
    bool IsBufferEmpty() const;
    std::vector<ByteBuffer> GetBuffer();
private:
    beast::flat_buffer wsBuffer_;
    beast::websocket::stream<beast::tcp_stream> ws_;

    bool isAsyncWriting_;
    std::vector<std::string> writeBuffer_;

    std::mutex readLock_;
    std::vector<ByteBuffer> readBuffer_;
    SocketManager* socketManager_;

    void Fail(beast::error_code ec, char const* what);
    void OnAccept(beast::error_code ec);
    void OnRead(beast::error_code ec, std::size_t bytes_transferred);
    void OnWrite(beast::error_code ec, std::size_t bytes_transferred);
    void OnSend(const std::string ss);
}; // class Websocket

#endif // SERVER_NETWORK_WEBSOCKET_H
