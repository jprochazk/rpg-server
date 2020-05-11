
#pragma once
#ifndef SERVER_WORLD_OPCODE_H
#define SERVER_WORLD_OPCODE_H

#include "pch.h"

class WorldSession;
class WorldPacket;

enum Opcode : uint16_t {
//  OPCODE             LAYOUT
    CMSG_MOVE,      // <uint8_t input flags>
    SMSG_UPDATE,    // <uint16_t num entities> [<uint16_t id> <float x> <float y>]
    MAX             // NULL
};
constexpr uint16_t NUM_OPCODES = Opcode::MAX + 1;

class OpcodeTable {
public:
    typedef void(*HandlerFn)(WorldSession&, WorldPacket&);

    class OpcodeHandler {
        friend class OpcodeTable;
    public:
        OpcodeHandler(uint16_t, const char*, bool, HandlerFn);
        ~OpcodeHandler();
        bool IsAsync() const noexcept;
        void operator()(WorldSession& session, WorldPacket& packet) const;
    private:
        uint16_t    opcode;
        const char* name;
        bool        async;
        HandlerFn   fn;
    };

    static const OpcodeHandler& Get(uint16_t opcode);
    static std::string ToString(uint16_t opcode);

    OpcodeTable(OpcodeTable const&) = delete;
    void operator=(OpcodeTable const&) = delete;
    ~OpcodeTable();
private:
    static OpcodeTable& Instance();

    OpcodeTable();
    void RegisterHandler(uint16_t opcode, const char* name, bool async, HandlerFn fn);

    OpcodeHandler* handlers_[NUM_OPCODES];
};


#endif // SERVER_WORLD_OPCODE_H