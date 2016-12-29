#pragma once

#include <array>
#include "opcodes.h"
#include "common/bitstream.h"

class ServerStart {
public:
    unsigned int clientNonce;
    unsigned int serverNonce;
    std::array<unsigned char, 11> unk0;

    void encode(BitStream& bitStream) {
        unsigned char opcode = 0x00;
        bitStream.write(&opcode);
        unsigned char controlOpcode = OP_ServerStart;
        bitStream.write(&controlOpcode);

        bitStream.write(&clientNonce);
        bitStream.write(&serverNonce);
        bitStream.write(&unk0);
    }
};
