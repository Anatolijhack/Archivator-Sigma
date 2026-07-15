

#include "Archivator.h"


//void ArchivatorControl::Archive(const std::string& fileName, std::vector<std::string>& files)
//{
//    //std::ofstream out(fileName, std::ios::binary);
//    //if (!out) throw std::runtime_error("Failed to open output file");
//
// 
//    //    ArchiveHeader header{};
//    //memcpy(header.magic, "SIGMA", 5);
//    //header.fileCount = files.size();
//
//    //out.write((char*)&header, sizeof(header));
//
//    //std::vector<FileEntry> entries;
//
//    //FileWriter fileWriter(out);
//    //PositonControl pos(&fileWriter);
//
//    //for (const auto& path : files)
//    //{
//    //    std::ifstream in(path, std::ios::binary);
//    //    if (!in) throw std::runtime_error("Failed to open input file");
//
//    //    FileEntry e{};
//    //    e.name = path;
//
//    //    // ✅ правильный pipeline
//    //    RLECompress rle(&pos);
//    //    CRCStream crc(&rle);
//    //    CompressControl* head = &crc;
//
//    //    const size_t BUF_SIZE = 4096;
//    //    char buffer[BUF_SIZE];
//
//    //    uint64_t originalSize = 0;
//
//    //    uint64_t start = pos.tell();
//    //    e.offset = start + sizeof(ArchiveHeader);
//
//    //    while (in)
//    //    {
//    //        in.read(buffer, BUF_SIZE);
//    //        std::streamsize readBytes = in.gcount();
//
//    //        if (readBytes > 0)
//    //        {
//    //            head->write(buffer, (size_t)readBytes);
//    //            originalSize += (uint64_t)readBytes;
//    //        }
//    //    }
//
//    //    head->flush();
//
//    //    uint64_t end = pos.tell();
//    //    e.compresedSize = end - start;
//
//    //    e.size = originalSize;
//    //    e.compresed = true;
//    //    e.crc = crc.getCRC();
//
//    //    entries.push_back(e);
//    //}
//
//    //// --- индекс ---
//    //header.IndexofSet = pos.tell() + sizeof(ArchiveHeader);
//
//    //for (auto& e : entries)
//    //{
//    //    uint64_t nameSize = e.name.size();
//
//    //    out.write((char*)&nameSize, sizeof(nameSize));
//    //    out.write(e.name.data(), nameSize);
//
//    //    out.write((char*)&e.size, sizeof(e.size));
//    //    out.write((char*)&e.offset, sizeof(e.offset));
//    //    out.write((char*)&e.compresedSize, sizeof(e.compresedSize));
//
//    //    uint8_t c = e.compresed ? 1 : 0;
//    //    out.write((char*)&c, sizeof(c));
//
//    //    out.write((char*)&e.crc, sizeof(e.crc));
//    //}
//
//    //// --- переписать header ---
//    //out.seekp(0);
//    //out.write((char*)&header, sizeof(header));
//    std::ofstream out(fileName, std::ios::binary);
//
// 
//
//    FormatWriter format;
//    
//
//    FileWriter writer(out);
//    PositonControl pos(&writer);
//
//    ArchiveHeader header{};
//    memcpy(header.magic, "SIGMA", 5);
//    header.fileCount = files.size();
//    pos.write((char*)&header, sizeof(header));
//
//
//    FileProcessor processor;
//    std::vector<FileEntry> entries;
//
//    for (const auto& path : files)
//    {
//        std::ifstream in(path, std::ios::binary);
//        entries.push_back(processor.compressFile(path, in, pos));
//    }
//
//    header.IndexofSet = pos.tell() /*+ sizeof(ArchiveHeader)*/;
//
//    format.writeIndex(pos, entries);
// 
//    format.rewriteHeader(out, header);
//}
void ArchivatorControl::Archive(
    const std::string& archiveName,
    const std::string& folder)
{
    namespace fs = std::filesystem;


    std::ofstream out(
        archiveName,
        std::ios::binary
    );


    if (!out)
        throw std::runtime_error(
            "Cannot create archive"
        );



    FileWriter writer(out);

    PositonControl pos(&writer);



    ArchiveHeader header{};

    header.fileCount = 0;



    // резервируем место под header

    pos.write(
        (char*)&header,
        sizeof(header)
    );



    fs::path root =
        fs::absolute(folder);



    std::vector<FileEntry> entries;


    FileProcessor processor;



    // ================================
    // Добавляем корневую папку
    // ================================

    FileEntry rootEntry{};

    rootEntry.name =
        root.filename().string();

    rootEntry.isDirectory = true;

    rootEntry.compresed = false;


    entries.push_back(
        rootEntry
    );



    // ================================
    // Обход папки
    // ================================

    for (auto& item :
        fs::recursive_directory_iterator(root))
    {

        if (item.is_directory())
        {
            FileEntry e{};


            e.name =
                fs::relative(
                    item.path(),
                    root.parent_path()
                ).string();



            e.isDirectory = true;

            e.compresed = false;



            entries.push_back(
                e
            );


            continue;
        }



        if (item.is_regular_file())
        {
            fs::path full =
                item.path();



            // путь внутри архива

            std::string archivePath =
                fs::relative(
                    full,
                    root.parent_path()
                ).string();




            std::ifstream in(
                full,
                std::ios::binary
            );



            if (!in)
            {
                throw std::runtime_error(
                    "Cannot open file: "
                    + full.string()
                );
            }




            entries.push_back(
                processor.compressFile(
                    full.string(), // настоящий файл
                    archivePath,   // путь в архиве
                    in,
                    pos
                )
            );
        }
    }




    // ================================
    // Записываем индекс
    // ================================

    header.fileCount =
        entries.size();



    header.IndexofSet =
        pos.tell();




    FormatWriter format;



    format.writeIndex(
        pos,
        entries
    );



    // возвращаемся и переписываем header

    format.rewriteHeader(
        out,
        header
    );


    std::cout
        << "Archive files: "
        << header.fileCount
        << "\n";
}
//Рабочая версия ниже + коментарии // типа
//void ArchivatorControl::Extract(const std::string& fileName)
//{
    /*std::ifstream in(fileName, std::ios::binary);
    if (!in) throw std::runtime_error("Failed to open archive");

 
        ArchiveHeader header{};
    in.read((char*)&header, sizeof(header));

    if (memcmp(header.magic, "SIGMA", 5) != 0)
        throw std::runtime_error("Invalid archive");

    in.seekg(header.IndexofSet);

    std::vector<FileEntry> files;

    for (uint64_t i = 0; i < header.fileCount; i++)
    {
        FileEntry e{};

        uint64_t nameSize;
        in.read((char*)&nameSize, sizeof(nameSize));

        if (nameSize > 1024)
            throw std::runtime_error("string too long");

        e.name.resize(nameSize);
        in.read((char*)e.name.data(), nameSize);

        in.read((char*)&e.size, sizeof(e.size));
        in.read((char*)&e.offset, sizeof(e.offset));
        in.read((char*)&e.compresedSize, sizeof(e.compresedSize));

        uint8_t compressed;
        in.read((char*)&compressed, sizeof(compressed));
        e.compresed = compressed != 0;

        in.read((char*)&e.crc, sizeof(e.crc));

        files.push_back(e);
    }

    const size_t BUF_SIZE = 4096;
    char buffer[BUF_SIZE];

    for (auto& e : files)
    {
        in.seekg(e.offset);

        std::ofstream outFile(e.name, std::ios::binary);
        if (!outFile) throw std::runtime_error("Failed to create output file");

        FileWriter writer(outFile);

        
        CRCCheckStream crc(&writer);
        RLEDecodee rle(&crc);

        CompressControl* head = &rle;

        uint64_t remaining = e.compresedSize;

        while (remaining > 0)
        {
            size_t chunk = std::min<uint64_t>(BUF_SIZE, remaining);

            in.read(buffer, chunk);
            size_t readBytes = (size_t)in.gcount();

            if (readBytes == 0)
                throw std::runtime_error("Unexpected EOF");

            head->write(buffer, readBytes);
            remaining -= readBytes;
        }

        head->flush();

        if (crc.get() != e.crc)
            throw std::runtime_error("CRC mismatch");
    }*/
//    std::ifstream in(fileName, std::ios::binary);
//
//    FormatReader format;
//    auto header = format.readHeader(in);
//    auto files = format.readIndex(in, header);
//
//    FileProcessor processor;
//
//    for (auto& e : files)
//    {
//        processor.extractFile(in, e,"CopyExtract");
//    }
//
//}
void ArchivatorControl::Extract(const std::string& fileName)
{
    namespace fs = std::filesystem;

    std::ifstream in(fileName, std::ios::binary);
    if (!in)
        throw std::runtime_error("Cannot open archive");

    FormatReader format;
    auto header = format.readHeader(in);
    auto files = format.readIndex(in, header);

    FileProcessor processor;

    fs::path dataDir = "Extracted";
    fs::path tmpDir = "Extracted_tmp";
    fs::path backupDir = "Extracted_backup";

    // =========================
    // 🔁 RECOVERY после краша
    // =========================

    // если data пропала, но backup есть → восстановим
    if (!fs::exists(dataDir) && fs::exists(backupDir))
    {
        fs::rename(backupDir, dataDir);
    }

    // если остался tmp → удаляем (незавершённая операция)
    if (fs::exists(tmpDir))
    {
        fs::remove_all(tmpDir);
    }

    // =========================
    // 📁 создаём tmp
    // =========================
    fs::create_directories(tmpDir);

    // =========================
    // 📦 распаковка В TMP
    // =========================
    for (auto& e : files)
    {
        processor.extractFile(in, e, tmpDir);
    }

    // =========================
    // 🔄 атомарный swap
    // =========================

    fs::remove_all(backupDir);

    if (fs::exists(dataDir))
    {
        fs::rename(dataDir, backupDir);
    }

    fs::rename(tmpDir, dataDir);

    fs::remove_all(backupDir);

    std::cout << "[OK] Atomic restore complete\n";
}
void ArchivatorControl::AddFolder(
    const fs::path& folder,
    const fs::path& root,
    std::vector<std::string>& files)
{
    for (auto& entry : fs::recursive_directory_iterator(folder))
    {
        fs::path relative =
            fs::relative(entry.path(), root);

        files.push_back(relative.string());
    }
}

