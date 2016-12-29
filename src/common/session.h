#pragma once

#include <array>
#include <vector>
#include "asio.hpp"
#include "bitstream.h"
#include "dh.h"
#include "rc5.h"

/**
 * Represents a session between a server and a client.
 */
class Session {
public:
    enum CryptoState {
        CS_Init,
        CS_Challenge,
        CS_Finished
    };

    Session(asio::ip::udp::endpoint clientEndpoint) :
        clientEndpoint(clientEndpoint),
        cryptoState(CS_Init) {

    }

    /**
     * Generates the first stage of crypto values.
     */
    void generateCrypto1(uint32_t clientTime, const std::array<uint8_t, 12>& clientChallenge, const CryptoPP::Integer& p, const CryptoPP::Integer& g);

    /**
     * Generates the second stage of crypto values.
     */
    void generateCrypto2(const std::array<uint8_t, 16>& pubKey, const std::array<uint8_t, 12>& clientChallengeResult);

    /**
     * Decrypts packet data using pre-established crypto values.
     * Also checks for MAC match and removes MAC and padding.
     */
    bool decryptPacket(BitStream& bitStream, std::vector<uint8_t>& outBuf) const;

    /**
     * Encrypts packet data in-place using pre-established crypto values.
     * Also adds MAC and appropriate padding.
     */
    bool encryptPacket(std::vector<uint8_t>& data) const;

    asio::ip::udp::endpoint clientEndpoint;
    int cryptoState;

    std::vector<uint8_t> macBuffer;

    std::vector<uint8_t> serverChallengeResult;
    
    uint32_t storedClientTime;
    std::array<uint8_t, 12> storedClientChallenge;

    uint32_t storedServerTime;
    std::array<uint8_t, 12> storedServerChallenge;

    std::vector<uint8_t> serverPrivKey;
    std::vector<uint8_t> serverPubKey;

    std::vector<uint8_t> decMACKey;
    std::vector<uint8_t> encMACKey;

    size_t lastPokeMS;

private:
    CryptoPP::RC5::Decryption decRC5;
    CryptoPP::RC5::Encryption encRC5;
    
    CryptoPP::DH dh;
};
