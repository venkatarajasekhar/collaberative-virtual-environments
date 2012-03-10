#ifndef QUADTREE_H
#define QUADTREE_H

#include "QuadtreeNode.h"

class QuadtreeNode;
class BoundingBox;

/**
 * Quadtree
 * Used to split the terrain into quads for efficiency.
 */
class Quadtree {
public:
	void Build(BoundingBox* pBBox, ivec2 HMSize, unsigned int minHMSize);
	void ComputeBoundingBox(const vec3* vertices);
	void Destroy();

	int  DrawGround(bool bReflection);
	int  DrawGrass(bool bReflection);
	int  DrawObjects(bool bReflection);
	void DrawBBox();

	QuadtreeNode*	FindLeaf(vec2& pos);

	Quadtree()	{m_pRoot = NULL;}
	~Quadtree()	{Destroy();}

private:
	QuadtreeNode*	m_pRoot;	// Root node of quadtree
};

#endif

