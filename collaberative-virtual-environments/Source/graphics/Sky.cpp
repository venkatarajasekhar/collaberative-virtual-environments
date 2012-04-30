#include "Sky.h"

#include "texture\TextureCubemap.h"
#include "../utilities/ResourceManager.h"
#include "../graphics/SingleCamera.h"
#include "../utilities/types.h"
#include "../tasks/GlobalTimer.h"
#include "../utilities/VarManager.h"

#include "../scenes/SceneTerrain.h"

#include <string>
#include <iostream>
#include <sstream>

void Sky::Init()
{
	ResourceManager& res = ResourceManager::GetSingleton();

	m_pShd			= (Shader*)res.LoadResource(ResourceManager::SHADER, "sky");
	m_pShdClouds	= (Shader*)res.LoadResource(ResourceManager::SHADER, "clouds");

	std::cerr << "Generating clouds...\n";

	SetNoise(map32);
	OverlapOctaves(map32, map256);
	ExpFilter(map256);
	map0 = CreateTextures(map256, 256, 256, 4);

	std::cerr << "Generating clouds... OK!\n";
}

void Sky::DrawSkyAndSun(const vec3& vEyePos, const vec3& vSunVect, const GLuint& cubemap, const GLuint& nightsky, bool invert) const
{
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glTranslatef(vEyePos.x, vEyePos.y, vEyePos.z);
	if(invert)
		glScalef(1.0f, -1.0f, 1.0f);

	glPushAttrib(GL_ENABLE_BIT);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubemap);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_CUBE_MAP, nightsky);
	m_pShd->Activate();
	{
		m_pShd->UniformTexture("texSky", 0);
		m_pShd->UniformTexture("texNight", 1);

		m_pShd->Uniform("enable_sun", true);
		m_pShd->Uniform("sun_vector", vSunVect);

		glDisable(GL_CULL_FACE);
		glDisable(GL_LIGHTING);
		mySolidSphere(1.0, 4, 4);
	}
	m_pShd->Deactivate();

	glPopAttrib();

	glPopMatrix();

	glClear(GL_DEPTH_BUFFER_BIT);
}

void Sky::DrawSky(const vec3& vEyePos, const GLuint& cubemap, bool invert) const
{
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glTranslatef(vEyePos.x, vEyePos.y, vEyePos.z);
	if(invert)
		glScalef(1.0f, -1.0f, 1.0f);


	glPushAttrib(GL_ENABLE_BIT);

	glActiveTexture(GL_TEXTURE0);
	glEnable(GL_TEXTURE_CUBE_MAP);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubemap);
	m_pShd->Activate();
	{
		m_pShd->UniformTexture("texSky", 0);

		m_pShd->Uniform("enable_sun", false);

		glDisable(GL_CULL_FACE);
		glDisable(GL_LIGHTING);
		mySolidSphere (1.0, 4, 4);
	}
	m_pShd->Deactivate();
	glDisable(GL_TEXTURE_CUBE_MAP);

	glPopAttrib();

	glPopMatrix();

	glClear(GL_DEPTH_BUFFER_BIT);

/*
	static GLfloat xPlane[] = { 1.0f, 0.0f, 0.0f, 0.0f };
	static GLfloat yPlane[] = { 0.0f, 1.0f, 0.0f, 0.0f };
	static GLfloat zPlane[] = { 0.0f, 0.0f, 1.0f, 0.0f };

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glTranslatef(vEyePos.x, vEyePos.y, vEyePos.z);
	if(invert)
		glScalef(1.0f, -1.0f, 1.0f);

	glPushAttrib(GL_ENABLE_BIT);
	glPushAttrib(GL_POLYGON_BIT);

	glEnable (GL_TEXTURE_GEN_S);
	glEnable (GL_TEXTURE_GEN_T);
	glEnable (GL_TEXTURE_GEN_R);

	glEnable (GL_TEXTURE_CUBE_MAP);

	glBindTexture(GL_TEXTURE_CUBE_MAP, cubemap);

	glTexGeni (GL_S, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);
	glTexGeni (GL_T, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);
	glTexGeni (GL_R, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);

	glTexGenfv (GL_S, GL_OBJECT_PLANE, xPlane);
	glTexGenfv (GL_T, GL_OBJECT_PLANE, yPlane);
	glTexGenfv (GL_R, GL_OBJECT_PLANE, zPlane);

	glDisable(GL_CULL_FACE);
	glDisable(GL_LIGHTING);
	glColor3f(1.0f, 1.0f, 1.0f);
	glutSolidSphere (1.0, 4, 4);

	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

	glDisable (GL_TEXTURE_CUBE_MAP);

	glDisable (GL_TEXTURE_GEN_S);
	glDisable (GL_TEXTURE_GEN_T);
	glDisable (GL_TEXTURE_GEN_R);

	glPopAttrib();
	glPopAttrib();

	glClear(GL_DEPTH_BUFFER_BIT);
	glPopMatrix();*/
}

void Sky::DrawSun(const vec3& vEyePos, const vec3& vSunVect) const
{
	vec4 color;
	glGetLightfv(GL_LIGHT0, GL_DIFFUSE, color);

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glTranslatef(vEyePos.x, vEyePos.y, vEyePos.z);
	glTranslatef(-vSunVect.x, -vSunVect.y, -vSunVect.z);

	glPushAttrib(GL_ENABLE_BIT);

	glDisable(GL_LIGHTING);
	glDisable(GL_TEXTURE_2D);
	glColor3f(color.r, color.g, color.b);
	mySolidSphere(0.1, 16, 16);

	glPopAttrib();

	glClear(GL_DEPTH_BUFFER_BIT);
	glPopMatrix();
}

void Sky::DrawClouds(const vec3& vEyePos, const vec3& vSunVect, bool invert)
{
	VarManager& var = VarManager::GetSingleton();

	vec3 vWind = vec3(.01, 0.01, 0.02);
	vWind.x *= GlobalTimer::dT;
	vWind.y *= GlobalTimer::dT;

	float width = var.getf("cloud_width");
	float height = var.getf("cloud_height");


	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glTranslatef(vEyePos.x, vEyePos.y, vEyePos.z);
	glTranslatef(0.0f, 0.0f, 0.0f);
	if(invert)
		glScalef(1.0f, -1.0f, 1.0f);

	glPushAttrib(GL_ENABLE_BIT);

	glEnable(GL_BLEND); 
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);
	glDisable(GL_LIGHTING);

	m_pShdClouds->Activate();
	{
		glEnable(GL_TEXTURE_2D);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, map0);

		m_pShdClouds->UniformTexture("tex0", 0);

		//m_pShdClouds->Uniform("CloudsParams",		vec4(0.45f, 0.94f, 0.0f, 0.5f) );	// x = cover, y = sharpness, z = pixoffset, w = ambient
		//m_pShdClouds->Uniform("OctavesScales0",		vec4(1.0f, 2.0f, 4.0f, 8.0f) );
		//m_pShdClouds->Uniform("OctavesWeights0",	vec4(1.0f, 0.5f, 0.25f, 0.125f) );

		m_pShdClouds->Uniform("enable_sun", var.getb("cloud_ray_tracing"));				// Enable this for raytracing.
		m_pShdClouds->Uniform("sun_vector", vSunVect);

		m_pShdClouds->Uniform("wind_vec", vWind);
		m_pShdClouds->Uniform("tiling", 1.0f);

		static float s, t;  
		s+=vWind.s; if(s > 1.0f) s = 0.001f;	
		t+=vWind.t; if(t > 1.0f) t = 0.001f;
		vec2 vOffset(s, t);
		m_pShdClouds->Uniform("offset", vOffset);

		glBegin(GL_QUADS);
			glTexCoord2d(1,1); glVertex3f( width, height,  width);
			glTexCoord2d(0,1); glVertex3f(-width, height,  width);
			glTexCoord2d(0,0); glVertex3f(-width, height, -width);
			glTexCoord2d(1,0); glVertex3f( width, height, -width);
		glEnd(); 
	}
	m_pShdClouds->Deactivate();

	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
	glDisable(GL_BLEND);

	glPopAttrib();
	glPopMatrix();
	
	glClear(GL_DEPTH_BUFFER_BIT);
}

// Creates texture id's for textures from array
GLuint Sky::CreateTextures(float *map, const int w, const int h, const int d)
{
	char texture[256][256][4];				// Temporary array to hold texture RGBA values (256, 256) for now
	for(int i=0; i<w; i++)					// Set cloud color value to temporary array
		for(int j=0; j<h; j++) 
		{
			float color = map[(i*256+j)];
			texture[i][j][0]=color;
			texture[i][j][1]=color;
			texture[i][j][2]=color;
			if(d==4)
				texture[i][j][3]=color;
		}

	glActiveTexture( GL_TEXTURE0 );
	GLuint ID;								// Generate an ID for texture binding                     
	glGenTextures(1, &ID);					// Texture binding 
	glBindTexture(GL_TEXTURE_2D, ID);		// Bind texture ID

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
   
	gluBuild2DMipmaps(GL_TEXTURE_2D, d==3?GL_RGB:GL_RGBA, w, h, d==3?GL_RGB:GL_RGBA, GL_UNSIGNED_BYTE, texture);

	std::cout << ID << std::endl;

	return ID;
}

float Sky::Noise(int x, int y, int random)
{
    int n = x + y * 57 + random * 131;
    n = (n<<13) ^ n;
    return (1.0f - ( (n * (n * n * 15731 + 789221) +
            1376312589)&0x7fffffff)* 0.000000000931322574615478515625f);
}

// Set noise to map  32 * 32
void Sky::SetNoise(float  *map)
{
	float temp[34][34];

	int random=rand() % 5000;

	for (int y=1; y<33; y++) 
		for (int x=1; x<33; x++)
			temp[x][y] = 128.0f + Noise(x,  y,  random)*128.0f;
	
	// Seamless cloud
	for (int x=1; x<33; x++)
	{
		temp[0][x] = temp[32][x];
		temp[33][x] = temp[1][x];
		temp[x][0] = temp[x][32];
		temp[x][33] = temp[x][1];
	}

	temp[0][0] = temp[32][32];
	temp[33][33] = temp[1][1];
	temp[0][33] = temp[32][1];
	temp[33][0] = temp[1][32];

	// Smooth
	for (int y=1; y<33; y++)
		for (int x=1; x<33; x++)
		{
			float center = temp[x][y]/4.0f;
			float sides = (temp[x+1][y] + temp[x-1][y] + temp[x][y+1] + temp[x][y-1])/8.0f;
			float corners = (temp[x+1][y+1] + temp[x+1][y-1] + temp[x-1][y+1] + temp[x-1][y-1])/16.0f;

			map32[((x-1)*32) + (y-1)] = center + sides + corners;
		}
}

// Make it less blocky
float Sky::Interpolate(float x, float y, float  *map)
{
	int Xint = (int)x;
	int Yint = (int)y;

	float Xfrac = x - Xint;
	float Yfrac = y - Yint;

	int X0 = Xint % 32;
	int Y0 = Yint % 32;
	int X1 = (Xint + 1) % 32;
	int Y1 = (Yint + 1) % 32;

	float bot = map[X0*32 + Y0] + Xfrac * (map[X1*32 + Y0] - map[X0*32 + Y0]);
	float top = map[X0*32 + Y1] + Xfrac * (map[X1*32 +  Y1] - map[X0*32 + Y1]);

	return (bot + Yfrac * (top - bot));
}

void Sky::OverlapOctaves(float  *map32, float  *map256)
{
	for (int x=0; x<256*256; x++)
		map256[x] = 0;
	
	for (int octave=0; octave<8; octave++)
		for (int x=0; x<256; x++)
			for (int y=0; y<256; y++)
			{
				float scale = 1 / pow(2.0f, 3-octave);
				float noise = Interpolate(x*scale, y*scale , map32);
				map256[(y*256) + x] += noise / pow(2.0f, octave);
			}
}

void Sky::ExpFilter(float  *map)
{
	float cover = CLOUD_COVER;
	float sharpness = CLOUD_SHARPNESS;

	for (int x=0; x<256*256; x++)
	{
		float c = map[x] - (255.0f-cover);
		if (c<0)
			c = 0;
		map[x] = 255.0f - ((float)(pow(sharpness, c))*255.0f);
	}
}
