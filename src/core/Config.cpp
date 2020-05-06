
#include "pch.h"
#include "Config.h"

Config& Config::Instance() {
	static Config instance;
	return instance;
}

void Config::Load(const std::string& path) {
	auto& instance = Instance();

	auto full_path = fs::complete(path).string();
	LOG_INFO("Loading config file \"{}\"", full_path);

	std::ifstream config_file(full_path);
	if (config_file.bad()) {
		LOG_ERROR("Could not open file \"{}\", aborting execution...", full_path);
		abort();
	}

	instance.path_ = std::move(full_path);
	try {
		instance.content_ = json::parse(config_file);
	}
	catch (json::exception e) {
		LOG_ERROR("{}", e.what());
		abort();
	}
}