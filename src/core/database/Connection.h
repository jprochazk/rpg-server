#pragma once
#ifndef SERVER_CORE_DATABASE_CONNECTION_H
#define SERVER_CORE_DATABASE_CONNECTION_H

#include "pch.h"
#include <pqxx/pqxx>

#define BOOST_THREAD_PROVIDES_FUTURE
#define BOOST_THREAD_PROVIDES_FUTURE_CONTINUATION
#include <boost/thread/future.hpp>

namespace DB
{
	typedef struct Settings {
		std::string user;
		std::string password;
		std::string host;
		uint16_t	port;
		std::string name;
	} Settings;

	using Result = pqxx::result;
	using QueryHandle = boost::future<Result>;

	class Connection {
	public:
		Connection(const Settings& settings);
		~Connection();

		
		Result Query(const std::string& query);
		QueryHandle AsyncQuery(const std::string& query);

		template<typename ...Args>
		Result Execute(const std::string& name, Args&& ...args);

		template<typename ...Args>
		QueryHandle AsyncExecute(const std::string& name, Args&& ...args);

		void PrepareStatement(const std::string& name, const std::string& query);

		void UnprepareStatement(const std::string& name);
	private:
		std::mutex connectionMutex_;
		pqxx::connection connection_;

		std::unordered_set<std::string> preparedStatements_;
	};
}; // namespace DB


template<typename ...Args>
DB::Result DB::Connection::Execute(const std::string& name, Args&& ...args) {
	if (preparedStatements_.find(name) == preparedStatements_.end())
		throw std::runtime_error(fmt::format("No prepared statement named \"{}\"", name));

	std::lock_guard<std::mutex> lock(connectionMutex_);

	pqxx::work w(connection_);
	auto result = w.exec_prepared(name, args...);
	w.commit();

	return result;
}

template<typename ...Args>
DB::QueryHandle DB::Connection::AsyncExecute(const std::string& name, Args&& ...args) {
	if (preparedStatements_.find(name) == preparedStatements_.end())
		throw std::runtime_error(fmt::format("No prepared statement named \"{}\"", name));

	return boost::async([this, name, args...]{
		std::lock_guard<std::mutex> lock(connectionMutex_);

		pqxx::work w(connection_);
		auto result = w.exec_prepared(name, args...);
		w.commit();

		return result;
	});
}

#endif // SERVER_CORE_DATABASE_CONNECTION_H