#pragma once

#include <string>
#include <vector>

template<typename Iterator>
std::string strAscii(Iterator first, Iterator last) {
    std::string result;
    result.reserve(std::distance(first, last));

    for (; first != last; ++first) {
        result += (char)*first;
    }

    return result;
}

template<size_t arraySize>
std::string strAscii(const std::array<uint8_t, arraySize>& data) {
    return strAscii(data.begin(), data.end());
}

std::string strAscii(const std::vector<uint8_t>& data);

std::string strAscii(const uint8_t* data, size_t len);

template<typename Iterator>
std::string strHex(Iterator first, Iterator last) {
    std::string result;
    result.reserve(std::distance(first, last) * 3);

    char buf[4];
    for (; first != last; ++first) {
        sprintf(buf, " %02X", *first);
        result += buf;
    }

    return result;
}

template<size_t arraySize>
std::string strHex(const std::array<uint8_t, arraySize>& data) {
    return strHex(data.begin(), data.end());
}

std::string strHex(const std::vector<uint8_t>& data);

std::string strHex(const uint8_t* data, size_t len);
