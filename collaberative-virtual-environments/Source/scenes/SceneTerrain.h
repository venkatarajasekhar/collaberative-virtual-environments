#ifndef SCENETERRAIN_H
#define SCENETERRAIN_H

#include "SceneBase.h"

#include "../graphics/texture/ImageTools.h"

#include "../player/player.h"

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
	// Rendering of the Environment
	void RenderEnvironment();

private:
	Terrain*				m_pTerrain;

	Shader*					m_pShaderTerrain;
	Texture2D*				m_pTerrainDiffuseMap;
	std::vector<Texture2D*>	m_tTextures;

public:
	Player		Mark;		// TODO : Create a player manager when networking is in.
};

#endif

