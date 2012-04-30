#include "SceneTerrain.h"	

#include "terrain/Terrain.h"

#include "../graphics/SingleCamera.h"
#include "../utilities/ResourceManager.h"
#include "../utilities/VarManager.h"
#include "../tasks/InputTask.h"
#include "../tasks/GlobalTimer.h"
#include "../tasks/InputTask.h"
#include "../graphics/Sky.h"

#include "../kinect/KinectCode.h"
#include "../player/player.h"


void SceneTerrain::Init()
{
	SINGLETON_GET( VarManager, var )
	SINGLETON_GET( ResourceManager, res )

	m_vSunAngle = vec2(0.0f, RADIANS(45.0f));

	m_pSkybox   = (TextureCubemap*)res.LoadResource(ResourceManager::TEXTURECUBEMAP, "xposD.jpg xnegD.jpg yposD.jpg ynegD.jpg zposD.jpg znegD.jpg");
	m_pNightbox = (TextureCubemap*)res.LoadResource(ResourceManager::TEXTURECUBEMAP, "xposN.jpg xnegN.jpg yposN.jpg ynegN.jpg zposN.jpg znegN.jpg");

	m_pShaderTerrain	= (Shader*)res.LoadResource(ResourceManager::SHADER, "terrain_ground");

	// These are textures used to render the final terrain
	m_tTextures.push_back( (Texture2D*)res.LoadResource( ResourceManager::TEXTURE2D, "terrain_detail_NM.tga") );
	m_tTextures.push_back( (Texture2D*)res.LoadResource( ResourceManager::TEXTURE2D, "terrain_sand_512.jpg") );
	m_tTextures.push_back( (Texture2D*)res.LoadResource( ResourceManager::TEXTURE2D, "terrain_rocky_1024.png") );
	m_tTextures.push_back( (Texture2D*)res.LoadResource( ResourceManager::TEXTURE2D, "terrain_grass_1024.png") );
	
	m_pTerrainDiffuseMap = (Texture2D*)res.LoadResource( ResourceManager::TEXTURE2D, "heightmap_1024_diffusemap.jpg");

	m_pTerrain = new Terrain();
	assert(m_pTerrain != NULL);
	BoundingBox bbox( vec3(.0f, .0f, .0f), vec3(1025.0f, 255.0f, 1025.0f) );	// TODO : Calculate this so can be modified in game.
	m_pTerrain->Load("heightmap_1024.jpg", bbox, 32);

	ImageTools::ImageData img;
	ImageTools::OpenImage("heightmap_1024.jpg", img);
	std::cout << "   HeightMap D: " << img.d << " Size: " << img.w << "x" << img.h << std::endl;
	m_pTerrain->ComputeBoundingBox();
	img.Destroy();

	vec3 fogColor(0.7f, 0.7f, 0.9f);
	m_pShaderTerrain->Activate();
		m_pShaderTerrain->Uniform("bbox_min", m_pTerrain->getBoundingBox().min);
		m_pShaderTerrain->Uniform("bbox_max", m_pTerrain->getBoundingBox().max);
		m_pShaderTerrain->Uniform("fog_color", fogColor);
	m_pShaderTerrain->Deactivate();

	Sky::GetSingleton().Init();

	Camera::GetSingleton().setEye( ( m_pTerrain->getBoundingBox().min + (m_pTerrain->getBoundingBox().max * 0.5) ) );

	// Player set up. TODO : Need to add multiple players and networking.
	Mark.m_szName = "Mark";
	res.LoadResource(ResourceManager::MESH, "pointy.3d");

}

void SceneTerrain::Destroy()
{
	m_pTerrain->Destroy();
	delete m_pTerrain;
	m_tTextures.clear();

	ISceneBase::Destroy();
}

void SceneTerrain::Idle(float fElapsedTime)
{
	SINGLETON_GET( Camera, cam )
	SINGLETON_GET( Kinect, kin )

	// Get texture space positions.
	vec3 tEye     = m_pTerrain->getPosition( (float)(cam.getEye().x / m_pTerrain->getHMWidth()),
											 (float)(cam.getEye().z / m_pTerrain->getHMHeight()) );
	vec3 tPointer = m_pTerrain->getPosition( (float)(Mark.m_pointer.m_vPosition.x / m_pTerrain->getHMWidth()),
											 (float)(Mark.m_pointer.m_vPosition.z / m_pTerrain->getHMHeight()) );

	// Set eye and pointer values above terrain.
	Mark.m_pointer.m_vPosition.y = tPointer.y + 1.0f;
	if( cam.getEye().y < tEye.y + 1.0f )
		cam.vEye.y = tEye.y + 1.0f;

	// Kinect Camera Movement
	kin.Update();
	//if(!((kin.getRightArm().x < 0.005) && (kin.getRightArm().x > -0.005)))
		cam.RotateX(kin.getRightArm().x * 0.015);
	//if(!((kin.getRightArm().y < 0.005) && (kin.getRightArm().y > -0.005)))
		cam.RotateY(kin.getRightArm().y * -0.015);

	float tmp = kin.getRightShoulder().z - kin.getRightArm().z;
	printf("X: %f2.2 Y: %f2.2 S: %f2.2 A: %f2.2 Tmp: %f2.2\n", kin.getRightArm().x , kin.getRightArm().y, kin.getRightShoulder().z, kin.getRightArm().z, tmp);
	if(!((tmp < 0.05) && (tmp > -0.05)))
	{
		//cam.PlayerMoveForward( ((1.0 - kin.getRightArm().z) ) * 0.3 );
		cam.PlayerMoveForward( (tmp - 0.2) * 3 );
	}

	// Update players
	Mark.Update();


	// Update Sun
	m_vSunVector = vec4(	-cosf(m_vSunAngle.x) * sinf(m_vSunAngle.y),
							-cosf(m_vSunAngle.y),
							-sinf(m_vSunAngle.x) * sinf(m_vSunAngle.y),
							0.0f );

	Keyboard(fElapsedTime);
}

void SceneTerrain::Keyboard(float fElapsedTime)
{
	ISceneBase::Keyboard();
	SINGLETON_GET( Camera, cam )

	// Pointer Movement
	if( InputTask::keyStillDown( SDLK_UP    ) )			Mark.m_pointer.MoveForward( cam.getViewDir());
	if( InputTask::keyStillDown( SDLK_DOWN  ) )			Mark.m_pointer.MoveForward(-cam.getViewDir());
	if( InputTask::keyStillDown( SDLK_RIGHT ) )			Mark.m_pointer.MoveForward(-cam.getLeftDir());
	if( InputTask::keyStillDown( SDLK_LEFT  ) )			Mark.m_pointer.MoveForward( cam.getLeftDir());
	if( InputTask::keyStillDown( SDLK_KP0   ) )			Mark.m_pointer.MoveTo( cam.getEye() );	

	// AoI Size
	if( InputTask::keyDown( SDLK_z ) )				  { Mark.EditAoi( 1); printf("%i\n", Mark.m_pointer.m_areaOfInfluence); }
	if( InputTask::keyDown( SDLK_x ) )				  { Mark.EditAoi(-1); printf("%i\n", Mark.m_pointer.m_areaOfInfluence); }

	// Terrain Editing
	if( InputTask::keyStillDown( SDLK_EQUALS  ) )
	{
		vec3 tPointer = m_pTerrain->getPosition( (float)(Mark.m_pointer.m_vPosition.x / m_pTerrain->getHMWidth()),
											     (float)(Mark.m_pointer.m_vPosition.z / m_pTerrain->getHMHeight()) );
		m_pTerrain->EditMap( Terrain::HEIGHT, vec2(tPointer.x, tPointer.z),  5.0f * fElapsedTime, Mark.m_pointer.m_areaOfInfluence);
	}
	if( InputTask::keyStillDown( SDLK_MINUS  ) )			
	{
		vec3 tPointer = m_pTerrain->getPosition( (float)(Mark.m_pointer.m_vPosition.x / m_pTerrain->getHMWidth()),
				    							 (float)(Mark.m_pointer.m_vPosition.z / m_pTerrain->getHMHeight()) );
		m_pTerrain->EditMap( Terrain::HEIGHT, vec2(tPointer.x, tPointer.z), -5.0f * fElapsedTime, Mark.m_pointer.m_areaOfInfluence);
	}
}

void SceneTerrain::Reset()
{
	ISceneBase::Reset();
}

void SceneTerrain::SetLightCameraMatrices()
{
	VarManager& var = VarManager::GetSingleton();
	vec3 eye = Camera::GetSingleton().getEye();

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();
	gluLookAt(	eye.x - 500*m_vSunVector.x,		eye.y - 500*m_vSunVector.y,		eye.z - 500*m_vSunVector.z,
				eye.x,							eye.y,							eye.z,
				0.0,							1.0,							0.0	);
}

void SceneTerrain::RestoreLightCameraMatrices()
{
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
}

void SceneTerrain::PreRender()
{
	Frustum& frust = Frustum::GetSingleton();
	VarManager& var = VarManager::GetSingleton();

	vec3 eye_pos = Camera::GetSingleton().getEye();
	vec3 sun_pos = eye_pos - vec3(m_vSunVector);
	GLdouble tabOrtho[TERRAIN_SHADOWMAPS_COUNT] = {20.0, 100.0}; // 20, 100
	float sizey = 1.0f;

	//The camera is placed in the light and we get the matrix
	SetLightCameraMatrices();

	glMatrixMode(GL_PROJECTION);
	glPushMatrix();

	// Depth map displayed from the perspective of the sun in an FBO
	for(GLuint i=0; i<TERRAIN_SHADOWMAPS_COUNT; i++)
	{
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glOrtho(-tabOrtho[i], tabOrtho[i], -tabOrtho[i]*sizey, tabOrtho[i]*sizey, (GLdouble)var.getf("cam_znear"), (GLdouble)var.getf("cam_zfar"));
		glMatrixMode(GL_MODELVIEW);
		Frustum::GetSingleton().Extract(sun_pos);
		
		m_fboDepthMapFromLight[i].Begin();
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			RenderEnvironment(true);
		m_fboDepthMapFromLight[i].End();
	}

	// Get MV and proj matrix
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(-1.0, 1.0, -1.0, 1.0, (GLdouble)var.getf("cam_znear"), (GLdouble)var.getf("cam_zfar"));
	Frustum::GetSingleton().Extract(sun_pos);
	mat4& matLightMV = frust.getModelviewMatrix();
	mat4& matLightProj = frust.getProjectionMatrix();
	m_matSunModelviewProj = matLightProj * matLightMV;


	// Return matrices to before
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	RestoreLightCameraMatrices();
	Frustum::GetSingleton().Extract(eye_pos);

}

void SceneTerrain::Render()
{
	SINGLETON_GET( VarManager, var )

	// Render stage as normal
	RenderEnvironment(false);

	// Render pointer(s)
	Mark.m_pointer.Draw();

}

// Rendering of the Environment (for the reflection the water or not)
void SceneTerrain::RenderEnvironment(bool bDepthMap)
{
	SINGLETON_GET( ResourceManager, res )
	SINGLETON_GET( VarManager, var )
	SINGLETON_GET( Camera, cam )

	vec4 white(1.0f, 1.0f, 1.0f, 1.0f);
	vec4 black(0.0f, 0.0f, 0.0f, 1.0f);
	vec4 orange(1.0f, 0.5f, 0.0f, 1.0f);
	vec4 yellow(1.0f, 1.0f, 0.8f, 1.0f);
	float sun_cosy = cosf(m_vSunAngle.y);

	glPushAttrib(GL_ENABLE_BIT);			// Save state

	if(!bDepthMap)
	{
		// Draw the sky and sun
		Sky::GetSingleton().DrawSkyAndSun( Camera::GetSingleton().getEye(), vec3(m_vSunVector), m_pSkybox->getHandle(), m_pNightbox->getHandle(), false/*bWaterReflection*/ );
		Sky::GetSingleton().DrawClouds( cam.getEye(), vec3(m_vSunVector), false);
	}

	vec4 vSunColor = white.lerp(orange, yellow, 0.25f + sun_cosy * 0.75f);

	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	vec3 zeros(0.0f, 0.0f, 0.0f);
	vec3 position( 50.0f, 50.0f, 50.0f );
	vec3 other( 0.7f, 0.7f, 0.7f );
	glLightfv(GL_LIGHT0, GL_SPOT_DIRECTION,	zeros);
	glLightfv(GL_LIGHT0, GL_POSITION,		position);
	glLightfv(GL_LIGHT0, GL_AMBIENT,		white);
	glLightfv(GL_LIGHT0, GL_DIFFUSE,		other);
	glLightfv(GL_LIGHT0, GL_SPECULAR,		other);

	vec4 vGroundAmbient = other;
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT,		vGroundAmbient);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE,		white);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR,	white);
	glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS,	50.0f);

	glMatrixMode(GL_TEXTURE);
	glActiveTexture(GL_TEXTURE0);
	glMatrixMode(GL_MODELVIEW);

	// Bind the textures
	GLuint idx=0;
	for(GLuint i=0; i<m_tTextures.size(); i++)
		m_tTextures[i]->Bind(idx++);
	m_pTerrainDiffuseMap->Bind(idx++);
	
	// Render terrain
	m_pShaderTerrain->Activate();
	{
		m_pShaderTerrain->Uniform("detail_scale", 120.0f);
		m_pShaderTerrain->Uniform("diffuse_scale", 350.0f);

		m_pShaderTerrain->Uniform("water_height", var.getf("water_height"));

		m_pShaderTerrain->Uniform("time", GlobalTimer::totalTime);

		m_pShaderTerrain->UniformTexture("texNormalHeightMap", 0);
		m_pShaderTerrain->UniformTexture("texDiffuse0", 1);
		m_pShaderTerrain->UniformTexture("texDiffuse1", 2);
		m_pShaderTerrain->UniformTexture("texDiffuse2", 3);
		m_pShaderTerrain->UniformTexture("texDiffuseMap", 4);

		int ret = m_pTerrain->DrawGround(false);
		var.set("terrain_chunks_drawn", ret);

		m_pTerrain->DrawInfinitePlane(Camera::GetSingleton().getEye(), 2.0f*var.getf("cam_zfar"));
	}
	m_pShaderTerrain->Deactivate();

	// Unbind the textures.
	m_pTerrainDiffuseMap->Unbind(--idx);
	for(GLint i=(GLint)m_tTextures.size()-1; i>=0; i--)
		m_tTextures[i]->Unbind(--idx);
		

	glMatrixMode(GL_TEXTURE);
	glActiveTexture(GL_TEXTURE0);
	glLoadIdentity();
	glMatrixMode(GL_MODELVIEW);

	glPopAttrib();
}