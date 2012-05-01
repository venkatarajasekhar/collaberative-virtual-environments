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
	// Matrices are placed and the camera point of view of light for pre-made
	void SetLightCameraMatrices();
	void RestoreLightCameraMatrices();

	// Rendering of the Environment (for reflection of the water not)
	void RenderEnvironment(bool bWaterReflection, bool bDepthMap);

	// Render the surface of the water
	void RenderWaterSurface();

public:
	inline void	setSunAngle(const vec2& a)	{m_vSunAngle = a;}


private:
	Terrain*				m_pTerrain;
	vec2					m_vSunAngle;
	vec4					m_vSunVector;
	mat4					m_matSunModelviewProj;

	TextureCubemap*			m_pSkybox;
	TextureCubemap*			m_pNightbox;

	Shader*					m_pShaderLighting;
	Shader*					m_pShaderTerrain;
	Shader*					m_pShaderWater;
	Shader*					m_pShaderGrass;
	Shader*					m_pShaderTree;
	Texture2D*				m_pTerrainDiffuseMap;
	std::vector<Texture2D*>	m_tTextures;

	Texture2D*				m_pTexWaterNoiseNM;
	FrameBufferObject		m_fboWaterReflection;

	FrameBufferObject		m_fboDepthMapFromLight[TERRAIN_SHADOWMAPS_COUNT];

public:
	Player		Mark;		// TODO : Create a player manager when networking is in.
};

#endif
