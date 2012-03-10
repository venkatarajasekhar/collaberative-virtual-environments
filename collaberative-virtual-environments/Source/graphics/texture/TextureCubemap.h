#ifndef TEXTURECUBEMAP_H
#define TEXTURECUBEMAP_H

#include "Texture.h"

class TextureCubemap : public Texture
{
public:
	virtual GLenum getTextureType() const {return GL_TEXTURE_CUBE_MAP;}
	bool Load(const std::string& name);

	TextureCubemap() : Texture() {}

protected:
	

protected:


};

#endif
