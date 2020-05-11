#pragma once
#ifndef SERVER_WORLD_WORLD_DATABASE_H
#define SERVER_WORLD_WORLD_DATABASE_H

#include "pch.h"
#include "core/database/Connection.h"

// ideally, i'd like this to be an abstraction of the DBAL
// i shouldn't have to write queries in my business logic

class WorldDatabase {
public:

	WorldDatabase(WorldDatabase const&) = delete;
	void operator=(WorldDatabase const&) = delete;
	~WorldDatabase();
private:
	static WorldDatabase& Instance();
	WorldDatabase();

	DB::Connection db;
};

#endif //SERVER_WORLD_WORLD_DATABASE_H