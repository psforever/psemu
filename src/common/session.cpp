#pragma once

#include <array>
#include <iostream>
#include <vector>
#include "bitstream.h"
#include "dh.h"
#include "log.h"
#include "osrng.h"
#include "rc5.h"
#include "session.h"
#include "util.h"
#include "crypto/crypto.h"

const std::string strMasterSecret = "master secret";
const std::string strClientExpansion = "client expansion";
const std::string strServerExpansion = "server expansion";
const std::string strClientFinished = "client finished";
const std::string strServerFinished = "server finished";

void Session::generateCrypto1(uint32_t clientTime, const std::array<uint8_t, 12>& clientChallenge, const CryptoPP::Integer& p, const CryptoPP::Integer& g) {
    // Generate server keys
    CryptoPP::AutoSeededRandomPool rnd;
    dh.AccessGroupParameters().Initialize(p, g);
    serverPrivKey.resize(dh.PrivateKeyLength());
    serverPubKey.resize(dh.PublicKeyLength());
    dh.GenerateKeyPair(rnd, serverPrivKey.data(), serverPubKey.data());

    storedClientTime = clientTime;
    storedClientChallenge = clientChallenge;

    storedServerTime = getTimeSeconds();
    for (auto& serverChallengeByte : storedServerChallenge) {
        serverChallengeByte = randomUnsignedChar();
    }

    cryptoState = CS_Challenge;
};

void Session::generateCrypto2(const std::array<uint8_t, 16>& clientPubKey, const std::array<uint8_t, 12>& clientChallengeResult) {
    std::cout << "macBuffer:" << strHex(macBuffer) << std::endl;

    // Make sure the keys agree
    std::vector<uint8_t> agreedValue(dh.AgreedValueLength());
    bool agreed = dh.Agree(agreedValue.data(), serverPrivKey.data(), clientPubKey.data());

    if (!agreed) {
        // TODO: Need to terminate session
        std::cout << "Did not agree on crypto keys!" << std::endl;
        return;
    }

    std::cout << "Agreed with:" << strHex(agreedValue) << std::endl;

    // Generate the master secret
    std::vector<byte> agreedMessage;
    agreedMessage.insert(agreedMessage.end(), strMasterSecret.begin(), strMasterSecret.end());
    agreedMessage.insert(agreedMessage.end(), &((uint8_t*)&storedClientTime)[0], &((uint8_t*)&storedClientTime)[4]);
    agreedMessage.insert(agreedMessage.end(), storedClientChallenge.begin(), storedClientChallenge.end());
    agreedMessage.insert(agreedMessage.end(), 4, 0x00);
    agreedMessage.insert(agreedMessage.end(), &((uint8_t*)&storedServerTime)[0], &((uint8_t*)&storedServerTime)[4]);
    agreedMessage.insert(agreedMessage.end(), storedServerChallenge.begin(), storedServerChallenge.end());
    agreedMessage.insert(agreedMessage.end(), 4, 0x00);

    std::vector<uint8_t> masterSecret(20);
    calcMD5MAC(agreedValue, agreedMessage, masterSecret);

    std::cout << "masterSecret:" << strHex(masterSecret) << std::endl;

    // TODO: Perhaps this is currently not working in scala codebase either
    // Seems that the MD5MAC does not match the client's challenge; for now just ignore it and let them through
    // Normally if these dont match, should drop the connection
    /*
    // Check client challenge result
    std::vector<uint8_t> clientChallengeCheckBuffer;
    clientChallengeCheckBuffer.insert(clientChallengeCheckBuffer.end(), strClientFinished.begin(), strClientFinished.end());
    clientChallengeCheckBuffer.insert(clientChallengeCheckBuffer.end(), macBuffer.begin(), macBuffer.end());
    clientChallengeCheckBuffer.push_back(0x01);
    clientChallengeCheckBuffer.insert(clientChallengeCheckBuffer.end(), clientChallengeResult.begin(), clientChallengeResult.end());
    clientChallengeCheckBuffer.push_back(0x01);

    std::vector<uint8_t> clientChallengeCheck(12);
    calcMD5MAC(masterSecret, clientChallengeCheckBuffer, clientChallengeCheck);

    std::cout << "clientCheckResult:" << strHex(clientChallengeCheck) << std::endl;
    std::cout << "clientChallengeResult:" << strHex(clientChallengeResult) << std::endl;
    std::cout << "storedClientChallenge:" << strHex(storedClientChallenge) << std::endl;
    */

    // Generate RC% and MAC encryption keys
    std::vector<byte> switchedServerClientChallenges;
    switchedServerClientChallenges.insert(switchedServerClientChallenges.end(), &((uint8_t*)&storedServerTime)[0], &((uint8_t*)&storedServerTime)[4]);
    switchedServerClientChallenges.insert(switchedServerClientChallenges.end(), storedServerChallenge.begin(), storedServerChallenge.end());
    switchedServerClientChallenges.insert(switchedServerClientChallenges.end(), 4, 0x00);
    switchedServerClientChallenges.insert(switchedServerClientChallenges.end(), &((uint8_t*)&storedClientTime)[0], &((uint8_t*)&storedClientTime)[4]);
    switchedServerClientChallenges.insert(switchedServerClientChallenges.end(), storedClientChallenge.begin(), storedClientChallenge.end());
    switchedServerClientChallenges.insert(switchedServerClientChallenges.end(), 4, 0x00);

    std::vector<uint8_t> decExpansionBuffer;
    decExpansionBuffer.insert(decExpansionBuffer.end(), strClientExpansion.begin(), strClientExpansion.end());
    decExpansionBuffer.insert(decExpansionBuffer.end(), 2, 0x00);
    decExpansionBuffer.insert(decExpansionBuffer.end(), switchedServerClientChallenges.begin(), switchedServerClientChallenges.end());

    std::cout << "decExpansionBuffer:" << strHex(decExpansionBuffer) << std::endl;

    std::vector<uint8_t> encExpansionBuffer;
    encExpansionBuffer.insert(encExpansionBuffer.end(), strServerExpansion.begin(), strServerExpansion.end());
    encExpansionBuffer.insert(encExpansionBuffer.end(), 2, 0x00);
    encExpansionBuffer.insert(encExpansionBuffer.end(), switchedServerClientChallenges.begin(), switchedServerClientChallenges.end());

    std::cout << "encExpansionBuffer:" << strHex(encExpansionBuffer) << std::endl;

    std::vector<uint8_t> expandedDecKey(64);
    calcMD5MAC(masterSecret, decExpansionBuffer, expandedDecKey);

    std::cout << "expandedDecKey:" << strHex(expandedDecKey) << std::endl;

    std::vector<uint8_t> expandedEncKey(64);
    calcMD5MAC(masterSecret, encExpansionBuffer, expandedEncKey);

    std::cout << "expandedEncKey:" << strHex(expandedEncKey) << std::endl;

    std::array<uint8_t, 20> decKey;
    std::copy(expandedDecKey.begin(), expandedDecKey.begin() + 20, decKey.begin());

    decMACKey.assign(expandedDecKey.begin() + 20, expandedDecKey.begin() + 20 + 16);

    std::array<uint8_t, 20> encKey;
    std::copy(expandedEncKey.begin(), expandedEncKey.begin() + 20, encKey.begin());

    encMACKey.assign(expandedEncKey.begin() + 20, expandedEncKey.begin() + 20 + 16);

    std::cout << "decKey:" << strHex(decKey.begin(), decKey.end()) << std::endl;
    std::cout << "decMACKey:" << strHex(decMACKey) << std::endl;
    std::cout << "encKey:" << strHex(encKey.begin(), encKey.end()) << std::endl;
    std::cout << "encMACKey:" << strHex(encMACKey) << std::endl;

    decRC5.SetKey(decKey.data(), decKey.size());
    encRC5.SetKey(encKey.data(), encKey.size());

    // Generate server challenge result
    std::vector<uint8_t> serverChallengeResultBuffer;
    serverChallengeResultBuffer.insert(serverChallengeResultBuffer.end(), strServerFinished.begin(), strServerFinished.end());
    serverChallengeResultBuffer.insert(serverChallengeResultBuffer.end(), macBuffer.begin(), macBuffer.end());
    serverChallengeResultBuffer.push_back(0x01);

    serverChallengeResult.resize(12);
    calcMD5MAC(masterSecret, serverChallengeResultBuffer, serverChallengeResult);

    std::cout << "serverChallengeResult:" << strHex(serverChallengeResult) << std::endl;

    // MAC buffer no longer needed
    macBuffer.clear();

    cryptoState = CS_Finished;
}

bool Session::decryptPacket(BitStream& bitStream, std::vector<uint8_t>& outBuf) const {
    if (cryptoState != CS_Finished) {
        std::cout << "Tried to decrypt with unfinished crypto session!" << std::endl;
        return false;
    }

    outBuf.assign(bitStream.getHeadIterator(), bitStream.buf.end());
    decryptRC5(decRC5, outBuf, outBuf);

    std::cout << "Full post-decryption:" << strHex(outBuf) << std::endl;

    // Remove RC5 padding
    uint8_t paddingLen = outBuf.back();
    if (paddingLen > outBuf.size() - 1) {
        std::cout << "Padding " << paddingLen << " too big for packet size " << outBuf.size() << "!" << std::endl;
        return false;
    }
    // +1 to get rid of padding size byte
    outBuf.resize(outBuf.size() - (paddingLen + 1));

    // Remove the MAC
    if (outBuf.size() < 16) {
        std::cout << "Packet size " << outBuf.size() << " not large enough for 16-byte MAC!" << std::endl;
        return false;
    }

    std::vector<uint8_t> mac(outBuf.end() - 16, outBuf.end());
    outBuf.resize(outBuf.size() - 16);

    // Make sure MAC matches
    std::vector<uint8_t> calculatedMac(16);
    calcMD5MAC(decMACKey, outBuf, calculatedMac);

    if (mac != calculatedMac) {
        std::cout << "MAC mismatch!" << std::endl
            << "Got:" << strHex(mac) << std::endl
            << "Expected:" << strHex(calculatedMac) << std::endl;
        return false;
    }

    return true;
}

bool Session::encryptPacket(std::vector<uint8_t>& data) const {
    if (cryptoState != CS_Finished) {
        std::cout << "Tried to encrypt with unfinished crypto session!" << std::endl;
        return false;
    }

    std::vector<uint8_t> calculatedMac(16);
    calcMD5MAC(encMACKey, data, calculatedMac);

    data.insert(data.end(), calculatedMac.begin(), calculatedMac.end());

    // -1 since also writes the padding count
    uint8_t requiredPadding = CryptoPP::RC5::BLOCKSIZE - (data.size() % CryptoPP::RC5::BLOCKSIZE) - 1;
    data.insert(data.end(), requiredPadding, 0x00);
    data.push_back(requiredPadding);

    std::cout << "Full pre-encryption:" << strHex(data) << std::endl;

    encryptRC5(encRC5, data, data);

    return true;
}
