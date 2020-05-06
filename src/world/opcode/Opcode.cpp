#include "pch.h"
#include "Opcode.h"

#include "world/World.h"
#include "world/network/WorldSession.h"
#include "world/network/WorldPacket.h"


OpcodeTable::OpcodeTable()
	: handlers_() // default initialize all handler ptrs to nullptr
{

#define INIT_NULL_HANDLER(opcode) \
	RegisterHandler(Opcode::##opcode, #opcode, true, nullptr);
#define INIT_HANDLER(opcode, async, fn) \
	RegisterHandler(Opcode::##opcode, #opcode, async, fn);

	INIT_NULL_HANDLER(STEST);
	// a handler function can either be a non-capturing lambda or a function pointer
	// TODO: maybe put handlers in their own functions?
	INIT_HANDLER(CTEST, false, [](WorldSession* session, WorldPacket& packet) {
		std::optional<uint32_t> sequence;
		packet >> sequence;

		if (!sequence) {
			return;
		}

		std::string date = sWorld.GetDate();
		WorldPacket pkt(Opcode::STEST, sWorld.GetTimeStamp());
		pkt << date;

		session->SendPacket(pkt);

		session->Close();
	});
	INIT_NULL_HANDLER(MAX);

	// failing to initialize a handler for each opcode is a programmer error
	for (size_t i = 0; i < NUM_OPCODES; i++) {
		DEBUG_ASSERT(handlers_[i] != nullptr, "Uninitialized opcode handler for opcode #{}", i);
	}
	LOG_INFO("Opcode table initialized");

#undef INIT_NULL_HANDLER
#undef INIT_HANDLER
}


OpcodeTable::OpcodeHandler::OpcodeHandler(uint16_t opcode, const char* name, bool async, HandlerFn fn)
	: opcode(opcode), name(name), async(async), fn(fn)
{
}

OpcodeTable::OpcodeHandler::~OpcodeHandler() 
{
}

bool OpcodeTable::OpcodeHandler::IsAsync() const noexcept 
{
	return async;
}

void OpcodeTable::OpcodeHandler::operator()(WorldSession* session, WorldPacket& packet) const 
{
	if(fn) fn(session, packet);
}

const OpcodeTable::OpcodeHandler& OpcodeTable::GetHandler(uint16_t opcode) 
{
	if (opcode <= Opcode::MAX) {
		auto* handler = Instance().handlers_[opcode];
		DEBUG_ASSERT(handler != nullptr, "Uninitialized handler for opcode {}", opcode);
		return *handler;
	}
	else {
		return *(Instance().handlers_[Opcode::MAX]);
	}

}

OpcodeTable::~OpcodeTable() {
	delete[] &handlers_;
}

std::string OpcodeTable::ToString(uint16_t opcode) {
	return GetHandler(opcode).name;
}

bool OpcodeTable::IsAsync(uint16_t opcode) {
	return GetHandler(opcode).async;
}

OpcodeTable& OpcodeTable::Instance() {
	static OpcodeTable instance;
	return instance;
}

void OpcodeTable::RegisterHandler(
	uint16_t opcode, 
	const char* name, 
	bool async, 
	OpcodeTable::HandlerFn fn
) {
	DEBUG_ASSERT(opcode <= Opcode::MAX, "Invalid opcode {}", opcode);
	handlers_[opcode] = new OpcodeHandler(opcode, name, async, fn);
}

