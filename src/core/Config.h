#pragma once
#ifndef SERVER_CORE_CONFIG_H
#define SERVER_CORE_CONFIG_H

#include "pch.h"
#include <nlohmann/json.hpp>
using json = nlohmann::json;

#include <boost/filesystem.hpp>
namespace fs = boost::filesystem;

class Config {
public:
	static void Load(const std::string& path);

	template<class T>
	static std::optional<T> Get(const std::string& key) {
		auto& content = Instance().content_;
		auto it = content.find(key);
		if (it != content.end()) {
			return it.value();
		}
		else {
			LOG_WARN("Config entry \"{}\" is undefined", key);
			return {};
		}
	}

	template<class T>
	static T GetOrThrow(const std::string& key) {
		auto it = Get<T>(key);
		if (it) { 
			return it.value(); 
		}
		else {
			throw std::runtime_error(
				fmt::format("Config file \"{}\" has no field \"{}\"", Instance().path_, key));
		}
	}

	template<class T>
	static T GetOrDefault(const std::string& key, const T& default_value) {
		auto it = Get<T>(key);
		if (it) { 
			return it.value(); 
		}
		else { 
			return default_value; 
		}
	}

private:
	static Config& Instance();
	Config() = default;

	std::string path_;
	json content_;

public:
	Config(Config const&) = delete;
	void operator=(Config const&) = delete;
	~Config() = default;
};

#endif // SERVER_CORE_CONFIG_H