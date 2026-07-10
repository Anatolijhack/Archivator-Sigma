#include "RLE.h"
RLECompress::RLECompress(CompressControl* next)
{
	this->next = next;
}
void RLECompress::write(const char* data, size_t size)
{

    for (size_t i = 0; i < size; i++)
    {
        char c = data[i];

        if (!has)
        {
            current = c;
            count = 1;
            has = true;
            continue;
        }

        if (c == current && count < 255)
        {
            count++;
        }
        else
        {
            emit(current, count);
            current = c;
            count = 1;
        }
    }
}
void RLECompress::emit(char value, uint8_t count)
{
    uint8_t buf[2] = { (uint8_t)value, count };

    if (next)
        next->write((char*)buf, 2);
}
void RLECompress::flush()
{
    if (has)
    {
        emit(current, count);
        has = false;
        count = 0;
    }

    if (next)
        next->flush();
}
RLEDecodee::RLEDecodee(CompressControl* next)
{
    this->next = next;
}
void RLEDecodee::write(const char* data, size_t size)
{
    for (size_t i = 0; i < size; i++)
    {
        switch (state)
        {
        case WAIT_VALUE:
            value = data[i];
            state = WAIT_COUNT;
            break;

        case WAIT_COUNT:
        {
            uint8_t count = (uint8_t)data[i];

            if (count == 0)
                throw std::runtime_error("Invalid RLE count");

            char outBuf[256];

            for (uint8_t j = 0; j < count; j++)
                outBuf[j] = value;

            if (next)
                next->write(outBuf, count);

            state = WAIT_VALUE;
            break;
        }
        }
    }
}

void RLEDecodee::flush()
{
    if (state == WAIT_COUNT)
        throw std::runtime_error("Corrupted RLE stream");

    state = WAIT_VALUE;

    if (next)
        next->flush();
}
void RLEDecodee::reset()
{
    state = WAIT_VALUE;
    value = 0;
}