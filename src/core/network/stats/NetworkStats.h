#pragma once
#ifndef SERVER_CORE_NETWORK_STATS_NETWORK_STATS_H
#define SERVER_CORE_NETWORK_STATS_NETWORK_STATS_H

#include "pch.h"

class NetworkStats {
public:
	typedef struct Measurement {
		size_t connections;
		size_t sent;
		size_t read;
		int timePassed;

		std::string ToString();
	} Measurement;
public:
	static void Connected();
	static void Disconnected();
	static void Read(size_t bytes);
	static void Sent(size_t bytes);
	static Measurement Measure();
private:
	NetworkStats();
	static NetworkStats& Instance();

	void Connected_();
	void Disconnected_();
	void Read_(size_t bytes);
	void Sent_(size_t bytes);
	Measurement Measure_();
private:
	std::atomic<size_t> connections_;
	std::atomic<size_t> sent_;
	std::atomic<size_t> read_;

	Measurement lastMeasurement_;
	std::chrono::steady_clock::time_point lastMeasuredAt_;
};

#endif // SERVER_CORE_NETWORK_STATS_NETWORK_STATS_H