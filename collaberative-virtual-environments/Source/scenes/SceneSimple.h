#ifndef SCENESIMPLE_H
#define SCENESIMPLE_H

#include "SceneBase.h"

#include "../graphics/texture/texture2d.h"

#include <GL\glew.h>


class CTexture2D;

class SceneSimple : public ISceneBase
{
public:
	// Init scene specific data
	virtual void Init();

	// Destroy scene specific data
	virtual void Destroy();

	// Extra update function, more for tidiness.
	virtual void Idle(float fElapsedTime);

	// Reset the scene data
	virtual void Reset();

	// Render the scene
	virtual void Render();



	// Teapot stuff -----------------
	void myTeapot( GLint grid, GLdouble scale, GLenum type );
	void myWireTeapot( GLdouble size );
	void mySolidTeapot( GLdouble size );

private:
	// Here we put the data specific to the scene
	CTexture2D*	m_pMyTex;
	float		m_fAngle;
};

#endif

