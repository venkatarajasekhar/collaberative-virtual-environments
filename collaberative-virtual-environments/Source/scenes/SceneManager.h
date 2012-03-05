#ifndef SCENEMANAGER_H
#define SCENEMANAGER_H

#include <map>
#include <hash_map>

#include "../utilities/Singleton.h"
#include "SceneBase.h"

class ISceneBase;

SINGLETON_BEGIN( SceneManager )
public:
	SceneManager();

	void Init();
	void Destroy();
	void Idle(float fElapsedTime);
	void Keyboard();
	void PreRender();
	void Render();

	bool setCurrent(const std::string& name);
	ISceneBase* getScenePointer(const std::string& name);
	inline ISceneBase* getCurrentScenePointer()	const {return m_pCurrentScene;}
	
private:
	std::map<std::string, ISceneBase*>	m_SceneDB;			// All the scenes
	ISceneBase*							m_pCurrentScene;	// Pointer to the current scene

SINGLETON_END()

#endif

