#pragma once
#ifndef SERVER_CORE_COMMON_BIT_MASK_H
#define SERVER_CORE_COMMON_BIT_MASK_H

template<typename NumericType>
constexpr inline bool HasBitFlag(const NumericType& mask, const NumericType& flag) {
	return (mask & flag) == flag;
}


#endif // SERVER_CORE_COMMON_BIT_MASK_H