#include "TerrainObject.h"
#include "../../utilities/ResourceManager.h"

TerrainObject::TerrainObject(TYPE mesh, vec4 tr)
{
	SINGLETON_GET( ResourceManager, res)

	switch(mesh) {
	case PALM:
		m_tMesh.push_back((Mesh*)res.LoadResource(ResourceManager::MESH, "palm_lod0.3d"));
		m_tMesh.push_back((Mesh*)res.LoadResource(ResourceManager::MESH, "palm_lod1.3d"));
		m_tMesh.push_back((Mesh*)res.LoadResource(ResourceManager::MESH, "palm_lod2.3d"));
		break;
	case PALM2:
		m_tMesh.push_back((Mesh*)res.LoadResource(ResourceManager::MESH, "palm2.3d"));
		// TODO : LOD for tree
		break;
	default:
		assert(0);
	}

	m_vPosition = vec3(tr);
	m_fAngle = tr.a;
}

void TerrainObject::Destroy()
{
	m_tMesh.clear();
}

void TerrainObject::Draw(unsigned int id)
{
	assert(id<m_tMesh.size());

	glPushMatrix();
	glTranslatef(m_vPosition.x, m_vPosition.y, m_vPosition.z);
	// TODO Fix Scale
	glScalef(0.2f, 0.2f, 0.2f);
	glRotatef(m_fAngle, 0.0f, 1.0f, 0.0f);
	m_tMesh[id]->Draw();
	glPopMatrix();
}



