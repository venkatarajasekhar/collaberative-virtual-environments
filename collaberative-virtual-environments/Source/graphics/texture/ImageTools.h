#ifndef IMAGETOOLS_H
#define IMAGETOOLS_H

#include <il.h>
#include <GL\glew.h>

#include "../../utilities/math.h"

namespace ImageTools
{
	class ImageData 
	{
	public:
		GLubyte*	data;
		unsigned int w, h, d;

		ivec3	getColor(unsigned int x, unsigned int y) const;

		ImageData() {w = h = d = 0; data = NULL;}
		~ImageData() {Destroy();}
		void Destroy();

	};

	void     OpenImage(const std::string& filename, ImageData& img);
	GLubyte* OpenImage(const std::string& filename, unsigned int& w, unsigned int& h, unsigned int& d);

	GLubyte* OpenImagePPM(const std::string& filename, unsigned int& w, unsigned int& h, unsigned int& d);
	GLubyte* OpenImageDevIL(const std::string& filename, unsigned int& w, unsigned int& h, unsigned int& d);

}

#endif // IMAGETOOLS_H
