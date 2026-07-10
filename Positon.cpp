	#include "PositionWriter.h"

	PositonControl::PositonControl(CompressControl* next) : next(next) {};
	void PositonControl::write(const char* data, size_t size)
	{
		pos += size;
		if (next) next->write(data, size);
	}
	void PositonControl::flush()
	{
		if (next) next->flush();
	}

	uint64_t PositonControl::tell() const
	{
		return pos;
	}