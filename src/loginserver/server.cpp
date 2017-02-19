#include <iostream>
#include <memory>
#include <vector>
#include "common/enums.h"
#include "common/log.h"
#include "common/server.h"
#include "common/session.h"
#include "common/util.h"
#include "common/crypto/crypto.h"
#include "common/crypto/md5mac.h"
#include "common/packet/pkt_all.h"

// TODO: A bunch of this (mainly sending funcs and crypto/control packet handling) should be moved into common so it can be shared

uint16_t curSeqNum;
std::vector<uint8_t> sendBuf;

void handlePacket(Server& server, BitStream& bitStream, std::shared_ptr<Session> session);
void handleNormalPacket(Server& server, BitStream& bitStream, std::shared_ptr<Session> session);

void encodeHeaderCrypto(BitStream& bitStream) {
    PacketHeader header;
    header.packetType = PT_Crypto;
    header.unused = false;
    header.secured = false;
    header.advanced = true;
    header.lenSpecified = false;
    header.seqNum = 0;

    header.encode(bitStream);
}

void encodeHeaderEncrypted(BitStream& bitStream) {
    PacketHeader header;
    header.packetType = PT_Normal;
    header.unused = false;
    header.secured = true;
    header.advanced = true;
    header.lenSpecified = false;
    header.seqNum = 0;

    header.encode(bitStream);

    uint8_t paddingForEncryptAlign = 0x00;
    bitStream.write(paddingForEncryptAlign);
}

void encryptAndSend(Server& server, std::vector<uint8_t>& data, std::shared_ptr<Session> session) {
    // TODO: Need better flow for constructing encrypted packets to prevent copies: allow encryption to be applied
    // to a subset of a buffer so that the header can be written first, yet only packet content gets encrypted
    std::cout << "Sending encrypted (minus header+MAC+padding):" << strHex(data) << std::endl;

    if (!session->encryptPacket(data)) {
        return;
    }

    std::vector<uint8_t> sendBufFinal;
    BitStream sendStreamFinal(sendBufFinal);

    encodeHeaderEncrypted(sendStreamFinal);
    sendStreamFinal.write(data);

    std::cout << "Encrypted:" << strHex(sendBufFinal) << std::endl;

    server.send(sendBufFinal, session);
}

void handleCryptoPacket(Server& server, BitStream& bitStream, std::shared_ptr<Session> session) {
    std::cout << "---- CryptoPacket, state: ";

    // Note: No opcodes in crypto packets, so the state of the crypto exchange is tracked
    switch (session->cryptoState) {
    case Session::CS_Init: {
        std::cout << "OP_ClientChallengeXchg" << std::endl;

        session->macBuffer.insert(session->macBuffer.end(), bitStream.getHeadIterator(), bitStream.buf.end());

        ClientChallengeXchg clientChallengePacket = ClientChallengeXchg::decode(bitStream);

        if (bitStream.getLastError() != BitStream::Error::NONE) {
            std::cout << "Bitstream error reading packet! (" << static_cast<int>(bitStream.getLastError()) << ")" << std::endl;
            return;
        }

        CryptoPP::Integer p(clientChallengePacket.p.data(), clientChallengePacket.p.size());
        CryptoPP::Integer g(clientChallengePacket.g.data(), clientChallengePacket.g.size());

        session->generateCrypto1(clientChallengePacket.clientTime, clientChallengePacket.challenge, p, g);

        ServerChallengeXchg response;
        response.unk0 = 2;
        response.unk1 = 1;
        response.serverTime = session->storedServerTime;
        response.challenge = session->storedServerChallenge;
        response.unkChallengeEnd = 0;
        response.unkObjects = 1;
        response.unk2 = { 0x03, 0x07, 0x00, 0x00, 0x00, 0x0C, 0x00 };
        response.pubKeyLen = 16;
        std::copy(session->serverPubKey.begin(), session->serverPubKey.end(), response.pubKey.begin());
        response.unk3 = 14;

        sendBuf.clear();
        BitStream sendStream(sendBuf);
        encodeHeaderCrypto(sendStream);
        response.encode(sendStream);

        // +3 to skip header
        session->macBuffer.insert(session->macBuffer.end(), sendBuf.begin() + 3, sendBuf.end());

        std::cout << "Sending crypto:" << strHex(sendBuf) << std::endl;

        server.send(sendBuf, session);

        break;
    }
    case Session::CS_Challenge: {
        std::cout << "OP_ClientFinished" << std::endl;

        session->macBuffer.insert(session->macBuffer.end(), bitStream.getHeadIterator(), bitStream.buf.end());

        ClientFinished packet = ClientFinished::decode(bitStream);

        if (bitStream.getLastError() != BitStream::Error::NONE) {
            std::cout << "Bitstream error reading packet! (" << static_cast<int>(bitStream.getLastError()) << ")" << std::endl;
            return;
        }

        session->generateCrypto2(packet.pubKey, packet.challengeResult);

        ServerFinished response;
        response.unk0 = 0x1401;
        std::copy(session->serverChallengeResult.begin(), session->serverChallengeResult.end(), response.challengeResult.begin());

        sendBuf.clear();
        BitStream sendStream(sendBuf);
        encodeHeaderCrypto(sendStream);
        response.encode(sendStream);

        std::cout << "Sending crypto:" << strHex(sendBuf) << std::endl;

        server.send(sendBuf, session);

        break;
    }
    default: {
        std::cout << "Unknown " << session->cryptoState << std::endl;
        break;
    }
    }
}

void handleControlPacket(Server& server, BitStream& bitStream, std::shared_ptr<Session> session) {
    // Skip over the first byte - always zero in control packets
    bitStream.deltaPos(8 * sizeof(uint8_t));

    uint8_t opcode;
    bitStream.read(opcode);

    std::cout << "---- ControlPacket, op: " << std::hex << std::uppercase << (unsigned)opcode << std::dec << std::nouppercase << ", type: ";

    switch (opcode) {
    case OP_ClientStart: {
        std::cout << "OP_ClientStart" << std::endl;

        ClientStart packet = ClientStart::decode(bitStream);

        if (bitStream.getLastError() != BitStream::Error::NONE) {
            std::cout << "Bitstream error reading packet! (" << static_cast<int>(bitStream.getLastError()) << ")" << std::endl;
            return;
        }

        ServerStart response;
        response.clientNonce = packet.clientNonce;
        response.serverNonce = randomUnsignedInt();
        response.unk0 = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0xD3, 0x00, 0x00, 0x00, 0x02 };

        sendBuf.clear();
        BitStream sendStream(sendBuf);
        response.encode(sendStream);

        // This is a control packet, but no crypto established yet so send without header/crypto
        std::cout << "Sending raw:" << strHex(sendBuf) << std::endl;

        server.send(sendBuf, session);

        break;
    }
    case OP_ControlSync: {
        std::cout << "OP_ControlSync" << std::endl;

        ControlSync packet = ControlSync::decode(bitStream);

        if (bitStream.getLastError() != BitStream::Error::NONE) {
            std::cout << "Bitstream error reading packet! (" << static_cast<int>(bitStream.getLastError()) << ")" << std::endl;
            return;
        }

        ControlSyncResp response;
        response.timeDiff = packet.timeDiff;
        response.serverTick = getTimeNanoseconds();
        response.field1 = packet.field64A;
        response.field2 = packet.field64B;
        response.field3 = packet.field64B;
        response.field4 = packet.field64A;

        sendBuf.clear();
        BitStream sendStream(sendBuf);
        response.encode(sendStream);

        encryptAndSend(server, sendBuf, session);

        break;
    }
    case OP_SlottedMetaPacket0:
    case OP_SlottedMetaPacket1:
    case OP_SlottedMetaPacket2:
    case OP_SlottedMetaPacket3:
    case OP_SlottedMetaPacket4:
    case OP_SlottedMetaPacket5:
    case OP_SlottedMetaPacket6:
    case OP_SlottedMetaPacket7: {
        std::cout << "OP_SlottedMetaPacket" << std::endl;

        SlottedMetaPacket packet = SlottedMetaPacket::decode(bitStream, opcode - OP_SlottedMetaPacket0);

        if (bitStream.getLastError() != BitStream::Error::NONE) {
            std::cout << "Bitstream error reading packet! (" << static_cast<int>(bitStream.getLastError()) << ")" << std::endl;
            return;
        }

        // TODO: This needs to actually follow the slotting mechanism, and not just immediately process the inner packet

        // Send ack
        SlottedMetaAck response;
        response.slot = packet.slot;
        response.subslot = packet.subslot;

        sendBuf.clear();
        BitStream sendStream(sendBuf);
        response.encode(sendStream);

        encryptAndSend(server, sendBuf, session);

        // Handle the inner packet
        BitStream innerPacketBitStream(packet.rest);
        handleNormalPacket(server, innerPacketBitStream, session);

        break;
    }
    case OP_MultiPacket: {
        std::cout << "OP_MultiPacket" << std::endl;

        // Handle all inner packets
        uint8_t packetsSize;
        bitStream.read(packetsSize);

        size_t packetsBitSize = (size_t)packetsSize * 8;
        size_t initialPos = bitStream.getPos();
        while (bitStream.getRemainingBits() > 0 && bitStream.getPos() - initialPos < packetsBitSize) {
            handlePacket(server, bitStream, session);
        }

        break;
    }
    case OP_ConnectionClose: {
        std::cout << "OP_ConnectionClose" << std::endl;
        // TODO: Kill session
        break;
    }
    default: {
        std::cout << "Unknown" << std::endl;
        break;
    }
    }
}

void generateToken(const std::string username, const std::string password, std::array<uint8_t, 16>& token) {
    token = { 'T', 'H', 'I', 'S', 'I', 'S', 'M', 'Y', 'T', 'O', 'K', 'E', 'N', 'Y', 'E', 'S' };
}

void handleGamePacketLogin(Server& server, BitStream& bitStream, std::shared_ptr<Session> session) {
    uint8_t opcode;
    bitStream.read(opcode);

    std::cout << "---- GamePacket, op: " << std::hex << std::uppercase << (unsigned)opcode << std::dec << std::nouppercase << ", type: ";

    switch (opcode) {
    case OP_LoginMessage: {
        std::cout << "OP_LoginMessage" << std::endl;

        LoginMessage packet = LoginMessage::decode(bitStream);

        if (bitStream.getLastError() != BitStream::Error::NONE) {
            std::cout << "Bitstream error reading packet! (" << static_cast<int>(bitStream.getLastError()) << ")" << std::endl;
            return;
        }

        std::cout << "Connecting user with v" << packet.majorVersion << "." << packet.minorVersion << " built on " << packet.buildDate << " revision " << packet.revision << std::endl;
        std::cout << "Username: " << packet.username << ", Password: " << packet.password << " built on " << packet.buildDate << std::endl;

        LoginRespMessage response;
        generateToken(packet.username, packet.password, response.token);
        response.unk0 = { 0x00, 0x00, 0x00, 0x00, 0x18, 0xFA, 0xBE, 0x0C, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
        response.error = 0;
        response.stationError = 1;
        response.subscriptionStatus = 2;
        response.unk1 = 685276011;
        response.username = packet.username;
        response.privilege = 10001;

        sendBuf.clear();
        BitStream sendStream(sendBuf);
        response.encode(sendStream);

        encryptAndSend(server, sendBuf, session);

        // TODO: Add delay before sending world status packet?
        VNLWorldStatusMessage::WorldInfo world1;
        world1.name = "hello_c++";
        world1.status2 = VNLWorldStatusMessage::WS_Up;
        world1.serverType = VNLWorldStatusMessage::ST_Released;
        world1.status1 = VNLWorldStatusMessage::WS_Up;
        world1.empireNeed = EM_NC;

        VNLWorldStatusMessage response2;
        response2.welcomeMessage = L"ASDF";
        response2.worlds.push_back(world1);

        sendBuf.clear();
        BitStream sendStream2(sendBuf);
        response2.encode(sendStream2);

        encryptAndSend(server, sendBuf, session);

        break;
    }
    case OP_ConnectToWorldRequestMessage: {
        std::cout << "OP_ConnectToWorldRequestMessage" << std::endl;

        ConnectToWorldRequestMessage packet = ConnectToWorldRequestMessage::decode(bitStream);

        if (bitStream.getLastError() != BitStream::Error::NONE) {
            std::cout << "Bitstream error reading packet! (" << static_cast<int>(bitStream.getLastError()) << ")" << std::endl;
            return;
        }

        ConnectToWorldMessage response;
        response.serverName = "psemu";
        response.serverAddress = "127.0.0.1";
        response.serverPort = 51001;

        sendBuf.clear();
        BitStream sendStream(sendBuf);
        response.encode(sendStream);

        encryptAndSend(server, sendBuf, session);

        break;
    }
    default: {
        std::cout << "Unknown" << std::endl;
        break;
    }
    }
}

std::vector<uint8_t> objectHex = { 0x18, 0x57, 0x0C, 0x00, 0x00, 0xBC, 0x84, 0xB0, 0x06, 0xC2, 0xD7, 0x65, 0x53, 0x5C, 0xA1, 0x60, 0x00, 0x01, 0x34, 0x40, 0x00, 0x09, 0x70, 0x49, 0x00, 0x6C, 0x00, 0x6C, 0x00, 0x6C, 0x00, 0x49, 0x00, 0x49, 0x00, 0x49, 0x00, 0x6C, 0x00, 0x6C, 0x00, 0x6C, 0x00, 0x49, 0x00, 0x6C, 0x00, 0x49, 0x00, 0x6C, 0x00, 0x6C, 0x00, 0x49, 0x00, 0x6C, 0x00, 0x6C, 0x00, 0x6C, 0x00, 0x49, 0x00, 0x6C, 0x00, 0x6C, 0x00, 0x49, 0x00, 0x84, 0x52, 0x70, 0x76, 0x1E, 0x80, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3F, 0xFF, 0xC0, 0x00, 0x00, 0x00, 0x20, 0x00, 0x00, 0x0F, 0xF6, 0xA7, 0x03, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFD, 0x90, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x90, 0x01, 0x90, 0x00, 0x64, 0x00, 0x00, 0x01, 0x00, 0x7E, 0xC8, 0x00, 0xC8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0xC0, 0x00, 0x42, 0xC5, 0x46, 0x86, 0xC7, 0x00, 0x00, 0x00, 0x80, 0x00, 0x00, 0x12, 0x40, 0x78, 0x70, 0x65, 0x5F, 0x73, 0x61, 0x6E, 0x63, 0x74, 0x75, 0x61, 0x72, 0x79, 0x5F, 0x68, 0x65, 0x6C, 0x70, 0x90, 0x78, 0x70, 0x65, 0x5F, 0x74, 0x68, 0x5F, 0x66, 0x69, 0x72, 0x65, 0x6D, 0x6F, 0x64, 0x65, 0x73, 0x8B, 0x75, 0x73, 0x65, 0x64, 0x5F, 0x62, 0x65, 0x61, 0x6D, 0x65, 0x72, 0x85, 0x6D, 0x61, 0x70, 0x31, 0x33, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x0A, 0x23, 0x02, 0x60, 0x04, 0x04, 0x40, 0x00, 0x00, 0x10, 0x00, 0x06, 0x02, 0x08, 0x14, 0xD0, 0x08, 0x0C, 0x80, 0x00, 0x02, 0x00, 0x02, 0x6B, 0x4E, 0x00, 0x82, 0x88, 0x00, 0x00, 0x02, 0x00, 0x00, 0xC0, 0x41, 0xC0, 0x9E, 0x01, 0x01, 0x90, 0x00, 0x00, 0x64, 0x00, 0x44, 0x2A, 0x00, 0x10, 0x91, 0x00, 0x00, 0x00, 0x40, 0x00, 0x18, 0x08, 0x38, 0x94, 0x40, 0x20, 0x32, 0x00, 0x00, 0x00, 0x80, 0x19, 0x05, 0x48, 0x02, 0x17, 0x20, 0x00, 0x00, 0x08, 0x00, 0x70, 0x29, 0x80, 0x43, 0x64, 0x00, 0x00, 0x32, 0x00, 0x0E, 0x05, 0x40, 0x08, 0x9C, 0x80, 0x00, 0x06, 0x40, 0x01, 0xC0, 0xAA, 0x01, 0x19, 0x90, 0x00, 0x00, 0xC8, 0x00, 0x3A, 0x15, 0x80, 0x28, 0x72, 0x00, 0x00, 0x19, 0x00, 0x04, 0x0A, 0xB8, 0x05, 0x26, 0x40, 0x00, 0x03, 0x20, 0x06, 0xC2, 0x58, 0x00, 0xA7, 0x88, 0x00, 0x00, 0x02, 0x00, 0x00, 0x80, 0x00, 0x00 };

void handleGamePacketWorld(Server& server, BitStream& bitStream, std::shared_ptr<Session> session) {
    uint8_t opcode;
    bitStream.read(opcode);

    std::cout << "---- GamePacket, op: " << std::hex << std::uppercase << (unsigned)opcode << std::dec << std::nouppercase << ", type: ";

    switch (opcode) {
    case OP_KeepAliveMessage: {
        std::cout << "OP_KeepAliveMessage" << std::endl;

        KeepAliveMessage packet = KeepAliveMessage::decode(bitStream);

        if (bitStream.getLastError() != BitStream::Error::NONE) {
            std::cout << "Bitstream error reading packet! (" << static_cast<int>(bitStream.getLastError()) << ")" << std::endl;
            return;
        }

        KeepAliveMessage response;
        response.keepAliveCode = packet.keepAliveCode;

        sendBuf.clear();
        BitStream sendStream(sendBuf);
        response.encode(sendStream);

        encryptAndSend(server, sendBuf, session);

        break;
    }
    case OP_ConnectToWorldRequestMessage: {
        std::cout << "OP_ConnectToWorldRequestMessage" << std::endl;

        ConnectToWorldRequestMessage packet = ConnectToWorldRequestMessage::decode(bitStream);

        if (bitStream.getLastError() != BitStream::Error::NONE) {
            std::cout << "Bitstream error reading packet! (" << static_cast<int>(bitStream.getLastError()) << ")" << std::endl;
            return;
        }

        std::vector<uint8_t> objectHexCopy = objectHex;
        encryptAndSend(server, objectHexCopy, session);

        std::vector<uint8_t> hardcodedStuff = { 0x14, 0x0F, 0x00, 0x00, 0x00, 0x10, 0x27, 0x00, 0x00, 0xC1, 0xD8, 0x7A, 0x02, 0x4B, 0x00, 0x26, 0x5C, 0xB0, 0x80, 0x00 };
        encryptAndSend(server, hardcodedStuff, session);

        CharacterInfoMessage response;
        response.unknown = 0;
        response.zoneId = 1;
        response.charId = 0;
        response.charGUID = 0;
        response.finished = true;
        response.secondsSinceLastLogin = 0;

        sendBuf.clear();
        BitStream sendStream(sendBuf);
        response.encode(sendStream);

        encryptAndSend(server, sendBuf, session);

        break;
    }
    case OP_CharacterRequestMessage: {
        std::cout << "OP_CharacterRequestMessage" << std::endl;

        CharacterRequestMessage packet = CharacterRequestMessage::decode(bitStream);

        if (bitStream.getLastError() != BitStream::Error::NONE) {
            std::cout << "Bitstream error reading packet! (" << static_cast<int>(bitStream.getLastError()) << ")" << std::endl;
            return;
        }

        switch (packet.action) {
        case CharacterRequestMessage::CRA_Select: {
            LoadMapMessage loadMapResponse;
            loadMapResponse.mapName = "map13";
            loadMapResponse.navMapName = "home3";
            loadMapResponse.unk1 = 40100;
            loadMapResponse.unk2 = 25;
            loadMapResponse.weaponsUnlocked = true;
            loadMapResponse.checksum = 3770441820;

            sendBuf.clear();
            BitStream loadMapSendStream(sendBuf);
            loadMapResponse.encode(loadMapSendStream);

            encryptAndSend(server, sendBuf, session);

            std::vector<uint8_t> objectHexCopy = objectHex;
            encryptAndSend(server, objectHexCopy, session);

            BitStream objectHexBitStream(objectHex);
            // Get rid of the opcode
            objectHexBitStream.deltaPos(8 * sizeof(uint8_t));
            ObjectCreateMessage objectHexDecoded = ObjectCreateMessage::decode(objectHexBitStream);

            SetCurrentAvatarMessage setCurAvatarResponse;
            setCurAvatarResponse.guid = objectHexDecoded.guid;
            setCurAvatarResponse.unk1 = 0;
            setCurAvatarResponse.unk2 = 0;

            sendBuf.clear();
            BitStream setCurAvatarSendStream(sendBuf);
            setCurAvatarResponse.encode(setCurAvatarSendStream);

            encryptAndSend(server, sendBuf, session);

            break;
        }
        default: {
            std::cout << "Unhandled character action " << packet.action << std::endl;
            break;
        }
        }

        break;
    }
    default: {
        std::cout << "Unknown" << std::endl;
        break;
    }
    }
}

void handleGamePacket(Server& server, BitStream& bitStream, std::shared_ptr<Session> session) {
    if (server.getPort() == 51000) {
        handleGamePacketLogin(server, bitStream, session);
    } else {
        handleGamePacketWorld(server, bitStream, session);
    }
}

void handleNormalPacket(Server& server, BitStream& bitStream, std::shared_ptr<Session> session) {
    uint8_t controlPacketType;
    bitStream.read(controlPacketType, true);

    if (controlPacketType == 0x00) {
        handleControlPacket(server, bitStream, session);
    } else {
        handleGamePacket(server, bitStream, session);
    }
}

void handleEncryptedPacket(Server& server, BitStream& bitStream, std::shared_ptr<Session> session) {
    std::vector<uint8_t> plaintext;
    session->decryptPacket(bitStream, plaintext);

    BitStream plaintextBitStream(plaintext);
    handleNormalPacket(server, plaintextBitStream, session);
}

void handleNonControlPacket(Server& server, BitStream& bitStream, std::shared_ptr<Session> session) {
    PacketHeader header = PacketHeader::decode(bitStream);

    // Encrypted packets must be 4-byte aligned
    if (header.secured) {
        bitStream.deltaPos(8 * sizeof(uint8_t));
    }

    if (bitStream.getLastError() != BitStream::Error::NONE) {
        std::cout << "Bitstream error reading packet header! (" << static_cast<int>(bitStream.getLastError()) << ")" << std::endl;
        return;
    }

    curSeqNum = header.seqNum;

    switch (header.packetType) {
    case PT_Crypto:
        handleCryptoPacket(server, bitStream, session);
        break;
    case PT_Normal:
        handleEncryptedPacket(server, bitStream, session);
        break;
    default:
        std::cout << "Unhandled packet type " << header.packetType << "!" << std::endl;
        // Go to end of stream for now so that we don't try anything else with the packet (such as reading more if this is a MultiPacket)
        bitStream.deltaPos(bitStream.getRemainingBits());
        break;
    }
}

void handlePacket(Server& server, BitStream& bitStream, std::shared_ptr<Session> session) {
    uint8_t controlPacketType;
    bitStream.read(controlPacketType, true);

    if (controlPacketType == 0x00) {
        handleControlPacket(server, bitStream, session);
    } else {
        handleNonControlPacket(server, bitStream, session);
    }
}

void serverRecvHandler(Server& server, std::vector<uint8_t>& data, std::shared_ptr<Session> session) {
    std::cout << (server.getPort() == 51000 ? "LOGIN: " : "WORLD: ") << "Received packet of " << data.size() << " bytes" << std::endl;

    std::cout << "ASCII: " << strAscii(data) << std::endl;
    std::cout << "HEX:" << strHex(data) << std::endl;

    BitStream bitStream(data);
    handlePacket(server, bitStream, session);

    std::cout << std::endl;
}

void keepSessionsAlive(Server& server) {
    auto& sessions = server.getSessionMap();
    for (auto& sessionEntry : sessions) {
        auto& session = sessionEntry.second;

        if (session->cryptoState == Session::CS_Finished) {
            size_t curTimeMS = getTimeMilliseconds();
            if (curTimeMS - session->lastPokeMS > 500) {
                session->lastPokeMS = curTimeMS;

                std::cout << "ClientPoke:" << std::endl;

                KeepAliveMessage response;
                response.keepAliveCode = 0;

                sendBuf.clear();
                BitStream sendStream(sendBuf);
                response.encode(sendStream);

                encryptAndSend(server, sendBuf, session);

                std::cout << std::endl;
            }
        }
    }
}
