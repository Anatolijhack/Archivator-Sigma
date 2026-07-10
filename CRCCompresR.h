#pragma once
#include "Compress.h"
class CRCCheckStream : public CompressControl 
{
    CompressControl* next;
    uint32_t crc = 0xFFFFFFFF;

public:
    CRCCheckStream(CompressControl* next);

    void write(const char* data, size_t size) override;
    void flush() override;

    uint32_t get() const;
};