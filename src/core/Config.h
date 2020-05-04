#pragma once
#ifndef SERVER_COMMON_CONFIG_H
#define SERVER_COMMON_CONFIG_H

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
		std::lock_guard<std::mutex> lock(accessMutex_);

		auto it = instance_->content_.find(key);
		if (it != instance_->content_.end()) {
			return it.value();
		}
		else {
			spdlog::warn("Config entry \"{}\" is undefined", key);
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
				fmt::format("Config file \"{}\" has no field \"{}\"", instance_->path_, key));
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
	static std::mutex accessMutex_;
	static std::unique_ptr<Config> instance_;

	Config(const std::string& path);

	std::string path_;
	json content_;

public:
	Config(Config const&) = delete;
	void operator=(Config const&) = delete;
	~Config() = default;
};

#endif // SERVER_COMMON_CONFIG_H