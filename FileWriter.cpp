#include "FileWriter.h"
FileWriter::FileWriter(std::ostream& out) : out(out) {};

void FileWriter::write(const char* data, size_t size)
{
	out.write(data, size);
}
void FileWriter::flush() {};

FileReader::FileReader(std::istream& in) : in(in) {}

size_t FileReader::read(char* buffer, size_t maxSize)
{
    in.read(buffer, maxSize);
    return (size_t)in.gcount();
}

void FileReader::seek(size_t pos)
{
    in.seekg(pos);
}
