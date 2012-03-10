#ifndef TERRAINCHUNK_H
#define TERRAINCHUNK_H

#include <GL\glew.h>
#include <vector>

#include "../../utilities/math.h"

#define TERRAIN_CHUNKS_LOD	3

#define TERRAIN_CHUNK_LOD0	100.0f		// 100
#define TERRAIN_CHUNK_LOD1	180.0f		// 180

#define TERRAIN_GRASS_MAX_DISTANCE	800.0f	// 80

class TerrainObject;

/**
 * Terrain Chunk
 *
 */
class TerrainChunk
{
public:
	void Destroy();
	int  DrawGround(GLuint lod);
	int  DrawGrass(GLuint lod, float d);
	int  DrawObjects(GLuint lod);

	void Load(unsigned int depth, ivec2 pos, ivec2 HMsize);

	inline std::vector<GLuint>&				getIndiceArray(GLuint lod)	{return m_tIndice[lod];}
	inline std::vector<GLuint>&				getGrassIndiceArray()		{return m_tGrassIndice;}
	inline std::vector<TerrainObject*>&		getObjectsArray()			{return m_tObject;}
	void									addObject(TerrainObject* obj);

	TerrainChunk() {}
	~TerrainChunk() {Destroy();}

private:
	void ComputeIndicesArray(unsigned int lod, unsigned int depth, ivec2 pos, ivec2 HMsize);
	
private:
	// Indices for the terrain
	std::vector<GLuint>		m_tIndice[TERRAIN_CHUNKS_LOD];
	GLuint					m_tIndOffsetW[TERRAIN_CHUNKS_LOD];
	GLuint					m_tIndOffsetH[TERRAIN_CHUNKS_LOD];

	// Indices for the grass
	std::vector<GLuint>		m_tGrassIndice;

	// Objets
	std::vector<TerrainObject*>	m_tObject;

};
#endif

