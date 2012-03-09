#ifndef MESH_H
#define MESH_H

#include <GL\glew.h>

#include "BoundingBox.h"
#include "VertexBufferObject.h"

class VertexBufferObject;

// -------------------------------
// Objet 3D
// Load from model type .obj
// -------------------------------

class Mesh
{
public:
	bool Load(const std::string& name);
	void Destroy();

	void Draw();
	void Draw(GLuint group);

	inline GLuint		getGroupCount()		const	{return (GLuint)m_tGroup.size();}
	inline BoundingBox&	getBoundingBox()			{return m_BBox;}

	static void EnableComputeNormals(bool b) {s_bComputeNormals=b;}

	Mesh();
	~Mesh() {Destroy();}
private:
	bool LoadOBJ(const std::string& filename);

	bool ComputeVBO();
	void ComputeNormals();
	void ComputeTangents();
	void ComputeBoundingBox();

private:
	struct sFace {
		GLuint ind[3];
	};

	struct sGroup {
		std::string			strName;
		long				nMaterial;
		std::vector<sFace>	tFace;
	};

private:
	std::vector<sGroup>		m_tGroup;

	BoundingBox				m_BBox;

	// VBO
	VertexBufferObject*		m_pVBO;

	static bool s_bComputeNormals;	// Comput normals or not

};

#endif

