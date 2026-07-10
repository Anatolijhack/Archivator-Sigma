#include "CRCCompresR.h"
#include "ัสั.h"

CRCCheckStream::CRCCheckStream(CompressControl* next)
    : next(next) {}

void CRCCheckStream::write(const char* data, size_t size) {
    crc = DoCRC32(data, size, crc);
    if (next)
        next->write(data, size);
}

void CRCCheckStream::flush() {
    if (next)
        next->flush();
}

uint32_t CRCCheckStream::get() const {
    return crc ^ 0xFFFFFFFF;
}