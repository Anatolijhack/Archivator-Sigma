#include "Archivator.h"
#include <fstream>
#include <vector>
#include <filesystem>
#include <cassert>
#include <iostream>

namespace fs = std::filesystem;

//bool compareFiles(const std::string& a, const std::string& b)
//{
//    std::ifstream f1(a, std::ios::binary);
//    std::ifstream f2(b, std::ios::binary);
//
//    if (!f1 || !f2) return false;
//
//    std::vector<char> buf1(4096), buf2(4096);
//
//    while (f1 && f2)
//    {
//        f1.read(buf1.data(), buf1.size());
//        f2.read(buf2.data(), buf2.size());
//
//        if (f1.gcount() != f2.gcount())
//            return false;
//
//        if (!std::equal(buf1.begin(), buf1.begin() + f1.gcount(), buf2.begin()))
//            return false;
//    }
//
//    return true;
//
//}
//
//void writeTestFile(const std::string& name, const std::string& content)
//{
//    std::ofstream out(name, std::ios::binary);
//    out.write(content.data(), content.size());
//}

//int main()
//{
//    //try
//    //{
//    //    std::cout << "=== TEST START ===\n";
//
//
//    //    writeTestFile("file1.txt", "AAAAABBBBBCCCCCDDDDDEEEEE");
//    //    writeTestFile("file2.txt", "Hello world Hello world Hello world");
//    //    writeTestFile("file3.txt", std::string(10000, 'X'));
//
//    //    std::vector<std::string> files = {
//    //        "file1.txt",
//    //        "file2.txt",
//    //        "file3.txt"
//    //    };
//
//    //    ArchivatorControl arch;
//
//    //    arch.Archive("test.arc", files);
//
//    //    fs::copy("file1.txt", "file1_orig.txt", fs::copy_options::overwrite_existing);
//    //    fs::copy("file2.txt", "file2_orig.txt", fs::copy_options::overwrite_existing);
//    //    fs::copy("file3.txt", "file3_orig.txt", fs::copy_options::overwrite_existing);
//
//
//    //    fs::remove("file1.txt");
//    //    fs::remove("file2.txt");
//    //    fs::remove("file3.txt");
//
//
//    //    arch.Extract("test.arc");
//
//
//    //    assert(compareFiles("file1.txt", "file1_orig.txt"));
//    //    assert(compareFiles("file2.txt", "file2_orig.txt"));
//    //    assert(compareFiles("file3.txt", "file3_orig.txt"));
//
//    //    std::cout << "ALL TESTS PASSED ✅\n";
//
//    //    return 0;
//    //}
//    //catch(const std::exception& e)
//    //{
//    //    std::cout << "ERROR: " << e.what() << std::endl;
//    //}
//
//        try
//        {
//            ArchivatorControl arch;
//
//            // 🔹 файлы для архивации
//            std::vector<std::string> files =
//            {
//                "file1.txt",
//                "file2.txt",
//                "file3.txt"
//            };
//
//            std::string archiveName = "test.sigma";
//
//            // 🔥 СЖАТИЕ
//            std::cout << "Archiving...\n";
//            arch.Archive(archiveName, files);
//            std::cout << "Archive created: " << archiveName << "\n";
//
//            // 🔥 РАСПАКОВКА
//            std::cout << "Extracting...\n";
//            arch.Extract(archiveName);
//            std::cout << "Done\n";
//        }
//        catch (const std::exception& ex)
//        {
//            std::cerr << "Error: " << ex.what() << "\n";
//        }
//
//        return 0;
//}



int main()
{
    try
    {
        ArchivatorControl arch;

     
            std::string folder =
            "Test";


        std::string archive =
            "test2.sigma";



        std::cout
            << "Archiving folder...\n";


        arch.Archive(
            archive,
            folder
        );


        std::cout
            << "Archive created: "
            << archive
            << "\n";



        std::cout
            << "Extracting...\n";


        std::cout << "Start Extract\n";
        arch.Extract(archive);
        std::cout << "End Extract\n";


        std::cout
            << "Done\n";
    }
    catch (const std::exception& ex)
    {
        std::cerr
            << "Error: "
            << ex.what()
            << "\n";
    }


    return 0;
 
}

