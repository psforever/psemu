#pragma once

#include <array>
#include <string>
#include "common/bitstream.h"
#include "common/util.h"

class LoginMessage {
public:
    unsigned int majorVersion;
    unsigned int minorVersion;
    std::string buildDate;
    std::string username;
    std::string password;
    std::array<unsigned char, 32> token;
    unsigned int revision;

    static LoginMessage decode(BitStream& bitStream) {
        LoginMessage packet;
        bitStream.read(&packet.majorVersion);
        bitStream.read(&packet.minorVersion);
        readString(bitStream, packet.buildDate);
        if (!bitStream.readBit()) {
            readString(bitStream, packet.username);
            readString(bitStream, packet.password);
        } else {
            // TODO: Fix this. Not sure if it's supposed to be padded or aligned or what.
            bitStream.read(&packet.token);
            readString(bitStream, packet.username);
        }
        bitStream.read(&packet.revision);
        return packet;
    }
};
