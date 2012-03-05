#include <string>
#include <iostream>
#include <map>

#include "ResourceManager.h"

void ResourceManager::Destroy()
{
	for(std::map<std::string, void*>::iterator it = m_ResDB.begin(); it != m_ResDB.end(); it++) {
		delete (*it).second;
		(*it).second = NULL;
	}
	m_ResDB.clear();
}


void* ResourceManager::LoadResource(RES_TYPE type, const std::string& name)
{
	// The resource is already loaded, dont change it
	if(m_ResDB.find(name) != m_ResDB.end())
		return m_ResDB.find(name)->second;

	std::cerr << "Loading resource " << name << std::endl;

	// Resource is loaded
	void* ptr = NULL;
	switch(type) {
	case TEXTURE2D: {
		ptr = new CTexture2D();
		((CTexture2D*)ptr)->Load(name);
		break;}
	case TEXTURECUBEMAP: {
		ptr = new CTextureCubemap();
		((CTextureCubemap*)ptr)->Load(name);
		break;}
	//case SHADER: {
	//	ptr = new Shader();
	//	((Shader*)ptr)->Load(name);
	//	break;}
	//case MESH: {
	//	ptr = new Mesh();
	//	((Mesh*)ptr)->Load(name);
	//	break;}
	}

	if(!ptr) return NULL;

	m_ResDB[name] = ptr;
	return ptr;
}

void* ResourceManager::NewResource(void* data, const std::string& name)
{
	if(!data) return NULL;

	// The resource is already loaded, dont change it
	if(m_ResDB.find(name) != m_ResDB.end())
		return m_ResDB.find(name)->second;

	m_ResDB[name] = data;
	return data;
}


