#ifndef SKY_H
#define SKY_H

#include <GL/glew.h>

#include "../utilities/singleton.h"
#include "../utilities/math.h"
#include "../graphics/Shader.h"

#define CLOUD_COVER			10.0f
#define CLOUD_SHARPNESS		0.95f

class Shader;

SINGLETON_BEGIN( Sky )
public:
	void Init();
	void DrawSky(const vec3& vEyePos, const GLuint& cubemap, bool invert=false) const;
	void DrawSun(const vec3& vEyePos, const vec3& vSunVect) const;
	void DrawClouds(const vec3& vEyePos,  const vec3& vSunVect, bool invert=false);

	void DrawSkyAndSun(const vec3& vEyePos, const vec3& vSunVect, const GLuint& cubemap, const GLuint& nightsky, bool invert) const;

	// Cloud helper functions
	float	Noise(int x, int y, int random);
	void	SetNoise(float  *map);
	float	Interpolate(float x, float y, float  *map);
	void	OverlapOctaves(float  *map256, float  *map2048);
	void	ExpFilter(float  *map);
	GLuint	CreateTextures(float *map, const int w, const int h, const int d);

	float	map32 [32  * 32 ];
	float	map256[256 * 256];		GLuint	map0;

private:
	Shader*	m_pShd;
	Shader* m_pShdClouds;

SINGLETON_END()

#endif