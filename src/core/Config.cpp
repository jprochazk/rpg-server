
#include "pch.h"
#include "Config.h"

void Config::Load(const std::string& path) {
	std::lock_guard<std::mutex> lock(accessMutex_);

	instance_.reset(new Config(path));
}

Config::Config(const std::string& path) {
	path_ = fs::complete(path).string();
	LOG_INFO("Loading config file \"{}\"", path_);

	std::ifstream config_file(path_);
	if (config_file.bad()) {
		LOG_ERROR("Could not open file \"{}\", aborting execution...", path_);
		abort();
	}

	try {
		content_ = json::parse(config_file);
	}
	catch (json::exception e) {
		LOG_ERROR("{}", e.what());
		abort();
	}
}

std::mutex Config::accessMutex_;
std::unique_ptr<Config> Config::instance_(nullptr);