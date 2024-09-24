#include "file_func.h"
#include <string>
#include <fstream>
#include <vector>

using namespace std;


// получение размеров файла в байтах
int getFileSize(const string& filepath)
{
    ifstream file(filepath.c_str(), ios::binary | ios::ate);
    return static_cast<int>(file.tellg());;
}


template<typename T>
vector<uint8_t> toByteVector(const T& data)
{
    vector<uint8_t> bytes(sizeof(T));
    memcpy(bytes.data(), &data, sizeof(T));
    return bytes;
}
template<>
vector<uint8_t> toByteVector<string>(const string& data)
{
    vector<uint8_t> bytes(data.begin(), data.end());
    return bytes;
}


vector<uint8_t> getBytesFromFile(const string& filepath, streampos start, streamsize size)
{
    ifstream file(filepath, ios::binary);
    vector<uint8_t> buffer(size);
    if (file) {
        file.seekg(start);
        file.read(reinterpret_cast<char*>(buffer.data()), size);
    }
    return buffer;
}




