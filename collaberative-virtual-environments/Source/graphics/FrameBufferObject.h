#ifndef FBO_H
#define FBO_H

#include <GL/glew.h>
#include <iostream>

/**
 * Frame Buffer Object
 * Render into a free FBO texture.
 */
class FrameBufferObject
{
public:
	enum FBO_TYPE { FBO_2D_COLOR, FBO_CUBE_COLOR, FBO_2D_DEPTH };

	FrameBufferObject();
	~FrameBufferObject() {Destroy();}

	bool Create(FBO_TYPE type, GLuint width, GLuint height);
	void Destroy();

	void Begin(GLuint nFace=0) const;	// Starts rendering to a texture
	void End(GLuint nFace=0) const;		// Stops rendering to a texture

	void Bind(GLint unit=0) const;		// Binds texture
	void Unbind(GLint unit=0) const;	// Unbinds texture

	inline GLuint getTextureHandle() const	{return m_nTexId;} 
	inline GLuint getWidth() const			{return m_nWidth;}
	inline GLuint getHeight() const			{return m_nHeight;}

private:
	bool CheckStatus();

private:
	bool		m_bUseFBO;
	bool		m_bUseDepthBuffer;
	GLuint		m_nTexId;
	GLuint		m_nWidth, m_nHeight;
	GLuint		m_nFrameBufferHandle;
	GLuint		m_nDepthBufferHandle;
	GLenum		m_eTextureType;
	GLenum		m_eAttachment;
	
};


#endif

