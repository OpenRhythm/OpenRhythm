#include <string>
#include <iostream>
#include "image.hpp"
#include "vfs.hpp"
#include <sstream>
#include <vector>
#include <utility>
#include "png.hpp"

namespace MgCore
{

	Image loadPNG(std::string filename)
	{
	    std::istringstream file(MgCore::read_file( filename ));
	    png::image<png::rgba_pixel> image(file);

	    auto pixelBuffer = image.get_pixbuf();

	    Image imgData;

	    imgData.width = image.get_width();
	    imgData.height = image.get_height();
	    imgData.length = imgData.width * imgData.height * 4;
	    std::unique_ptr<unsigned char[]> data(new unsigned char[imgData.length]());
	    imgData.pixelData = std::move(data);

	    int i = 0;

	    for (int x = 0;x < imgData.width; x++) {
	        for (int y = 0; y < imgData.height; y++) {
	            auto pixel = pixelBuffer.get_pixel(x, y);
	            i = 4 * (x * imgData.width + y);

	            imgData.pixelData[i+0] = pixel.red;
	            imgData.pixelData[i+1] = pixel.green;
	            imgData.pixelData[i+2] = pixel.blue;
	            imgData.pixelData[i+3] = pixel.alpha;

	        }
	    }
	    return imgData;
	}
}