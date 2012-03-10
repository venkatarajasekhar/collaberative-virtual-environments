#include <assert.h>

#include "Quadtree.h"

int Quadtree::DrawGround(bool bReflection) {
	assert(m_pRoot);
	int options = CHUNK_BIT_TESTCHILDREN;
	if(bReflection)
		options |= CHUNK_BIT_WATERREFLECTION;
	return m_pRoot->DrawGround(&Frustum::GetSingleton(), options);
}

int Quadtree::DrawGrass(bool bReflection)
{
	assert(m_pRoot);
	return m_pRoot->DrawGrass(bReflection);
}

int Quadtree::DrawObjects(bool bReflection)
{
	assert(m_pRoot);
	return m_pRoot->DrawObjects(bReflection);
}


void Quadtree::DrawBBox() {
	assert(m_pRoot);
	m_pRoot->DrawBBox(true);
}


QuadtreeNode* Quadtree::FindLeaf(vec2& pos)
{
	assert(m_pRoot);
	QuadtreeNode* node = m_pRoot;

	while(!node->isALeaf()) {
		int i=0;
		for(i=0; i<4; i++) {
			QuadtreeNode* child = &(node->getChildren()[i]);
			if(child->getBoundingBox().ContainsPoint( vec3(pos.x, child->getBoundingBox().getCenter().y, pos.y) ))
			{
				node = child;
				break;
			}
		}

		if(i>=4) {
			return NULL;
		}
	}

	return node;
}


void Quadtree::Build(BoundingBox* pBBox,		// Field bounding box
					 ivec2 HMsize,				// Length and width of heightmap
					 unsigned int minHMSize)	// Minimum chunk size
{
	assert(pBBox);

	// Create root node
	m_pRoot = new QuadtreeNode();
	m_pRoot->setBoundingBox(*pBBox);

	// Recursive function to create nodes
	m_pRoot->Build(0, ivec2(0,0), HMsize, minHMSize);
}

void Quadtree::ComputeBoundingBox(const vec3* vertices)
{
	assert(m_pRoot);
	assert(vertices);

	// Recursive function for calculating bounding box
	m_pRoot->ComputeBoundingBox(vertices);

}


void Quadtree::Destroy()
{
	if(m_pRoot) {
		m_pRoot->Destroy();
		delete m_pRoot;
		m_pRoot = NULL;
	}
}