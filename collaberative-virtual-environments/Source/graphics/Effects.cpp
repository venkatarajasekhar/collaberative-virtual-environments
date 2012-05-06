#include "Effects.h"

#include "../utilities/ResourceManager.h"
#include "../utilities/VarManager.h"
#include "../graphics/SingleCamera.h"
#include "../scenes/SceneManager.h"
#include "../tasks/GlobalTimer.h"

void Effects::init()
{
	ResourceManager& res = ResourceManager::GetSingleton();

	m_pVignette	= (Texture2D*)res.LoadResource(ResourceManager::TEXTURE2D, "vignette.jpeg");
	m_pBruit	= (Texture2D*)res.LoadResource(ResourceManager::TEXTURE2D, "bruit_gaussien.jpg");


	ResourceManager::GetSingleton().LoadResource(ResourceManager::SHADER, "anaglyph");
	ResourceManager::GetSingleton().LoadResource(ResourceManager::SHADER, "postfx");
	ResourceManager::GetSingleton().LoadResource(ResourceManager::SHADER, "blur");
	ResourceManager::GetSingleton().LoadResource(ResourceManager::SHADER, "bright");
	ResourceManager::GetSingleton().LoadResource(ResourceManager::SHADER, "depthblur");

	randomCoeffNoise = 0.0f;
	randomCoeffFlash = 0.0f;

	m_fT = 0;
	m_fPeriod = 1.0f/24.0f;
}

void Effects::reshapeFBO(int newwidth , int newheight)
{
	// Recreate FBO
	fboScreen.Create(FrameBufferObject::FBO_2D_COLOR, newwidth, newheight);
	fboDepth.Create(FrameBufferObject::FBO_2D_DEPTH, newwidth, newheight);
	fboAnaglyph[0].Create(FrameBufferObject::FBO_2D_COLOR, newwidth, newheight);
	fboAnaglyph[1].Create(FrameBufferObject::FBO_2D_COLOR, newwidth, newheight);

	fboDepthScreenTemp.Create(FrameBufferObject::FBO_2D_COLOR, newwidth, newheight);
	fboDepthScreen.Create(FrameBufferObject::FBO_2D_COLOR, newwidth, newheight);

	int w = newwidth/4, h = newheight/4;
	fboBloomFinal.Create(FrameBufferObject::FBO_2D_COLOR, w, h);
	fboBloomFinalTemp.Create(FrameBufferObject::FBO_2D_COLOR, w, h);

}

void Effects::DrawQuadAtScreen()
{
	// Display a quad on screen
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glEnable(GL_TEXTURE_2D);
	glColor3f(1.0f, 1.0f, 1.0f);

	glBegin(GL_QUADS);
		glTexCoord2f(0.0f, 0.0f);	glVertex2f(-1.0f, -1.0f);
		glTexCoord2f(1.0f, 0.0f);	glVertex2f( 1.0f, -1.0f);
		glTexCoord2f(1.0f, 1.0f);	glVertex2f( 1.0f,  1.0f);
		glTexCoord2f(0.0f, 1.0f);	glVertex2f(-1.0f,  1.0f);
	glEnd();

	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
}

void Effects::displaySceneWithAnaglyph(void)
{
	VarManager& var = VarManager::GetSingleton();
	ResourceManager& res = ResourceManager::GetSingleton();

	Camera::GetSingleton().SaveCamera();
	GLfloat eye_offset = var.getf("cam_anaglyph_offset");
	GLfloat tEyePos[2] = {eye_offset/2, -eye_offset};
	for(int i=0; i<2; i++)
	{
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		Camera::GetSingleton().MoveAnaglyph(tEyePos[i]);
		Camera::GetSingleton().RenderLookAt();

		fboAnaglyph[i].Begin();
		{
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			SceneManager::GetSingleton().Render();
		}
		fboAnaglyph[i].End();
	}

	Camera::GetSingleton().RestoreCamera();



	// Displays a quad on screen
	Shader* pAnag = res.getShader("anaglyph");
	pAnag->Activate();
	{
		fboAnaglyph[0].Bind(0);
		pAnag->UniformTexture("texLeftEye", 0);
		fboAnaglyph[1].Bind(1);
		pAnag->UniformTexture("texRightEye", 1);

		DrawQuadAtScreen();

		fboAnaglyph[1].Unbind(1);
		fboAnaglyph[0].Unbind(0);
	}
	pAnag->Deactivate();
}

void Effects::displaySceneWithoutAnaglyph(void)
{
	VarManager& var = VarManager::GetSingleton();
	ResourceManager& res = ResourceManager::GetSingleton();

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	Camera::GetSingleton().RenderLookAt();
	
	if(var.getb("enable_effects"))
	{
		// Render the normal scene
		if(!var.getb("enable_pdc"))
		{
			fboScreen.Begin();
			{
				glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
				SceneManager::GetSingleton().Render();
			}
			fboScreen.End();
		}
		else
		{
			fboDepth.Begin();
			{
				glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
				SceneManager::GetSingleton().Render();
			}
			fboDepth.End();

			fboScreen.Begin();
			{
				glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
				SceneManager::GetSingleton().Render();
			}
			fboScreen.End();
		}

		// Render bloom
		if(var.getb("enable_bloom"))
		{
			GenerateBloomTexture();
		}

		// Render blur for depth
		if(var.getb("enable_pdc"))
		{
			GenerateDepthBlurTexture();
		}

		// Final render with the effects
		Shader* pShdFX = res.getShader("postfx");
		int id = 0;
		pShdFX->Activate();
		{
			if(!var.getb("enable_pdc"))
			{
				fboScreen.Bind(id);
			}
			else
			{
				fboDepthScreen.Bind(id);
			}
			pShdFX->UniformTexture("texScreen", id++);
			pShdFX->Uniform("enable_bloom",var.getb("enable_bloom"));
			pShdFX->Uniform("enable_vignette",var.getb("enable_vignette"));
			pShdFX->Uniform("enable_noise",var.getb("enable_noise"));
			pShdFX->Uniform("enable_pdc",var.getb("enable_pdc"));
			pShdFX->Uniform("enable_underwater",var.getb("enable_underwater"));	
			pShdFX->Uniform("enable_rain",var.getb("enable_rain"));
			//pShdFX->Uniform("enable_gui",var.getb("enable_gui"));	

			if(var.getb("enable_underwater"))
			{
				Texture2D* texWater = res.getTexture2D("terrain_water_NM.jpg");
				texWater->Bind(id);
				pShdFX->UniformTexture("texWaterNoiseNM", id++);
				pShdFX->Uniform("screnWidth", var.geti("win_width"));
				pShdFX->Uniform("screnHeight", var.geti("win_height"));
				pShdFX->Uniform("noise_tile", 0.05f);
				pShdFX->Uniform("noise_factor", 0.02f);
				pShdFX->Uniform("time", GlobalTimer::dT);
			}
			
			if(var.getb("enable_bloom"))
			{
				fboBloomFinal.Bind(id);
				pShdFX->UniformTexture("texBloom", id++);
				pShdFX->Uniform("bloom_factor", 4.0f);
			}

			if(var.getb("enable_noise"))
			{
				m_pBruit->Bind(id);
				pShdFX->UniformTexture("texBruit", id++);
				randomCoeffNoise = (float)(rand()%1000)/1000.0f;
				pShdFX->Uniform("randomCoeffNoise", randomCoeffNoise);
				pShdFX->Uniform("randomCoeffFlash", randomCoeffFlash);
			}

			if(var.getb("enable_vignette"))
			{
				this->m_pVignette->Bind(id);
				pShdFX->UniformTexture("texVignette", id++);
			}

			// MICHAEL STICK YOUR UI DRAWING HERE BY BINDING IT AND PASSING IT TO THE SHADER!
			// CHECK THE SHADER IT IS ALREADY SET UP IN THEIR.
			//if(var.getb("enable_gui"))
			//{
			//	this->m_pGui->Bind(id);
			//	pShdFX->UniformTexture("texGui", id++);
			//}

			DrawQuadAtScreen();

			/*if(var.getb("enable_gui"))
			{
				this->m_pGui->Unbind(--id);
			}*/
			if(var.getb("enable_vignette"))
			{
				this->m_pVignette->Unbind(--id);
			}

			if(var.getb("enable_noise"))
			{
				m_pBruit->Unbind(--id);
			}
		
			if(var.getb("enable_bloom"))
			{
				fboBloomFinal.Unbind(--id);
			}

			if(var.getb("enable_underwater"))
			{
				Texture2D* texWater = res.getTexture2D("terrain_water_NM.jpg");
				texWater->Unbind(--id);
			}

			if(!var.getb("enable_pdc"))
			{
				fboScreen.Unbind(--id);
			}
			else
			{
				fboDepthScreen.Unbind(--id);
			}
		}
		pShdFX->Deactivate();

	}
	else {
		SceneManager::GetSingleton().Render();
	}
}

void Effects::GenerateBloomTexture()
{
	ResourceManager& res = ResourceManager::GetSingleton();

	// Render the light sources
	Shader* pShdBright = res.getShader("bright");
	fboBloomFinal.Begin();
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		pShdBright->Activate();
		{
			fboScreen.Bind(0);
				pShdBright->UniformTexture("texScreen", 0);
				pShdBright->Uniform("threshold", 0.95f);
				DrawQuadAtScreen();
			fboScreen.Unbind(0);
		}
		pShdBright->Deactivate();
	}
	fboBloomFinal.End();


	// Blur horizontal light sources
	Shader* pShdBlur = res.getShader("blur");
	fboBloomFinalTemp.Begin();
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		pShdBlur->Activate();
		{
			fboBloomFinal.Bind(0);
				pShdBlur->UniformTexture("texScreen", 0);
				pShdBlur->Uniform("size", vec2((float)fboBloomFinal.getWidth(), (float)fboBloomFinal.getHeight()));
				pShdBlur->Uniform("horizontal", true);
				pShdBlur->Uniform("kernel_size", 10);
				DrawQuadAtScreen();
			fboBloomFinal.Unbind(0);
		}
		pShdBlur->Deactivate();
		
	}
	fboBloomFinalTemp.End();


	// Blur on vertical light sources
	fboBloomFinal.Begin();
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		pShdBlur->Activate();
		{
			fboBloomFinalTemp.Bind(0);
				pShdBlur->UniformTexture("texScreen", 0);
				pShdBlur->Uniform("size", vec2((float)fboBloomFinalTemp.getWidth(), (float)fboBloomFinalTemp.getHeight()));
				pShdBlur->Uniform("horizontal", false);
				DrawQuadAtScreen();
			fboBloomFinalTemp.Unbind(0);
		}
		pShdBlur->Deactivate();
		
	}
	fboBloomFinal.End();


}

void Effects::GenerateDepthBlurTexture()
{
	VarManager& var = VarManager::GetSingleton();
	ResourceManager& res = ResourceManager::GetSingleton();

	// Vertical blur	
	fboDepthScreenTemp.Begin();
	{
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			Shader* pShdDB = res.getShader("depthblur");
			pShdDB->Activate();
			{
				pShdDB->Uniform("screenWidth", var.geti("win_width"));
				pShdDB->Uniform("screenHeight", var.geti("win_height"));
				pShdDB->Uniform("bHorizontal", false);

				fboScreen.Bind(0);
				pShdDB->UniformTexture("texScreen", 0);
					
				fboDepth.Bind(1);
				pShdDB->UniformTexture("texDepth",1);
					
				DrawQuadAtScreen();
				
				fboDepth.Unbind(1);
				fboScreen.Unbind(0);
			}
			pShdDB->Deactivate();
				
	}
	fboDepthScreenTemp.End();

	// Blur horizontal
	fboDepthScreen.Begin();
	{
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			Shader* pShdDB = res.getShader("depthblur");
			pShdDB->Activate();
			{
				pShdDB->Uniform("screenWidth", var.geti("win_width"));
				pShdDB->Uniform("screenHeight", var.geti("win_height"));
				pShdDB->Uniform("bHorizontal", true);

				fboDepthScreenTemp.Bind(0);
				pShdDB->UniformTexture("texScreen", 0);
					
				fboDepth.Bind(1);
				pShdDB->UniformTexture("texDepth",1);
					
				DrawQuadAtScreen();
				
				fboDepth.Unbind(1);
				fboDepthScreenTemp.Unbind(0);
			}
			pShdDB->Deactivate();
				
	}
	fboDepthScreen.End();
}

void Effects::Render()
{
	VarManager& var = VarManager::GetSingleton();

	// Render anaglyph
	if(var.getb("enable_anaglyph"))
	{
		displaySceneWithAnaglyph();
	}
	// Render without anaglyph
	else
	{
		displaySceneWithoutAnaglyph();
	}
}

void Effects::idle()
{
	VarManager& var = VarManager::GetSingleton();
	ResourceManager& res = ResourceManager::GetSingleton();
		
	if(var.getb("enable_noise"))
	{
		this->m_fT += GlobalTimer::dT;
		if(m_fT > m_fPeriod )
		{
			m_fT = 0;
			randomCoeffFlash = (float)(rand()%1000)/1000.0f;
		}
	}
}
