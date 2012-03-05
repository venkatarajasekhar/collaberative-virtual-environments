#ifndef SCENEBASE_H
#define SCENEBASE_H

#include "../utilities/math.h"
#include "Spline3D/SplineGL.h"

class SplineGL;

// Abstract base class for scenes
class ISceneBase
{
public:
	// Initialize the data specific to the scene
	virtual void Init() {}

	// Destruction of data specific to the scene
	virtual void Destroy();

	// Idle, might not need this, more of a helper update function.
	virtual void Idle(float fElapsedTime) {}

	// Handle Keyboard Input
	virtual void Keyboard();

	// Scene reset
	virtual void Reset();

	// Prerender of the scene, for FBO
	virtual void PreRender() {}

	// Render of the scene, this cant be used by FBO
	virtual void Render() = 0;

	// Update camera position using splines
	void InterpolCameraTraj(float fElapsedTime);

	// Display camera path
	void DrawTraj();	

	ISceneBase();
	virtual ~ISceneBase()  {Destroy();}

private:
	ISceneBase(const ISceneBase& sc) {}
	ISceneBase &operator=(const ISceneBase& sc) {return (*this);}

public:
	// Draws axes
	static void DrawAxes();

	// Displays a character string to scene, might not need this!
	static void DrawString(vec2 pos, void* font, const std::string& str);

	// Load camera path from a file
	bool LoadCameraTrajFromFile(const std::string& name);

	// Save the camera path to a file
	bool SaveCameraTrajInFile(const std::string& name);

	inline SplineGL*	getCamEyeSpline()	const	{return m_pCamEyeTraj;}

protected:
	SplineGL*		m_pCamEyeTraj;
	SplineGL*		m_pCamLookAtTraj;
};

#endif


