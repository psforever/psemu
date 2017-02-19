#pragma once

#define assertOpcode(bitStream, expectedOpcode) do {\
    uint8_t opcode;\
    bitStream.read(opcode);\
    assertEqual(opcode, expectedOpcode);\
} while (0)

#define assertControlOpcode(bitStream, expectedOpcode) do {\
    uint8_t opcode;\
    bitStream.read(opcode);\
    assertEqual(opcode, 0x00);\
    assertOpcode(bitStream, expectedOpcode);\
} while (0)

void testPacketCodingControl();
void testPacketCodingCrypto();
void testPacketCodingGame();
