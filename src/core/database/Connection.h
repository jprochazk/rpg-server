#pragma once

#include <unordered_set>
#include <pqxx/pqxx>

#define BOOST_THREAD_PROVIDES_FUTURE
#define BOOST_THREAD_PROVIDES_FUTURE_CONTINUATION
#include <boost/thread/future.hpp>

namespace database
{

typedef struct settings {
	std::string user;
	std::string password;
	std::string host;
	uint16_t	port;
	std::string name;
} settings;

using result = pqxx::result;
using query_handle = boost::future<result>;

class connection {
public:
	connection(const settings& settings);
	~connection();

		
	result query(const std::string& query);
	query_handle async_query(const std::string& query);

	template<typename ...Args>
	result execute(const std::string& name, Args&& ...args);

	template<typename ...Args>
	query_handle async_execute(const std::string& name, Args&& ...args);

	void prepare_statement(const std::string& name, const std::string& query);

	void unprepare_statement(const std::string& name);
private:
	std::mutex connection_mutex_;
	pqxx::connection connection_;

	std::unordered_set<std::string> prepared_statements_;
}; // class connection


template<typename ...Args>
result connection::execute(const std::string& name, Args&& ...args) {
	if (prepared_statements_.find(name) == prepared_statements_.end())
		throw std::runtime_error(fmt::format("No prepared statement named \"{}\"", name));

	std::lock_guard<std::mutex> lock(connection_mutex_);

	pqxx::work w(connection_);
	auto result = w.exec_prepared(name, args...);
	w.commit();

	return result;
}

template<typename ...Args>
query_handle connection::async_execute(const std::string& name, Args&& ...args) {
	if (prepared_statements_.find(name) == prepared_statements_.end())
		throw std::runtime_error(fmt::format("No prepared statement named \"{}\"", name));

	return boost::async([this, name, args...]{
		std::lock_guard<std::mutex> lock(connection_mutex_);

		pqxx::work w(connection_);
		auto result = w.exec_prepared(name, args...);
		w.commit();

		return result;
	});
}

}; // namespace database
