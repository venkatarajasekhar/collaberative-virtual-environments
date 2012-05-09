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
	SINGLETON_GET( ResourceManager, res )
	SINGLETON_GET( VarManager, var )

	m_vSunAngle = vec2(0.0f, RADIANS(45.0f));

	m_pSkybox   = (TextureCubemap*)res.LoadResource(ResourceManager::TEXTURECUBEMAP, "xposD.jpg xnegD.jpg yposD.jpg ynegD.jpg zposD.jpg znegD.jpg");
	m_pNightbox = (TextureCubemap*)res.LoadResource(ResourceManager::TEXTURECUBEMAP, "xposN.jpg xnegN.jpg yposN.jpg ynegN.jpg zposN.jpg znegN.jpg");


	m_pShaderLighting	= (Shader*)res.LoadResource(ResourceManager::SHADER, "lighting");
	m_pShaderTerrain	= (Shader*)res.LoadResource(ResourceManager::SHADER, "terrain_ground");
	m_pShaderWater		= (Shader*)res.LoadResource(ResourceManager::SHADER, "terrain_water");
	m_pShaderGrass		= (Shader*)res.LoadResource(ResourceManager::SHADER, "terrain_grass");
	m_pShaderTree		= (Shader*)res.LoadResource(ResourceManager::SHADER, "terrain_tree");

	res.LoadResource(ResourceManager::MESH, "terrain_house.3d");

	res.LoadResource(ResourceManager::TEXTURE2D, "wall_diffuse.jpg");
	res.LoadResource(ResourceManager::TEXTURE2D, "wall_NM_height.tga");

	// These are textures used to render the final terrain
	m_tTextures.push_back( (Texture2D*)res.LoadResource( ResourceManager::TEXTURE2D, "terrain_detail_NM.tga") );
	m_tTextures.push_back( (Texture2D*)res.LoadResource( ResourceManager::TEXTURE2D, "terrain_sand_512.jpg") );
	m_tTextures.push_back( (Texture2D*)res.LoadResource( ResourceManager::TEXTURE2D, "terrain_rocky_1024.png") );
	m_tTextures.push_back( (Texture2D*)res.LoadResource( ResourceManager::TEXTURE2D, "terrain_grass_1024.png") );
	m_tTextures.push_back( (Texture2D*)res.LoadResource( ResourceManager::TEXTURE2D, "terrain_water_caustics.jpg") );	
	
	
	res.LoadResource(ResourceManager::TEXTURE2D, "grass_billboards.tga");
	res.LoadResource(ResourceManager::TEXTURE2D, "palm_texture.tga");

	m_pTexWaterNoiseNM = (Texture2D*)res.LoadResource(ResourceManager::TEXTURE2D, "terrain_water_NM.jpg");
	m_pTerrainDiffuseMap = (Texture2D*)res.LoadResource(ResourceManager::TEXTURE2D, "heightmap_1024_diffusemap.jpg");

	m_pTerrain = new Terrain();
	assert(m_pTerrain != NULL);
	BoundingBox bbox( vec3(.0f, .0f, .0f), vec3(1025.0f, 255.0f, 1025.0f) );	// TODO : Calculate this so can be modified in game.
	m_pTerrain->Load("heightmap_1024.jpg", bbox, 32);	// 16

	ImageTools::ImageData img;
	ImageTools::OpenImage("heightmap_1024.jpg", img);
	std::cout << "   HeightMap D: " << img.d << " Size: " << img.w << "x" << img.h << std::endl;
	m_pTerrain->GenerateGrass(img, 200);				// 200000 GRASS AMOUNT
	m_pTerrain->GenerateVegetation(img, 25);
	m_pTerrain->ComputeBoundingBox();
	img.Destroy();

	vec3 fogColor(0.7f, 0.7f, 0.9f);
	m_pShaderTerrain->Activate();
		m_pShaderTerrain->Uniform("bbox_min", m_pTerrain->getBoundingBox().min);
		m_pShaderTerrain->Uniform("bbox_max", m_pTerrain->getBoundingBox().max);
		m_pShaderTerrain->Uniform("fog_color", fogColor);
	m_pShaderTerrain->Deactivate();

	m_pShaderWater->Activate();
		m_pShaderWater->Uniform("bbox_min", m_pTerrain->getBoundingBox().min);
		m_pShaderWater->Uniform("bbox_max", m_pTerrain->getBoundingBox().max);
		m_pShaderWater->Uniform("fog_color", fogColor);
	m_pShaderWater->Deactivate();


	m_fboWaterReflection.Create(FrameBufferObject::FBO_2D_COLOR, 512, 512);
	m_fboDepthMapFromLight[0].Create(FrameBufferObject::FBO_2D_DEPTH, 2048, 2048);
	m_fboDepthMapFromLight[1].Create(FrameBufferObject::FBO_2D_DEPTH, 2048, 2048);

	Camera::GetSingleton().setEye( ( m_pTerrain->getBoundingBox().min + (m_pTerrain->getBoundingBox().max * 0.5) ) );
	//Camera::GetSingleton().setAngle( 45.0f, 45.0f );

	// Player set up. TODO : Need to add multiple players and networking.
	Mark.m_szName = "Mark";
	Mark.m_pointer.MoveTo( ( m_pTerrain->getBoundingBox().min + (m_pTerrain->getBoundingBox().max * 0.5) ));
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
	SINGLETON_GET( VarManager, var )

	if( var.getb("dynamic_sun") )
		m_vSunAngle.y += fElapsedTime * 0.01f;

	m_vSunVector = vec4(	-cosf(m_vSunAngle.x) * sinf(m_vSunAngle.y),
							-cosf(m_vSunAngle.y),
							-sinf(m_vSunAngle.x) * sinf(m_vSunAngle.y),
							0.0f );
	
	// Get texture space positions.
	vec3 tEye     = m_pTerrain->getPosition( (float)(cam.getEye().x / m_pTerrain->getHMWidth()),
											 (float)(cam.getEye().z / m_pTerrain->getHMHeight()) );
	vec3 tPointer = m_pTerrain->getPosition( (float)(Mark.m_pointer.m_vPosition.x / m_pTerrain->getHMWidth()),
											 (float)(Mark.m_pointer.m_vPosition.z / m_pTerrain->getHMHeight()) );

	// Set eye and pointer values above terrain. 
	Mark.m_pointer.m_vPosition.y = tPointer.y + 1.0f;
	if( cam.getEye().y < tEye.y + 1.0f )
		cam.vEye.y = tEye.y + 1.0f;

		if(var.getb("using_kinect"))
	{
		// Kinect Camera Movement
		kin.Update();

		if(kin.getRightFoot().y <= (kin.getLeftFoot().y-0.2))
		{
			Mark.m_pointer.MoveTo( cam.getEye() + (2 * cam.getViewDir()) );
		}

		if(kin.getLeftArm().y <= -0.3)
		{
			cam.RotateX( kin.getRightArm().x * GlobalTimer::dT * 2 );
			cam.RotateY( kin.getRightArm().y * -GlobalTimer::dT * 2 );

			float tmp = kin.getRightArm().distance( kin.getRightShoulder() );

			//printf("X: %f2.2 Y: %f2.2 S: %f2.2 H: %f2.2 Tmp: %f2.2\n", kin.getRightArm().x , kin.getRightArm().y, kin.getRightShoulder().z, kin.getRightArm().z, tmp);
			
			//printf("pied right = %f2.2 \n pied gauche = %f2.2 \n", kin.getRightFoot(), kin. getLeftFoot());
			if(tmp <=0.3)			
			{
				tmp*=-1;
				cam.PlayerMoveForward( (tmp * 50) * GlobalTimer::dT * 1.5f );
			}
			else //if(tmp >=0.35)
			{
		
				cam.PlayerMoveForward( (tmp * 50) * GlobalTimer::dT * 1.5f );
			}
		
		}
		else if(kin.getLeftArm().y >= 0.5)
		{
			//cam.RotateX(kin.getRightArm().x * GlobalTimer::dT);
			//cam.RotateY(kin.getRightArm().y * -GlobalTimer::dT);

			Mark.setAoi(20);

			vec3 tPointer = m_pTerrain->getPosition( (float)(Mark.m_pointer.m_vPosition.x / m_pTerrain->getHMWidth()),
													 (float)(Mark.m_pointer.m_vPosition.z / m_pTerrain->getHMHeight()) );
			m_pTerrain->EditMap( Terrain::HEIGHT, vec2(tPointer.x, tPointer.z),  kin.getRightArm().y * 10.0f * fElapsedTime, Mark.m_pointer.m_areaOfInfluence, true);

		}
		else if (kin.getLeftArm().y >= -0.05 && kin.getLeftArm().y <= 0.15)
		{
			Mark.m_pointer.MoveForward(kin.getRightArm().y * cam.getViewDir() * GlobalTimer::dT * 125);
			Mark.m_pointer.MoveForward(kin.getRightArm().x * cam.getLeftDir() * GlobalTimer::dT * -125);
			
			Mark.setAoi(20);

		}
		
	}

//	if( ( Mark.m_pointer.m_vPosition.x + Mark.m_pointer.m_areaOfInfluence + 1 ) > m_pTerrain->getBoundingBox().max.x ) Mark.m_pointer.m_vPosition.x = m_pTerrain->getBoundingBox().max.x - (Mark.m_pointer.m_areaOfInfluence + 2);
//	if( ( Mark.m_pointer.m_vPosition.x - Mark.m_pointer.m_areaOfInfluence - 1 ) < m_pTerrain->getBoundingBox().min.x ) Mark.m_pointer.m_vPosition.x = m_pTerrain->getBoundingBox().min.x + (Mark.m_pointer.m_areaOfInfluence - 2);
//	if( ( Mark.m_pointer.m_vPosition.z + Mark.m_pointer.m_areaOfInfluence + 1 ) > m_pTerrain->getBoundingBox().max.z ) Mark.m_pointer.m_vPosition.z = m_pTerrain->getBoundingBox().max.z - (Mark.m_pointer.m_areaOfInfluence + 2);
//	if( ( Mark.m_pointer.m_vPosition.z - Mark.m_pointer.m_areaOfInfluence - 1 ) < m_pTerrain->getBoundingBox().min.z ) Mark.m_pointer.m_vPosition.z = m_pTerrain->getBoundingBox().min.z + (Mark.m_pointer.m_areaOfInfluence - 2);

	// Update players
	Mark.Update();

	Keyboard(fElapsedTime);
}

void SceneTerrain::Keyboard(float fElapsedTime)
{
	ISceneBase::Keyboard();

	SINGLETON_GET( Camera, cam )
	SINGLETON_GET( VarManager, var )
	SINGLETON_GET( Kinect, kin )

	if( InputTask::keyStillDown( SDLK_KP5 ) ){	m_vSunAngle.y += 0.01f; printf("Angle(%3.3f, %3.3f) Vector(%3.3f, %3.3f, %3.3f)\n", m_vSunAngle.x, m_vSunAngle.y, m_vSunVector.x, m_vSunVector.y, m_vSunVector.z); }
	if( InputTask::keyStillDown( SDLK_KP8 ) ){	m_vSunAngle.y -= 0.01f; printf("Angle(%3.3f, %3.3f) Vector(%3.3f, %3.3f, %3.3f)\n", m_vSunAngle.x, m_vSunAngle.y, m_vSunVector.x, m_vSunVector.y, m_vSunVector.z); }
	if( InputTask::keyStillDown( SDLK_KP6 ) ){	m_vSunAngle.x += 0.01f; printf("Angle(%3.3f, %3.3f) Vector(%3.3f, %3.3f, %3.3f)\n", m_vSunAngle.x, m_vSunAngle.y, m_vSunVector.x, m_vSunVector.y, m_vSunVector.z); }
	if( InputTask::keyStillDown( SDLK_KP4 ) ){	m_vSunAngle.x -= 0.01f; printf("Angle(%3.3f, %3.3f) Vector(%3.3f, %3.3f, %3.3f)\n", m_vSunAngle.x, m_vSunAngle.y, m_vSunVector.x, m_vSunVector.y, m_vSunVector.z); }
	
	// Pointer Movement
	if( InputTask::keyStillDown( SDLK_UP    ) )		Mark.m_pointer.MoveForward( cam.getViewDir());
	if( InputTask::keyStillDown( SDLK_DOWN  ) )		Mark.m_pointer.MoveForward(-cam.getViewDir());
	if( InputTask::keyStillDown( SDLK_RIGHT ) )		Mark.m_pointer.MoveForward(-cam.getLeftDir());
	if( InputTask::keyStillDown( SDLK_LEFT  ) )		Mark.m_pointer.MoveForward( cam.getLeftDir());
	if( InputTask::keyStillDown( SDLK_KP0   ) )		Mark.m_pointer.MoveTo( cam.getEye() );	

	if( InputTask::keyDown( SDLK_KP3 ) )			{ kin.setAngle( kin.getAngle() + 1 ); }
	if( InputTask::keyDown( SDLK_KP2  ) )			{ kin.setAngle( kin.getAngle() - 1 ); }

	// AoI Size
	if( InputTask::keyDown( SDLK_z ) )				{ Mark.EditAoi( 1); printf("%i\n", Mark.m_pointer.m_areaOfInfluence); }
	if( InputTask::keyDown( SDLK_x ) )				{ Mark.EditAoi(-1); printf("%i\n", Mark.m_pointer.m_areaOfInfluence); }

	// Terrain Editing
	if( InputTask::keyStillDown( SDLK_EQUALS  ) )
	{
		vec3 tPointer = m_pTerrain->getPosition( (float)(Mark.m_pointer.m_vPosition.x / m_pTerrain->getHMWidth()),
											     (float)(Mark.m_pointer.m_vPosition.z / m_pTerrain->getHMHeight()) );
		m_pTerrain->EditMap( Terrain::HEIGHT, vec2(tPointer.x, tPointer.z),  5.0f * fElapsedTime, Mark.m_pointer.m_areaOfInfluence, true);
	}
	if( InputTask::keyStillDown( SDLK_MINUS  ) )			
	{
		vec3 tPointer = m_pTerrain->getPosition( (float)(Mark.m_pointer.m_vPosition.x / m_pTerrain->getHMWidth()),
				    							 (float)(Mark.m_pointer.m_vPosition.z / m_pTerrain->getHMHeight()) );
		m_pTerrain->EditMap( Terrain::HEIGHT, vec2(tPointer.x, tPointer.z), -5.0f * fElapsedTime, Mark.m_pointer.m_areaOfInfluence, true);
	}

	// K : Kinect Toggle
	if( InputTask::keyDown(SDLK_k) )
	{		
		var.set("using_kinect",		!var.getb("using_kinect"));
		std::cerr << "\r using_kinect : " << var.getb("using_kinect") << "        "; 
	}
}

void SceneTerrain::Reset()
{
	VarManager& var = VarManager::GetSingleton();

	Camera::GetSingleton().setEye(vec3(0.0f, 0.0f, 0.0f));

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
			RenderEnvironment(false, true);
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


	// Display scene reflected in a FBO
	m_fboWaterReflection.Begin();
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		RenderEnvironment(true, false);
	m_fboWaterReflection.End();
}

void SceneTerrain::Render()
{
	VarManager& var = VarManager::GetSingleton();

	// Render scene as normal
	RenderEnvironment(false, false);

	// Render the water surface
	RenderWaterSurface();

	// Draw Pointers TODO : Make this a vector of players from server
	Mark.Draw();
}

// Render the water surface
void SceneTerrain::RenderWaterSurface()
{
	VarManager& var = VarManager::GetSingleton();
	float h = var.getf("water_height");


	glPushAttrib(GL_ENABLE_BIT);
	glDisable(GL_CULL_FACE);
	glEnable(GL_BLEND);

	glMatrixMode(GL_TEXTURE);
	glActiveTexture(GL_TEXTURE0);
	glLoadMatrixf( m_matSunModelviewProj );
	glMatrixMode(GL_MODELVIEW);


	m_pShaderWater->Activate();
	{
		m_fboWaterReflection.Bind(0);
		m_pShaderWater->UniformTexture("texWaterReflection", 0);

		m_pTexWaterNoiseNM->Bind(1);
		m_pShaderWater->UniformTexture("texWaterNoiseNM", 1);

		m_pShaderWater->Uniform("win_width", var.geti("win_width"));
		m_pShaderWater->Uniform("win_height", var.geti("win_height"));
		m_pShaderWater->Uniform("noise_tile", 100.0f);	//10
		m_pShaderWater->Uniform("noise_factor", 0.1f);
		m_pShaderWater->Uniform("time", GlobalTimer::totalTime);
		m_pShaderWater->Uniform("water_shininess", 50.0f);

		for(GLuint i=0; i<TERRAIN_SHADOWMAPS_COUNT; i++)
			m_fboDepthMapFromLight[i].Bind(2 + i);

		m_pShaderWater->UniformTexture("texDepthMapFromLight0", 2);
		m_pShaderWater->UniformTexture("texDepthMapFromLight1", 3);
		m_pShaderWater->Uniform("depth_map_size", 512);


		vec3 e = Camera::GetSingleton().getEye();
		float d = 2.0f * var.getf("cam_zfar");

		glBegin(GL_QUADS);
			glVertex3f(e.x - d, h, e.z - d);
			glVertex3f(e.x - d, h, e.z + d);
			glVertex3f(e.x + d, h, e.z + d);
			glVertex3f(e.x + d, h, e.z - d);
		glEnd();

		for(GLint i=TERRAIN_SHADOWMAPS_COUNT-1; i>=0; i--)
			m_fboDepthMapFromLight[i].Unbind(2 + i);

		m_pTexWaterNoiseNM->Unbind(1);
		m_fboWaterReflection.Unbind(0);
	}
	m_pShaderWater->Deactivate();

	glMatrixMode(GL_TEXTURE);
	glActiveTexture(GL_TEXTURE0);
	glLoadIdentity();
	glMatrixMode(GL_MODELVIEW);

	glPopAttrib();
}

// Rendering of the Environment (for the reflection the water or not)
void SceneTerrain::RenderEnvironment(bool bWaterReflection, bool bDepthMap)
{
	ResourceManager& res = ResourceManager::GetSingleton();
	VarManager& var = VarManager::GetSingleton();
	Camera& cam = Camera::GetSingleton();

	vec4 white(1.0f, 1.0f, 1.0f, 1.0f);
	vec4 black(0.0f, 0.0f, 0.0f, 1.0f);
	vec4 orange(1.0f, 0.5f, 0.0f, 1.0f);
	vec4 yellow(1.0f, 1.0f, 0.8f, 1.0f);
	float sun_cosy = cosf(m_vSunAngle.y);

	glPushAttrib(GL_ENABLE_BIT);

	if(!bDepthMap)
	{
		vec4 vSunColor = white.lerp(orange, yellow, 0.25f + sun_cosy * 0.75f);


		// Check if underwater.
		if(cam.getEye().y < var.getf("water_height"))
		{
			bWaterReflection = false;
			var.set("enable_underwater", true);
			var.set("enable_rain", false);
			//fx->enableEchoSoundEffect();
		}
		else
		{	
			var.set("enable_underwater", false);
			if( var.getb("raining") )
			{
				var.set("enable_rain", true);
				//fx->disableEchoSoundEffect();
			}
		}

		// Draw the sky and sun
		Sky::GetSingleton().DrawSkyAndSun( Camera::GetSingleton().getEye(), vec3(m_vSunVector), m_pSkybox->getHandle(), m_pNightbox->getHandle(), bWaterReflection );

		// Draw Clouds
		if( var.getb("draw_clouds") )
			Sky::GetSingleton().DrawClouds( Camera::GetSingleton().getEye(), vec3(m_vSunVector), bWaterReflection );

		// If reflection is enabled, we draw the scene upside down.
		if(bWaterReflection) {
			glDisable(GL_CULL_FACE);
			glLoadIdentity();
			
			glScalef(-1.0f, 1.0f, 1.0f);
			cam.RenderLookAt(true, var.getf("water_height"));
		}

		glEnable(GL_LIGHTING);
		glEnable(GL_LIGHT0);
		vec3 zeros(0.0f, 0.0f, 0.0f);
		glLightfv(GL_LIGHT0, GL_SPOT_DIRECTION, zeros);
		glLightfv(GL_LIGHT0, GL_POSITION, m_vSunVector);
		glLightfv(GL_LIGHT0, GL_AMBIENT, white);
		glLightfv(GL_LIGHT0, GL_DIFFUSE, vSunColor);
		glLightfv(GL_LIGHT0, GL_SPECULAR, vSunColor);

	}

	/*/ Draw houses on scene
	m_pShaderLighting->Activate();
	{
		glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, white/6);
		glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, white);

		res.getTexture2D("wall_diffuse.jpg")->Bind(0);
		res.getTexture2D("wall_NM_height.tga")->Bind(1);

		//m_pShaderLighting->UniformTexture("texDiffuse", 0);
		m_pShaderLighting->UniformTexture("texNormalHeightMap", 1);
		m_pShaderLighting->Uniform("mode", 1);
		m_pShaderLighting->Uniform("enable_shadow_mapping", 0);
		m_pShaderLighting->Uniform("tile_factor", 2.0f);

		glPushMatrix();
		glTranslatef(172.0f, 5.18f, 175.2f);
		res.getMesh("terrain_house.obj")->Draw();
		glPopMatrix();

		glPushMatrix();
		glTranslatef(19.0f, 35.8f, -123.1f);
		res.getMesh("terrain_house.obj")->Draw();
		glPopMatrix();

		glPushMatrix();
		glTranslatef(1.338f, 18.02f, 259.0f);
		res.getMesh("terrain_house.obj")->Draw();
		glPopMatrix();

		glPushMatrix();
		glTranslatef(178.6f, 32.42f, 26.31f);
		res.getMesh("terrain_house.obj")->Draw();
		glPopMatrix();

		res.getTexture2D("wall_NM_height.tga")->Unbind(1);
		res.getTexture2D("wall_diffuse.jpg")->Unbind(0);
	}
	m_pShaderLighting->Deactivate();*/


	if(!bDepthMap)
	{
		vec4 vGroundAmbient = white.lerp(white*0.2f, black, sun_cosy);
		glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, vGroundAmbient);
		glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, white);
		glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, white);
		glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 50.0f);

		glMatrixMode(GL_TEXTURE);
		glActiveTexture(GL_TEXTURE0);
		glLoadMatrixf( m_matSunModelviewProj );
		glMatrixMode(GL_MODELVIEW);

		// Bind the textures
		GLuint idx=0;
		for(GLuint i=0; i<m_tTextures.size(); i++)
			m_tTextures[i]->Bind(idx++);
		m_pTerrainDiffuseMap->Bind(idx++);
		if(!bDepthMap) {
			for(GLuint i=0; i<TERRAIN_SHADOWMAPS_COUNT; i++)
				m_fboDepthMapFromLight[i].Bind(idx++);
		}

		// Render terrain
		m_pShaderTerrain->Activate();
		{
			m_pShaderTerrain->Uniform("detail_scale", 1200.0f);
			m_pShaderTerrain->Uniform("diffuse_scale", 700.0f);

			m_pShaderTerrain->Uniform("water_height", var.getf("water_height"));
			m_pShaderTerrain->Uniform("water_reflection_rendering", bWaterReflection);

			m_pShaderTerrain->Uniform("time", GlobalTimer::totalTime);

			m_pShaderTerrain->UniformTexture("texNormalHeightMap", 0);
			m_pShaderTerrain->UniformTexture("texDiffuse0", 1);
			m_pShaderTerrain->UniformTexture("texDiffuse1", 2);
			m_pShaderTerrain->UniformTexture("texDiffuse2", 3);
			m_pShaderTerrain->UniformTexture("texWaterCaustics", 4);
			m_pShaderTerrain->UniformTexture("texDiffuseMap", 5);
			m_pShaderTerrain->UniformTexture("texDepthMapFromLight0", 6);
			m_pShaderTerrain->UniformTexture("texDepthMapFromLight1", 7);

			m_pShaderTerrain->Uniform("depth_map_size", 512);

			int ret = m_pTerrain->DrawGround(bWaterReflection);
			var.set(bWaterReflection? "terrain_chunks_reflected_drawn" : "terrain_chunks_drawn", ret);

			m_pTerrain->DrawInfinitePlane(Camera::GetSingleton().getEye(), 2.0f*var.getf("cam_zfar"));
		}
		m_pShaderTerrain->Deactivate();


		// Unbind the textures.
		if(!bDepthMap) {
			for(GLint i=TERRAIN_SHADOWMAPS_COUNT-1; i>=0; i--)
				m_fboDepthMapFromLight[i].Unbind(--idx);
		}
		m_pTerrainDiffuseMap->Unbind(--idx);
		for(GLint i=(GLint)m_tTextures.size()-1; i>=0; i--)
			m_tTextures[i]->Unbind(--idx);
		

		// Render grass
		if(!bWaterReflection)
		{
			glEnable(GL_BLEND);
			m_pShaderGrass->Activate();
			{
				res.getTexture2D("grass_billboards.tga")->Bind(0);
				for(GLuint i=0; i<TERRAIN_SHADOWMAPS_COUNT; i++)
					m_fboDepthMapFromLight[i].Bind(1+i);

				m_pShaderGrass->UniformTexture("texDiffuse", 0);
				m_pShaderGrass->UniformTexture("texDepthMapFromLight0", 1);
				m_pShaderGrass->UniformTexture("texDepthMapFromLight1", 2);
				m_pShaderGrass->Uniform("depth_map_size", 512);

				m_pShaderGrass->Uniform("time", GlobalTimer::totalTime);
				m_pShaderGrass->Uniform("lod_metric", TERRAIN_GRASS_MAX_DISTANCE);

					m_pTerrain->DrawGrass(bWaterReflection);

				for(GLint i=TERRAIN_SHADOWMAPS_COUNT-1; i>=0; i--)
					m_fboDepthMapFromLight[i].Unbind(1+i);
				res.getTexture2D("grass_billboards.tga")->Unbind(0);
			}
			m_pShaderGrass->Deactivate();
			glDisable(GL_BLEND);
		}

		glMatrixMode(GL_TEXTURE);
		glActiveTexture(GL_TEXTURE0);
		glLoadIdentity();
		glMatrixMode(GL_MODELVIEW);
	}


	// Render tree's
	m_pShaderTree->Activate();
	{
		glMatrixMode(GL_TEXTURE);
		glActiveTexture(GL_TEXTURE0);
		glLoadMatrixf( Frustum::GetSingleton().getModelviewInvMatrix() );
		glMatrixMode(GL_MODELVIEW);

		GLuint idx=0;
		res.getTexture2D("palm_texture.tga")->Bind(idx++);
		if(!bDepthMap) {
			for(GLuint i=0; i<TERRAIN_SHADOWMAPS_COUNT; i++)
				m_fboDepthMapFromLight[i].Bind(idx++);
		}


		m_pShaderTree->UniformTexture("texDiffuse", 0);
		m_pShaderTree->UniformTexture("texDepthMapFromLight0", 1);
		m_pShaderTree->UniformTexture("texDepthMapFromLight1", 2);

		m_pShaderTree->Uniform("time", GlobalTimer::totalTime);

			m_pTerrain->DrawObjects(bWaterReflection);

		res.getTexture2D("palm_texture.tga")->Unbind(--idx);
		if(!bDepthMap) {
			for(GLint i=TERRAIN_SHADOWMAPS_COUNT-1; i>=0; i--)
				m_fboDepthMapFromLight[i].Unbind(--idx);
		}

		glMatrixMode(GL_TEXTURE);
		glActiveTexture(GL_TEXTURE0);
		glLoadIdentity();
		glMatrixMode(GL_MODELVIEW);
	}
	m_pShaderTree->Deactivate();

	glPopAttrib();
}