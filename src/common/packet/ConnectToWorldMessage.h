#pragma once

#include <string>
#include "opcodes.h"
#include "common/bitstream.h"
#include "common/util.h"

class ConnectToWorldMessage {
public:
    std::string serverName;
    std::string serverAddress;
    unsigned short serverPort;

    void encode(BitStream& bitStream) {
        unsigned char opcode = OP_ConnectToWorldMessage;
        bitStream.write(&opcode);

        writeString(bitStream, serverName);
        writeString(bitStream, serverAddress);
        bitStream.write(&serverPort);
    }
};
