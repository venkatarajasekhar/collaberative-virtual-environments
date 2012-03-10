#ifndef TERRAIN_H
#define TERRAIN_H

#include "../../graphics/texture/ImageTools.h"
#include "../../graphics/VertexBufferObject.h"
#include "Quadtree.h"

class Quadtree;
class VertexBufferObject;
class TerrainObject;

/**
 * Terrain
 * All code to create and draw the terrain is in here.
 * Including texturing and objects on it as well as helper
 * functions to assist in rendering terrain.
 */
class Terrain
{
public:
	void Load(const std::string& heightmap, const BoundingBox& bbox, GLuint chunkSize=64);
	bool GenerateGrass(const ImageTools::ImageData& map, unsigned int density);
	bool GenerateVegetation(const ImageTools::ImageData& map, unsigned int density);
	bool ComputeBoundingBox();

	void Destroy();
	int  DrawGround(bool bReflection);
	int  DrawGrass(bool bReflection);
	int  DrawObjects(bool bReflection);
	void DrawInfinitePlane(const vec3& eye, float max_distance);

	vec3  getPosition(float x_clampf, float z_clampf) const;
	vec3  getNormal(float x_clampf, float z_clampf) const;
	vec3  getTangent(float x_clampf, float z_clampf) const;

	const BoundingBox&	getBoundingBox()	{return m_BBox;}

	Terrain();
	~Terrain() {Destroy();}

private:

	
private:
	BoundingBox				m_BBox;
	GLuint					m_nHMWidth, m_nHMHeight;

	// Quadtree to manage data
	Quadtree*				m_pQuadtree;

	// VBO
	VertexBufferObject*		m_pGroundVBO;
	VertexBufferObject*		m_pGrassVBO;


};

#endif

