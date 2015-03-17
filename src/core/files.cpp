#include <fstream>
#include "files.hpp"
#include "vfs.hpp"

namespace MgCore
{
    std::string read_file(std::string filename)
    {
        std::ifstream in(filename, std::ios::in | std::ios_base::ate);
        if (in) {
            std::string contents;
            contents.resize(static_cast<unsigned int>(in.tellg()));
            in.seekg(0, std::ios::beg);
            in.read(&contents[0], contents.size());
            in.close();
            return contents;
        } else {
            return "";
        }
    }

    std::string vfs_read_file(std::string filename)
    {
        ttvfs::File *vf = VFS.GetFile( filename.c_str() );

        if ( vf && vf->open("r") ) {
            std::string contents;
            contents.resize( static_cast<unsigned int>(vf->size()) );
            vf->read(&contents[0], contents.size());
            vf->close();
            return contents;
        } else {
            return "";
        }
    }
}
