#pragma once

#include <cstdlib>
#include <string>
#include "bitstream.h"

unsigned int randomUnsignedInt();
unsigned char randomUnsignedChar();
std::size_t getTimeSeconds();
std::size_t getTimeMilliseconds();
std::size_t getTimeNanoseconds();

void readString(BitStream& bitStream, std::string& str);
void readString(BitStream& bitStream, std::wstring& str);
void writeString(BitStream& bitStream, const std::string str);
void writeString(BitStream& bitStream, const std::wstring str);

void utilSleep(size_t ms);
