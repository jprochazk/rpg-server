#pragma once
#ifndef SERVER_COMMON_BYTE_CONVERT_H
#define SERVER_COMMON_BYTE_CONVERT_H

#include "pch.h"
/* Detect platform endianness at compile time */

// When available, these headers can improve platform endianness detection
#ifdef __has_include // C++17, supported as extension to C++11 in clang, GCC 5+, vs2015
#  if __has_include(<endian.h>)
#    include <endian.h> // gnu libc normally provides, linux
#  elif __has_include(<machine/endian.h>)
#    include <machine/endian.h> //open bsd, macos
#  elif __has_include(<sys/param.h>)
#    include <sys/param.h> // mingw, some bsd (not open/macos)
#  elif __has_include(<sys/isadefs.h>)
#    include <sys/isadefs.h> // solaris
#  endif
#endif

#if !defined(__LITTLE_ENDIAN__) && !defined(__BIG_ENDIAN__)
#  if (defined(__BYTE_ORDER__)  && __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__) || \
     (defined(__BYTE_ORDER) && __BYTE_ORDER == __BIG_ENDIAN) || \
	 (defined(_BYTE_ORDER) && _BYTE_ORDER == _BIG_ENDIAN) || \
	 (defined(BYTE_ORDER) && BYTE_ORDER == BIG_ENDIAN) || \
     (defined(__sun) && defined(__SVR4) && defined(_BIG_ENDIAN)) || \
     defined(__ARMEB__) || defined(__THUMBEB__) || defined(__AARCH64EB__) || \
     defined(_MIBSEB) || defined(__MIBSEB) || defined(__MIBSEB__) || \
     defined(_M_PPC)
#    define __BIG_ENDIAN__
#  elif (defined(__BYTE_ORDER__) && __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__) /* gcc */ || \
     (defined(__BYTE_ORDER) && __BYTE_ORDER == __LITTLE_ENDIAN) /* linux header */ || \
	 (defined(_BYTE_ORDER) && _BYTE_ORDER == _LITTLE_ENDIAN) || \
	 (defined(BYTE_ORDER) && BYTE_ORDER == LITTLE_ENDIAN) /* mingw header */ ||  \
     (defined(__sun) && defined(__SVR4) && defined(_LITTLE_ENDIAN)) || /* solaris */ \
     defined(__ARMEL__) || defined(__THUMBEL__) || defined(__AARCH64EL__) || \
     defined(_MIPSEL) || defined(__MIPSEL) || defined(__MIPSEL__) || \
     defined(_M_IX86) || defined(_M_X64) || defined(_M_IA64) || /* msvc for intel processors */ \
     defined(_M_ARM) /* msvc code on arm executes in little endian mode */
#    define __LITTLE_ENDIAN__
#  endif
#endif

#if defined(__LITTLE_ENDIAN__)

namespace ByteConverter
{
    template<size_t T>
    inline void convert(char *val)
    {
        std::swap(*val, *(val + T - 1));
        convert<T - 2>(val + 1);
    }

    // specialize for corner cases sizeof(T) = 0 or sizeof(T) = 1
    // 0-byte values are invalid, 1-byte values don't need conversion
    template<> inline void convert<0>(char *) { }
    template<> inline void convert<1>(char *) { }

    template<typename T> 
    inline void apply(T *val) { convert<sizeof(T)>((char *)(val)); }
}

template<typename T> inline void EndianConvert(T& val) { 
    ByteConverter::apply<T>(&val); 
}
#elif defined(__BIG_ENDIAN__)
template<typename T> inline void EndianConvert(T&) { }
#else
# warning "Unknown platform/endianness, byte swaps not defined."
#endif

// single byte does not need endian convert
inline void EndianConvert(uint8_t&) { }
inline void EndianConvert( int8_t&) { }

#endif // SERVER_COMMON_BYTE_CONVERT_H
