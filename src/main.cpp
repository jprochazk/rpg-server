
#include "pch.h"
#include "network/net.h"
#include "network/SocketListener.h"
#include "world/World.h"

std::atomic<bool> exitSignal = false;
void SignalHandler(boost::system::error_code const& error, int signalNum) 
{
	std::cout << std::endl;
	spdlog::info("Received signal {}", (signalNum == SIGINT) ? "SIGINT" : "SIGTERM");
	if(error) {
		spdlog::info("Error: {}", error.message());
	}
	exitSignal.store(true, std::memory_order_release);
}

int main(int argc, char* argv[])
{
	if(argc > 3) {
		std::cout 	<< "\n"
					<< "Usage: " << argv[0] << " <address> [port]\n"
					<< "\n"
					<< "address\t - server address, default is 127.0.0.1\n"
					<< "port\t - optional server port, default is 8001\n"
					<< std::endl;

		return 0;
	}
	
	std::string address_str;
	net::ip::address address;
	if(argc > 1) {
		address_str = argv[1];
		try {
			address = net::ip::make_address(address_str);
		} catch(...) {
			std::cerr << "Invalid address \"" << argv[1] << "\"" << std::endl;
			return EXIT_FAILURE;
		}
	}
	
	uint16_t port;
	if(argc > 2) {
		try {
			port = static_cast<uint16_t>(std::atoi(argv[2]));
		} catch(...) {
			std::cerr << "Invalid port \"" << argv[2] << "\"" << std::endl;
			return EXIT_FAILURE;
		}
	} else {
		port = 8001U;
	}

	spdlog::info("Starting server at {}:{}", address_str, port);

	// instantiate world singleton
	World::Instance();

	// The io_context is required for all I/O
	net::io_context ioc;

	std::shared_ptr<std::thread> network_thread(new std::thread(
	[&] {
		spdlog::info("Starting network thread");

		net::signal_set signals(ioc, SIGINT, SIGTERM);
		signals.async_wait(SignalHandler);

		// Create and launch a listening port
		std::make_shared<SocketListener>(
			ioc,
			tcp::endpoint{ address, port },
			World::Instance()->GetSocketManager()
		)->Run();

		// Run the I/O service on the requested number of threads
		ioc.run();
	}), 
	[&ioc](std::thread* thread) {
		ioc.stop();
		thread->join();
		delete thread;
	});

	World::Instance()->StartMainLoop(&exitSignal, 1, 5);

	return EXIT_SUCCESS;
}
