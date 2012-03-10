#ifndef TERRAINOBJECT_H
#define TERRAINOBJECT_H

#include <vector>

#include "../../utilities/math.h"

class Mesh;


/**
 * Terrain Object
 * Objects which can be placed onto the terrain. (.obj)
 */
class TerrainObject
{
public:
	enum TYPE {PALM, PALM2};

	void Draw(unsigned int id = 0);
	void Destroy();

	inline Mesh*		getMesh(unsigned int id = 0)	const {return m_tMesh[id];}
	inline const vec3&	getPosition()					const {return m_vPosition;}

	TerrainObject(TYPE mesh, vec4 tr);
	~TerrainObject() {Destroy();}

private:
	vec3				m_vPosition;
	float				m_fAngle;
	std::vector<Mesh*>	m_tMesh;

};

#endif

