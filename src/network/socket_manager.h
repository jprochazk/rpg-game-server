//
// Copyright (c) 2016-2019 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/vinniefalco/CppCon2018
//

#ifndef SERVER_NETWORK_SOCKET_MANAGER_H
#define SERVER_NETWORK_SOCKET_MANAGER_H

#include <boost/smart_ptr.hpp>
#include <memory>
#include <mutex>
#include <string>
#include <unordered_set>

// Forward declaration
class websocket_session;

// Represents the shared server state
class socket_manager
{
    // This mutex synchronizes all access to sessions_
    std::mutex mutex_;

    // Keep a list of all the connected clients
    std::unordered_set<websocket_session*> sessions_;

public:
    explicit socket_manager();

    void join(websocket_session* session);
    void leave(websocket_session* session);
    void send(std::string message);
}; // class socket_manager

#endif // SERVER_NETWORK_SOCKET_MANAGER_H
