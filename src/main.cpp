
#include "pch.h"
#include "core/network/net.h"
#include "core/network/SocketListener.h"
#include "world/World.h"
#include "core/Config.h"

#include "core/database/Connection.h"

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

int main()
{
	Config::Load("config.json");

	auto address_str = Config::GetOrDefault<std::string>("ip", "127.0.0.1");
	auto port = Config::GetOrDefault<uint16_t>("port", 8001);

	spdlog::info("Starting server at {}:{}", address_str, port);

	{
		auto options = Config::GetOrThrow<json>("database");
		auto user = options["user"].get<std::string>();
		auto password = options["password"].get<std::string>();
		auto host = options["host"].get<std::string>();
		auto port = options["port"].get<uint16_t>();
		auto name = options["name"].get<std::string>();
		spdlog::info("Connecting to database postgresql://{}:{}@{}:{}/{}",
			user, password, host, port, name);

		DB::Connection conn(DB::Settings{ user,password,host,port,name });
		auto result = conn.Query("SELECT 1");
		spdlog::info("Query result:");
		for (auto& row : result) {
			spdlog::info("Row: {}", row[0].as<int>());
		}

		return 0;
	}

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
			tcp::endpoint{ net::ip::make_address(address_str), port },
			sWorld.GetSocketManager()
		)->Run();

		// Run the I/O service on the requested number of threads
		ioc.run();
	}), 
	[&ioc](std::thread* thread) {
		ioc.stop();
		thread->join();
		delete thread;
	});

	sWorld.StartMainLoop(&exitSignal, 1, 5);

	return EXIT_SUCCESS;
}
