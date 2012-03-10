#ifndef QUADTREENODE
#define QUADTREENODE

#include "../../graphics/BoundingBox.h"
#include "../../graphics/Frustum.h"
#include "TerrainChunk.h"
#include "TerrainObject.h"

#define CHILD_NW	0
#define CHILD_NE	1
#define CHILD_SW	2
#define CHILD_SE	3

#define CHUNK_BIT_TESTCHILDREN		0x1
#define CHUNK_BIT_WATERREFLECTION	0x2


class Frustum;
class TerrainChunk;


/**
 * Quadtree Node
 * A quadtree node has 4 children.
 */
class QuadtreeNode {
public:
	// recursive function of processing nodes
	void Build(unsigned int depth, ivec2 pos, ivec2 HMsize, unsigned int minHMSize);
	void ComputeBoundingBox(const vec3* vertices);
	void Destroy();

	int  DrawGround(Frustum* pFrust, int options);
	int  DrawGrass(bool bReflection);
	int  DrawObjects(bool bReflection);
	void DrawBBox(bool bTest);

	inline bool isALeaf() const							{return m_pChildren==0;}
	inline const BoundingBox&	getBoundingBox()		{return m_BBox;}
	inline void setBoundingBox(const BoundingBox& bbox)	{m_BBox = bbox;}
	inline QuadtreeNode*	getChildren()				{return m_pChildren;}
	inline TerrainChunk*	getChunk()					{return m_pTerrainChunk;}

	QuadtreeNode()  {m_pChildren = NULL; m_pTerrainChunk = NULL; m_nLOD = 0;}
	~QuadtreeNode() {Destroy();}

private:
	int				m_nLOD;				// Level of detail
	float			m_fDistance;		// Distance from the center of the camera
	BoundingBox		m_BBox;				// Size of the 3D BBOX including the node
	QuadtreeNode*	m_pChildren;		// Children of node
	TerrainChunk*	m_pTerrainChunk;	// Chunk (part of the land) if it is a leaf
};
#endif
