#pragma once
#include "Compress.h"
class RLECompress : public CompressControl
{
private:
	CompressControl* next = nullptr;
	char current = 0;
	int count = 0;
	bool has = false;
	void emit(char value, uint8_t count);
public:
	RLECompress(CompressControl* next);

	void write(const char* data, size_t size) override;
	void flush() override;
};
class RLEDecodee : public CompressControl
{
private:

	enum State
	{
		WAIT_VALUE,
		WAIT_COUNT
	};

	CompressControl* next = nullptr;


	char value = 0;
public:
	void reset();   // 🔥 ДОБАВИТЬ

	State state = WAIT_VALUE;
	
	RLEDecodee(CompressControl* next);
	void write(const char* data, size_t size) override;
	void flush() override;

};