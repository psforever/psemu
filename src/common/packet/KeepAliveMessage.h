#pragma once

#include "opcodes.h"
#include "common/bitstream.h"

class KeepAliveMessage {
public:
    unsigned short keepAliveCode;

    static KeepAliveMessage decode(BitStream& bitStream) {
        KeepAliveMessage packet;
        bitStream.read(&packet.keepAliveCode);
        return packet;
    }

    void encode(BitStream& bitStream) {
        unsigned char opcode = OP_KeepAliveMessage;
        bitStream.write(&opcode);

        bitStream.write(&keepAliveCode);
    }
};
