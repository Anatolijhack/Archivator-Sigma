#include "FileProcesor.h"
#include <vector>
#include <memory>
#include <functional>
#include <algorithm> // ← обязательн
#include <map>


class Pipeline
{
    std::vector<std::unique_ptr<CompressControl>> stages;
    CompressControl* head = nullptr;

public:
    template<typename T>
    void add()
    {
        stages.push_back(nullptr); // просто резерв
    }

    CompressControl* build(CompressControl& sink)
    {
        stages.clear();

        CompressControl* next = &sink;

        // пример: вручную добавим стадии (позже автоматизируем)
        auto rle = std::make_unique<RLECompress>(next);
        next = rle.get();

        auto crc = std::make_unique<CRCStream>(next);
        next = crc.get();

        stages.push_back(std::move(crc));
        stages.push_back(std::move(rle));

        head = stages.front().get();
        return head;
    }
};
class PipelineBuilder
{
    using Factory = std::function<std::unique_ptr<CompressControl>(CompressControl*)>;
    std::vector<Factory> factories;

public:
    template<typename T>
    PipelineBuilder& add()
    {
        factories.push_back([](CompressControl* next)
            {
                return std::make_unique<T>(next);
            });
        return *this;
    }

    std::vector<std::unique_ptr<CompressControl>> build(CompressControl& sink)
    {
        std::vector<std::unique_ptr<CompressControl>> stages;

        CompressControl* next = &sink;

        for (auto it = factories.rbegin(); it != factories.rend(); ++it)
        {
            auto stage = (*it)(next);
            next = stage.get();
            stages.push_back(std::move(stage));
        }

        std::reverse(stages.begin(), stages.end());
        return stages;
    }
};

//FileEntry FileProcessor::compressFile(const std::string& path, std::istream& in, PositonControl& pos)
//{
//    FileEntry e{};
//    e.name = path;
//    FileReader source(in);
//    RLECompress rle(&pos);
//    CRCStream crc(&rle);
//
//    CompressControl* head = &crc;
//
//    char buffer[4096];
//    uint64_t originalSize = 0;
//
//    uint64_t start = pos.tell();
//    e.offset = start /*+ sizeof(ArchiveHeader)*/;
//
//    StreamPipeline::run(source, crc);
//
//    head->flush();
//
//    uint64_t end = pos.tell();
//
//    e.compresedSize = end - start;
//    e.size = originalSize;
//    e.compresed = true;
//    e.crc = crc.getCRC();
//
//    return e;
//}
// 
// 
//  Рабочий Pipeline
//FileEntry FileProcessor::compressFile(const std::string& path, std::istream& in, PositonControl& pos)
//{
//    FileEntry e{};
//    e.name = path;
//
//    FileReader source(in);
//
//    PipelineBuilder builder;
//
//    auto stages = builder
//        .add<CRCStream>()      // ✅ сначала CRC
//        .add<RLECompress>()    // ✅ потом сжатие
//        .build(pos);
//
//    CompressControl* head = stages.front().get();
//
//    uint64_t start = pos.tell();
//    e.offset = start;
//
//    // 🔥 запуск pipeline
//    StreamPipeline::run(source, *head);
//
//    uint64_t end = pos.tell();
//
//    e.compresedSize = end - start;
//    e.compresed = true;
//
//    // ✅ НАДЁЖНО получаем CRC
//    CRCStream* crcPtr = nullptr;
//    for (auto& s : stages)
//    {
//        if (auto* c = dynamic_cast<CRCStream*>(s.get()))
//        {
//            crcPtr = c;
//            break;
//        }
//    }
//
//    if (!crcPtr)
//        throw std::runtime_error("CRC stage not found");
//
//    e.crc = crcPtr->getCRC();
//
//    return e;
//}
//void FileProcessor::extractFile(std::istream& in, const FileEntry& e)
//{
//            in.seekg(e.offset);
//
//        FileReader source(in);
//        std::ofstream outFile(e.name, std::ios::binary);
//        FileWriter writer(outFile);
//
//        CRCCheckStream crc(&writer);
//        RLEDecodee rle(&crc);
//
//        CompressControl* head = &rle;
//
//        char buffer[4096];
//        uint64_t remaining = e.compresedSize;
//
//        while (remaining > 0)
//        {
//            size_t chunk = std::min<uint64_t>(sizeof(buffer), remaining);
//
//            size_t readBytes = source.read(buffer, chunk);
//            if (readBytes == 0)
//                throw std::runtime_error("EOF");
//
//            rle.write(buffer, readBytes);
//            remaining -= readBytes;
//        }
//
//        head->flush();
//
//        if (crc.get() != e.crc)
//            throw std::runtime_error("CRC mismatch");
//}
// Тут тоже рабочий
//void FileProcessor::extractFile(std::istream& in, const FileEntry& e)
//{
//    in.seekg(e.offset);
//
//    FileReader source(in);
//    std::ofstream outFile(e.name, std::ios::binary);
//    FileWriter writer(outFile);
//
//    PipelineBuilder builder;
//
//    auto stages = builder
//        .add<RLEDecodee>()       // ✅ сначала распаковка
//        .add<CRCCheckStream>()   // ✅ потом проверка
//        .build(writer);
//
//    CompressControl* head = stages.front().get();
//
//    char buffer[4096];
//    uint64_t remaining = e.compresedSize;
//
//    while (remaining > 0)
//    {
//        size_t chunk = std::min<uint64_t>(sizeof(buffer), remaining);
//
//        size_t readBytes = source.read(buffer, chunk);
//        if (readBytes == 0)
//            throw std::runtime_error("EOF");
//
//        head->write(buffer, readBytes);
//        remaining -= readBytes;
//    }
//
//    head->flush();
//
//    // ✅ НАДЁЖНО получаем CRC
//    CRCCheckStream* crcPtr = nullptr;
//    for (auto& s : stages)
//    {
//        if (auto* c = dynamic_cast<CRCCheckStream*>(s.get()))
//        {
//            crcPtr = c;
//            break;
//        }
//    }
//
//    if (!crcPtr)
//        throw std::runtime_error("CRC stage not found");
//
//    if (crcPtr->get() != e.crc)
//        throw std::runtime_error("CRC mismatch");
//}
//FileEntry FileProcessor::compressFile(
//    const std::string& path,
//    std::istream& in,
//    PositonControl& pos)
//{
//    FileEntry e{};
//    e.name = path;
//
//    const size_t CHUNK_SIZE = 64 * 1024;
//
//    std::vector<std::future<std::pair<size_t, std::vector<char>>>> futures;
//
//    size_t index = 0;
//
//    while (true)
//    {
//        std::vector<char> buffer(CHUNK_SIZE);
//
//        in.read(buffer.data(), CHUNK_SIZE);
//        size_t readBytes = (size_t)in.gcount();
//
//        if (readBytes == 0)
//            break;
//
//        buffer.resize(readBytes);
//
//        e.size += readBytes;
//
//        size_t currentIndex = index++;
//
//        futures.push_back(
//            pool.submit(0,
//                [buffer = std::move(buffer), currentIndex]() mutable
//                {
//                    MemoryWriter memWriter;
//                    memWriter.buffer.reserve(65536);
//
//                    PositonControl pos(&memWriter);
//
//                    PipelineBuilder builder;
//
//                    auto stages = builder
//                        .add<CRCStream>()
//                        .add<RLECompress>()
//                        .build(pos);
//
//                    CompressControl* head = stages.front().get();
//
//                    MemorySource src(buffer);
//
//                    StreamPipeline::run(src, *head);
//
//                    return std::make_pair(currentIndex, std::move(memWriter.buffer));
//                }));
//    }
//
//    std::map<size_t, std::vector<char>> ready;
//    size_t next = 0;
//
//    uint32_t fileCRC = 0xFFFFFFFF;
//
//    e.offset = pos.tell();
//
//    for (auto& f : futures)
//    {
//        auto [i, data] = f.get();
//        ready[i] = std::move(data);
//
//        while (ready.count(next))
//        {
//            auto& chunk = ready[next];
//
//            if (!chunk.empty())
//            {
//                ChunkInfo ci;
//
//                ci.offset = pos.tell();
//                ci.size = (uint32_t)chunk.size();
//
//                uint32_t crc = 0xFFFFFFFF;
//                crc = DoCRC32(chunk.data(), chunk.size(), crc);
//                ci.crc = crc ^ 0xFFFFFFFF;
//
//                ci.originalSize = 0;
//
//                pos.write(chunk.data(), chunk.size());
//
//                fileCRC = DoCRC32(chunk.data(), chunk.size(), fileCRC);
//
//                e.compresedSize += chunk.size();
//                e.chunks.push_back(ci);
//            }
//
//            ready.erase(next);
//            next++;
//        }
//    }
//
//    e.crc = fileCRC ^ 0xFFFFFFFF;
//    e.compresed = true;
//
//    return e;
//}
//
//void FileProcessor::extractFile(std::istream& in, const FileEntry& e)
//{
//    std::ofstream outFile(e.name, std::ios::binary);
//
//    for (auto& ci : e.chunks)
//    {
//        in.seekg(ci.offset);
//
//        std::vector<char> buffer(ci.size);
//        in.read(buffer.data(), ci.size);
//
//        uint32_t crc = 0xFFFFFFFF;
//        crc = DoCRC32(buffer.data(), buffer.size(), crc);
//        crc ^= 0xFFFFFFFF;
//
//        if (crc != ci.crc)
//            throw std::runtime_error("Chunk CRC mismatch");
//
//        MemorySource src(buffer);
//
//        FileWriter writer(outFile);
//
//        PipelineBuilder builder;
//
//        auto stages = builder
//            .add<RLEDecodee>()
//            .add<CRCCheckStream>()
//            .build(writer);
//
//        CompressControl* head = stages.front().get();
//
//        StreamPipeline::run(src, *head);
//    }
//}
//FileEntry FileProcessor::compressFile(
//    const std::string& fullPath,
//    const std::string& archivePath,
//    std::istream& in,
//    PositonControl& pos)
//{
//
//    FileEntry e{};
//
//    e.name = archivePath;
//    e.isDirectory = false;
//
//    const size_t CHUNK_SIZE = 64 * 1024;
//
//    struct Result
//    {
//        size_t index;
//        std::vector<char> compressed;
//        uint32_t crc;
//        size_t originalSize;
//    };
//
//    std::vector<std::future<Result>> futures;
//
//    size_t index = 0;
//
//    while (true)
//    {
//        std::vector<char> buffer(CHUNK_SIZE);
//
//        in.read(buffer.data(), CHUNK_SIZE);
//
//        size_t readBytes = (size_t)in.gcount();
//
//        if (readBytes == 0)
//            break;
//
//        buffer.resize(readBytes);
//
//        e.size += readBytes;
//
//        size_t currentIndex = index++;
//
//        futures.push_back(
//            pool.submit(0,
//                [buffer = std::move(buffer), currentIndex]() mutable -> Result
//                {
//                    // CRC оригинальных данных
//                    uint32_t crc = 0xFFFFFFFF;
//
//                    crc = DoCRC32(
//                        buffer.data(),
//                        buffer.size(),
//                        crc
//                    );
//
//                    crc ^= 0xFFFFFFFF;
//
//
//                    MemoryWriter memWriter;
//
//                    PositonControl pos(&memWriter);
//
//
//                    PipelineBuilder builder;
//
//                    auto stages = builder
//                        .add<RLECompress>()
//                        .build(pos);
//
//
//                    CompressControl* head =
//                        stages.front().get();
//
//
//                    MemorySource source(buffer);
//
//
//                    StreamPipeline::run(
//                        source,
//                        *head
//                    );
//
//
//                    // Huffman после RLE
//                    std::vector<char> compressed =
//                        HuffmanCodec::compress(
//                            memWriter.buffer
//                        );
//
//
//                    return {
//                        currentIndex,
//                        std::move(compressed),
//                        crc,
//                        buffer.size()
//                    };
//                }));
//    }
//
//
//    std::map<size_t, Result> ready;
//
//    size_t next = 0;
//
//
//    uint32_t fileCRC = 0xFFFFFFFF;
//
//
//    e.offset = pos.tell();
//
//
//    for (auto& f : futures)
//    {
//        Result r = f.get();
//
//        ready[r.index] = std::move(r);
//
//
//        while (ready.count(next))
//        {
//            auto& item = ready[next];
//
//
//            ChunkInfo ci{};
//
//
//            ci.offset = pos.tell();
//
//            ci.size =
//                (uint32_t)item.compressed.size();
//
//            ci.originalSize =
//                (uint32_t)item.originalSize;
//
//
//            // CRC сжатого чанка
//            uint32_t chunkCRC = 0xFFFFFFFF;
//
//            chunkCRC = DoCRC32(
//                item.compressed.data(),
//                item.compressed.size(),
//                chunkCRC
//            );
//
//            ci.crc = chunkCRC ^ 0xFFFFFFFF;
//
//
//
//            pos.write(
//                item.compressed.data(),
//                item.compressed.size()
//            );
//
//
//            // CRC файла от оригинала
//            fileCRC = DoCRC32(
//                (char*)&item.crc,
//                sizeof(item.crc),
//                fileCRC
//            );
//
//
//            e.compresedSize +=
//                item.compressed.size();
//
//
//            e.chunks.push_back(ci);
//
//
//            ready.erase(next);
//
//            next++;
//        }
//    }
//
//
//    e.crc = fileCRC ^ 0xFFFFFFFF;
//
//    e.compresed = true;
//
//
//    std::cout
//        << "WRITE CRC: "
//        << std::hex
//        << e.crc
//        << std::dec
//        << "\n";
//
//
//    return e;
//}
FileEntry FileProcessor::compressFile(
    const std::string& fullPath,
    const std::string& archivePath,
    std::istream& in,
    PositonControl& pos)
{
    FileEntry e{};

    e.name = archivePath;
    e.isDirectory = false;

    const size_t CHUNK_SIZE = 16 * 1024;
    const size_t MAX_IN_FLIGHT = 4;
        

    struct Result
    {
        size_t index;
        std::vector<char> compressed;
        uint32_t crc;
        size_t originalSize;
    };

    std::deque<std::future<Result>> futures;

    std::map<size_t, Result> ready;

    size_t index = 0;
    size_t next = 0;

    uint32_t fileCRC = 0xFFFFFFFF;

    e.offset = pos.tell();

    auto flushReady = [&]()
        {
            while (ready.count(next))
            {
                auto& item = ready[next];

                ChunkInfo ci{};

                ci.offset = pos.tell();
                ci.size = (uint32_t)item.compressed.size();
                ci.originalSize = (uint32_t)item.originalSize;

                uint32_t chunkCRC = 0xFFFFFFFF;

                chunkCRC = DoCRC32(
                    item.compressed.data(),
                    item.compressed.size(),
                    chunkCRC
                );

                ci.crc = chunkCRC ^ 0xFFFFFFFF;

                pos.write(
                    item.compressed.data(),
                    item.compressed.size()
                );

                fileCRC = DoCRC32(
                    (char*)&item.crc,
                    sizeof(item.crc),
                    fileCRC
                );

                e.compresedSize += item.compressed.size();
                e.chunks.push_back(ci);

                ready.erase(next);
                next++;
            }
        };

    while (true)
    {
        std::vector<char> buffer(CHUNK_SIZE);

        in.read(buffer.data(), CHUNK_SIZE);
        size_t readBytes = (size_t)in.gcount();

        if (readBytes == 0)
            break;

        buffer.resize(readBytes);
        e.size += readBytes;

        size_t currentIndex = index++;

        futures.push_back(
            pool.submit(0,
                [buffer = std::move(buffer), currentIndex]() mutable -> Result
                {
                    uint32_t crc = 0xFFFFFFFF;

                    crc = DoCRC32(
                        buffer.data(),
                        buffer.size(),
                        crc
                    );

                    crc ^= 0xFFFFFFFF;

                    MemoryWriter memWriter;
                    PositonControl pos(&memWriter);

                    PipelineBuilder builder;

                    auto stages = builder
                        .add<RLECompress>()
                        .build(pos);

                    auto* head = stages.front().get();

                    MemorySource source(buffer);

                    StreamPipeline::run(source, *head);

                    BufferReader huffIn(memWriter.buffer);
                    BufferWriter huffOut;

                    HuffmanCodec::compressStream(
                        huffIn,
                        huffOut
                    );

                    std::vector<char> compressed =
                        std::move(huffOut.buffer);

                    return {
                        currentIndex,
                        std::move(compressed),
                        crc,
                        buffer.size()
                    };
                }));


        // 🔥 ВАЖНО: ограничиваем очередь задач
        while (futures.size() >= MAX_IN_FLIGHT)
        {
            Result r = futures.front().get();
            futures.pop_front();

            ready[r.index] = std::move(r);
            flushReady();
        }
    }

    // 🔥 дочищаем оставшиеся задачи
    while (!futures.empty())
    {
        Result r = futures.front().get();
        futures.pop_front();

        ready[r.index] = std::move(r);
        flushReady();
    }

    e.crc = fileCRC ^ 0xFFFFFFFF;
    e.compresed = true;

    std::cout
        << "WRITE CRC: "
        << std::hex
        << e.crc
        << std::dec
        << "\n";

    return e;
}
//многопоточная разархивация
void FileProcessor::extractFile(
    std::istream& in,
    const FileEntry& e)
{
    namespace fs = std::filesystem;


        fs::path outputRoot = "Extracted";

    // 📁 директория
    if (e.isDirectory)
    {
        fs::create_directories(outputRoot / e.name);
        std::cout << "CREATE DIR: "
            << (outputRoot / e.name).string()
            << "\n";
        return;
    }

    // 🔥 очистка имени
    std::string cleanName = std::string(e.name.c_str());

    while (!cleanName.empty() &&
        (cleanName.back() == '\0' ||
            cleanName.back() == '\r' ||
            cleanName.back() == '\n' ||
            cleanName.back() == ' '))
    {
        cleanName.pop_back();
    }

    fs::path outPath = outputRoot / fs::path(cleanName).lexically_normal();

    if (outPath.has_parent_path())
    {
        fs::create_directories(outPath.parent_path());
    }

    std::ofstream outFile(outPath, std::ios::binary);

    if (!outFile)
    {
        throw std::runtime_error(
            "Cannot create output file: " + outPath.string()
        );
    }

    // 🔥 теперь возвращаем (data + blockCRC)
    using Result = std::pair<std::vector<char>, uint32_t>;

    std::vector<std::future<Result>> futures;
    const size_t MAX_IN_FLIGHT = 4;

    uint32_t fileCRC = 0xFFFFFFFF;

    for (const auto& chunk : e.chunks)
    {
        in.clear();
        in.seekg(chunk.offset);

        if (!in)
            throw std::runtime_error("Seek error");

        std::vector<char> compressed(chunk.size);

        in.read(compressed.data(), chunk.size);

        if ((size_t)in.gcount() != chunk.size)
            throw std::runtime_error("Unexpected EOF");

        futures.push_back(
            pool.submit(0,
                [buf = std::move(compressed), chunk]() mutable -> Result
                {
                    // 🔥 CRC сжатого блока
                    uint32_t crc = DoCRC32(
                        buf.data(),
                        buf.size(),
                        0xFFFFFFFF
                    );
                    crc ^= 0xFFFFFFFF;

                    if (crc != chunk.crc)
                        throw std::runtime_error("Chunk CRC mismatch");

                    // 🔥 Huffman decode
                    BufferReader br(buf);
                    BufferWriter bw;

                    HuffmanCodec::decompressStream(br, bw);

                    // 🔥 RLE decode
                    BufferWriter rleOut;

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

                    WriterAdapter writer(rleOut);
                    RLEDecodee rle(&writer);

                    rle.write(bw.buffer.data(), bw.buffer.size());
                    rle.flush();

                    // 🔥 CRC РАСПАКОВАННОГО блока (КЛЮЧ!)
                    uint32_t blockCRC = 0xFFFFFFFF;

                    blockCRC = DoCRC32(
                        rleOut.buffer.data(),
                        rleOut.buffer.size(),
                        blockCRC
                    );

                    blockCRC ^= 0xFFFFFFFF;

                    return { std::move(rleOut.buffer), blockCRC };
                })
        );

        // 🔥 держим порядок + ограничение памяти
        if (futures.size() >= MAX_IN_FLIGHT)
        {
            auto result = futures.front().get();

            auto& data = result.first;
            uint32_t blockCRC = result.second;

            outFile.write(data.data(), data.size());

            // 🔥 ВАЖНО: как в однопоточке
            fileCRC = DoCRC32(
                (char*)&blockCRC,
                sizeof(blockCRC),
                fileCRC
            );

            futures.erase(futures.begin());
        }
    }

    // 🔹 дочищаем
    for (auto& fut : futures)
    {
        auto result = fut.get();

        auto& data = result.first;
        uint32_t blockCRC = result.second;

        outFile.write(data.data(), data.size());

        fileCRC = DoCRC32(
            (char*)&blockCRC,
            sizeof(blockCRC),
            fileCRC
        );
    }

    fileCRC ^= 0xFFFFFFFF;

    std::cout
        << "READ CRC: "
        << std::hex
        << fileCRC
        << " EXPECTED: "
        << e.crc
        << std::dec
        << "\n";

    if (fileCRC != e.crc)
    {
        throw std::runtime_error("FILE CRC mismatch");
    }

    outFile.flush();


}



//не многопоточная версия
//void FileProcessor::extractFile(
//    std::istream& in,
//    const FileEntry& e)
//{
//    namespace fs = std::filesystem;
//
//
//    // куда распаковываем архив
//    fs::path outputRoot = "Extracted";
//
//
//    // Если это папка
//    if (e.isDirectory)
//    {
//        fs::create_directories(
//            outputRoot / e.name
//        );
//
//
//        std::cout
//            << "CREATE DIR: "
//            << (outputRoot / e.name).string()
//            << "\n";
//
//
//        return;
//    }
//
//
//
//    // Полный путь файла после распаковки
//    fs::path outPath =
//        outputRoot / e.name;
//
//
//
//    // Создаем вложенные папки
//    if (outPath.has_parent_path())
//    {
//        fs::create_directories(
//            outPath.parent_path()
//        );
//    }
//
//
//
//    std::ofstream outFile(
//        outPath,
//        std::ios::binary
//    );
//
//
//    if (!outFile)
//    {
//        throw std::runtime_error(
//            "Cannot create output file: "
//            + outPath.string()
//        );
//    }
//
//
//
//    uint32_t fileCRC = 0xFFFFFFFF;
//
//
//
//    for (const auto& ci : e.chunks)
//    {
//        in.seekg(
//            ci.offset
//        );
//
//
//        if (!in)
//        {
//            throw std::runtime_error(
//                "Seek error"
//            );
//        }
//
//
//
//        std::vector<char> buffer(
//            ci.size
//        );
//
//
//
//        in.read(
//            buffer.data(),
//            ci.size
//        );
//
//
//        if ((size_t)in.gcount() != ci.size)
//        {
//            throw std::runtime_error(
//                "Unexpected EOF"
//            );
//        }
//
//
//
//        // Проверяем CRC сжатого блока
//
//        uint32_t chunkCRC =
//            0xFFFFFFFF;
//
//
//        chunkCRC =
//            DoCRC32(
//                buffer.data(),
//                buffer.size(),
//                chunkCRC
//            );
//
//
//        chunkCRC ^= 0xFFFFFFFF;
//
//
//
//        if (chunkCRC != ci.crc)
//        {
//            throw std::runtime_error(
//                "Chunk CRC mismatch"
//            );
//        }
//
//
//
//
//        // Huffman decode
//
//        BufferReader huffIn(buffer);
//        BufferWriter huffOut;
//
//        HuffmanCodec::decompressStream(
//            huffIn,
//            huffOut
//        );
//
//        std::vector<char> decoded =
//            std::move(huffOut.buffer);
//
//
//
//        MemorySource source(
//            decoded
//        );
//
//
//
//        FileWriter writer(
//            outFile
//        );
//
//
//
//        // RLE decode -> CRC -> file
//
//        CRCCheckStream crc(
//            &writer
//        );
//
//
//        RLEDecodee rle(
//            &crc
//        );
//
//
//
//        StreamPipeline::run(
//            source,
//            rle
//        );
//
//
//
//
//        // CRC оригинального блока
//
//        uint32_t blockCRC =
//            crc.get();
//
//
//
//        fileCRC =
//            DoCRC32(
//                (char*)&blockCRC,
//                sizeof(blockCRC),
//                fileCRC
//            );
//    }
//
//
//
//    fileCRC ^= 0xFFFFFFFF;
//
//
//
//    std::cout
//        << "READ CRC: "
//        << std::hex
//        << fileCRC
//        << " EXPECTED: "
//        << e.crc
//        << std::dec
//        << "\n";
//
//
//
//    if (fileCRC != e.crc)
//    {
//        throw std::runtime_error(
//            "FILE CRC mismatch"
//        );
//    }
//}

