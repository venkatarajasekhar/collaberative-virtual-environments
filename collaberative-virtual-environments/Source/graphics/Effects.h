#ifndef EFFECTS_H
#define EFFECTS_H

#include "../utilities/singleton.h"
#include "../graphics/FrameBufferObject.h"
#include "../graphics/texture/texture2d.h"

#define FBO_BLOOM_SAMPLES 1

SINGLETON_BEGIN( Effects )
public:
	// Rendered on screen
	FrameBufferObject	fboScreen;
	FrameBufferObject	fboDepth;

	// Depth of field
	FrameBufferObject	fboDepthScreen;
	FrameBufferObject	fboDepthScreenTemp;

	// Anaglyph
	FrameBufferObject	fboAnaglyph[2];

	// Bloom
	FrameBufferObject	fboBloomFinal;
	FrameBufferObject	fboBloomFinalTemp;

	// Vignette
	Texture2D*			m_pVignette;
	
	// MICHAEL GUI TEXTURE HERE
	Texture2D*			m_pGui;
	
	// Noise film
	Texture2D*	m_pBruit;
	float randomCoeffNoise;
	float randomCoeffFlash;
	float m_fT;
	float m_fPeriod;

private:
	void displaySceneWithoutAnaglyph(void);
	void displaySceneWithAnaglyph(void);
	void GenerateBloomTexture();
	void GenerateDepthBlurTexture();

public:
	void init();
	void idle();
	void Render();
	void reshapeFBO(int newwidth , int newheight);
	
	void DrawQuadAtScreen();

SINGLETON_END()

#endif