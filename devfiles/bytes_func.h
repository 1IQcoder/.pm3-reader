#ifndef BYTES_FUNC_H
#define BYTES_FUNC_H

#include <string>
#include <vector>
#include <cstdint>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <type_traits>
using namespace std;


struct bytesPos {
    int start;
    int width;
    int end;
};

template<typename T>
void printBytes(T data);
template<typename T> 
void printBytes(vector<uint8_t>& array);

vector<uint8_t> getByteFromFile(string& filepath, streampos start, streamsize numBytes);
vector<uint8_t> getByteFromVector(const vector<uint8_t>& buffer, size_t start, size_t size);

// склеивание байтов в нужный тип
template<typename T>
T joinBytes(const vector<uint8_t>& bytes, bool isBigEndian)
{
    if constexpr (is_same_v<T, float>) {
        // float
        union {
            uint8_t byteArray[4];
            float floatValue;
        } converter;

        if (!isBigEndian) {
            for (int i = 0; i < 4; ++i) {
                converter.byteArray[i] = bytes[i];
            }
        } else {
            for (int i = 0; i < 4; ++i) {
                converter.byteArray[i] = bytes[3 - i];
            }
        }
        return converter.floatValue;
    } else {
        // int, string
        size_t length = bytes.size();
        short byteCount = sizeof(int);
        T res = 0;
        short byteStep = 8;
        short left = 0;
        if (isBigEndian) {
            left = 8*byteCount-8;
            byteStep *= -1;
        }
        for (short i=0; i<length; i++) {
            res += (bytes[i] << left);
            left += byteStep;
        }
        return res;
    }
}


template<typename T>
vector<uint8_t> toByteVector(const T& data);
template<>
vector<uint8_t> toByteVector<string>(const string& data);

template<typename T>
vector<bytesPos> findBytes(vector<uint8_t> buffer, T& findData);

void writeBytesToTXT(vector<uint8_t> buffer);

#endif  // BYTES_FUNC_H