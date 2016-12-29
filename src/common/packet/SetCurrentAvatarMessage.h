#pragma once

#include "opcodes.h"
#include "common/bitstream.h"

class SetCurrentAvatarMessage {
public:
    unsigned short guid;
    unsigned char unk1;
    unsigned char unk2;

    void encode(BitStream& bitStream) {
        unsigned char opcode = OP_SetCurrentAvatarMessage;
        bitStream.write(&opcode);

        bitStream.write(&guid);
        bitStream.writeBits(&unk1, 3);
        bitStream.writeBits(&unk2, 3);
    }
};
