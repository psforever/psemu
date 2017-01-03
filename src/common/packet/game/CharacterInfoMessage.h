#pragma once

#include "opcodes.h"
#include "common/bitstream.h"

class CharacterInfoMessage {
public:
    uint32_t unknown;
    uint32_t zoneId;
    uint32_t charId;
    uint16_t charGUID;
    bool finished;
    uint32_t secondsSinceLastLogin;

    void encode(BitStream& bitStream) {
        uint8_t opcode = OP_CharacterInfoMessage;
        bitStream.write(opcode);

        bitStream.write(unknown);
        bitStream.write(zoneId);
        bitStream.write(charId);
        bitStream.write(charGUID);
        bitStream.writeBit(finished);
        bitStream.write(secondsSinceLastLogin);
    }
};
