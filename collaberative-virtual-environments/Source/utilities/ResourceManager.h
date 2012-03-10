#ifndef RESOURCEMANAGER_H
#define RESOURCEMANAGER_H

#include <map>
#include <assert.h>

#include "singleton.h"
#include "../graphics/texture/texture2d.h"
#include "../graphics/texture/TextureCubemap.h"
#include "../graphics/Shader.h"
#include "../graphics/Mesh.h"

SINGLETON_BEGIN(ResourceManager)
public:
	enum RES_TYPE {TEXTURE2D, TEXTURECUBEMAP, MESH, SHADER};

	void* LoadResource(RES_TYPE type, const std::string& name);
	void* NewResource(void* data, const std::string& name);

	inline void*			getResource(const std::string& name)		{assert(m_ResDB.find(name)!=m_ResDB.end()); return m_ResDB.find(name)->second;}
	inline Texture2D*		getTexture2D(const std::string& name)		{return (Texture2D*)getResource(name);}
	inline TextureCubemap*	getTextureCubemap(const std::string& name)	{return (TextureCubemap*)getResource(name);}
	inline Mesh*			getMesh(const std::string& name)			{return (Mesh*)getResource(name);}
	inline Shader*			getShader(const std::string& name)			{return (Shader*)getResource(name);}

	void Destroy();

	~ResourceManager() {Destroy();}

private:
	std::map<std::string, void*>	m_ResDB;
SINGLETON_END()

#endif