#pragma once
#include <string>
#include <vector>
#include <memory>

// Utility functions for finding paths
namespace ORCore
{

    // TODO - Figure out how to Implement merged mounts
    struct VFSObjectNode
    {
        bool leaf; // True, a file no children. False, a directory has children.
        std::vector<std::string> sysPath;
        std::string vfsPath;
        std::string name;
        VFSObjectNode *parent;
        std::vector<std::unique_ptr<VFSObjectNode>> children;
        VFSObjectNode(std::string sysloc, std::string vfsloc, std::string nodename, VFSObjectNode *nodeparent);
        VFSObjectNode* add_child(std::string pathVFS, std::string pathSys, std::string name);

    };

    std::string getPrimaryDelimiter(std::string path);
    bool recurse_to(std::string recrsePath, bool failOnNoChild, std::function<void(VFSObjectNode *, std::string, bool)> callback);
    void mount(std::string sysPath, std::string vfsPath);
    std::string getPathDelimiter();
    std::vector<std::string> resolveSystemPath(std::string objectPath);

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


    enum class FileMode
    {
        Binary,
        Normal,
    };

    // TODO - Merge these functions to be more integrated with the VFS
    std::vector<FileInfo> sysGetPathContents(std::string sysPath);
    std::string read_file(std::string filename, FileMode mode = FileMode::Normal);
    void SetBasePath( std::string newPath ); // set basePath
    std::string GetBasePath(); // executable path
    std::string GetHomePath(); // home/library path to store configs

#if OSX_APP_BUNDLE
    std::string GetAppPath(); // OSX get internal app path
#endif
} // namespace ORCore
