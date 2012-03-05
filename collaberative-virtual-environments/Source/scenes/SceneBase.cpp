#ifdef WIN32
	#define WIN32_LEAN_AND_MEAN
	#include <WinSock2.h> // Has to go here to prevent windows.h conflicts.
	#include <Windows.h>
#endif

#include <assert.h>
#include <GL\glew.h>
#include <fstream>
#include <sstream>

#include "SceneBase.h"
#include "../utilities/VarManager.h"
#include "../graphics/SingleCamera.h"
//#include "../../Engine/InputTask.h"

ISceneBase::ISceneBase() 
{
	m_pCamEyeTraj = new SplineGL();
	m_pCamLookAtTraj = new SplineGL();
}

void ISceneBase::Reset()
{
	m_pCamEyeTraj->setTime(0.0f);
	m_pCamLookAtTraj->setTime(0.0f);
	VarManager::GetSingleton().set("enable_underwater", false);
}

void ISceneBase::DrawAxes()
{
	glPushAttrib(GL_ENABLE_BIT);
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_LIGHTING);

	glBegin(GL_LINES);
		glColor3f(1.0f, 0.0f, 0.0f);
		glVertex3f(0.0f, 0.0f, 0.0f);
		glVertex3f(1.0f, 0.0f, 0.0f);

		glColor3f(0.0f, 1.0f, 0.0f);
		glVertex3f(0.0f, 0.0f, 0.0f);
		glVertex3f(0.0f, 1.0f, 0.0f);

		glColor3f(0.0f, 0.0f, 1.0f);
		glVertex3f(0.0f, 0.0f, 0.0f);
		glVertex3f(0.0f, 0.0f, 1.0f);
	glEnd();
	glPopAttrib();
}

void ISceneBase::DrawTraj()
{
	assert(m_pCamEyeTraj);
	assert(m_pCamLookAtTraj);

	glColor3f(1.0f, 0.0f, 0.0f);
	m_pCamEyeTraj->DrawGL();

	glColor3f(0.0f, 1.0f, 0.0f);
	m_pCamLookAtTraj->DrawGL();
}


void ISceneBase::InterpolCameraTraj(float fElapsedTime)
{
	assert(m_pCamEyeTraj);
	assert(m_pCamLookAtTraj);

	if(m_pCamEyeTraj->isValid() && m_pCamLookAtTraj->isValid()) {
		Camera& cam = Camera::GetSingleton();
		m_pCamEyeTraj->Idle(fElapsedTime);
		m_pCamLookAtTraj->Idle(fElapsedTime);
		cam.setEye(m_pCamEyeTraj->getPoint());
		cam.setCenter(m_pCamLookAtTraj->getPoint());
	}
}


void ISceneBase::DrawString(vec2 pos, void* font, const std::string& str)
{

}


void ISceneBase::Destroy()
{
	if(m_pCamEyeTraj) delete m_pCamEyeTraj;
	m_pCamEyeTraj = NULL;
	if(m_pCamLookAtTraj) delete m_pCamLookAtTraj;
	m_pCamLookAtTraj = NULL;
}


bool ISceneBase::LoadCameraTrajFromFile(const std::string& name)
{
	assert(m_pCamEyeTraj);
	assert(m_pCamLookAtTraj);

	m_pCamEyeTraj->Clear();
	m_pCamLookAtTraj->Clear();

	// Set up path for splines
	char szPath[MAX_PATH+1];

	GetCurrentDirectory(MAX_PATH, szPath);				// Get Our Working Directory
	strcat_s(szPath, "\\Data\\Splines\\");				// Append "\\Data\\Splines\\" After The Working Directory
	strcat_s(szPath, name.c_str());						// Append The PathName

	// convert back to string
	std::string filename;
	filename = szPath;

	std::ifstream fp(filename.c_str());
	if(!fp) return false;

	float speed = 0.01f;
	std::string str;
	std::string strBuffer;
	std::getline(fp, strBuffer);
	if(strBuffer.size() > 3) {
		std::stringstream strStream(strBuffer);
		float f;
		strStream >> str >> f;
		if(str == "SPEED")
			speed = f;
	}

	while(std::getline(fp, strBuffer)) {
		if(strBuffer.size() > 3) {
			std::stringstream strStream(strBuffer);
			
			vec3 point;
			strStream >> str >> point.x >> point.y >> point.z;

			if(str == "EYE")			m_pCamEyeTraj->AddPoint(point);
			else if(str == "LOOKAT")	m_pCamLookAtTraj->AddPoint(point);
		}
	}

	m_pCamEyeTraj->BuildSplines(false, speed);
	m_pCamLookAtTraj->BuildSplines(false, speed);

	fp.close();

	return true;
}


bool ISceneBase::SaveCameraTrajInFile(const std::string& name)
{
	assert(m_pCamEyeTraj);
	assert(m_pCamLookAtTraj);

	// Set up path for splines
	char szPath[MAX_PATH+1];

	GetCurrentDirectory(MAX_PATH, szPath);				// Get Our Working Directory
	strcat_s(szPath, "\\Data\\Splines\\");				// Append "\\Data\\Splines\\" After The Working Directory
	strcat_s(szPath, name.c_str());						// Append The PathName

	// convert back to string
	std::string filename;
	filename = szPath;

	std::ofstream fp(filename.c_str());
	if(!fp) return false;

	fp << "SPEED " << m_pCamEyeTraj->getSpeed() << std::endl;

	for(int i=0; i<(int)m_pCamEyeTraj->getControlPoints().size(); i++) {
		fp << "EYE"	<< " " << m_pCamEyeTraj->getControlPoints()[i].x
					<< " " << m_pCamEyeTraj->getControlPoints()[i].y
					<< " " << m_pCamEyeTraj->getControlPoints()[i].z << std::endl;

		if(i<(int)m_pCamLookAtTraj->getControlPoints().size()) {
			fp << "LOOKAT"	<< " " << m_pCamLookAtTraj->getControlPoints()[i].x
							<< " " << m_pCamLookAtTraj->getControlPoints()[i].y
							<< " " << m_pCamLookAtTraj->getControlPoints()[i].z << std::endl;
		}

		fp << std::endl;
	}

	fp.close();

	return true;
}


void ISceneBase::Keyboard()
{
	int key = NULL;
/*
	if(InputTask::keyDown(key)) {
		switch(key) {
		case SDLK_e:
			m_pCamEyeTraj->AddPoint( Camera::GetSingleton().getEye() );
			m_pCamEyeTraj->BuildSplines(false);
			SaveCameraTrajInFile("out.txt");
			break;

		case SDLK_r:
			m_pCamEyeTraj->DeleteLastPoint();
			m_pCamEyeTraj->BuildSplines(false);
			SaveCameraTrajInFile("out.txt");
			break;

		case SDLK_f:
			m_pCamLookAtTraj->AddPoint( Camera::GetSingleton().getEye() );
			m_pCamLookAtTraj->BuildSplines(false);
			SaveCameraTrajInFile("out.txt");
			break;

		case SDLK_g:
			m_pCamLookAtTraj->DeleteLastPoint();
			m_pCamLookAtTraj->BuildSplines(false);
			SaveCameraTrajInFile("out.txt");
			break;
		}
	}*/
}



