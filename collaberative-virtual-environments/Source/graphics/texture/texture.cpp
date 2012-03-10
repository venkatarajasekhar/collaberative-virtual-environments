#include <iostream>

#include "texture.h"

bool Texture::s_bGenerateMipmaps = true;

bool Texture::LoadFile(GLenum target, const std::string& name)
{
	unsigned int w, h, d;
	GLubyte* ptr = ImageTools::OpenImage(name, w, h, d);
	if(!ptr) {
		std::cerr << "[Error] Unable to load texture " << name.c_str() << std::endl;
		return false;
	}

	LoadData(target, ptr, w, h, d);

	delete[] ptr;
	return true;
}

void Texture::LoadData(GLenum target, GLubyte* ptr, unsigned int w, unsigned int h, unsigned int d)
{
	glTexImage2D(target, 0, d==3?GL_RGB:GL_RGBA, w, h, 0, d==3?GL_RGB:GL_RGBA, GL_UNSIGNED_BYTE, ptr);
}

void Texture::Gen()
{
	Destroy();
	glGenTextures(1, &m_nHandle);
}

bool Texture::Load(const std::string& name)
{
	Gen();

	if(m_nHandle == 0){
		std::cerr << "Texture ID incorrect." << std::endl;
		return false;
	}

	return true;
}

void Texture::Destroy()
{
	glDeleteTextures(1, &m_nHandle);
}

void Texture::Bind() const {
	glBindTexture(getTextureType(), m_nHandle);
}

void Texture::Bind(GLuint slot) const {
	glActiveTexture(GL_TEXTURE0+slot);
	glEnable(getTextureType());
	glBindTexture(getTextureType(), m_nHandle);
}

void Texture::Unbind() const {
	glBindTexture(getTextureType(), 0);
}

void Texture::Unbind(GLuint slot) const {
	glActiveTexture(GL_TEXTURE0+slot);
	glBindTexture(getTextureType(), 0);
}
