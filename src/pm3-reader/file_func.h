#ifndef FILE_FUNC_H
#define FILE_FUNC_H

#include <string>
#include <vector>
#include "bmp_preview.h"
using namespace std;


struct bytesPos {
    int start;
    int width;
    int end;
};


int getFileSize(const string& filepath);
vector<uint8_t> getBytesFromFile(const string& filepath, streampos start, streamsize size);

template<typename T>
vector<uint8_t> toByteVector(const T& data);
template<>
vector<uint8_t> toByteVector<string>(const string& data);


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
};


// поиск значений в файле
template<typename T>
int findLastByteOf(const string& filepath, const T& findData)
{
    vector<uint8_t> targetBytes = toByteVector(findData);
    size_t targetSize = targetBytes.size();
    size_t fileSize = getFileSize(filepath);
    
    size_t blockI = 1;
    short blockSize = 4096;
    vector<uint8_t> buffer = getBytesFromFile(filepath, 0, blockSize);

    short c = 0;
    size_t i = 0;
    for (size_t g = 0; g < fileSize; ++g) {

        if (buffer[i] == targetBytes[c]) {
            ++c;
            if (c == targetSize) {
                return static_cast<int>(g);
            }
        } else {
            c = 0;
        }

        if (i == blockSize-1) {
            buffer = getBytesFromFile(filepath, blockSize*blockI, blockSize);
            blockI++;
            i = 0;
        } else {
            ++i;
        }
    }

    return -1;
};

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


class pm3File {
    private:
        size_t filesize;
        string filepath;
    
        vector<uint8_t> getBytesVector(const string& base, size_t start, short size)
        {
            size_t newStart = 0;
            if (base == "HEADER") {
                newStart = this->HEADER+start;
            }  else if (base == "EXTRA") {
                newStart = this->EXTRA+start;
            } else if (base == "MACHINE") {
                newStart = this->MACHINE+start;
            } else {
                newStart = start;
            }
            return getBytesFromFile(this->filepath, newStart, size);
        }

    public:
        // позиция последнего байта заголовка
        size_t HEADER;
        size_t EXTRA;
        size_t MACHINE;


        pm3File(const string& path)
        {
            this->filepath = path;
            string find = "HEADER";
            this->HEADER = findLastByteOf(this->filepath, find);
            find = "EXTRA";
            this->EXTRA = findLastByteOf(this->filepath, find);
            find = "MACHINE";
            this->MACHINE = findLastByteOf(this->filepath, find);
        }


        template<typename T>
        T getValueFrom(const string& base, size_t start, size_t size, bool isBigEndian)
        {
            vector<uint8_t> buffer = this->getBytesVector(base, start, size);
            return joinBytes<T>(buffer, isBigEndian);
        }


        template<typename T>
        string getStringOfValueFrom(const string& base, size_t start, size_t size, bool isBigEndian)
        {
            T value = this->getValueFrom<T>(base, start, size, isBigEndian);
            string res(to_string(value));
            short dotPos = res.find('.');
            if (dotPos != string::npos && dotPos+3 < res.length()) {
                res.erase(res.find('.')+3);
            }
            
            return res;
        }


        void savePreview(const string& filename)
        {
            string find = "PREVIEW";
            size_t res = findLastByteOf(this->filepath, find);
            if (res < 0) {
                return;
            }

            vector<uint8_t> marginBytes = getBytesFromFile(this->filepath, res+5, 4);
            int margin = joinBytes<int>(marginBytes, false);

            vector<uint8_t> previewBytes = getBytesFromFile(this->filepath, res+19, margin);
            saveBMP(filename, 224, 120, previewBytes);
        }
};


#endif // FILE_FUNC_H




