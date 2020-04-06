//
// Copyright (c) 2016-2019 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/vinniefalco/CppCon2018
//

#include "socket_manager.h"
#include "websocket_session.h"

socket_manager::socket_manager()
{
}

void socket_manager::join(websocket_session* session)
{
    std::lock_guard<std::mutex> lock(mutex_);
    sessions_.insert(session);
}

void socket_manager::leave(websocket_session* session)
{
    std::lock_guard<std::mutex> lock(mutex_);
    sessions_.erase(session);
}

// Broadcast a message to all websocket client sessions
void socket_manager::send(std::string message)
{
    auto const ss = boost::make_shared<std::string const>(std::move(message));
    
    std::vector<boost::weak_ptr<websocket_session>> v;
    {
        std::lock_guard<std::mutex> lock(mutex_);
        v.reserve(sessions_.size());
        for (auto p : sessions_)
            v.emplace_back(p->weak_from_this());
    }
    
    for (auto const& wp : v)
        if (auto sp = wp.lock())
            sp->send(ss);
}

