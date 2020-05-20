#pragma once


// self-contained integration test

#include <gtest/gtest.h>
#include <map>
#include <atomic>
#include "network/socket_listener.h"
#include "network/socket_handler.h"
#include "network/socket.h"
#include "network_test_client.h"
#include <spdlog/spdlog.h>

class test_server_handler : public network::socket_handler {
public:
	std::function<void(uint32_t, std::weak_ptr<network::socket>)> on_open_fn;
	void set_on_open(std::function<void(uint32_t, std::weak_ptr<network::socket>)> fn) {
		on_open_fn = fn;
	}
	void on_open(uint32_t id, std::weak_ptr<network::socket> socket) override {
		if (on_open_fn) on_open_fn(id, socket);
	}

	std::function<void(uint32_t)> on_close_fn;
	void set_on_close(std::function<void(uint32_t)> fn) {
		on_close_fn = fn;
	}
	void on_close(uint32_t id) override {
		if (on_close_fn) on_close_fn(id);
	}

	std::function<void(uint32_t, std::vector<uint8_t>&&)> on_message_fn;
	void set_on_message(std::function<void(uint32_t,std::vector<uint8_t>&&)> fn) {
		on_message_fn = fn;
	}
	void on_message(uint32_t id, std::vector<uint8_t>&& data) override {
		if (on_message_fn) on_message_fn(id, std::move(data));
	}

	std::function<void(uint32_t, std::string_view, beast::error_code)> on_error_fn;
	void set_on_error(std::function<void(uint32_t,std::string_view,beast::error_code)> fn) {
		on_error_fn = fn;
	}
	void on_error(uint32_t id, std::string_view what, beast::error_code ec) override {
		if (on_error_fn) on_error_fn(id, what, ec);
	}
};
class test_client_handler : public network_test::client_handler {
public:
	std::function<void()> on_open_fn;
	void set_on_open(std::function<void()> fn) {
		on_open_fn = fn;
	}
	void on_open() override {
		if (on_open_fn) on_open_fn();
	}

	std::function<void()> on_close_fn;
	void set_on_close(std::function<void()> fn) {
		on_close_fn = fn;
	}
	void on_close() override {
		if (on_close_fn) on_close_fn();
	}

	std::function<void(std::vector<uint8_t>&&)> on_message_fn;
	void set_on_message(std::function<void(std::vector<uint8_t>&&)> fn) {
		on_message_fn = fn;
	}
	void on_message(std::vector<uint8_t>&& data) override {
		if (on_message_fn) on_message_fn(std::move(data));
	}

	std::function<void(std::string_view, beast::error_code)> on_error_fn;
	void set_on_error(std::function<void(std::string_view, beast::error_code)> fn) {
		on_error_fn = fn;
	}
	void on_error(std::string_view what, beast::error_code ec) override {
		if (on_error_fn) on_error_fn(what, ec);
	}
};

class NetworkTest : public ::testing::Test {
public:
	std::atomic<bool> done;
	std::atomic<bool> success;

	net::io_context ioc;

	std::shared_ptr<test_server_handler> server_handler;
	std::shared_ptr<network::socket_listener> server;
	std::shared_ptr<test_client_handler> client_handler;
	std::shared_ptr<network_test::client> client;

	inline void finish(bool status) {
		success.store(status, std::memory_order_release);
		done.store(true, std::memory_order_release);
	}

	bool get_status() {
		return success.load(std::memory_order_acquire);
	}

	NetworkTest()
		: done(false)
		, success(false)
		, ioc()
	{
		server_handler = std::make_shared<test_server_handler>();
		server = std::make_shared<network::socket_listener>(
			ioc, tcp::endpoint{ net::ip::make_address("127.0.0.1"), 8001 },
			server_handler);
		client_handler = std::make_shared<test_client_handler>();
		client = std::make_shared<network_test::client>(ioc, "127.0.0.1", "8001", client_handler);
	}
};

std::shared_ptr<std::thread> dispatch_test_task(net::io_context& ioc, std::function<void()> fn) {
	return std::shared_ptr<std::thread>(new std::thread([&ioc, fn] {
		fn();
		ioc.run();
	}), [&ioc](std::thread* thread) {
		ioc.stop();
		thread->join();
		delete thread;
	});
}

bool wait_for(
	NetworkTest* context, 
	std::chrono::duration<uint64_t> timeout = std::chrono::seconds(5)) 
{
	auto start = std::chrono::steady_clock::now();
	while (!context->done.load(std::memory_order_acquire)) {
		if (std::chrono::steady_clock::now() >= start + timeout) {
			spdlog::info("timed out");
			return false;
		}
	}

	return context->get_status();
}

TEST_F(NetworkTest, network_connection) {
	auto handle = dispatch_test_task(ioc, [this]() {
		server->open();
		client->open();

		client_handler->set_on_error([this](std::string_view what, beast::error_code ec) {
			static const auto start = std::chrono::steady_clock::now();
			static const auto timeout = std::chrono::seconds(2);
			if (std::chrono::steady_clock::now() >= start + timeout) {
				spdlog::error("{}: {}", what, ec.message());
			}
			else {
				client->open();
			}
		});
		client_handler->set_on_open([this]() {
			finish(true);
		});
	});

	EXPECT_TRUE(wait_for(this));
}

TEST_F(NetworkTest, network_client_receive) {
	auto handle = dispatch_test_task(ioc, [this] {
		server->open();
		client->open();

		client_handler->set_on_error([this](std::string_view what, beast::error_code ec) {
			static const auto start = std::chrono::steady_clock::now();
			static const auto timeout = std::chrono::seconds(2);
			if (std::chrono::steady_clock::now() >= start + timeout) {
				spdlog::error("{}: {}", what, ec.message());
			}
			else {
				client->open();
			}
		});

		server_handler->set_on_open([this](uint32_t, std::weak_ptr<network::socket> socket) {
			if (auto ptr = socket.lock()) {
				ptr->send({ 0,0,0,0 });
			}
			else {
				spdlog::info("could not acquire socket");
				finish(false);
			}
		});
		client_handler->set_on_message([this](std::vector<uint8_t>&& packet) {
			if (packet != std::vector<uint8_t>{ 0,0,0,0 }) {
				spdlog::info("got invalid packet");
				finish(false);
			}
			else {
				finish(true);
			}
		});
	});

	EXPECT_TRUE(wait_for(this));
}

TEST_F(NetworkTest, network_server_receive) {
	auto handle = dispatch_test_task(ioc, [this] {
		server->open();
		client->open();

		client_handler->set_on_error([this](std::string_view what, beast::error_code ec) {
			static const auto start = std::chrono::steady_clock::now();
			static const auto timeout = std::chrono::seconds(2);
			if (std::chrono::steady_clock::now() >= start + timeout) {
				spdlog::error("{}: {}", what, ec.message());
			}
			else {
				client->open();
			}
		});

		server_handler->set_on_message([this](uint32_t, std::vector<uint8_t>&& data) {
			if (data != std::vector<uint8_t>{0, 0, 0, 0}) {
				spdlog::info("got invalid packet");
				finish(false);
			}
			else {
				finish(true);
			}
		});
		client_handler->set_on_open([this]() {
			client->send({ 0,0,0,0 });
		});
	});

	EXPECT_TRUE(wait_for(this));
}
