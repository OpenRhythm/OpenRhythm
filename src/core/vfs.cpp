#include "config.hpp"
#include "vfs.hpp"
#include <iostream>

#ifdef PLATFORM_WINDOWS
#include <windows.h>
#include <Winerror.h>
#elif defined(PLATFORM_OSX)
#include <mach-o/dyld.h>
#else
#include <unistd.h>
#include <linux/limits.h>
#endif

static std::string basePath;
static std::string homePath;
#ifdef PLATFORM_OSX
static std::string appPath;
#endif

namespace MgCore
{
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

        std::cout << basePath << std::endl;

        return basePath;
    }

    std::string GetHomePath() // home/library path to store configs
    {
#if defined(PLATFORM_WINDOWS)
        TCHAR szPath[MAX_PATH];
        FARPROC qSHGetFolderPath;
        HMODULE shfolder = LoadLibrary("shfolder.dll");

        if(shfolder == NULL)
            return NULL;

        if( homePath.empty() )
        {
            qSHGetFolderPath = GetProcAddress(shfolder, "SHGetFolderPathA");
            if(qSHGetFolderPath == NULL)
            {
                FreeLibrary(shfolder);
                return NULL;
            }

            if( !SUCCEEDED( qSHGetFolderPath( NULL, CSIDL_APPDATA, NULL, 0, szPath ) ) )
            {
                FreeLibrary(shfolder);
                return NULL;
            }

            homePath = szPath;
            homePath += PATH_SEP;

            homePath += HOMEPATH_NAME;
        }

        FreeLibrary(shfolder);
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
