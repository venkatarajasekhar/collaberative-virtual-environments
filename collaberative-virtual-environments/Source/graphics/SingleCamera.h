#ifndef SCAMERA_H
#define SCAMERA_H

#include "../utilities/math.h"
#include "../utilities/Singleton.h"

/// A singleton camera, more helpful for multiplayer applications. Use other camera for scene cameras. 
// TODO fix public/private
SINGLETON_BEGIN( Camera )
public:	enum TYPE {FREE, DRIVEN};
public:
	vec3	vEye;		// Position of the camera
	vec3	vCenter;	// Position of the sight center
	vec3	vViewDir;	// View vector
	vec3	vLeftDir;	// Left Vector
	vec3	vUp;		// Up vector

	float	fAngleX;	// Angle Theta
	float	fAngleY;	// Angle Phi

	TYPE	eType;		// Type of camera


	vec3	tSaveVectors[5];
	float	tSaveFloats[2];
	bool	bSaved;

public:
	inline TYPE  getType()				const	{return eType;}
	inline const vec3& getEye()			const	{return vEye;}
	inline const vec3& getCenter()		const	{return vCenter;}
	inline const vec3& getViewDir()		const	{return vViewDir;}
	inline float getAngleX()			const	{return fAngleX;}
	inline float getAngleY()			const	{return fAngleY;}

public:
	Camera();

public:
	void SaveCamera();
	void RestoreCamera();

	void Refresh();

	// Paramater setters
	void setType(TYPE t)				{ eType = t;		Refresh(); }
	void setEye(vec3 vPos)				{ vEye = vPos;		Refresh(); }
	void setCenter(vec3 vPos)			{ vCenter = vPos;	Refresh(); }
	void setAngleX(float Angle)			{ fAngleX = Angle;	Refresh(); }
	void setAngleY(float Angle)			{ fAngleY = Angle;	Refresh(); }
	void setAngle(float AngleX, float AngleY)	{
		fAngleX = AngleX;
		fAngleY = AngleY;
		Refresh();
	}

	// Change settings
	void Translate(vec3 vector)	{ vEye += vector;	Refresh(); }
	void RotateX(float AngleX)		{
		fAngleX += AngleX;
				if(fAngleX<0.0f)	fAngleX += M_PI*2;
		else	if(fAngleX>M_PI*2)	fAngleX -= M_PI*2;
		Refresh();
	}
	void RotateY(float AngleY)		{
		fAngleY += AngleY;
		if(fAngleY<0 || fAngleY>M_PI)	fAngleY -= AngleY;
		Refresh();
	}
	void Rotate(float AngleX, float AngleY)		{
		fAngleX += AngleX;
		fAngleY += AngleY;
				if(fAngleX<0.0f)		fAngleX += M_PI*2;
		else	if(fAngleX>M_PI*2)		fAngleX -= M_PI*2;
		if(fAngleY<0 || fAngleY>M_PI)	fAngleY -= AngleY;
		Refresh();
	}

	// Helper functions
	void PlayerMoveForward(float factor);	// Forward / Backward
	void PlayerMoveUp(float factor);		// Up / Down
	void PlayerMoveStrafe(float factor);	// Strafe left / right
	void TranslateForward(float factor);	// Forward / Backward
	void TranslateStrafe(float factor);		// Left / Right
	void MoveAnaglyph(float factor);		// Adjust for for anaglyph
	void SetViewByMouse();					// Allow control of camera with mouse

	void RenderLookAt(bool inverty=false, float planey=0.0f);

	void RenderLookAtToCubeMap(const vec3& eye, unsigned int nFace);
SINGLETON_END()

#endif