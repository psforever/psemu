#include <vector>
#include "pkt_all.h"
#include "pkt_test.h"
#include "common/log.h"

void testControlSync() {
    uint8_t expectedOpcode = OP_ControlSync;
    static std::vector<uint8_t> encodedBuf = std::vector<uint8_t>({
        0x00, expectedOpcode, 0x52, 0x68, 0x00, 0x00, 0x00, 0x4D, 0x00, 0x00, 0x00, 0x52, 0x00, 0x00, 0x00, 0x4D,
        0x00, 0x00, 0x00, 0x7C, 0x00, 0x00, 0x00, 0x4D, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x76,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x75
    });

    // Decode
    // TODO: Are these actually big endian?
    BitStream decodeBitStream(encodedBuf);
    assertControlOpcode(decodeBitStream, expectedOpcode);
    ControlSync decodePacket = ControlSync::decode(decodeBitStream);
    assertEqual(decodePacket.timeDiff, 0x6852);
    assertEqual(decodePacket.unk, 0x4D000000);
    assertEqual(decodePacket.field1, 0x52000000);
    assertEqual(decodePacket.field2, 0x4D000000);
    assertEqual(decodePacket.field3, 0x7C000000);
    assertEqual(decodePacket.field4, 0x4D000000);
    assertEqual(decodePacket.field64A, 0x7602000000000000);
    assertEqual(decodePacket.field64B, 0x7502000000000000);
}

void testControlSyncResp() {
    uint8_t expectedOpcode = OP_ControlSyncResp;
    static std::vector<uint8_t> encodedBuf = std::vector<uint8_t>({
        0x00, expectedOpcode, 0x52, 0x68, 0x21, 0x39, 0x2D, 0x92, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x76,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x75, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x75,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x76
    });

    // Encode
    // TODO: Are these actually big endian?
    ControlSyncResp encodePacket;
    encodePacket.timeDiff = 0x6852;
    encodePacket.serverTick = 0x922D3921;
    encodePacket.field1 = 0x7602000000000000;
    encodePacket.field2 = 0x7502000000000000;
    encodePacket.field3 = 0x7502000000000000;
    encodePacket.field4 = 0x7602000000000000;

    std::vector<uint8_t> testEncodingBuf;
    encodePacket.encode(BitStream(testEncodingBuf));
    assertBuffersEqual(testEncodingBuf, encodedBuf);
}

void testClientStart() {
    uint8_t expectedOpcode = OP_ClientStart;
    static std::vector<uint8_t> encodedBuf = std::vector<uint8_t>({
        0x00, expectedOpcode, 0x00, 0x00, 0x00, 0x02, 0x00, 0x26, 0x1E, 0x27, 0x00, 0x00, 0x01, 0xF0
    });

    // Decode
    BitStream decodeBitStream(encodedBuf);
    assertControlOpcode(decodeBitStream, expectedOpcode);
    ClientStart decodePacket = ClientStart::decode(decodeBitStream);
    assertEqual(decodePacket.unk0, 0x02000000);
    assertEqual(decodePacket.clientNonce, 0x271E2600);
    assertEqual(decodePacket.unk1, 0xF0010000);
}

void testServerStart() {
    uint8_t expectedOpcode = OP_ServerStart;
    static std::vector<uint8_t> encodedBuf = std::vector<uint8_t>({
        0x00, expectedOpcode, 0x00, 0x26, 0x1E, 0x27, 0x51, 0xBD, 0xC1, 0xCE, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01,
        0xD3, 0x00, 0x00, 0x00, 0x02
    });

    // Encode
    ServerStart encodePacket;
    encodePacket.clientNonce = 0x271E2600;
    encodePacket.serverNonce = 0xCEC1BD51;
    encodePacket.unk0 = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0xD3, 0x00, 0x00, 0x00, 0x02 };

    std::vector<uint8_t> testEncodingBuf;
    encodePacket.encode(BitStream(testEncodingBuf));
    assertBuffersEqual(testEncodingBuf, encodedBuf);
}

void testSlottedMetaAck() {
    uint8_t expectedSlot = 2;
    uint8_t expectedOpcode = OP_RelatedB0 + expectedSlot % 4;
    static std::vector<uint8_t> encodedBuf = std::vector<uint8_t>({
        0x00, expectedOpcode, 0x34, 0x12
    });

    // Encode
    SlottedMetaAck encodePacket;
    encodePacket.slot = expectedSlot;
    encodePacket.subslot = 0x1234;

    std::vector<uint8_t> testEncodingBuf;
    encodePacket.encode(BitStream(testEncodingBuf));
    assertBuffersEqual(testEncodingBuf, encodedBuf);
}

void testPacketCodingControl() {
    testControlSync();
    testControlSyncResp();
    testClientStart();
    testServerStart();
    testSlottedMetaAck();
    // TODO: Test SlottedMetaPacket
}
