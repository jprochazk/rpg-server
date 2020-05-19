#pragma once


#ifdef DEBUG
# include <spdlog/spdlog.h>
# define DEBUG_ASSERT(expr, msg, ...) \
	if(!(expr)) { \
		spdlog::error("DEBUG ASSERT FAILURE: {}", msg, __VA_ARGS__); \
		abort(); \
	}
#elif NDEBUG
# define DEBUG_ASSERT(expr, format, ...) ;
#endif