// Copyright (c) 2015-2017 Matthew Sitton <matthewsitton@gmail.com>
// See LICENSE in the project root for license information.

#include "config.hpp"

#include <iostream>
#include <fstream>
#include <algorithm>

#if defined(PLATFORM_WINDOWS)
#   include <windows.h>
#   include <Winerror.h>
#   include <shlobj.h>
#else
#   include <dirent.h>
#   include <sys/stat.h>
#   if defined(PLATFORM_OSX)
#       include <mach-o/dyld.h>
#   else
#       include <unistd.h>
#       include <linux/limits.h>
#   endif
#endif

#include <fmt/format.h>

#include "filesystem.hpp"
#include "stringutils.hpp"


namespace ORCore
{

    static std::string basePath;
    static std::string homePath;
    #if OSX_APP_BUNDLE
        static std::string appPath;
    #endif

    #if defined(PLATFORM_WINDOWS)
    static const std::string sys_path_delimiter = "\\";
    #else
    static const std::string sys_path_delimiter = "/";
    #endif

    static const std::vector<std::string> supported_sys_delimiters = {"\\", "/"};

    std::string read_file(std::string filename, FileMode mode)
    {
        auto fileMode = std::ios::in | std::ios_base::ate;
        if (mode == FileMode::Binary)
        {
             fileMode |= std::ios_base::binary;
        }
        std::ifstream in(filename, fileMode);
        if (in)
        {
            std::string contents;
            contents.resize(static_cast<unsigned int>(in.tellg()));
            in.seekg(0, std::ios::beg);
            in.read(&contents[0], contents.size());
            in.close();
            return contents;
        }
        else
        {
            throw std::runtime_error(fmt::format("Failed to load {}", filename));
        }
    }


    // TODO - provide implementation of this with the C++17 std filesystem.
    std::vector<FileInfo> get_path_contents(std::string sysPath)
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

            if (S_ISDIR(sb.st_mode))
            {
                file.fileType = FileType::Folder;
            }
            else if (S_ISREG(sb.st_mode))
            {
                file.fileType = FileType::File;
            }
            else
            {
                continue;
            }
            contents.push_back(std::move(file));
        }
        while(dp);
        closedir(dir);
        #endif

        return contents;
    }

    std::string get_base_path() // executable path
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

            if (_NSGetExecutablePath( path, &size ) < 0)
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
            }
            else
            {
                basePath = "";
            }
#endif

            // remove executable name so we just have the path
            int pos = basePath.rfind( PATH_SEP );

            basePath = basePath.substr( 0, pos+1 );
            pos = basePath.rfind( "."+sys_path_delimiter );
#pragma GCC diagnostic ignored "-Wsign-compare"
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
                    {
                        basePath = basePath.substr( 0, pos );
                    }
                }
            }
#endif
        }

        return basePath;
    }

    // TODO - Properly support os standard config/data/cache paths
    // Windows:
    //  - `%AppData%/APPNAME` - For config data this is mainly required for roaming user profiles.
    //  - `%LocalAppData%/APPNAME` - For app data that should not be stored with roaming user profiles. Cache for example.
    //  - We currently use CSIDL when we should be using the newer KnownFolder API.
    //    - Potential Downsides? This would break for versions of windows older than Vista.(Could provide 2 implementations?)
    //    - https://msdn.microsoft.com/en-us/library/windows/desktop/bb776911(v=vs.85).aspx
    // Linux:
    //  - Follow XDG Base Directory Specification
    // OSX:
    //  - `~/Library/Application Support/APPNAME` - For config data this will end up backed up by stuff like timemachine.
    //  - `~/Library/Caches/APPNAME` - Cached data which will not be backed up by timemachine.
    // This is also badly named atm.
    // home/library path to store configs
    std::string get_home_path() 
    {
#if defined(PLATFORM_WINDOWS)
        TCHAR szPath[MAX_PATH];

        if( homePath.empty() )
        {
            if(!SUCCEEDED(SHGetFolderPath(NULL, CSIDL_APPDATA, NULL, 0, szPath)))
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

            if((p = getenv( "HOME" )) != NULL)
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
    std::string get_app_path() // OSX get internal app path
    {
        return appPath;
    }
#endif
} // namespace ORCore
