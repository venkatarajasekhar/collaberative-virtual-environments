#ifndef SHADER_H
#define SHADER_H

#include <string>
#include <GL/glew.h>
#include <GL/gl.h>
#include <GL/glu.h>

#include "../utilities/math.h"

/**
 * Shader
 * Vertex and Pixel, and maybe geometry.
 */
class Shader
{
public:
	static bool Init();
	bool Load(const std::string& name, const std::string& extensions="");
	bool Activate();
	static void Deactivate();

	static Shader* getActivatedShader()	{return s_pCurrent;}
/*
	static bool isExtensionSupported(const std::string& ext);
	static void PrintSupportedExtensions();*/

	// Communicate with the shader
	void Uniform(const std::string& ext, GLint value);
	void Uniform(const std::string& ext, GLfloat value);
	void Uniform(const std::string& ext, const vec2& value);
	void Uniform(const std::string& ext, const vec3& value);
	void UniformTexture(const std::string& ext, GLint slot);

//	GLhandleARB	getHandle() {return m_nProgram;}

	Shader() {m_nProgram=0; m_strName="undef";}

private:
	static GLhandleARB loadShader(const std::string& filestr);
	static bool compileShader(GLhandleARB object);
	static GLhandleARB linkShaders(GLhandleARB* object, const unsigned int& nb);
	

private:
	static bool		s_bInitialized;
	static Shader*	s_pCurrent;
	std::string		m_strName;
	GLhandleARB		m_nProgram;

};

#endif
