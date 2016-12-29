#pragma once

#include <vector>
#include "rc5.h"

/**
 * Calculates the MD5-MAC of a message.
 * Completely fills the output buffer by repeating the MAC digest.
 */
bool calcMD5MAC(const std::vector<uint8_t>& key, const std::vector<uint8_t>& msg, std::vector<uint8_t>& outBuf);

/**
 * Decrypts an RC5 message.
 */
bool decryptRC5(const CryptoPP::RC5::Decryption& decryptor, const std::vector<uint8_t>& msg, std::vector<uint8_t>& outBuf);

/**
 * Encrypts an RC5 message.
 */
bool encryptRC5(const CryptoPP::RC5::Encryption& encryptor, const std::vector<uint8_t>& msg, std::vector<uint8_t>& outBuf);
