#pragma once
#include <string>
#include <vector>
#include <memory>
#include <functional>

//
// To add archive to VFS:
// 	VFS.AddArchive( "test.zip" );
//
// To mount archive to root of VFS:
// 	VFS.Mount( "test.zip", "" );
//
// To mount folder to root of VFS:
// 	VFS.Mount( "dataDir", "" );
//
// To use file that is in VFS:
//	ttvfs::File *vf = VFS.GetFile( "data.txt" );
//	vf->open( "r" );
//	char buf[513];
//	size_t bytes = vf->read( buf, 512 );
//	puts( buf );
//	vf->close();
//
//
// Notes:
// - Archives MUST be added before being mounted.
// - The second arguement to VFS.Mount() is the alias you want to use when
//   accessing the folder/archive. Setting it to "" will add it's contents
//   to the root of the VFS, effectively overwriting same-named files with
//   the new ones.
// - Any directory or archive in the VFS can also be re-mounted.
//

// Utility functions for finding paths
namespace MgCore
{

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


    enum class FileMode
    {
        Binary,
        Normal,
    };

    std::vector<std::string> sysGetPathFiles(std::string sysPath);
    std::vector<std::string> sysGetPathFolders(std::string sysPath);
    std::string read_file(std::string filename, FileMode mode = FileMode::Normal);
    void SetBasePath( std::string newPath ); // set basePath
    std::string GetBasePath(); // executable path
    std::string GetHomePath(); // home/library path to store configs

#if OSX_APP_BUNDLE
    std::string GetAppPath(); // OSX get internal app path
#endif
}
