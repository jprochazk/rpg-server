
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
#include <vector>

#include <spdlog/spdlog.h>

#endif // SERVER_PRECOMPILED_HEADER