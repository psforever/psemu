#pragma once

#include <string>
#include "opcodes.h"
#include "common/bitstream.h"
#include "common/util.h"

class ConnectToWorldMessage {
public:
    std::string serverName;
    std::string serverAddress;
    uint16_t serverPort;

    void encode(BitStream& bitStream) {
        uint8_t opcode = OP_ConnectToWorldMessage;
        bitStream.write(opcode);

        bitStream.write(serverName);
        bitStream.write(serverAddress);
        bitStream.write(serverPort);
    }
};
