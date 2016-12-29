#pragma once

#include <array>
#include <string>
#include "opcodes.h"
#include "common/bitstream.h"
#include "common/util.h"

class LoginRespMessage {
public:
    std::array<unsigned char, 16> token;
    std::array<unsigned char, 16> unk0;
    unsigned int error;
    unsigned int stationError;
    unsigned int subscriptionStatus;
    unsigned int unk1;
    std::string username;
    unsigned int privilege;

    void encode(BitStream& bitStream) {
        unsigned char opcode = OP_LoginRespMessage;
        bitStream.write(&opcode);

        bitStream.writeBytes(token.data(), token.size());
        bitStream.writeBytes(unk0.data(), unk0.size());
        bitStream.write(&error);
        bitStream.write(&stationError);
        bitStream.write(&subscriptionStatus);
        bitStream.write(&unk1);
        writeString(bitStream, username);
        bitStream.write(&privilege);
        bitStream.writeBit(privilege & 1);
    }
};
