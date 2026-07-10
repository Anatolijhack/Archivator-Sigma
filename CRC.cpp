#include "ัสั.h"

CRCStream::CRCStream(CompressControl* next)
{
    this->next = next;
}

void CRCStream::write(const char* data, size_t size)
{
    crc = DoCRC32(data, size, crc);

   
    if (next)
        next->write(data, size);
}

void CRCStream::flush()
{
    if (next)
        next->flush();
}

uint32_t CRCStream::getCRC() const
{
    return crc ^ 0xFFFFFFFF;
}
