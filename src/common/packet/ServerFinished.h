#pragma once

#include <array>
#include "common/bitstream.h"

class ServerFinished {
public:
    unsigned short unk0;
    std::array<unsigned char, 12> challengeResult;

    void encode(BitStream& bitStream) {
        bitStream.write(&unk0);
        bitStream.write(&challengeResult);
    }
};
