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
    void generateCrypto1(unsigned int clientTime, const std::array<unsigned char, 12>& clientChallenge, CryptoPP::Integer p, CryptoPP::Integer g);

    /**
     * Generates the second stage of crypto values.
     */
    void generateCrypto2(const std::array<unsigned char, 16>& pubKey, const std::array<unsigned char, 12>& clientChallengeResult);

    /**
     * Decrypts packet data using pre-established crypto values.
     * Also checks for MAC match and removes MAC and padding.
     */
    bool decryptPacket(BitStream& bitStream, std::vector<unsigned char>& outBuf) const;

    /**
     * Encrypts packet data in-place using pre-established crypto values.
     * Also adds MAC and appropriate padding.
     */
    bool encryptPacket(std::vector<unsigned char>& data) const;

    asio::ip::udp::endpoint clientEndpoint;
    int cryptoState;

    std::vector<unsigned char> macBuffer;

    std::vector<unsigned char> serverChallengeResult;
    
    unsigned int storedClientTime;
    std::array<unsigned char, 12> storedClientChallenge;

    unsigned int storedServerTime;
    std::array<unsigned char, 12> storedServerChallenge;

    std::vector<unsigned char> serverPrivKey;
    std::vector<unsigned char> serverPubKey;

    std::vector<unsigned char> decMACKey;
    std::vector<unsigned char> encMACKey;

    size_t lastPokeMS;

private:
    CryptoPP::RC5::Decryption decRC5;
    CryptoPP::RC5::Encryption encRC5;
    
    CryptoPP::DH dh;
};
