#pragma once

#include <array>
#include "common/bitstream.h"

class ServerChallengeXchg {
public:
    unsigned char unk0;
    unsigned char unk1;
    unsigned int serverTime;
    std::array<unsigned char, 12> challenge;
    unsigned char unkChallengeEnd;
    unsigned char unkObjects;
    std::array<unsigned char, 7> unk2;
    unsigned short pubKeyLen;
    std::array<unsigned char, 16> pubKey;
    unsigned char unk3;

    void encode(BitStream& bitStream) {
        bitStream.write(&unk0);
        bitStream.write(&unk1);
        bitStream.write(&serverTime);
        bitStream.write(&challenge);
        bitStream.write(&unkChallengeEnd);
        bitStream.write(&unkObjects);
        bitStream.write(&unk2);
        bitStream.write(&pubKeyLen);
        bitStream.write(&pubKey);
        bitStream.write(&unk3);
    }
};
