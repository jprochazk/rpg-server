#pragma once


#ifdef DEBUG
# include <spdlog/spdlog.h>
# define DEBUG_ASSERT(expr, msg, ...) \
	if(!(expr)) { \
		spdlog::error("DEBUG ASSERT FAILURE: {}", msg, __VA_ARGS__); \
		abort(); \
	}
#else
# define DEBUG_ASSERT(expr, format, ...) ;
#endif