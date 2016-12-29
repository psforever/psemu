#pragma once

#include "opcodes.h"
#include "common/bitstream.h"

class KeepAliveMessage {
public:
    uint16_t keepAliveCode;

    static KeepAliveMessage decode(BitStream& bitStream) {
        KeepAliveMessage packet;
        bitStream.read(&packet.keepAliveCode);
        return packet;
    }

    void encode(BitStream& bitStream) {
        uint8_t opcode = OP_KeepAliveMessage;
        bitStream.write(&opcode);

        bitStream.write(&keepAliveCode);
    }
};
