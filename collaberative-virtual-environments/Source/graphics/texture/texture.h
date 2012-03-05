#ifndef TEXTURE_H
#define TEXTURE_H

#include "ImageTools.h"

class CTexture
{
public:
	virtual GLenum getTextureType() const = 0;

	void Gen();
	virtual bool Load(const std::string& name);
	void Destroy();

	void Bind(GLuint slot) const;
	void Unbind(GLuint slot) const;

	GLuint getHandle() const {return m_nHandle;} 

	static void EnableGenerateMipmaps(bool b) {s_bGenerateMipmaps=b;}

	CTexture() {m_nHandle=0;}
	~CTexture() {Destroy();}

protected:
	void Bind() const;
	void Unbind() const;
	bool LoadFile(GLenum target, const std::string& name);
	void LoadData(GLenum target, GLubyte* ptr, unsigned int w, unsigned int h, unsigned int d);
	
protected:
	GLuint	m_nHandle;				// Texture ID
	static bool s_bGenerateMipmaps;	// Create Mipmaps?
};

#endif