#include <fstream>
#include "files.hpp"

namespace MgCore
{
	std::string read_file(std::string filename)
	{
		std::ifstream in(filename, std::ios::in | std::ios_base::ate);
		if (in) {
			std::string contents;
			contents.resize(in.tellg());
			in.seekg(0, std::ios::beg);
			in.read(&contents[0], contents.size());
			in.close();
			return contents;
		} else {
			return "";
		}
	}
}