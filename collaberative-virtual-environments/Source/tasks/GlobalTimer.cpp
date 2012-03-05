#include <SDL.h>

#include "GlobalTimer.h"

float GlobalTimer::dT=0;
unsigned long GlobalTimer::lastFrameIndex=0;
unsigned long GlobalTimer::thisFrameIndex=0;
float GlobalTimer::totalTime=0;

GlobalTimer::GlobalTimer()
{

}

GlobalTimer::~GlobalTimer()
{

}

bool GlobalTimer::Start()
{
	Reset();
	return true;
}

void GlobalTimer::Update()
{
	lastFrameIndex=thisFrameIndex;
	thisFrameIndex=SDL_GetTicks();
	dT=((float)(thisFrameIndex-lastFrameIndex))/1000.0f;
	totalTime=SDL_GetTicks()/1000.0f;
}

void GlobalTimer::Reset()
{
	thisFrameIndex=SDL_GetTicks();
	lastFrameIndex=thisFrameIndex;
	dT=0;
	totalTime=0;
}

void GlobalTimer::Stop()
{

}

float GlobalTimer::GetTime()
{
	return totalTime;
}
