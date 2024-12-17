#include <string>
#include <vector>
#include <cstdint>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <type_traits>
#include "bytes_func.h"
using namespace std;


// вывод байтов
template<typename T>
void printBytes(vector<uint8_t>& array)
{
    if constexpr (is_same_v<T, int>) {
        for (uint8_t symbol : array) {
            cout << hex << setw(2) << setfill('0') << static_cast<int>(symbol) << " ";
        }
    } else if constexpr (is_same_v<T, char>)
    {
        for (uint8_t symbol : array) {
            cout << hex << static_cast<char>(symbol) << " ";
        }
    } else {
        cerr << "cant print bytes in this type" << endl;
    }
    cout << dec << endl;
}

template<typename T>
void printBytes(T data)
{
    cout << "hex: " << hex << static_cast<T>(data) << endl;
    cout << "dec: " << dec << static_cast<T>(data) << endl;
    cout << "char: " << hex << static_cast<char>(data) << endl;
    cout << dec << endl;
}


// получение вектора байтов из файла
vector<uint8_t> getByteFromFile(string& filepath, streampos start, streamsize numBytes)
{
    ifstream file(filepath, ios::binary);
    vector<uint8_t> buffer(numBytes);
    if (file) {
        file.seekg(start);
        file.read(reinterpret_cast<char*>(buffer.data()), numBytes);
    }
    return buffer;
}


vector<uint8_t> getByteFromVector(const vector<uint8_t>& buffer, size_t start, size_t size)
{
    if (start >= buffer.size()) {
        throw std::out_of_range("Начальная позиция выходит за пределы вектора.");
    }

    size_t end = min(start + size, buffer.size());

    vector<uint8_t> result(buffer.begin() + start, buffer.begin() + end);
    return result;
}


// конвертиция в вектор байтов
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


template<typename T>
vector<bytesPos> findBytes(vector<uint8_t> buffer, T& findData)
{
    int resStart = -1;
    vector<bytesPos> result;
    vector<uint8_t> targetBytes = toByteVector(findData);
    int targetSize = targetBytes.size();
    
    short c = 0;
    for (int i = 0; i < buffer.size(); i++) {
        if (buffer[i] == targetBytes[c]) {
            c++;
            if (c == targetSize) {
                resStart = i - targetSize + 1;
                result.push_back({resStart, targetSize, resStart+targetSize-1});
                c = 0;
            }
        } else {
            c = 0;
        }
    }
    
    return result;
}
// шаблоны для компилятора
template std::vector<bytesPos> findBytes<int>(std::vector<uint8_t> buffer, int& findData);
template std::vector<bytesPos> findBytes<float>(std::vector<uint8_t> buffer, float& findData);
template std::vector<bytesPos> findBytes<std::string>(std::vector<uint8_t> buffer, std::string& findData);
template std::vector<bytesPos> findBytes<std::vector<uint8_t>>(std::vector<uint8_t> buffer, std::vector<uint8_t>& findData);


void writeBytesToTXT(vector<uint8_t> buffer)
{
    std::ofstream outputFile("output.txt");

    if (outputFile.is_open()) {
        for (size_t i = 0; i < buffer.size(); ++i) {
            outputFile << std::hex << std::setw(2) << std::setfill('0') 
                       << static_cast<int>(buffer[i]);

            if (i != buffer.size() - 1) {
                outputFile << " ";
            }
        }

        outputFile.close();
        std::cout << "Данные успешно записаны в файл в hex формате." << std::endl;
    } else {
        std::cerr << "Ошибка при открытии файла!" << std::endl;
    }
}