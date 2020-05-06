
#pragma once
#ifndef SERVER_WORLD_OPCODE_H
#define SERVER_WORLD_OPCODE_H

#include "pch.h"

class WorldSession;
class WorldPacket;
class World;

enum Opcode : uint16_t {
//  OPCODE  N     LAYOUT
    STEST = 0, // <uint32_t timestamp> <string date>
    CTEST = 1, // <uint16_t sequence>
    MAX   = 2  // NULL
};
constexpr uint16_t NUM_OPCODES = Opcode::MAX + 1;

/**
 * To add a new opcode: 
 * - Add it to the Opcode enum, increment MAX by one
 * - Register it in OpcodeTable constructor
 */
class OpcodeTable {
public:
    typedef void(*HandlerFn)(WorldSession*, WorldPacket&);

    class OpcodeHandler {
        friend class OpcodeTable;
    public:
        OpcodeHandler(uint16_t, const char*, bool, HandlerFn);
        ~OpcodeHandler();
        bool IsAsync() const noexcept;
        void operator()(WorldSession* session, WorldPacket& packet) const;
    private:
        uint16_t    opcode;
        const char* name;
        bool        async;
        HandlerFn   fn;
    };

    static const OpcodeHandler& GetHandler(uint16_t opcode);
    static std::string ToString(uint16_t opcode);
    static bool IsAsync(uint16_t opcode);

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