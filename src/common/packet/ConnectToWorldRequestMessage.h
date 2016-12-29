#pragma once

#include <array>
#include <string>
#include "common/bitstream.h"
#include "common/util.h"

class ConnectToWorldRequestMessage {
public:
    std::string serverName;
    std::array<unsigned char, 32> token;
    unsigned int majorVersion;
    unsigned int minorVersion;
    unsigned int revision;
    std::string buildDate;
    unsigned short unk0;
    
    static ConnectToWorldRequestMessage decode(BitStream& bitStream) {
        ConnectToWorldRequestMessage packet;
        readString(bitStream, packet.serverName);
        bitStream.read(&packet.token);
        bitStream.read(&packet.majorVersion);
        bitStream.read(&packet.minorVersion);
        bitStream.read(&packet.revision);
        readString(bitStream, packet.buildDate);
        bitStream.read(&packet.unk0);
        return packet;
    }
};
