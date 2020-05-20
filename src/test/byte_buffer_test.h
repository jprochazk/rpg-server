#pragma once

#include <gtest/gtest.h>
#include "network/byte_buffer.h"

TEST(byte_buffer_test, write_std_string) {
	std::vector<uint8_t> expected = {
		6, 72, 101, 108, 108, 111, 33
	};

	network::byte_buffer bb(7);
	bb << std::string("Hello!");

	auto actual = bb.contents();

	EXPECT_EQ(expected, actual);
}
TEST(byte_buffer_test, read_str_string) {
	auto expected = std::string{ "Hello!" };

	network::byte_buffer bb{ std::vector<uint8_t>{ 6, 72, 101, 108, 108, 111, 33 } };
	std::optional<std::string> actual_opt;
	bb >> actual_opt;
	EXPECT_TRUE(actual_opt.has_value());
	auto actual = actual_opt.value();

	EXPECT_EQ(expected, actual);
}

namespace bbtest {
	typedef struct test_struct {
		int8_t		i8;
		int16_t		i16;
		int32_t		i32;
		uint8_t		u8;
		uint16_t	u16;
		uint32_t	u32;
		float		f32;
	} test_struct;
	typedef struct test_struct_opt {
		std::optional<int8_t>	i8;
		std::optional<int16_t>	i16;
		std::optional<int32_t>	i32;
		std::optional<uint8_t>	u8;
		std::optional<uint16_t>	u16;
		std::optional<uint32_t>	u32;
		std::optional<float>	f32;
	} test_struct_opt;
	bool operator==(test_struct const& lhs, test_struct const& rhs) {
		return lhs.i8 == rhs.i8 &&
			lhs.i16 == rhs.i16 &&
			lhs.i32 == rhs.i32 &&
			lhs.u8 == rhs.u8 &&
			lhs.u16 == rhs.u16 &&
			lhs.u32 == rhs.u32 &&
			lhs.f32 == rhs.f32;
	}
	bool has_values(test_struct_opt const& s) {
		return s.i8.has_value() &&
			s.i32.has_value() &&
			s.i16.has_value() &&
			s.u8.has_value() &&
			s.u16.has_value() &&
			s.u32.has_value() &&
			s.f32.has_value();
	}

}

TEST(byte_buffer_test, read_numeric) {
	bbtest::test_struct expected = {
		-10,
		-4000,
		-1295223,
		10,
		4000,
		1295223,
		1000.5
	};

	network::byte_buffer test_buffer{ std::vector<uint8_t>{
		246,				// i8
		240, 96,			// i16
		255, 236, 60, 137,	// i32
		10,					// u8
		15, 160,			// u16
		0, 19, 195, 119,	// u32
		68, 122, 32, 0		// f32
	} };

	bbtest::test_struct_opt actual_opt;
	test_buffer >> actual_opt.i8;
	test_buffer >> actual_opt.i16;
	test_buffer >> actual_opt.i32;
	test_buffer >> actual_opt.u8;
	test_buffer >> actual_opt.u16;
	test_buffer >> actual_opt.u32;
	test_buffer >> actual_opt.f32;

	EXPECT_TRUE(bbtest::has_values(actual_opt));

	bbtest::test_struct actual = {
		actual_opt.i8.value(),
		actual_opt.i16.value(),
		actual_opt.i32.value(),
		actual_opt.u8.value(),
		actual_opt.u16.value(),
		actual_opt.u32.value(),
		actual_opt.f32.value()
	};

	EXPECT_EQ(expected, actual);
}
TEST(byte_buffer_test, write_numeric) {
	std::vector<uint8_t> expected = {
		246,				// i8
		240, 96,			// i16
		255, 236, 60, 137,	// i32
		10,					// u8
		15, 160,			// u16
		0, 19, 195, 119,	// u32
		68, 122, 32, 0		// f32
	};

	bbtest::test_struct test_values = {
		-10,
		-4000,
		-1295223,
		10,
		4000,
		1295223,
		1000.5
	};
	network::byte_buffer bb{ 18 };
	bb << test_values.i8;
	bb << test_values.i16;
	bb << test_values.i32;
	bb << test_values.u8;
	bb << test_values.u16;
	bb << test_values.u32;
	bb << test_values.f32;

	auto actual = bb.contents();

	EXPECT_EQ(expected, actual);
}
TEST(byte_buffer_test, append_bytes) {
	std::vector<uint8_t> expected = {
		0, 0, 0, 1
	};

	network::byte_buffer bb;
	bb << static_cast<uint8_t>(0);
	std::vector<uint8_t> test = {
		0, 0, 1
	};
	bb << test;
	auto actual = bb.contents();

	EXPECT_EQ(expected, actual);
}