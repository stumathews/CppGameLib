#ifndef CRC_H
#define CRC_H
#include <cstdint>
#include <cstddef>

class Crc32 {
public:
    Crc32() { generate_table(); }
    uint32_t compute(const uint8_t* data, size_t length) const {
        uint32_t crc = 0xFFFFFFFF;
        for (size_t i = 0; i < length; ++i) {
            uint8_t index = (crc ^ data[i]) & 0xFF;
            crc = (crc >> 8) ^ table[index];
        }
        return crc ^ 0xFFFFFFFF;
    }

private:
    uint32_t table[256];

    void generate_table() {
        const uint32_t polynomial = 0xEDB88320;
        for (uint32_t i = 0; i < 256; ++i) {
            uint32_t c = i;
            for (uint32_t j = 0; j < 8; ++j) {
                if (c & 1)
                    c = polynomial ^ (c >> 1);
                else
                    c = c >> 1;
            }
            table[i] = c;
        }
    }
};

#endif
