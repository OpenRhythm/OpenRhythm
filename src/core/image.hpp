#include <string>
#include <memory>


namespace MgCore
{
	struct Image
	{
		std::string path;
		int width;
		int height;
		std::unique_ptr<unsigned char[]> pixelData;
		int length;
	};


	Image loadPNG(std::string filename);
}