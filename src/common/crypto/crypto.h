#pragma once

#include <vector>
#include "rc5.h"

/**
 * Calculates the MD5-MAC of a message.
 * Completely fills the output buffer by repeating the MAC digest.
 */
bool calcMD5MAC(const std::vector<unsigned char>& key, const std::vector<unsigned char>& msg, std::vector<unsigned char>& outBuf);

/**
 * Decrypts an RC5 message.
 */
bool decryptRC5(const CryptoPP::RC5::Decryption& decryptor, const std::vector<unsigned char>& msg, std::vector<unsigned char>& outBuf);

/**
 * Encrypts an RC5 message.
 */
bool encryptRC5(const CryptoPP::RC5::Encryption& encryptor, const std::vector<unsigned char>& msg, std::vector<unsigned char>& outBuf);
