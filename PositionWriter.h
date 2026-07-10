#pragma once
#include "Compress.h"
class PositonControl : public  CompressControl
{
private:
	CompressControl* next;
	uint64_t pos = 0;
public:
	PositonControl(CompressControl* next);
	void write(const char* data, size_t size) override;
	void flush() override;
	uint64_t tell() const;
};