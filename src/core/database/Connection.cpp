
#include "database/connection.h"

#include <spdlog/spdlog.h>


namespace database {

connection::connection(const settings& settings)
	: connection_(fmt::format("postgresql://{}:{}@{}:{}/{}",
		settings.user, settings.password, settings.host, settings.port, settings.name))
{
}

connection::~connection() {
	std::lock_guard<std::mutex> lock(connection_mutex_);
	for (const auto& s : prepared_statements_) {
		connection_.unprepare(s);
	}
}

result connection::query(const std::string& query) {
	std::lock_guard<std::mutex> lock(connection_mutex_);

	pqxx::work w(connection_);
	auto result = w.exec(query);
	w.commit();

	return result;
}

query_handle connection::async_query(const std::string& query) {
	return boost::async([query, this] {
		std::lock_guard<std::mutex> lock(connection_mutex_);

		pqxx::work w(connection_);
		auto result = w.exec(query);
		w.commit();

		return result;
	});
}

void connection::prepare_statement(const std::string& name, const std::string& query) {
	std::lock_guard<std::mutex> lock(connection_mutex_);

	if (prepared_statements_.find(name) != prepared_statements_.end()) {
		spdlog::error("Prepared statement \"{}\" already exists", name);
		abort();
	}

	try {
		connection_.prepare(name, query);
		prepared_statements_.insert(name);
	}
	catch (std::exception e) {
		spdlog::error("Failed to prepare statement {{ \"{}\": \"{}\" }}, {}", name, query, e.what());
		abort();
	}
}

void connection::unprepare_statement(const std::string& name) {
	std::lock_guard<std::mutex> lock(connection_mutex_);

	if (prepared_statements_.find(name) != prepared_statements_.end())
		return;

	connection_.unprepare(name);
}

}; // namespace DB