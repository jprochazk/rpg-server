#pragma once
#ifndef SERVER_CORE_SEQUENCE_H
#define SERVER_CORE_SEQUENCE_H

#include "pch.h"

template<typename NumberType>
class Sequence {
	static_assert(std::is_integral<NumberType>::value, "NumberType must be integral!");
public:
	Sequence(
		NumberType start = static_cast<NumberType>(0), 
		NumberType increment = static_cast<NumberType>(1)
	)	: current_(start), increment_(increment) {}

	NumberType Get() {
		auto next = current_.load(std::memory_order_acquire);
		current_.store(current_.load(std::memory_order_acquire) + increment_, std::memory_order_release);
		return next;
	}

	std::atomic<NumberType> current_;
	NumberType increment_;
};

#endif // SERVER_CORE_SEQUENCE_H