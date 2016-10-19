#include <iostream>
#include <fstream>
#include <algorithm>

#include "config.hpp"
#include "vfs.hpp"
#include "stringutils.hpp"

#if defined(PLATFORM_WINDOWS)
#include <windows.h>
#include <Winerror.h>
#include <shlobj.h>
#elif defined(PLATFORM_OSX)
#include <mach-o/dyld.h>
#else
#include <unistd.h>
#include <linux/limits.h>
#endif

#if !defined(PLATFORM_WINDOWS)
#include <dirent.h>
#include <sys/stat.h>
#endif

static std::string basePath;
static std::string homePath;
#if OSX_APP_BUNDLE
static std::string appPath;
#endif

#include <libintl.h>
#define _(STRING) gettext(STRING)


namespace ORCore
{
    #if defined(PLATFORM_WINDOWS)
    static const std::string sys_path_delimiter = "\\";
    #else
    static const std::string sys_path_delimiter = "/";
    #endif

    std::vector<std::string> supported_sys_delimiters = {"\\", "/"};

    const std::string vfs_path_delimiter = "/";

    VFSObjectNode::VFSObjectNode(std::string sysloc, std::string vfsloc, std::string nodename, VFSObjectNode *nodeparent)
    :vfsPath(vfsloc), name(nodename), parent(nodeparent)
    {
        sysPath.push_back(sysloc);
    }

    VFSObjectNode* VFSObjectNode::add_child(std::string pathVFS, std::string pathSys, std::string name)
    {
        auto node = std::make_unique<VFSObjectNode>(pathVFS, pathSys, name, this);
        VFSObjectNode *nodePtr = node.get();
        children.push_back(std::move(node));
        return nodePtr;
    }

    static VFSObjectNode vfsRoot("", "/", "", nullptr);

    std::string getPrimaryDelimiter(std::string path)
    {
        int largestCount = 0;
        std::string largestVal;
        for (auto delimit : supported_sys_delimiters) {
            int count = ORCore::stringCount(path, delimit);
            if (largestCount != 0 && count != 0) {
                // string has mixed path delimiters
                return "";
            } else if (count > largestCount) {
                largestCount = count;
                largestVal = delimit;
            }
        }
        return largestVal;
    }

    bool recurse_to(std::string recrsePath, bool failOnNoChild, std::function<void(VFSObjectNode *, std::string, bool)> callback)
    {
        std::vector<std::string> pathObjects = ORCore::stringSplit(recrsePath, vfs_path_delimiter);
        std::reverse(pathObjects.begin(), pathObjects.end()); // now we can pop from back of vector


        std::vector<std::string> pathCurrentLocation;
        // pathCurrentLocation.push_back("");

        VFSObjectNode *currentNode = &vfsRoot;
        std::string currentNodeName, vfsNodePath;
        bool foundNode;

        while (true) {
            foundNode = false;
            currentNodeName = pathObjects.back();
            pathObjects.pop_back();

            pathCurrentLocation.push_back(currentNodeName);
            vfsNodePath = ORCore::stringJoin(pathCurrentLocation, vfs_path_delimiter);

            for (auto& node : currentNode->children)
            {
                if (node->name == currentNodeName) {
                    currentNode = node.get();
                    foundNode = true;
                }
            }

            if (vfsNodePath != recrsePath && foundNode == false) // Create node if its not found
            {
                if (failOnNoChild == false)
                {
                    currentNode = currentNode->add_child(vfsNodePath, "", currentNodeName);
                } else {
                    return false;
                }
            }

            if (pathObjects.size() == 0)
            {
                callback(currentNode, currentNodeName, foundNode);
                break;
            }
        }
        return true;
    }

    void mount(std::string sysPath, std::string vfsPath)
    {
        // If the path being mounted already exists the two will be merged.
        auto doMount = [&](VFSObjectNode * currentNode, std::string currentNodeName, bool foundNode) {
            if (foundNode == false) {
                currentNode = currentNode->add_child(sysPath, vfsPath, currentNodeName);
            } else {
                currentNode->sysPath.push_back(sysPath);
            }

        };
        recurse_to(vfsPath, false, doMount);
    }

    std::string getPathDelimiter()
    {
        return vfs_path_delimiter;
    }

    std::vector<std::string> resolveSystemPath(std::string objectPath)
    {
        std::vector<std::string> potentialSystemMounts;
        // If the path being mounted already exists the two will be merged.
        auto getPath = [&](VFSObjectNode * currentNode, std::string currentNodeName, bool foundNode) {

            potentialSystemMounts.insert(
                potentialSystemMounts.end(),
                currentNode->sysPath.begin(),
                currentNode->sysPath.end()
            );
        };
        if (recurse_to(objectPath, true, getPath) != true) {
            return potentialSystemMounts;
        }
        std::cout << potentialSystemMounts.back() << " DASDHJASBD" << std::endl;
        return potentialSystemMounts;

    }


    std::string read_file(std::string filename, FileMode mode)
    {
        auto fileMode = std::ios::in | std::ios_base::ate;
        if (mode == FileMode::Binary) {
             fileMode |= std::ios_base::binary;
        }
        std::ifstream in(filename, fileMode);
        if (in) {
            std::string contents;
            contents.resize(static_cast<unsigned int>(in.tellg()));
            in.seekg(0, std::ios::beg);
            in.read(&contents[0], contents.size());
            in.close();
            return contents;
        } else {
            std::cout << _("Failed to load: ") << filename << std::endl;
            return "";
        }
    }


    std::vector<FileInfo> sysGetPathContents(std::string sysPath)
    {
        std::vector<FileInfo> contents;
        #if defined(PLATFORM_WINDOWS)
        // use FindFirstFile FindNextFile, and FindClose
        #else
        dirent *dp;
        DIR *dir = opendir(sysPath.c_str());
        struct stat sb;
        if (!dir)
        {
            // return early with empty vector
            return contents;
        }
        do
        {
            std::string filePath = sysPath;
            filePath += sys_path_delimiter;
            filePath += dp->d_name;

            FileInfo file;
            file.filePath = std::move(filePath);
            file.fileName = dp->d_name;

            stat(file.fileName.c_str(), &sb);

            dp = readdir(dir); // This is for next loop iteration

            if (S_ISDIR(sb.st_mode)) {
                file.fileType = FileType::Folder;
            } else if (S_ISREG(sb.st_mode)) {
                file.fileType = FileType::File;
            } else {
                continue;
            }
            contents.push_back(std::move(file));
        } while(dp);
        closedir(dir);
        #endif

        return contents;
    }

    std::string GetBasePath() // executable path
    {
        if ( basePath.empty() )
        {
#if defined(PLATFORM_WINDOWS)
            char buffer[MAX_PATH];//always use MAX_PATH for filepaths
            GetModuleFileName( NULL, buffer, sizeof(buffer) );

            basePath = buffer;

#elif defined(PLATFORM_OSX)
            char path[8192];
            uint32_t size = sizeof(path);

            if ( _NSGetExecutablePath( path, &size ) < 0 )
            {
                return "";
            }

            basePath = path;

#elif defined(PLATFORM_LINUX)
            char buff[PATH_MAX];
            ssize_t len = ::readlink("/proc/self/exe", buff, sizeof(buff)-1);
            if (len != -1)
            {
                buff[len] = '\0';
                basePath = buff;
            } else
                basePath = "";
#endif

            // remove executable name so we just have the path
            int pos = basePath.rfind( PATH_SEP );

            basePath = basePath.substr( 0, pos+1 );
            pos = basePath.rfind( "."+sys_path_delimiter );
            if (pos != std::string::npos)
            {
                basePath = basePath.substr( 0, pos-1 );
            }


#if OSX_APP_BUNDLE
            appPath = basePath; // store full appPath

            // on osx we only want the path containing the app when checking BasePath
            pos = basePath.rfind( "MacOS" );

            if ( pos != std::string::npos )
            {
                basePath = basePath.substr( 0, pos+1 );

                pos = basePath.rfind( "Contents" );

                if ( pos != std::string::npos )
                {
                    basePath = basePath.substr( 0, pos+1 );

                    pos = basePath.rfind( APP_NAME );

                    if ( pos != std::string::npos )
                        basePath = basePath.substr( 0, pos );
                }
            }
#endif
        }

        return basePath;
    }

    std::string GetHomePath() // home/library path to store configs
    {
#if defined(PLATFORM_WINDOWS)
        TCHAR szPath[MAX_PATH];

        if( homePath.empty() )
        {
            if( !SUCCEEDED( SHGetFolderPath( NULL, CSIDL_APPDATA, NULL, 0, szPath ) ) )
            {
                return NULL;
            }

            homePath = szPath;
            homePath += PATH_SEP;

            homePath += HOMEPATH_NAME;
        }
#else
        char *p;

        if( homePath.empty() )
        {

            if( ( p = getenv( "HOME" ) ) != NULL )
            {
                homePath = p ;
                homePath += PATH_SEP ;

#if defined(PLATFORM_OSX)
                homePath += "Library/Application Support/";
#endif

                homePath += HOMEPATH_NAME;
            }
        }
#endif
        return homePath;
    }

#if OSX_APP_BUNDLE
    std::string GetAppPath() // OSX get internal app path
    {
        return appPath;
    }
#endif
}
