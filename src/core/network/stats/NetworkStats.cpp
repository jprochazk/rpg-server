#include "pch.h"
#include "NetworkStats.h"

std::string NetworkStats::Measurement::ToString() {
	constexpr auto ONE_MB = 1024 * 1024;
	constexpr auto ONE_KB = 1024;
	constexpr auto ONE_B = 1;

	auto sunits = (sent >= ONE_MB) ? "MB"
		: (sent >= ONE_KB) ? "KB"
		: "B";
	auto sspace = (sent >= ONE_KB) ? "" : " ";
	auto sdivisor = (sent >= ONE_MB) ? ONE_MB
		: (sent >= ONE_KB) ? ONE_KB
		: ONE_B;

	auto runits = (read >= ONE_MB) ? "MB"
		: (read >= ONE_KB) ? "KB"
		: "B";
	auto rspace = (read >= ONE_KB) ? "" : " ";
	auto rdivisor = (read >= ONE_MB) ? ONE_MB
		: (read >= ONE_KB) ? ONE_KB
		: ONE_B;

	auto readActual = ((float)read / rdivisor) * (1000.f / timePassed);
	auto writtenActual = ((float)sent / sdivisor) * (1000.f / timePassed);
	return fmt::format("{:.1f} {}/s {}DOWN | {:.1f} {}/s {}UP | {} connections",
		readActual, runits, rspace, writtenActual, sunits, sspace, connections);
}

NetworkStats::NetworkStats()
	: connections_(0), read_(0), sent_(0)
	, lastMeasurement_(), lastMeasuredAt_(std::chrono::steady_clock::now())
{}

NetworkStats& NetworkStats::Instance() {
	static NetworkStats instance;
	return instance;
}

void NetworkStats::Connected() {
	return Instance().Connected_();
}
void NetworkStats::Disconnected() {
	return Instance().Disconnected_();
}
void NetworkStats::Read(size_t bytes) {
	return Instance().Read_(bytes);
}
void NetworkStats::Sent(size_t bytes) {
	return Instance().Sent_(bytes);
}
NetworkStats::Measurement NetworkStats::Measure() {
	return Instance().Measure_();
}

void NetworkStats::Connected_() {
	connections_.store(connections_.load(std::memory_order_acquire) + 1, std::memory_order_release);
}
void NetworkStats::Disconnected_() {
	if (connections_ == 0) return;
	connections_.store(connections_.load(std::memory_order_acquire) - 1, std::memory_order_release);
}
void NetworkStats::Read_(size_t bytes) {
	read_.store(read_.load(std::memory_order_acquire) + bytes, std::memory_order_release);
}
void NetworkStats::Sent_(size_t bytes) {
	sent_.store(sent_.load(std::memory_order_acquire) + bytes, std::memory_order_release);
}

NetworkStats::Measurement NetworkStats::Measure_() {
	auto timePassed = static_cast<int>(
		std::chrono::duration_cast<std::chrono::milliseconds>(
			std::chrono::steady_clock::now() - lastMeasuredAt_
			).count()
		);
	if (timePassed == 0) timePassed = 1;

	lastMeasuredAt_ = std::chrono::steady_clock::now();
	lastMeasurement_ = {
		/*connections = */ connections_.load(std::memory_order_acquire),
		/*sent		  = */ sent_.load(std::memory_order_acquire),
		/*read		  = */ read_.load(std::memory_order_acquire),
		/*timePassed  = */ timePassed
	};

	read_ = 0;
	sent_ = 0;
	return lastMeasurement_;
}
