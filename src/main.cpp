//
// Copyright (c) 2016-2019 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/vinniefalco/CppCon2018
//

//------------------------------------------------------------------------------
/*
	
*/
//------------------------------------------------------------------------------

#include "network/SocketListener.h"
#include "world/World.h"

#include <boost/smart_ptr.hpp>
#include <spdlog/spdlog.h>
#include <iostream>
#include <vector>
#include <chrono>
#include <functional>
#include <thread>
#include <string>
#include <atomic>

int
main(int argc, char* argv[])
{
	if(argc < 2 || argc > 3) {
		std::cout 	<< "\n"
					<< "Usage: " << argv[0] << " <address> [port]\n"
					<< "\n"
					<< "address\t - server address, e.g. 127.0.0.1\n"
					<< "port\t - optional server port, default is 8001\n"
					<< std::endl;

		return 0;
	}
	
	auto address = argv[1];
	uint16_t port;
	if(argc == 3) {
		try {
			port = static_cast<uint16_t>(std::atoi(argv[2]));
		} catch(...) {
			std::cerr << "Invalid port \"" << argv[2] << "\"" << std::endl;
			return EXIT_FAILURE;
		}
	} else {
		port = 8001U;
	}

	spdlog::info("Starting server at {}:{}", address, port);
	//instantiate world
	World::Instance();

	auto network_thread = std::thread([&] {
		// The io_context is required for all I/O
		net::io_context ioc;

		spdlog::info("Starting network thread");

		// Create and launch a listening port
		std::make_shared<SocketListener>(
			ioc,
			tcp::endpoint{ net::ip::make_address(address), port },
			World::Instance()->GetSocketManager()
		)->Run();

		// Run the I/O service on the requested number of threads
		ioc.run();
	});
	
	auto last_update = std::chrono::steady_clock::now();
	for (;;) {
		auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - last_update);
		if (elapsed.count() >= 1000) {
			last_update = std::chrono::steady_clock::now();
			World::Instance()->Update();
		}
	}

	network_thread.join();

	return EXIT_SUCCESS;
}
