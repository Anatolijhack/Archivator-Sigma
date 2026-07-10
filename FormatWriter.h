#pragma once
#include <fstream>
#include "Structs.h"
#include <vector>
#include "Compress.h"

class FormatWriter
{
public:
    void writeHeader(std::ostream& out, ArchiveHeader& header);
    void rewriteHeader(std::ostream& out, const ArchiveHeader& header);
    void writeIndex(CompressControl& out, const std::vector<FileEntry>& entries);
};

class FormatReader
{
public:
    ArchiveHeader readHeader(std::istream& in);
    std::vector<FileEntry> readIndex(std::istream& in, const ArchiveHeader& h);
};
