#pragma once

#include <string>
#include "opcodes.h"
#include "common/bitstream.h"
#include "common/util.h"

class LoadMapMessage {
public:
    std::string mapName;
    std::string navMapName;
    uint16_t unk1;
    uint32_t unk2;
    bool weaponsUnlocked;
    uint32_t checksum;

    void encode(BitStream& bitStream) {
        uint8_t opcode = OP_LoadMapMessage;
        bitStream.write(opcode);

        bitStream.write(mapName);
        bitStream.write(navMapName);
        bitStream.write(unk1);
        bitStream.write(unk2);
        bitStream.writeBit(weaponsUnlocked);
        bitStream.write(checksum);
    }
};
