#pragma once
#include "Structs.h"
#include "PositionWriter.h"
#include "RLE.h"
#include "FileWriter.h"
#include "CRCCompresR.h"
#include "СКС.h"
#include "ThreadPool.h"
#include "Hufman.h"
#include "Directory.h"
namespace fs = std::filesystem;
class MemorySource : public IDataSource
{
    std::vector<char> storage;   // 🔥 ВЛАДЕЕТ ДАННЫМИ
    const char* data = nullptr;
    size_t size = 0;
    size_t offset = 0;

public:
    MemorySource(std::vector<char> buffer)
        : storage(std::move(buffer)),
        data(storage.data()),
        size(storage.size()),
        offset(0)
    {}

    size_t read(char* buffer, size_t maxSize) override
    {
        size_t remaining = size - offset;
        size_t toRead = std::min(remaining, maxSize);

        if (toRead == 0)
            return 0;

        memcpy(buffer, data + offset, toRead);
        offset += toRead;

        return toRead;
    }

    void seek(size_t pos) override
    {
        offset = (pos > size) ? size : pos;
    }
};
class StreamPipeline
{
public:
    static void run(IDataSource& source, CompressControl& head)
    {
        const size_t BUF_SIZE = 4096;
        char buffer[BUF_SIZE];

        while (true)
        {
            size_t readBytes = source.read(buffer, BUF_SIZE);

            if (readBytes == 0)
                break;

            head.write(buffer, readBytes);
        }

        head.flush();
    }
};
class FileProcessor
{
private:
    ThreadPool pool{ 4 };
public:
    FileEntry compressFile(
        const std::string& fullPath,
        const std::string& archivePath,
        std::istream& in,
        PositonControl& pos);

    void extractFile(
        std::istream& in,
        const FileEntry& e);
};