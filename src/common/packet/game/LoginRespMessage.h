#pragma once

#include <array>
#include <string>
#include "opcodes.h"
#include "common/bitstream.h"
#include "common/util.h"

class LoginRespMessage {
public:
    std::array<uint8_t, 16> token;
    std::array<uint8_t, 16> unk0;
    uint32_t error;
    uint32_t stationError;
    uint32_t subscriptionStatus;
    uint32_t unk1;
    std::string username;
    uint32_t privilege;

    void encode(BitStream& bitStream) {
        uint8_t opcode = OP_LoginRespMessage;
        bitStream.write(opcode);

        bitStream.write(token);
        bitStream.write(unk0);
        bitStream.write(error);
        bitStream.write(stationError);
        bitStream.write(subscriptionStatus);
        bitStream.write(unk1);
        bitStream.write(username);
        bitStream.write(privilege);
        bitStream.writeBit(privilege & 1);
    }
};
