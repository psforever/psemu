#pragma once

#include <array>
#include "common/bitstream.h"

class ServerFinished {
public:
    uint16_t unk0;
    std::array<uint8_t, 12> challengeResult;

    void encode(BitStream& bitStream) {
        bitStream.write(unk0);
        bitStream.write(challengeResult);
    }
};
