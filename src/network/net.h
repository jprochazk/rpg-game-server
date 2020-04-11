//
// Copyright (c) 2016-2019 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/vinniefalco/CppCon2018
//

#pragma once
#ifndef SERVER_NETWORK_NET_H
#define SERVER_NETWORK_NET_H

#include <boost/asio.hpp>
#include <boost/beast.hpp>

namespace net = boost::asio;                    // from <boost/asio.hpp>
using tcp = net::ip::tcp;                       // from <boost/asio/ip/tcp.hpp>
namespace beast = boost::beast;                 // from <boost/beast.hpp>
namespace http = beast::http;                   // from <boost/beast/http.hpp>

#endif // SERVER_NETWORK_NET_H
