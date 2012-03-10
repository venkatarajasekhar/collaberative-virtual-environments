#include "SceneTerrain.h"	

#include "terrain/Terrain.h"

#include "../graphics/SingleCamera.h"
#include "../utilities/ResourceManager.h"
#include "../utilities/VarManager.h"
#include "../tasks/InputTask.h"
#include "../tasks/GlobalTimer.h"

void SceneTerrain::Init()
{
	SINGLETON_GET( VarManager, var )
	SINGLETON_GET( ResourceManager, res )

	m_pShaderTerrain	= (Shader*)res.LoadResource(ResourceManager::SHADER, "terrain_ground");

	// These are textures used to render the final terrain
	m_tTextures.push_back( (Texture2D*)res.LoadResource( ResourceManager::TEXTURE2D, "terrain_detail_NM.tga") );
	m_tTextures.push_back( (Texture2D*)res.LoadResource( ResourceManager::TEXTURE2D, "terrain_sand_512.jpg") );
	m_tTextures.push_back( (Texture2D*)res.LoadResource( ResourceManager::TEXTURE2D, "terrain_rocky_1024.png") );
	m_tTextures.push_back( (Texture2D*)res.LoadResource( ResourceManager::TEXTURE2D, "terrain_grass_1024.png") );
	
	m_pTerrainDiffuseMap = (Texture2D*)res.LoadResource( ResourceManager::TEXTURE2D, "heightmap_1024_diffusemap.jpg");

	m_pTerrain = new Terrain();
	assert(m_pTerrain != NULL);
	BoundingBox bbox( vec3(-300.0f, 0.0f, -300.0f), vec3(300.0f, 40.0f, 300.0f) );
	m_pTerrain->Load("heightmap_1024.jpg", bbox, 16);

	ImageTools::ImageData img;
	ImageTools::OpenImage("heightmap_1024.jpg", img);
	m_pTerrain->ComputeBoundingBox();
	img.Destroy();

	vec3 fogColor(0.7f, 0.7f, 0.9f);
	m_pShaderTerrain->Activate();
		m_pShaderTerrain->Uniform("bbox_min", m_pTerrain->getBoundingBox().min);
		m_pShaderTerrain->Uniform("bbox_max", m_pTerrain->getBoundingBox().max);
		m_pShaderTerrain->Uniform("fog_color", fogColor);
	m_pShaderTerrain->Deactivate();
}

void SceneTerrain::Destroy()
{
	m_pTerrain->Destroy();
	delete m_pTerrain;
	m_tTextures.clear();

	ISceneBase::Destroy();
}

void SceneTerrain::Idle(float fElapsedTime)
{

}

void SceneTerrain::Keyboard()
{
	ISceneBase::Keyboard();
}

void SceneTerrain::Reset()
{
	Camera::GetSingleton().setEye(vec3(0.0f, 0.0f, 0.0f));

	ISceneBase::Reset();
}

void SceneTerrain::PreRender()
{
}

void SceneTerrain::Render()
{
	SINGLETON_GET( VarManager, var )

	// Render stage as normal
	RenderEnvironment();

}

// Rendering of the Environment (for the reflection the water or not)
void SceneTerrain::RenderEnvironment()
{
	SINGLETON_GET( ResourceManager, res )
	SINGLETON_GET( VarManager, var )
	SINGLETON_GET( Camera, cam )

	glPushAttrib(GL_ENABLE_BIT);			// Save state


	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	vec3 zeros(0.0f, 0.0f, 0.0f);
	vec3 position( 50.0f, 50.0f, 50.0f );
	vec3 white( 1.0f, 1.0f, 1.0f );
	vec3 other( 0.7f, 0.7f, 0.7f );
	glLightfv(GL_LIGHT0, GL_SPOT_DIRECTION,	zeros);
	glLightfv(GL_LIGHT0, GL_POSITION,		position);
	glLightfv(GL_LIGHT0, GL_AMBIENT,		white);
	glLightfv(GL_LIGHT0, GL_DIFFUSE,		other);
	glLightfv(GL_LIGHT0, GL_SPECULAR,		other);

	vec4 vGroundAmbient = other;
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT,		vGroundAmbient);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE,		white);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR,	white);
	glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS,	50.0f);

	glMatrixMode(GL_TEXTURE);
	glActiveTexture(GL_TEXTURE0);
	glMatrixMode(GL_MODELVIEW);

	// Bind the textures
	GLuint idx=0;
	for(GLuint i=0; i<m_tTextures.size(); i++)
		m_tTextures[i]->Bind(idx++);
	m_pTerrainDiffuseMap->Bind(idx++);


	// Render terrain
	m_pShaderTerrain->Activate();
	{
		m_pShaderTerrain->Uniform("detail_scale", 120.0f);
		m_pShaderTerrain->Uniform("diffuse_scale", 70.0f);

		m_pShaderTerrain->Uniform("water_height", var.getf("water_height"));

		m_pShaderTerrain->Uniform("time", GlobalTimer::totalTime);

		m_pShaderTerrain->UniformTexture("texNormalHeightMap", 0);
		m_pShaderTerrain->UniformTexture("texDiffuse0", 1);
		m_pShaderTerrain->UniformTexture("texDiffuse1", 2);
		m_pShaderTerrain->UniformTexture("texDiffuse2", 3);
		m_pShaderTerrain->UniformTexture("texDiffuseMap", 4);

		int ret = m_pTerrain->DrawGround(false);
		var.set("terrain_chunks_drawn", ret);

		m_pTerrain->DrawInfinitePlane(Camera::GetSingleton().getEye(), 2.0f*var.getf("cam_zfar"));
	}
	m_pShaderTerrain->Deactivate();


	// Unbind the textures.
	m_pTerrainDiffuseMap->Unbind(--idx);
	for(GLint i=(GLint)m_tTextures.size()-1; i>=0; i--)
		m_tTextures[i]->Unbind(--idx);
		

	glMatrixMode(GL_TEXTURE);
	glActiveTexture(GL_TEXTURE0);
	glLoadIdentity();
	glMatrixMode(GL_MODELVIEW);

	glPopAttrib();
}