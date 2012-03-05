#ifndef RESOURCEMANAGER_H
#define RESOURCEMANAGER_H

#include <map>
#include <assert.h>

#include "singleton.h"
#include "../graphics/texture/texture2d.h"
#include "../graphics/texture/TextureCubemap.h"
//#include "../Engine/Shader.h"
//#include "../Engine/Mesh.h"

SINGLETON_BEGIN(ResourceManager)
public:
	enum RES_TYPE {TEXTURE2D, TEXTURECUBEMAP, MESH, SHADER};

	void* LoadResource(RES_TYPE type, const std::string& name);
	void* NewResource(void* data, const std::string& name);

	inline void*			getResource(const std::string& name)		{assert(m_ResDB.find(name)!=m_ResDB.end()); return m_ResDB.find(name)->second;}
	inline CTexture2D*		getTexture2D(const std::string& name)		{return (CTexture2D*)getResource(name);}
	inline CTextureCubemap*	getTextureCubemap(const std::string& name)	{return (CTextureCubemap*)getResource(name);}
	//inline Mesh*			getMesh(const std::string& name)			{return (Mesh*)getResource(name);}
	//inline Shader*			getShader(const std::string& name)			{return (Shader*)getResource(name);}

	void Destroy();

	~ResourceManager() {Destroy();}

private:
	std::map<std::string, void*>	m_ResDB;
SINGLETON_END()

#endif