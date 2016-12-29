#pragma once

#include "opcodes.h"
#include "common/bitstream.h"

class CharacterInfoMessage {
public:
    unsigned int unknown;
    unsigned int zoneId;
    unsigned int charId;
    unsigned short charGUID;
    bool finished;
    unsigned int secondsSinceLastLogin;

    void encode(BitStream& bitStream) {
        unsigned char opcode = OP_CharacterInfoMessage;
        bitStream.write(&opcode);

        bitStream.write(&unknown);
        bitStream.write(&zoneId);
        bitStream.write(&charId);
        bitStream.write(&charGUID);
        bitStream.writeBit(finished);
        bitStream.write(&secondsSinceLastLogin);
    }
};
