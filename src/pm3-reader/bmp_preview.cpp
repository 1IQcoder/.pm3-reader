#include "bmp_preview.h"
#include <fstream>
#include <iostream>
#include <cstdint>
#include <vector>
using namespace std;

// Функция преобразования RGB565 в RGB888
void RGB565toRGB888(uint16_t rgb565, uint8_t &r, uint8_t &g, uint8_t &b) {
    r = ((rgb565 >> 11) & 0x1F) << 3;
    g = ((rgb565 >> 5) & 0x3F) << 2;
    b = (rgb565 & 0x1F) << 3;
}

// Функция сохранения изображения в формате BMP
void saveBMP(const string &filename, int width, int height, const vector<uint8_t> &buffer) {
    // Вектор для хранения 24-битных (RGB888) пикселей
    std::vector<uint8_t> pixelData;
    pixelData.reserve(width * height * 3);

    // Конвертируем пиксели из RGB565 в RGB888
    for (size_t i = 0; i < buffer.size(); i += 2) {
        uint16_t pixel = (buffer[i+1] << 8) | buffer[i];
        uint8_t r, g, b;
        RGB565toRGB888(pixel, r, g, b);
        pixelData.push_back(b);  // BMP хранит цвет в порядке BGR
        pixelData.push_back(g);
        pixelData.push_back(r);
    }

    BMPHeader header;
    header.biWidth = width;
    header.biHeight = height;
    header.bfSize = sizeof(BMPHeader) + pixelData.size();
    header.biSizeImage = pixelData.size();
    
    ofstream file(filename, ios::binary);
    if (!file) {
        cerr << "Ошибка при открытии файла для записи\n";
        return;
    }

    // Запись заголовка
    file.write(reinterpret_cast<const char*>(&header), sizeof(header));

    // Пиксели хранятся построчно. Нужно инвертировать порядок строк, чтобы исправить ориентацию
    int rowSize = width * 3; // 3 байта на пиксель (RGB888)
    for (int row = height - 1; row >= 0; --row) {
        file.write(reinterpret_cast<const char*>(&pixelData[row * rowSize]), rowSize);
    }

    file.close();
}

