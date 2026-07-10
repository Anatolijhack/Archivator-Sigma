#pragma once
#include "Compress.h"
#include "DataSource.h"
#include <vector>
class FileWriter : public CompressControl
{
private:
	std::ostream& out;
public:
	FileWriter(std::ostream& out);
	void write(const char* data, size_t size) override;
	void flush() override;
};
class FileReader : public IDataSource
{
	std::istream& in;

public:
	FileReader(std::istream& in);

	size_t read(char* buffer, size_t maxSize) override;
	void seek(size_t pos) override;
};
class MemoryWriter : public CompressControl
{
public:
	std::vector<char> buffer;

	void write(const char* data, size_t size) override
	{
		size_t old = buffer.size();
		buffer.resize(old + size);
		memcpy(buffer.data() + old, data, size);
	}

	void flush() override {}
};