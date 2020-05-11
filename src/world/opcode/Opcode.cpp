#include "pch.h"
#include "Opcode.h"

#include "world/network/WorldSession.h"
#include "world/network/packet/WorldPacket.h"

/* Handler functions */
#define HANDLER_FN(opcode, body) \
	void Handle_##opcode## body ;

HANDLER_FN(CMSG_MOVE, (WorldSession&, WorldPacket&) {
	// <uint8_t input flags>
});

#undef HANDLER_FN



OpcodeTable::OpcodeTable()
	: handlers_() // default initialize all handler ptrs to nullptr
{
#define REGISTER(opcode, async) \
	RegisterHandler(Opcode::##opcode, #opcode, async, &Handle_##opcode);

#define REGISTER_NULL(opcode) \
	RegisterHandler(Opcode::##opcode, #opcode, true, nullptr);

	REGISTER(CMSG_MOVE, true);
	REGISTER_NULL(SMSG_UPDATE);
	REGISTER_NULL(MAX);

	// failing to initialize a handler for each opcode is a programmer error
	for (size_t i = 0; i < NUM_OPCODES; i++) {
		DEBUG_ASSERT(handlers_[i] != nullptr, "Uninitialized opcode handler for opcode #{}", i);
	}
	LOG_INFO("Opcode table initialized");

#undef REGISTER
#undef REGISTER_NULL
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

void OpcodeTable::OpcodeHandler::operator()(WorldSession& session, WorldPacket& packet) const 
{
	if(fn) fn(session, packet);
}

const OpcodeTable::OpcodeHandler& OpcodeTable::Get(uint16_t opcode)
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
#define DECLARE_CASE(x) case Opcode::##x##: return #x;
	switch (opcode) {
		DECLARE_CASE(CMSG_MOVE);
		DECLARE_CASE(SMSG_UPDATE);
		DECLARE_CASE(MAX);
		default: return "NULL";
	}
#undef DECLARE_CASE
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

