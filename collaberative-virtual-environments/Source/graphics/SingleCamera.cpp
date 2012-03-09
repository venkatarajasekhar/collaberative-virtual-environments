#ifdef WIN32
	#include <Windows.h>
#endif
#include <assert.h>
#include <GL/glew.h>
#include <SDL/SDL_syswm.h>

#include "SingleCamera.h"
#include "Frustum.h"
#include "../utilities/VarManager.h"

void Camera::SaveCamera()
{
	tSaveVectors[0] = vEye;
	tSaveVectors[1] = vCenter;
	tSaveVectors[2] = vViewDir;
	tSaveVectors[3] = vLeftDir;
	tSaveVectors[4] = vUp;

	tSaveFloats[0] = fAngleX;
	tSaveFloats[1] = fAngleY;

	bSaved = true;
}

void Camera::RestoreCamera()
{
	if(bSaved) {
		vEye = tSaveVectors[0];
		vCenter = tSaveVectors[1];
		vViewDir = tSaveVectors[2];
		vLeftDir = tSaveVectors[3];
		vUp = tSaveVectors[4];

		fAngleX = tSaveFloats[0];
		fAngleY = tSaveFloats[1];
	}

	bSaved = false;
}

Camera::Camera()
{
	// Default camera settings.
	fAngleX	=	3.0f;
	fAngleY	=	M_PI/2;

	vUp			= vec3(0.0f, 1.0f, 0.0f);
	vEye		= vec3(0.0f, 0.0f, 0.0f);

	eType = FREE;
	bSaved = false;

	Refresh();
}

void Camera::Refresh()
{
	switch(eType) 
	{
	case FREE:
		vViewDir.x = cosf(fAngleX) * sinf(fAngleY);
		vViewDir.y = cosf(fAngleY);
		vViewDir.z = sinf(fAngleX) * sinf(fAngleY);
		vCenter = vEye + vViewDir;
		vLeftDir.cross(vUp, vViewDir);
		vLeftDir.normalize();
		break;

	case DRIVEN:
		vViewDir = vCenter - vEye;
		vViewDir.normalize();
		vLeftDir.cross(vUp, vViewDir);
		vLeftDir.normalize();
		break;
	}

//	vLeftDir.cross(vUp, vViewDir);
}

void Camera::RenderLookAt(bool inverty, float planey) 
{

	if(inverty)
		gluLookAt(	vEye.x,		2.0f*planey-vEye.y,		vEye.z,
					vCenter.x,	2.0f*planey-vCenter.y,	vCenter.z,
					-vUp.x,		-vUp.y,					-vUp.z	);
	else
		gluLookAt(	vEye.x,		vEye.y,		vEye.z,
					vCenter.x,	vCenter.y,	vCenter.z,
					vUp.x,		vUp.y,		vUp.z	);
	
	Frustum::GetSingleton().Extract(vEye);
}

void Camera::PlayerMoveForward(float factor)
{
	vec3 vPrevEye = vEye;	// Save previous position

	// Move camera by factor
	vEye += vViewDir * factor;

	Refresh();
}

void Camera::PlayerMoveUp(float factor)
{
	vec3 vPrevEye = vEye;	// Save previous position

	// Move camera by factor
	vEye += vUp * factor;

	Refresh();
}

void Camera::TranslateForward(float factor)	
{
	// vec3 vPrevEye = vEye;

	// Move camera by factor
	vEye += vViewDir * factor;

	Refresh();
}

void Camera::PlayerMoveStrafe(float factor)	
{
	// vec3 vPrevEye = vEye;

	// Move camera by factor
	vEye += vLeftDir * factor;

	Refresh();
}

void Camera::TranslateStrafe(float factor)
{
	// vec3 vPrevEye = vEye;	// on sauvegarde la position précédente

	// Move camera by factor
	vEye += vLeftDir * factor;

	Refresh();
}

void Camera::MoveAnaglyph(float factor)
{
	vEye += vLeftDir * factor;
	vCenter += vLeftDir * factor;
}

void Camera::RenderLookAtToCubeMap(const vec3& eye, unsigned int nFace)
{
	assert(nFace < 6);

	// Vector array "Center" for the camera:
	vec3 TabCenter[6] = {	vec3(eye.x+1.0f,	eye.y,		eye.z),
							vec3(eye.x-1.0f,	eye.y,		eye.z),

							vec3(eye.x,			eye.y+1.0f,	eye.z),
							vec3(eye.x,			eye.y-1.0f,	eye.z),

							vec3(eye.x,			eye.y,		eye.z+1.0f),
							vec3(eye.x,			eye.y,		eye.z-1.0f) };


	// Vector array "Up" for the camera:
	static vec3 TabUp[6] = {	vec3(0.0f,	-1.0f,	0.0f),
								vec3(0.0f,	-1.0f,	0.0f),

								vec3(0.0f,	0.0f,	1.0f),
								vec3(0.0f,	0.0f,	-1.0f),

								vec3(0.0f,	-1.0f,	0.0f),
								vec3(0.0f,	-1.0f,	0.0f) };

	setEye( eye );

	glMatrixMode (GL_MODELVIEW);
	glLoadIdentity ();
	gluLookAt(	eye.x,						eye.y,						eye.z,
				TabCenter[nFace].x,	TabCenter[nFace].y,	TabCenter[nFace].z,
				TabUp[nFace].x,				TabUp[nFace].y,				TabUp[nFace].z		);

	Frustum::GetSingleton().Extract(eye);
}

void Camera::SetViewByMouse()
{
	// Get Window Pos
	SDL_SysWMinfo pInfo;
	SDL_VERSION(&pInfo.version);
	SDL_GetWMInfo(&pInfo);

	RECT r;
	GetWindowRect(pInfo.window, &r);

	SINGLETON_GET(VarManager, var)

	POINT mousePos;						// Mouse Screen Co-ordinates
	int middleX = r.left + (var.geti("win_width") >> 1);	// Set middleX to centre Screen Width
	int middleY = r.top + (var.geti("win_height") >> 1);	// Set middleY to centre Screen Height
	float angleY = 0.0f;				// Set angleY to 0
	float angleZ = 0.0f;				// Set angleZ to 0
	static float currentRotX = 0.0f;	// Set rotationX to 0

	// Get Cursor Position
	GetCursorPos(&mousePos);

	// If mouse is centred, do nothing and return from function
	if((mousePos.x == middleX) && (mousePos.y == middleY)) return;

	// Set cursor back to centre of screen
	SetCursorPos(middleX, middleY);

	// Gets the distance from the mouse position from the centre of the screen
	angleY = (float)( (middleX - mousePos.x) ) / var.getf("mouse_sensivity");		
	angleZ = (float)( (middleY - mousePos.y) ) / var.getf("mouse_sensivity");

	// Stores the current rotation into the last rotation so we now have 2 stored
	static float lastRotX = 0.0f; 
	lastRotX = currentRotX;

	// Increments Current rotation by the new Z rotation
	currentRotX += angleZ;

	/*/vec3d m_vView( 

	if(currentRotX > 1.0f) {	// Checks if rotation is greater than 1 (in radians)
		currentRotX = 1.0f;		// Set rotation back to 1 to prevent the camera from going right back
		if(lastRotX != 1.0f) {	// If both values are equal to 1 then no need to update values but if last value is not equal to 1 then
			vec3d vAxis = m_vView - m_vPosition;	// pass in the new vector
			vAxis = vAxis.cross(m_vUpVector);		// Find the cross product
			vAxis.normalize();						// Normalize vectors

			RotateView(1.0f - lastRotX, vAxis.x, vAxis.y, vAxis.z); // Rotate View
		}
	}else if(currentRotX < -1.0f) {		// Checks if rotation is less than -1 (in radians)
		currentRotX = -1.0f;			// This copies the same rotation above but working
		if(lastRotX != -1.0f) {			// in the minus figures to prevent the camera from going under
			vec3d vAxis = m_vView - m_vPosition;
			vAxis = vAxis.cross(m_vUpVector);
			vAxis.normalize();

			RotateView(-1.0f - lastRotX, vAxis.x, vAxis.y, vAxis.z);
		}
	}else{	// If the view is between -1 and 1
		vec3d vAxis = m_vView - m_vPosition; // pass in the new vector
		vAxis = vAxis.cross(m_vUpVector);		// Find Cross Product
		vAxis.normalize();						// Normalise

		RotateView(angleZ, vAxis.x, vAxis.y, vAxis.z);
	}
	RotateView(angleY, 0, 1, 0); // Rotate on X axis*/
}