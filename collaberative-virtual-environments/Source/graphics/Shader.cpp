#include "Shader.h"
#include "glInfo.h"

#ifdef WIN32
	#include <Windows.h>
#endif

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

bool	Shader::s_bInitialized = false;
Shader*	Shader::s_pCurrent = NULL;

const int MAX_LOG_STRING = 1024;
char logstring[MAX_LOG_STRING];

GLhandleARB Shader::loadShader(const std::string& filestr)
{

//  std::string filestr(filename);

  bool vertexshader = false;
  bool fragmentshader = false;
  bool geometryshader = false;

  if(filestr.find(".vert") != std::string::npos){
    vertexshader=true;
  }else{
    if(filestr.find(".frag") != std::string::npos){
      fragmentshader=true;
    }
#ifdef USE_GEOMETRY_SHADER
    else{
      if(filestr.find(".geo") != std::string::npos){
	geometryshader=true;
      }
#endif
  }

	if(!vertexshader && !fragmentshader && !geometryshader )
		return 0;

  std::ifstream file(filestr.c_str()); 
  if(!file)
    return 0;
  
  std::stringstream buffer;
  buffer << file.rdbuf();
  file.close();

  std::string s = buffer.str();

  GLcharARB * source = new GLcharARB[4*(buffer.str().size()/4+1)];
  if(source == 0)
    return 0;

  unsigned int i;
  for(i =0; i < buffer.str().size(); ++i){
   source[i] = s[i];
  }
  source[i] = '\0';

  GLhandleARB so = 0; 
  if(vertexshader)
    so = glCreateShaderObjectARB(GL_VERTEX_SHADER_ARB);
  if(fragmentshader)
    so = glCreateShaderObjectARB(GL_FRAGMENT_SHADER_ARB);
#ifdef USE_GEOMETRY_SHADER
  if(geometryshader)
    so = glCreateShaderObjectARB(GL_GEOMETRY_SHADER_ARB);  
#endif
  glShaderSourceARB(so, 1,(const GLcharARB**) &source, 0);

  return so;

}

bool Shader::compileShader(GLhandleARB object)
{

  if(object==0)
    return false;

  glCompileShaderARB(object);
  
  int status;
  glGetObjectParameterivARB(object, GL_OBJECT_COMPILE_STATUS_ARB, &status);
  if(status==0){
    int length = 0;
    glGetObjectParameterivARB(object, GL_OBJECT_INFO_LOG_LENGTH_ARB, &length);
    if(length>0){
      GLsizei minlength = min(MAX_LOG_STRING,length);
      glGetInfoLogARB(object, minlength, 0, logstring);
      std::cerr << logstring << std::endl;
    }
    return false;
  }
  return true;
}


GLhandleARB Shader::linkShaders(GLhandleARB* object, const unsigned int& nb)
{
  if(object==0)
    return 0;

  GLhandleARB po = 0;
  po = glCreateProgramObjectARB();

  for(unsigned int i = 0; i < nb; ++i){
    if(object[i]>0)
      glAttachObjectARB(po,object[i]);
  }
  
  glLinkProgramARB(po);

  int status;
  glGetObjectParameterivARB(po, GL_OBJECT_LINK_STATUS_ARB, &status);
  if(status==0){
    int length = 0;
    glGetObjectParameterivARB(po, GL_OBJECT_INFO_LOG_LENGTH_ARB, &length);
    if(length>0){
      GLsizei minlength = min(MAX_LOG_STRING,length);
      glGetInfoLogARB(po, minlength, 0, logstring);
      std::cerr << logstring << std::endl;
    }
    return 0;
  }
  return po;
}
/*
void Shader::PrintSupportedExtensions()
{
	std::stringstream extensions = std::stringstream((char*)(glGetString(GL_EXTENSIONS)));
	std::cout << "Supported extensions :" << std::endl;
	while(!extensions.eof()) {
		std::string str("end");
		extensions >> str;
		std::cout << "- " << str << std::endl;
	}

}

bool Shader::isExtensionSupported(const std::string& ext)
{  
	std::string extensions = std::string((char*)(glGetString(GL_EXTENSIONS)));
	if(extensions.find(ext) != std::string::npos)
		return true;
	std::cerr << "WARNING : Extension \"" << ext << "\" is not supported" << std::endl;
	return false;
}
*/

bool Shader::Init()
{
	s_bInitialized = true;
	s_pCurrent = NULL;

	if(! glInfo::GetSingleton().isExtensionSupported("GL_ARB_shading_language_100") )
		s_bInitialized = false;

	return s_bInitialized;
}


bool Shader::Load(const std::string& name, const std::string& extensions)
{
	if(!s_bInitialized)	
		if(!Init())
			return false;

	if(!extensions.empty()) {
		std::stringstream ss( extensions );
		std::string it;
		while(std::getline(ss, it, ' '))
		{
			glInfo::GetSingleton().isExtensionSupported(it);
		}
	}

	m_strName = name;

	GLhandleARB so[3];
	memset(so, 0, sizeof(GLhandleARB)*3);

	// Set up path for shaders
	char szPath[MAX_PATH+1];

	GetCurrentDirectory(MAX_PATH, szPath);				// Get Our Working Directory
	strcat_s(szPath, "\\Data\\Shaders\\");				// Append "\\Data\\Shaders\\" After The Working Directory
	strcat_s(szPath, name.c_str());						// Append The PathName

	// convert back to string
	std::string tempName;
	tempName = szPath;

	std::string s1 = tempName+".vert";
	so[0] = loadShader(s1);
	if(so[0]==0){
		std::cerr << "[Error] Coading shader "+s1+" failed (exiting...)" << std::endl;
		return false;
	}
	if(!compileShader(so[0])){
		std::cerr << "[Error] Compiling shader "+s1+" failed (exiting...)" << std::endl;
		return false;
	}

	std::string s2 = tempName+".frag";
	so[1] = loadShader(s2);
	if(so[1]==0){
		std::cerr << "[Error] Loading shader "+s2+" failed (exiting...)" << std::endl;
		return false;
	}
	if(!compileShader(so[1])){
		std::cerr << "[Error] Compiling shader "+s2+" failed (exiting...)" << std::endl;
		return false;
	}


	m_nProgram = linkShaders(so,2);
	glDeleteObjectARB(so[0]);
	glDeleteObjectARB(so[1]);


	return true;
}


bool Shader::Activate()
{
	if(!m_nProgram) {
		std::cerr << "[Error] Using the shader "+m_strName+" with no program." << std::endl;
		return false;
	}

	s_pCurrent = this;
	glUseProgramObjectARB(m_nProgram);
	return true;
}

void Shader::Deactivate()
{
	s_pCurrent = NULL;
	glUseProgramObjectARB(0);
}


// The shader must be activated before
void Shader::UniformTexture(const std::string& ext, GLint slot)
{
	glActiveTexture(GL_TEXTURE0+slot);
//	if(pTex) pTex->Bind(slot);
	int id = glGetUniformLocation(m_nProgram, ext.c_str());
	glUniform1iARB(id, slot);
}

// The shader must be activated before
void Shader::Uniform(const std::string& ext, GLint value)
{
	int id = glGetUniformLocation(m_nProgram, ext.c_str());
	glUniform1iARB(id, value);
}

// The shader must be activated before
void Shader::Uniform(const std::string& ext, GLfloat value)
{
	int id = glGetUniformLocation(m_nProgram, ext.c_str());
	glUniform1fARB(id, value);
}

// The shader must be activated before
void Shader::Uniform(const std::string& ext, const vec2& value)
{
	int id = glGetUniformLocation(m_nProgram, ext.c_str());
	glUniform2fvARB(id, 1, value);
}


// The shader must be activated before
void Shader::Uniform(const std::string& ext, const vec3& value)
{
	int id = glGetUniformLocation(m_nProgram, ext.c_str());
	glUniform3fvARB(id, 1, value);
}
