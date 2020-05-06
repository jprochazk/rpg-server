
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
	LOG_INFO("Received signal {}", (signalNum == SIGINT) ? "SIGINT" : "SIGTERM");
	if(error) {
		LOG_ERROR("Error: {}", error.message());
	}
	exitSignal.store(true, std::memory_order_release);
}

DB::Settings GetDBSettingsFromConfig() {
	auto options = Config::GetOrThrow<json>("database");
	auto user = options["user"].get<std::string>();
	auto password = options["password"].get<std::string>();
	auto host = options["host"].get<std::string>();
	auto port = options["port"].get<uint16_t>();
	auto name = options["name"].get<std::string>();

	return DB::Settings{ user,password,host,port,name };
}

std::shared_ptr<std::thread> StartNetwork(net::io_context& ioc, const std::string& address, uint16_t port) {
	return std::shared_ptr<std::thread>(new std::thread(
		[&ioc, address, port] {
		LOG_INFO("Starting network thread");

		net::signal_set signals(ioc, SIGINT, SIGTERM);
		signals.async_wait(SignalHandler);

		// Create and launch a listening port
		std::make_shared<SocketListener>(
			ioc,
			tcp::endpoint{ net::ip::make_address(address), port },
			sWorld.GetSocketManager()
			)->Run();

		// Run the I/O service on the requested number of threads
		ioc.run();
	}),
		// when the shared_ptr is dropped
		// ioc should be stopped
		[&ioc](std::thread* thread) {
		ioc.stop();
		thread->join();
		delete thread;
	});
}

int main()
{
	Config::Load("config.json");

	// TODO: database singleton
	auto settings = GetDBSettingsFromConfig();
	LOG_INFO("Connecting to database postgresql://{}:{}@{}:{}/{}",
		settings.user, settings.password, settings.host, settings.port, settings.name);
	DB::Connection conn(settings);

	// instantiate world singleton
	World::Instance();

	auto server_address_str = Config::GetOrDefault<std::string>("ip", "127.0.0.1");
	auto server_port = Config::GetOrDefault<uint16_t>("port", 8001);
	LOG_INFO("Starting server at {}:{}", server_address_str, server_port);

	net::io_context ioc;
	auto networkHandle = StartNetwork(ioc, server_address_str, server_port);

	sWorld.StartMainLoop(&exitSignal, 1, 5);

	return EXIT_SUCCESS;
}
