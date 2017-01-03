#include <chrono>
#include <random>
#include <string>
#include <thread>
#include "bitstream.h"
#include "util.h"

uint32_t randomUnsignedInt() {
    std::random_device randomDevice;
    std::mt19937_64 generator(randomDevice());
    std::uniform_int_distribution<uint32_t> distribution;

    return distribution(generator);
}

uint8_t randomUnsignedChar() {
    std::random_device randomDevice;
    std::mt19937_64 generator(randomDevice());
    std::uniform_int_distribution<uint32_t> distribution(0, UCHAR_MAX);

    return distribution(generator);
}

std::size_t getTimeSeconds() {
    std::chrono::system_clock::time_point now = std::chrono::system_clock::now();
    std::chrono::system_clock::duration tp = now.time_since_epoch();
    return std::chrono::duration_cast<std::chrono::seconds>(tp).count();
}

std::size_t getTimeMilliseconds() {
    std::chrono::system_clock::time_point now = std::chrono::system_clock::now();
    std::chrono::system_clock::duration tp = now.time_since_epoch();
    return std::chrono::duration_cast<std::chrono::milliseconds>(tp).count();
}

std::size_t getTimeNanoseconds() {
    std::chrono::system_clock::time_point now = std::chrono::system_clock::now();
    std::chrono::system_clock::duration tp = now.time_since_epoch();
    return std::chrono::duration_cast<std::chrono::nanoseconds>(tp).count();
}

void utilSleep(size_t ms) {
    std::this_thread::sleep_for(std::chrono::milliseconds(ms));
}
