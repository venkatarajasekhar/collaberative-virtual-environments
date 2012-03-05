#undef _UNICODE

#ifdef WIN32
	#define WIN32_LEAN_AND_MEAN
	#include <WinSock2.h> // Has to go here to prevent windows.h conflicts.
	#include <Windows.h>
#endif

#include <string>
#include <iostream>
#include <sstream>

#include <GL/glew.h>
#include <GL/gl.h>
#include <GL/glu.h>

#include "ImageTools.h"

namespace ImageTools {

GLubyte* OpenImagePPM(const std::string& filename, unsigned int& w, unsigned int& h, unsigned int& d)
{

	char head[70];
	int i,j;
	GLubyte * img = NULL;

	FILE * f = fopen(filename.c_str(), "rb");

	if(f==NULL)
		return 0;
	
	fgets(head,70,f);

	if(!strncmp(head, "P6", 2)){
		i=0;
		j=0;
		while(i<3){
			fgets(head,70,f);

			if(head[0] == '#')
				continue;
			

			if(i==0)
				i += sscanf(head, "%d %d %d", &w, &h, &d);
			else if(i==1)
				i += sscanf(head, "%d %d", &h, &d);
			else if(i==2)
				i += sscanf(head, "%d", &d);
		}

		img = new GLubyte[(size_t)(w) * (size_t)(h) * 3];
		if(img==NULL) {
			fclose(f);
			return 0; 
		}

		fread(img, sizeof(GLubyte), (size_t)w*(size_t)h*3,f);
		fclose(f);
	}
	else{
		fclose(f);
	}
	
	return img;
}

GLubyte* OpenImageDevIL(const std::string& filename, unsigned int& w, unsigned int& h, unsigned int& d)
{
	static bool first = true;
	if(first) {
		first = false;

		// Init DevIL
		ilInit();

		// Set origin of image to upper left corner
		ilOriginFunc(IL_ORIGIN_UPPER_LEFT);
		ilEnable(IL_ORIGIN_SET);

		ilEnable(IL_TYPE_SET);
		ilTypeFunc(IL_UNSIGNED_BYTE);
	}

    // Generating a new texture
    ILuint ilTexture;
    ilGenImages(1, &ilTexture);
    ilBindImage(ilTexture);

    // Loading image
	if (!ilLoadImage(filename.c_str()))
		return false;

	w = ilGetInteger(IL_IMAGE_WIDTH);
	h = ilGetInteger(IL_IMAGE_HEIGHT);
	d = ilGetInteger(IL_IMAGE_BPP);
	
	if(d==4)
		ilConvertImage(IL_RGBA, IL_UNSIGNED_BYTE);

    // Get the size of image
    const unsigned char* Pixels = ilGetData();

	GLubyte* img = new GLubyte[(size_t)(w) * (size_t)(h) * (size_t)(d)];
	memcpy(img, Pixels, (size_t)(w) * (size_t)(h) * (size_t)(d));

    // Remove the texture
    ilBindImage(0);
    ilDeleteImages(1, &ilTexture);
	
	return img;
	//return NULL;
}


GLubyte* OpenImage(const std::string& filename, unsigned int& w, unsigned int& h, unsigned int& d)
{
	char szPath[MAX_PATH+1];

	GetCurrentDirectory(MAX_PATH, szPath);				// Get Our Working Directory
	strcat_s(szPath, "\\Data\\Textures\\");				// Append "\\Data\\Textures\\" After The Working Directory
	strcat_s(szPath, filename.c_str());					// Append The PathName

	if(filename.find(".ppm") != std::string::npos){
		return ImageTools::OpenImagePPM(szPath, w, h, d);
	}
	else {
		return ImageTools::OpenImageDevIL(szPath, w, h, d);
	}
	std::cout << "Image error loading file: " << filename << std::endl;
	return NULL;
}

void OpenImage(const std::string& filename, ImageData& img)
{
	img.data = OpenImage(filename, img.w, img.h, img.d);
}


void ImageData::Destroy()
{
	if(data) {
		delete [] data;
		data = NULL;
	}
}

ivec3 ImageData::getColor(unsigned int x, unsigned int y) const
{
	int idx = (y * w + x) * d;
	return ivec3( data[idx+0], data[idx+1], data[idx+2]);
}

}