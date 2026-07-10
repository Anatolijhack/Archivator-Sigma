#pragma once
#include <fstream>
class CompressControl
{
public:
    virtual void write(const char* data, size_t size) = 0;
    virtual void flush() = 0;
    virtual ~CompressControl() = default;
};

