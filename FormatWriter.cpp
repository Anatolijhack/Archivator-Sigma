#include "FormatWriter.h"
void FormatWriter::writeIndex(
    CompressControl& out,
    const std::vector<FileEntry>& entries)
{
    for (auto& e : entries)
    {
        uint64_t nameSize = e.name.size();

        out.write(
            (char*)&nameSize,
            sizeof(nameSize)
        );

        out.write(
            e.name.data(),
            nameSize
        );


        out.write(
            (char*)&e.size,
            sizeof(e.size)
        );

        out.write(
            (char*)&e.offset,
            sizeof(e.offset)
        );

        out.write(
            (char*)&e.compresedSize,
            sizeof(e.compresedSize)
        );


        uint8_t compressed =
            e.compresed ? 1 : 0;

        out.write(
            (char*)&compressed,
            sizeof(compressed)
        );


        uint8_t directory =
            e.isDirectory ? 1 : 0;

        out.write(
            (char*)&directory,
            sizeof(directory)
        );


        out.write(
            (char*)&e.crc,
            sizeof(e.crc)
        );


        uint64_t chunkCount =
            e.chunks.size();

        out.write(
            (char*)&chunkCount,
            sizeof(chunkCount)
        );


        for (auto& ci : e.chunks)
        {
            out.write(
                (char*)&ci.offset,
                sizeof(ci.offset)
            );

            out.write(
                (char*)&ci.size,
                sizeof(ci.size)
            );

            out.write(
                (char*)&ci.crc,
                sizeof(ci.crc)
            );

            out.write(
                (char*)&ci.originalSize,
                sizeof(ci.originalSize)
            );
        }
    }
}
void FormatWriter::writeHeader(std::ostream& out, ArchiveHeader& header)
{
    memcpy(header.magic, "SIGMA", 5);
    out.write((char*)&header, sizeof(header));
}
void FormatWriter::rewriteHeader(std::ostream& out, const ArchiveHeader& header)
{
    out.seekp(0);
    out.write((char*)&header, sizeof(header));
}

ArchiveHeader FormatReader::readHeader(std::istream& in)
{
    ArchiveHeader h{};
    in.read((char*)&h, sizeof(h));

    if (memcmp(h.magic, "SIGMA", 5) != 0)
        throw std::runtime_error("Invalid archive");

    return h;
}
std::vector<FileEntry> FormatReader::readIndex(
    std::istream& in,
    const ArchiveHeader& h)
{
    in.seekg(h.IndexofSet);

    std::vector<FileEntry> files;


    for (uint64_t i = 0; i < h.fileCount; i++)
    {
        FileEntry e{};


        uint64_t nameSize;

        in.read(
            (char*)&nameSize,
            sizeof(nameSize)
        );


        if (nameSize > 1024)
            throw std::runtime_error(
                "string too long"
            );


        e.name.resize(nameSize);


        in.read(
            e.name.data(),
            nameSize
        );


        in.read(
            (char*)&e.size,
            sizeof(e.size)
        );


        in.read(
            (char*)&e.offset,
            sizeof(e.offset)
        );


        in.read(
            (char*)&e.compresedSize,
            sizeof(e.compresedSize)
        );


        uint8_t compressed;

        in.read(
            (char*)&compressed,
            sizeof(compressed)
        );

        e.compresed = compressed != 0;



        uint8_t directory;

        in.read(
            (char*)&directory,
            sizeof(directory)
        );

        e.isDirectory = directory != 0;



        in.read(
            (char*)&e.crc,
            sizeof(e.crc)
        );



        uint64_t chunkCount;


        in.read(
            (char*)&chunkCount,
            sizeof(chunkCount)
        );


        if (chunkCount > 100000)
        {
            throw std::runtime_error(
                "Corrupted archive: invalid chunk count"
            );
        }


        e.chunks.resize(chunkCount);



        for (auto& ci : e.chunks)
        {
            in.read(
                (char*)&ci.offset,
                sizeof(ci.offset)
            );

            in.read(
                (char*)&ci.size,
                sizeof(ci.size)
            );

            in.read(
                (char*)&ci.crc,
                sizeof(ci.crc)
            );

            in.read(
                (char*)&ci.originalSize,
                sizeof(ci.originalSize)
            );
        }


        files.push_back(e);
    }


    return files;
}