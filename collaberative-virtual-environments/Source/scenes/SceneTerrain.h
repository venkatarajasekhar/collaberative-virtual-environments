#ifndef SCENETERRAIN_H
#define SCENETERRAIN_H

#include "SceneBase.h"

#include "../graphics/texture/ImageTools.h"
#include "../graphics/texture/TextureCubemap.h"
#include "../graphics/FrameBufferObject.h"

#include "../player/player.h"

#define TERRAIN_SHADOWMAPS_COUNT 2

class Texture2D;
class Shader;
class Terrain;

class SceneTerrain : public ISceneBase
{
public:
	virtual void Init();
	virtual void Destroy();
	virtual void PreRender();
	virtual void Render();
	virtual void Idle(float fElapsedTime);
	virtual void Keyboard(float fElapsedTime);
	virtual void Reset();
	
private:
private:
	// Matrices are placed and the camera point of view of light for pre-made
	void SetLightCameraMatrices();
	void RestoreLightCameraMatrices();

	// Rendering of the Environment
	void RenderEnvironment(bool bDepthMap);

private:
	Terrain*				m_pTerrain;
	vec2					m_vSunAngle;
	vec4					m_vSunVector;
	mat4					m_matSunModelviewProj;

	TextureCubemap*			m_pSkybox;
	TextureCubemap*			m_pNightbox;

	Shader*					m_pShaderTerrain;
	Texture2D*				m_pTerrainDiffuseMap;
	std::vector<Texture2D*>	m_tTextures;

	FrameBufferObject		m_fboDepthMapFromLight[TERRAIN_SHADOWMAPS_COUNT];

public:
	Player		Mark;		// TODO : Create a player manager when networking is in.
};

#endif

