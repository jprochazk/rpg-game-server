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

#include "network/net.h"

#include <spdlog/spdlog.h>

#include <cstdlib>
#include <memory>
#include <string>
#include <vector>

class Websocket;

class SocketManager {
public:
    virtual void Add(Websocket* session) = 0;
    virtual void Remove(Websocket* session) = 0;

}; // class SocketManager

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
    void Send(std::shared_ptr<std::string const> const& ss);
private:
    beast::flat_buffer buffer_;
    beast::websocket::stream<beast::tcp_stream> ws_;
    bool isAsyncWriting_;
    std::vector<std::shared_ptr<std::string const>> writeQueue_;
    SocketManager* socketManager_;

    void Fail(beast::error_code ec, char const* what);
    void OnAccept(beast::error_code ec);
    void OnRead(beast::error_code ec, std::size_t bytes_transferred);
    void OnWrite(beast::error_code ec, std::size_t bytes_transferred);
    void OnSend(std::shared_ptr<std::string const> const& ss);
}; // class Websocket

#endif // SERVER_NETWORK_WEBSOCKET_H
