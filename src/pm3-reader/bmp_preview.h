#ifndef BMP_PREVIEW_H
#define BMP_PREVIEW_H

#include <vector>
#include <string>
#include <iostream>
#include <cstdint>

// Структура для хранения заголовка BMP
#pragma pack(push, 1)
struct BMPHeader {
    uint16_t bfType{0x4D42};       // 'BM'
    uint32_t bfSize;               // Размер файла
    uint16_t bfReserved1{0};       // Зарезервировано
    uint16_t bfReserved2{0};       // Зарезервировано
    uint32_t bfOffBits{54};        // Смещение пикселей от начала файла
    uint32_t biSize{40};           // Размер структуры
    int32_t biWidth;               // Ширина изображения
    int32_t biHeight;              // Высота изображения
    uint16_t biPlanes{1};          // Число плоскостей
    uint16_t biBitCount{24};       // Количество бит на пиксель
    uint32_t biCompression{0};     // Сжатие
    uint32_t biSizeImage{0};       // Размер изображения
    int32_t biXPelsPerMeter{0};    // Горизонтальное разрешение
    int32_t biYPelsPerMeter{0};    // Вертикальное разрешение
    uint32_t biClrUsed{0};         // Число цветов
    uint32_t biClrImportant{0};    // Важные цвета
};
#pragma pack(pop)

// Функция преобразования RGB565 в RGB888
void RGB565toRGB888(uint16_t rgb565, uint8_t &r, uint8_t &g, uint8_t &b);

// Функция сохранения изображения в формате BMP
void saveBMP(const std::string &filename, int width, int height, const std::vector<uint8_t> &buffer);

#endif // BMP_PREVIEW_H
