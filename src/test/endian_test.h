#pragma once

#include <gtest/gtest.h>
#include "common/endian.h"

TEST(endian_test, endian_convert_u8_ref) {
	uint8_t expected = 100;

	uint8_t actual = 100;
	common::endian_convert(actual);

	EXPECT_EQ(expected, actual);
}
TEST(endian_test, endian_convert_u16_ref) {
	uint16_t expected = 0x0010;

	uint16_t actual = 0x1000;
	common::endian_convert(actual);

	EXPECT_EQ(expected, actual);
}
TEST(endian_test, endian_convert_u32_ref) {
	uint32_t expected = 0x00000001;

	uint32_t actual = 0x01000000;
	common::endian_convert(actual);

	EXPECT_EQ(expected, actual);
}
TEST(endian_test, endian_convert_i8_ref) {
	int8_t expected = -0x10;

	int8_t actual = -0x10;
	common::endian_convert(actual);

	EXPECT_EQ(expected, actual);
}
TEST(endian_test, endian_convert_i16_ref) {
	int16_t expected = 0x0001;

	int16_t actual = 0x0100;
	common::endian_convert(actual);

	EXPECT_EQ(expected, actual);
}
TEST(endian_test, endian_convert_i32_ref) {
	int32_t expected = 0x00000001;

	int32_t actual = 0x01000000;
	common::endian_convert(actual);

	EXPECT_EQ(expected, actual);
}
TEST(endian_test, endian_convert_f32_ref) {
	auto expected = static_cast<float>(1142.31994628906);

	auto actual = static_cast<float>(0.09890416264534);
	common::endian_convert(actual);

	EXPECT_EQ(expected, actual);
}