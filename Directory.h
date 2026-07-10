#pragma once

#include <filesystem>
#include <vector>
#include <string>

namespace fs = std::filesystem;


inline std::vector<std::string> CollectFiles(
    const std::string& folder)
{
    std::vector<std::string> files;

    fs::path root = fs::absolute(folder);


    if (!fs::exists(root))
        throw std::runtime_error(
            "Folder does not exist"
        );


    for (auto& entry :
        fs::recursive_directory_iterator(root))
    {
        if (entry.is_regular_file())
        {
            files.push_back(
                entry.path().string()
            );
        }
    }


    return files;
}