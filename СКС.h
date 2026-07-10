#pragma once
#include "Compress.h"
static inline uint32_t DoCRC32(const void* data, size_t length, uint32_t crc)
{
    const uint8_t* p = static_cast<const uint8_t*>(data);

    for (size_t i = 0; i < length; i++)
    {
        crc ^= p[i];

        for (int j = 0; j < 8; j++)
        {
            if (crc & 1)
                crc = (crc >> 1) ^ 0xEDB88320;
            else
                crc >>= 1;
        }
    }

    return crc;

}

class CRCStream : public CompressControl
{
    CompressControl* next;
    uint32_t crc = 0xFFFFFFFF;

public:
    CRCStream(CompressControl* next);

    void write(const char* data, size_t size) override;
    void flush() override;

    uint32_t getCRC() const;
};