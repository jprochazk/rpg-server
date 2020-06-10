#pragma once

#include "game/world.h"
#include "game/system/system_base.h"
#include <gtest/gtest.h>

class test_system : public game::system_base {
public:
	size_t setupCalled = 0;
	size_t updateCalled = 0;

	void setup(game::world&) override {
		++setupCalled;
	}

	void update(game::world&) override {
		++updateCalled;
	}
};

class test_system2 : public game::system_base {
public:
	void setup(game::world&) override {}
	void update(game::world&) override {}
};

TEST(world, add_system) {
	auto system = std::make_shared<test_system>();

	game::world world;
	world.add_system("test", system);

	EXPECT_EQ(system->setupCalled, 1);
}

TEST(world, add_system_duplicate) {
	auto system = std::make_shared<test_system>();

	game::world world;
	world.add_system("test", system);
	try {
		world.add_system("test", system);
		FAIL();
	}
	catch (std::exception actual_ex) {
		std::exception expected_ex{ "System \"test\" already exists" };

		std::string actual{ actual_ex.what() };
		std::string expected{ expected_ex.what() };
		EXPECT_EQ(actual, expected);
	}
}

TEST(world, get_system) {
	game::world world;
	world.add_system("test", std::make_shared<test_system>());

	auto actual = world.get_system<test_system>("test");
	EXPECT_TRUE(static_cast<bool>(actual));
}

TEST(world, get_system_unknown) {
	game::world world;

	try {
		auto system = world.get_system<test_system>("test");
		FAIL();
	}
	catch (std::exception actual_ex) {
		std::exception expected_ex{ "System \"test\" does not exist" };

		std::string actual{ actual_ex.what() };
		std::string expected{ expected_ex.what() };
		EXPECT_EQ(actual, expected);
	}
}

TEST(world, get_system_wrong_type) {
	game::world world;
	world.add_system("test", std::make_shared<test_system>());

	try {
		auto system = world.get_system<test_system2>("test");
		FAIL();
	}
	catch (std::exception actual_ex) {
		std::exception expected_ex{ "System \"test\" is not of type \"class test_system2\"" };

		std::string actual{ actual_ex.what() };
		std::string expected{ expected_ex.what() };
		EXPECT_EQ(actual, expected);
	}
}

TEST(world, remove_system) {
	game::world world;

	auto system = std::make_shared<test_system>();
	world.add_system("test", system);
	world.remove_system("test");
	world.update();

	EXPECT_EQ(system->updateCalled, 0);
}