#pragma once
#include <string>
#include <vector>
#include <memory>
#include <functional>

// Utility functions for finding paths
namespace ORCore
{

    enum class FileMode
    {
        Binary,
        Normal,
    };

    enum class FileType
    {
        File,
        Folder,
    };

    struct FileInfo
    {
        std::string fileName;
        std::string filePath;
        FileType fileType;
    };

    // TODO - Merge these functions to be more integrated with the VFS
    std::vector<FileInfo> get_path_contents(std::string sysPath);
    std::string read_file(std::string filename, FileMode mode = FileMode::Normal);
    std::string get_base_path(); // executable path
    std::string get_home_path(); // home/library path to store configs

#if OSX_APP_BUNDLE
    std::string get_app_path(); // OSX get internal app path
#endif
} // namespace ORCore
