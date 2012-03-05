#include <assert.h>
#include <GL\glew.h>

#include "SceneManager.h"
#include "../graphics/SingleCamera.h"
#include "../scenes/Script.h"
#include "../utilities/VarManager.h"

#include "../Scenes/SceneSimple.h"
//#include "../Scenes/SceneTerrain.h"

SceneManager::SceneManager()
{

	//m_SceneDB["terrain"] = new SceneTerrain();
	m_SceneDB["simple"] = new SceneSimple();

	setCurrent("simple");
}

bool SceneManager::setCurrent(const std::string& name)
{
	
	std::map<std::string, ISceneBase*>::iterator it = m_SceneDB.find(name);
	if(it == m_SceneDB.end())
		return false;

	if(m_pCurrentScene != it->second)
	{
		m_pCurrentScene = it->second;
		assert(m_pCurrentScene != NULL);
		m_pCurrentScene->Reset();
	}
	return true;
}

ISceneBase* SceneManager::getScenePointer(const std::string& name)
{
	std::map<std::string, ISceneBase*>::iterator it = m_SceneDB.find(name);
	if(it == m_SceneDB.end())
		return NULL;
	return it->second;
}

void SceneManager::Init()
{
	if(!m_SceneDB.empty())
	{
		for(std::map<std::string, ISceneBase*>::iterator it = m_SceneDB.begin(); it != m_SceneDB.end(); it++)
			(*it).second->Init();
	}
	else
	{
		std::cerr << "ERROR: Scene Database is Empty!" << std::endl;
		assert("OH NOES!");
	}
}

void SceneManager::Destroy()
{
	for(std::map<std::string, ISceneBase*>::iterator it = m_SceneDB.begin(); it != m_SceneDB.end(); it++) {
		(*it).second->Destroy();
		delete (*it).second;
		(*it).second = NULL;
	}
}

void SceneManager::Idle(float fElapsedTime)
{
	if(Camera::GetSingletonPtr()->getType() == Camera::DRIVEN)
	{
		ScriptIdle(fElapsedTime);
		m_pCurrentScene->InterpolCameraTraj(fElapsedTime);
	}

	m_pCurrentScene->Idle(fElapsedTime);
}

void SceneManager::Keyboard()
{
	m_pCurrentScene->Keyboard();
}

void SceneManager::PreRender()
{
	glPushAttrib(GL_ENABLE_BIT);
	glMatrixMode(GL_MODELVIEW);
	m_pCurrentScene->PreRender();
	glPopAttrib();
}



void SceneManager::Render()
{
	VarManager& var = VarManager::GetSingleton();

	glPushAttrib(GL_ENABLE_BIT);
	glMatrixMode(GL_MODELVIEW);
	m_pCurrentScene->Render();

	if(var.getb("show_camera_splines"))
		m_pCurrentScene->DrawTraj();

	glPopAttrib();
}



