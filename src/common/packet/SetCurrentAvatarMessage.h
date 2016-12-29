#pragma once

#include "opcodes.h"
#include "common/bitstream.h"

class SetCurrentAvatarMessage {
public:
    uint16_t guid;
    uint8_t unk1;
    uint8_t unk2;

    void encode(BitStream& bitStream) {
        uint8_t opcode = OP_SetCurrentAvatarMessage;
        bitStream.write(&opcode);

        bitStream.write(&guid);
        bitStream.writeBits(&unk1, 3);
        bitStream.writeBits(&unk2, 3);
    }
};
