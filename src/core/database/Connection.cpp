#include "pch.h"
#include "Connection.h"


namespace DB {

Connection::Connection(const Settings& settings)
// format connection string using options
	: connection_(fmt::format("postgresql://{}:{}@{}:{}/{}",
		settings.user, settings.password, settings.host, settings.port, settings.name))
{
}

Connection::~Connection() {
	std::lock_guard<std::mutex> lock(connectionMutex_);
	for (const auto& s : preparedStatements_) {
		connection_.unprepare(s);
	}
}

Result Connection::Query(const std::string& query) {
	std::lock_guard<std::mutex> lock(connectionMutex_);

	pqxx::work w(connection_);
	auto result = w.exec(query);
	w.commit();

	return result;
}

QueryHandle Connection::AsyncQuery(const std::string& query) {
	return boost::async(boost::launch::async, [query, this] {
		std::lock_guard<std::mutex> lock(connectionMutex_);

		pqxx::work w(connection_);
		auto result = w.exec(query);
		w.commit();

		return result;
	});
}

void Connection::PrepareStatement(const std::string& name, const std::string& query) {
	std::lock_guard<std::mutex> lock(connectionMutex_);

	if (preparedStatements_.find(name) != preparedStatements_.end()) {
		spdlog::error("Prepared statement \"{}\" already exists", name);
		abort();
	}

	try {
		connection_.prepare(name, query);
		preparedStatements_.insert(name);
	}
	catch (std::exception e) {
		spdlog::error("Failed to prepare statement {{ \"{}\": \"{}\" }}, {}", name, query, e.what());
		abort();
	}
}

void Connection::UnprepareStatement(const std::string& name) {
	std::lock_guard<std::mutex> lock(connectionMutex_);

	if (preparedStatements_.find(name) != preparedStatements_.end())
		return;

	connection_.unprepare(name);
}

}; // namespace DB