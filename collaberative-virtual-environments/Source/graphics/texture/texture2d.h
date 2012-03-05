#ifndef TEXTURE2D_H
#define TEXTURE2D_H

#include "Texture.h"

#include <gl\glew.h>

class CTexture2D : public CTexture
{
public:
	virtual GLenum getTextureType() const {return GL_TEXTURE_2D;}
	bool Load(const std::string& name);
	bool Load(GLubyte* ptr, unsigned int w, unsigned int h, unsigned int d);

	CTexture2D() : CTexture() {}
};

#endif