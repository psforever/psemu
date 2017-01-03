#pragma once

#include <array>
#include "opcodes.h"
#include "common/bitstream.h"

class ServerStart {
public:
    uint32_t clientNonce;
    uint32_t serverNonce;
    std::array<uint8_t, 11> unk0;

    void encode(BitStream& bitStream) {
        uint8_t opcode = 0x00;
        bitStream.write(opcode);
        uint8_t controlOpcode = OP_ServerStart;
        bitStream.write(controlOpcode);

        bitStream.write(clientNonce);
        bitStream.write(serverNonce);
        bitStream.write(unk0);
    }
};
