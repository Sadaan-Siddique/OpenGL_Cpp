#include <iostream>
#include <fstream>
#include <vector>
#include <cstdint>

#pragma pack(push, 1) // Ensure no padding between struct members
struct BMPHeader {
    uint16_t type;          // "BM" magic number
    uint32_t size;          // File size
    uint16_t reserved1, reserved2;
    uint32_t offset;        // Offset to pixel data
    uint32_t header_size;   // Size of info header
    int32_t  width;
    int32_t  height;
    uint16_t planes;
    uint16_t bpp;           // Bits per pixel (must be 24 for this code)
    uint32_t compression;
    uint32_t img_size;
    int32_t  x_ppm, y_ppm;
    uint32_t colors_used, colors_important;
};
#pragma pack(pop)

unsigned char* loadBMP(const char* filename, int& width, int& height) {
    std::ifstream file(filename, std::ios::binary);
    if (!file) return nullptr;

    BMPHeader header;
    file.read(reinterpret_cast<char*>(&header), sizeof(header));

    if (header.type != 0x4D42 || header.bpp != 24) return nullptr;

    width = header.width;
    height = std::abs(header.height);

    // BMP rows must be multiples of 4 bytes
    int rowStride = (width * 3 + 3) & ~3; 
    std::vector<unsigned char> rowBuffer(rowStride);
    unsigned char* pixels = new unsigned char[width * height * 3];

    file.seekg(header.offset, std::ios::beg);

    for (int y = 0; y < height; ++y) {
        file.read(reinterpret_cast<char*>(rowBuffer.data()), rowStride);
        for (int x = 0; x < width; ++x) {
            // BMP stores pixels as BGR, convert to RGB
            int outIdx = (y * width + x) * 3;
            pixels[outIdx + 0] = rowBuffer[x * 3 + 2]; // Red
            pixels[outIdx + 1] = rowBuffer[x * 3 + 1]; // Green
            pixels[outIdx + 2] = rowBuffer[x * 3 + 0]; // Blue
        }
    }
    return pixels;
}

#include <iostream>

void printAsASCII(unsigned char* pixels, int width, int height) {
    // Standard ASCII characters from darkest to lightest
    const char* asciiScale = "@%#*+=-:. ";
    int scaleLen = 10;

    for (int y = height - 1; y >= 0; y -= 2) { // y-=2 because console chars are tall
        for (int x = 0; x < width; x++) {
            int idx = (y * width + x) * 3;
            
            // 1. Get RGB
            int r = pixels[idx + 0];
            int g = pixels[idx + 1];
            int b = pixels[idx + 2];

            // 2. Calculate Brightness (0 to 255)
            int brightness = (r + g + b) / 3;

            // 3. Map brightness to an ASCII character
            int charIdx = (brightness * (scaleLen - 1)) / 255;
            std::cout << asciiScale[charIdx];
        }
        std::cout << "\n";
    }
}


int main() {
    int w, h;
    unsigned char* data = loadBMP("blue-bmp-16-bit.bmp", w, h);
    printAsASCII(data, w, h);
    if (data) {
        std::cout << "Loaded: " << w << "x" << h << std::endl;
        delete[] data;
    }
    return 0;
}
