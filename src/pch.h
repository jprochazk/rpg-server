
#pragma once
#ifndef SERVER_PRECOMPILED_HEADER
#define SERVER_PRECOMPILED_HEADER

#ifdef _WIN32
# define WIN32_LEAN_AND_MEAN
# include <Windows.h>
# undef min
# undef max
#endif

#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <utility>
#include <type_traits>
#include <limits>
#include <memory>
#include <functional>
#include <algorithm>
#include <optional>

#include <cstdlib>
#include <cstddef>
#include <cstdint>
#include <csignal>
#include <cmath>
#include <cstring>
#include <ctime>

#include <string>

#include <chrono>
#include <ratio>

#include <thread>
#include <atomic>
#include <mutex>

#include <unordered_set>
#include <unordered_map>
#include <map>
#include <vector>

#include <spdlog/spdlog.h>
#define LOG_INFO(format, ...) \
	spdlog::info(format, __VA_ARGS__);
#define LOG_WARN(format, ...) \
	spdlog::warn(format, __VA_ARGS__);
#define LOG_ERROR(format, ...) \
	spdlog::error(format, __VA_ARGS__);

#ifdef DEBUG
#define DEBUG_ASSERT(expr, msg, ...) \
	if(!(expr)) { \
		spdlog::error("DEBUG ASSERT FAILURE: {}", msg, __VA_ARGS__); \
		abort(); \
	}
#define DEBUG_LOG_INFO(format, ...) \
	spdlog::info(format, __VA_ARGS__);
#define DEBUG_LOG_WARN(format, ...) \
	spdlog::warn(format, __VA_ARGS__);
#define DEBUG_LOG_ERROR(format, ...) \
	spdlog::error(format, __VA_ARGS__);
#else
#define DEBUG_ASSERT(expr, msg, ...) ;
#define DEBUG_LOG_INFO(format, ...) ;
#define DEBUG_LOG_WARN(format, ...) ;
#define DEBUG_LOG_ERROR(format, ...) ;
#endif

#endif // SERVER_PRECOMPILED_HEADER