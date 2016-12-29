#include <array>
#include <iostream>
#include <vector>
#include "crypto.h"
#include "md5mac.h"

bool calcMD5MAC(const std::vector<uint8_t>& key, const std::vector<uint8_t>& msg, std::vector<uint8_t>& outBuf) {
    std::array<byte, CryptoPP::MD5MAC::DIGESTSIZE> digest;

    if (key.size() < 16) {
        std::cout << "MD5MAC key too short!" << std::endl;
        return false;
    }

    CryptoPP::MD5MAC mac(key.data());
    mac.Update(msg.data(), msg.size());
    mac.Final(digest.data());

    for (size_t i = 0; i < outBuf.size(); i += CryptoPP::MD5MAC::DIGESTSIZE) {
        std::copy(digest.begin(), digest.begin() + std::min((size_t)CryptoPP::MD5MAC::DIGESTSIZE, outBuf.size() - i), outBuf.begin() + i);
    }
}

/**
 * Makes sure that the input and output buffers are valid for RC5 usage.
 */
bool checkRC5Buffers(const std::vector<uint8_t>& msg, const std::vector<uint8_t>& outBuf) {
    if (msg.size() % CryptoPP::RC5::BLOCKSIZE != 0) {
        std::cout << "RC5 content size must be a multiple of the RC5 block size!" << std::endl;
        return false;
    }

    if (outBuf.size() < msg.size()) {
        std::cout << "Not enough space in output buffer for RC5 decryption!" << std::endl;
        return false;
    }

    return true;
}

bool decryptRC5(const CryptoPP::RC5::Decryption& decryptor, const std::vector<uint8_t>& msg, std::vector<uint8_t>& outBuf) {
    if (!checkRC5Buffers(msg, outBuf)) {
        return false;
    }

    for (int i = 0; i < msg.size(); i += decryptor.BlockSize()) {
        decryptor.ProcessAndXorBlock(msg.data() + i, NULL, outBuf.data() + i);
    }

    return true;
}

bool encryptRC5(const CryptoPP::RC5::Encryption& encryptor, const std::vector<uint8_t>& msg, std::vector<uint8_t>& outBuf) {
    if (!checkRC5Buffers(msg, outBuf)) {
        return false;
    }

    for (int i = 0; i < msg.size(); i += encryptor.BlockSize()) {
        encryptor.ProcessAndXorBlock(msg.data() + i, NULL, outBuf.data() + i);
    }

    return true;
}
