#pragma once

#include <string>
#include "opcodes.h"
#include "common/bitstream.h"
#include "common/util.h"

class LoadMapMessage {
public:
    std::string mapName;
    std::string navMapName;
    unsigned short unk1;
    unsigned int unk2;
    bool weaponsUnlocked;
    unsigned int checksum;

    void encode(BitStream& bitStream) {
        unsigned char opcode = OP_LoadMapMessage;
        bitStream.write(&opcode);

        writeString(bitStream, mapName);
        writeString(bitStream, navMapName);
        bitStream.write(&unk1);
        bitStream.write(&unk2);
        bitStream.writeBit(weaponsUnlocked);
        bitStream.write(&checksum);
    }
};
