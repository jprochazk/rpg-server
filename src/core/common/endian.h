#pragma once

#include "common/platform.h"
#include <cstdint>
#include <cstddef>

#if defined(__LITTLE_ENDIAN__)

namespace common {

namespace byte {

template<size_t ByteSize>
inline void convert(char* val)
{
    std::swap(*val, *(val + ByteSize - 1));
    convert<ByteSize - 2>(val + 1);
}

// specialize for corner cases sizeof(T) = 0 or sizeof(T) = 1
// 0-byte values are invalid, 1-byte values don't need conversion
template<> inline void convert<0>(char*) { }
template<> inline void convert<1>(char*) { }

template<typename T>
inline void apply(T* val) { convert<sizeof(T)>((char*)(val)); }

}

template<typename T> inline void endian_convert(T& val) {
    byte::apply<T>(&val);
}
#elif defined(__BIG_ENDIAN__)
    template<typename T> inline void endian_convert(T&) { }
#else
    # warning "Unknown platform endianness"
#endif

// single byte does not need endian convert
template<> inline void endian_convert(uint8_t&) { }
template<> inline void endian_convert(int8_t&) { }

}
