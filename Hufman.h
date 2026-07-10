#pragma once
#include <vector>
#include <unordered_map>
#include <string>
#include <queue>
#include <stdexcept>
#include <cstdint>
#include "DataSource.h"
#include "Compress.h"

// ================= BUFFER =================

class BufferReader
{
    const std::vector<char>& data;
    size_t pos = 0;

public:
    BufferReader(const std::vector<char>& d);

    bool readByte(uint8_t& b);
    bool readData(void* dst, size_t size);
};

class BufferWriter
{
public:
    std::vector<char> buffer;

    void writeByte(uint8_t b);
    void writeData(const void* data, size_t size);
};

// ================= BIT =================

class BitWriter
{
    BufferWriter& out;
    uint8_t byte = 0;
    int count = 0;

public:
    BitWriter(BufferWriter& out);

    void writeBit(bool bit);
    void flush();
};

class BitReader
{
    BufferReader& in;
    uint8_t byte = 0;
    int count = 0;

public:
    BitReader(BufferReader& in);

    bool readBit(bool& bit);
};
class ReaderAdapter : public IDataSource
{
    BufferReader& in;

public:
    ReaderAdapter(BufferReader& r) : in(r) {}

        size_t read(char* buffer, size_t maxSize) override
    {
        size_t readBytes = 0;

        for (; readBytes < maxSize; ++readBytes)
        {
            uint8_t b;
            if (!in.readByte(b))
                break;

            buffer[readBytes] = (char)b;
        }

        return readBytes;
    }

    void seek(size_t pos) override
    {
        // если нужно — можно реализовать
        // но для декодера обычно не используется
        throw std::runtime_error("Seek not supported");
    }
    

};
class WriterAdapter : public CompressControl
{
    BufferWriter& out;

public:
    WriterAdapter(BufferWriter& w) : out(w) {}

    
        void write(const char* data, size_t size) override
    {
        out.writeData(data, size);
    }

    void flush() override {}
};


// ================= HUFFMAN =================

struct Node
{
    uint8_t value;
    uint64_t fcount;
    Node* left;
    Node* right;

    Node(uint8_t value, uint64_t fcount);
    Node(Node* left, Node* right);
};

struct Comprase
{
    bool operator()(Node* left, Node* right);
};

// ================= API =================

class HuffmanCodec
{
public:
    static void compressStream(BufferReader& in, BufferWriter& out);
    static void decompressStream(BufferReader& in, BufferWriter& out);
    static std::vector<char> compress(const std::vector<char>& input);
    static std::vector<char> decompress(const std::vector<char>& input);

    static void WriteTree(Node* node, BufferWriter& out);
    static Node* ReadTree(BufferReader& in);
};