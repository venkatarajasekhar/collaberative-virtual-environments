#ifndef VBO_H
#define VBO_H

#include <GL/glew.h>
#include <vector>

#include "../utilities/Math.h"


class VertexBufferObject
{
public:
	bool		Create(GLenum usage);
	void		Destroy();

	// Activate and deactivate the VBO
	void		Enable();
	void		Disable();

	// Update any changes made to VBO Data buffers
	bool	 Update(GLenum usage);		

	// Data buffers
	inline std::vector<vec3>&	getPosition()	{return m_tDataPosition;}
	inline std::vector<vec3>&	getNormal()		{return m_tDataNormal;}
	inline std::vector<vec2>&	getTexcoord()	{return m_tDataTexcoord;}
	inline std::vector<vec3>&	getTangent()	{return m_tDataTangent;}

	VertexBufferObject();
	~VertexBufferObject() {Destroy();}


private:
	void Enable_VA();	// Activate Vertex Array
	void Enable_VBO();	// Activate Vertex Buffer Object
	void Disable_VA();	// Deactivate Vertex Array
	void Disable_VBO();	// Deactivate Vertex Buffer Object

private:
	// VBO ID's
	GLuint		m_nVBOid;
	GLintptr	m_nVBO_OffsetPosition;
	GLintptr	m_nVBO_OffsetNormal;
	GLintptr	m_nVBO_OffsetTexcoord;
	GLintptr	m_nVBO_OffsetTangent;

public:
	// Data
	std::vector<vec3>	m_tDataPosition;
	std::vector<vec3>	m_tDataNormal;
	std::vector<vec2>	m_tDataTexcoord;
	std::vector<vec3>	m_tDataTangent;
};

#endif

