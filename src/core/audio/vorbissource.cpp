#include "vorbissource.hpp"

namespace ORCore
{
    VorbisSource::VorbisSource(std::string filename)
    : m_filename(filename)
    {

    }

    StreamFormat VorbisSource::get_format()
    {
        return {0, 0};
    }


    void VorbisSource::pull(Buffer& buffer)
    {
        
    }
}