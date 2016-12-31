#include <vector>
#include "pkt_all.h"
#include "pkt_test.h"
#include "common/log.h"

void testCharacterInfoMessage() {
    uint8_t expectedOpcode = OP_CharacterInfoMessage;
    // TODO: Replace with actual packet data
    static std::vector<uint8_t> encodedBuf = std::vector<uint8_t>({
        expectedOpcode, 0x21, 0xA5, 0x75, 0xB7, 0x45, 0x34, 0x23, 0x12, 0x32, 0x54, 0x76, 0x98, 0x34, 0x12, 0x8C, 0xB3, 0xC1, 0xA9, 0x80
    });

    // Encode
    CharacterInfoMessage encodePacket;
    encodePacket.unknown = 0xB775A521;
    encodePacket.zoneId = 0x12233445;
    encodePacket.charId = 0x98765432;
    encodePacket.charGUID = 0x1234;
    encodePacket.finished = true;
    encodePacket.secondsSinceLastLogin = 0x53836719;

    std::vector<uint8_t> testEncodingBuf;
    encodePacket.encode(BitStream(testEncodingBuf));
    assertBuffersEqual(testEncodingBuf, encodedBuf);
}

void testCharacterRequestMessage() {
    uint8_t expectedOpcode = OP_CharacterRequestMessage;
    // TODO: Replace with actual packet data
    static std::vector<uint8_t> encodedBuf = std::vector<uint8_t>({
        expectedOpcode, 0x45, 0x34, 0x23, 0x12, 0x32, 0x54, 0x76, 0x98
    });

    // Decode
    BitStream decodeBitStream(encodedBuf);
    assertOpcode(decodeBitStream, expectedOpcode);
    CharacterRequestMessage decodePacket = CharacterRequestMessage::decode(decodeBitStream);
    assertEqual(decodePacket.charId, 0x12233445);
    assertEqual(decodePacket.action, 0x98765432);
}

void testConnectToWorldMessage() {
    uint8_t expectedOpcode = OP_ConnectToWorldMessage;
    static std::vector<uint8_t> encodedBuf = std::vector<uint8_t>({
        expectedOpcode, 0x86, 0x67, 0x65, 0x6D, 0x69, 0x6E, 0x69, 0x8C, 0x36, 0x34, 0x2E, 0x33, 0x37, 0x2E, 0x31,
        0x35, 0x38, 0x2E, 0x36, 0x39, 0x3C, 0x75
    });

    // Encode
    ConnectToWorldMessage encodePacket;
    encodePacket.serverName = "gemini";
    encodePacket.serverAddress = "64.37.158.69";
    encodePacket.serverPort = 30012;

    std::vector<uint8_t> testEncodingBuf;
    encodePacket.encode(BitStream(testEncodingBuf));
    assertBuffersEqual(testEncodingBuf, encodedBuf);
}

void testConnectToWorldRequestMessage() {
    uint8_t expectedOpcode = OP_ConnectToWorldRequestMessage;
    // TODO: Doesnt seem like a very good test case...
    static std::vector<uint8_t> encodedBuf = std::vector<uint8_t>({
        expectedOpcode, 0x86, 0x67, 0x65, 0x6D, 0x69, 0x6E, 0x69, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x80, 0x00, 0x00
    });

    // Decode
    BitStream decodeBitStream(encodedBuf);
    assertOpcode(decodeBitStream, expectedOpcode);
    ConnectToWorldRequestMessage decodePacket = ConnectToWorldRequestMessage::decode(decodeBitStream);
    assertEqual(decodePacket.serverName, "gemini");
    static std::array<uint8_t, 32> expectedToken = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
    assertBuffersEqual(decodePacket.token, expectedToken);
    assertEqual(decodePacket.majorVersion, 0x00000000);
    assertEqual(decodePacket.minorVersion, 0x00000000);
    assertEqual(decodePacket.revision, 0x00000000);
    assertEqual(decodePacket.buildDate, "");
    assertEqual(decodePacket.unk0, 0);
}

void testKeepAliveMessage() {
    uint8_t expectedOpcode = OP_KeepAliveMessage;
    static std::vector<uint8_t> encodedBuf = std::vector<uint8_t>({
        expectedOpcode, 0x34, 0x12
    });

    // Encode
    KeepAliveMessage encodePacket;
    encodePacket.keepAliveCode = 0x1234;

    std::vector<uint8_t> testEncodingBuf;
    encodePacket.encode(BitStream(testEncodingBuf));
    assertBuffersEqual(testEncodingBuf, encodedBuf);

    // Decode
    BitStream decodeBitStream(encodedBuf);
    assertOpcode(decodeBitStream, expectedOpcode);
    KeepAliveMessage decodePacket = KeepAliveMessage::decode(decodeBitStream);
    assertEqual(decodePacket.keepAliveCode, 0x1234);
}

void testLoadMapMessage() {
    uint8_t expectedOpcode = OP_LoadMapMessage;
    static std::vector<uint8_t> encodedBuf = std::vector<uint8_t>({
        expectedOpcode, 0x85, 0x6D, 0x61, 0x70, 0x31, 0x30, 0x83, 0x7A, 0x31, 0x30, 0x0F, 0xA0, 0x19, 0x00, 0x00,
        0x00, 0xF6, 0xF1, 0x60, 0x86, 0x80
    });

    // Encode
    LoadMapMessage encodePacket;
    encodePacket.mapName = "map10";
    encodePacket.navMapName = "z10";
    encodePacket.unk1 = 40975;
    encodePacket.unk2 = 25;
    encodePacket.weaponsUnlocked = true;
    encodePacket.checksum = 230810349;

    std::vector<uint8_t> testEncodingBuf;
    encodePacket.encode(BitStream(testEncodingBuf));
    assertBuffersEqual(testEncodingBuf, encodedBuf);
}

void testLoginMessage() {
    uint8_t expectedOpcode = OP_LoginMessage;
    static std::vector<uint8_t> encodedBuf = std::vector<uint8_t>({
        expectedOpcode, 0x03, 0x00, 0x00, 0x00, 0x0F, 0x00, 0x00, 0x00, 0x8B, 0x44, 0x65, 0x63, 0x20, 0x20, 0x32,
        0x20, 0x32, 0x30, 0x30, 0x39, 0x42, 0x00, 0x61, 0x73, 0x64, 0x66, 0x84, 0x31, 0x32, 0x33, 0x34,
        0x54, 0x00, 0x00, 0x00
    });

    // Decode
    BitStream decodeBitStream(encodedBuf);
    assertOpcode(decodeBitStream, expectedOpcode);
    LoginMessage decodePacket = LoginMessage::decode(decodeBitStream);
    assertEqual(decodePacket.majorVersion, 3);
    assertEqual(decodePacket.minorVersion, 15);
    assertEqual(decodePacket.buildDate, "Dec  2 2009");
    assertEqual(decodePacket.credentialsType, LoginMessage::CT_UserPassword);
    assertEqual(decodePacket.username, "asdf");
    assertEqual(decodePacket.password, "1234");
    assertEqual(decodePacket.revision, 84);

    // TODO: Test decode with CT_UserToken
}

void testLoginRespMessage() {
    uint8_t expectedOpcode = OP_LoginRespMessage;
    static std::vector<uint8_t> encodedBuf = std::vector<uint8_t>({
        expectedOpcode, 0x54, 0x48, 0x49, 0x53, 0x49, 0x53, 0x4D, 0x59, 0x54, 0x4F, 0x4B, 0x45, 0x4E, 0x59, 0x45,
        0x53, 0x00, 0x00, 0x00, 0x00, 0x18, 0xFA, 0xBE, 0x0C, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x6B, 0x7B, 0xD8,
        0x28, 0x84, 0x61, 0x73, 0x64, 0x66, 0x11, 0x27, 0x00, 0x00, 0x80
    });

    // Encode
    LoginRespMessage encodePacket;
    encodePacket.token = { 'T', 'H', 'I', 'S', 'I', 'S', 'M', 'Y', 'T', 'O', 'K', 'E', 'N', 'Y', 'E', 'S' };
    encodePacket.unk0 = { 0x00, 0x00, 0x00, 0x00, 0x18, 0xFA, 0xBE, 0x0C, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
    encodePacket.error = 0;
    encodePacket.stationError = 1;
    encodePacket.subscriptionStatus = 2;
    encodePacket.unk1 = 685276011;
    encodePacket.username = "asdf";
    encodePacket.privilege = 10001;

    std::vector<uint8_t> testEncodingBuf;
    encodePacket.encode(BitStream(testEncodingBuf));
    assertBuffersEqual(testEncodingBuf, encodedBuf);
}

void testSetCurrentAvatarMessage() {
    uint8_t expectedOpcode = OP_SetCurrentAvatarMessage;
    // TODO: Doesnt seem like a very good test case...
    static std::vector<uint8_t> encodedBuf = std::vector<uint8_t>({
        expectedOpcode, 0x4B, 0x00, 0x00
    });

    // Encode
    SetCurrentAvatarMessage encodePacket;
    encodePacket.guid = 75;
    encodePacket.unk1 = 0;
    encodePacket.unk2 = 0;

    std::vector<uint8_t> testEncodingBuf;
    encodePacket.encode(BitStream(testEncodingBuf));
    assertBuffersEqual(testEncodingBuf, encodedBuf);
}

void testPacketCodingGame() {
    testCharacterInfoMessage();
    testCharacterRequestMessage();
    testConnectToWorldMessage();
    testConnectToWorldRequestMessage();
    testKeepAliveMessage();
    testLoadMapMessage();
    testLoginMessage();
    testLoginRespMessage();
    // TODO: Test ObjectCreateMessage
    testSetCurrentAvatarMessage();
    // TODO: Test VNLWorldStatusMessage
}
