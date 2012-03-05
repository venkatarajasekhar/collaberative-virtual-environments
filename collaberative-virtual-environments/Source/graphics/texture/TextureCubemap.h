#ifndef TEXTURECUBEMAP_H
#define TEXTURECUBEMAP_H

#include "Texture.h"

class CTextureCubemap : public CTexture
{
public:
	virtual GLenum getTextureType() const {return GL_TEXTURE_CUBE_MAP;}
	bool Load(const std::string& name);

	CTextureCubemap() : CTexture() {}

protected:
	

protected:


};

#endif
