#include "InputTask.h"


//unsigned char *InputTask::keys=0;
unsigned char *InputTask::keys[]={0};
unsigned char *InputTask::oldKeys[]={0};
int InputTask::keyCount=0;
int InputTask::dX=0;
int InputTask::dY=0;
unsigned int InputTask::buttons=0;
unsigned int InputTask::oldButtons=0;


InputTask::InputTask()
{

}

InputTask::~InputTask()
{

}

bool InputTask::Start()
{
	unsigned char *tempKeys=SDL_GetKeyState(&keyCount);
	*keys = new unsigned char[keyCount];
	memcpy((unsigned char*)(*keys), tempKeys, sizeof(unsigned char)*keyCount);
	*oldKeys = new unsigned char[keyCount];
	dX=dY=0;
	SDL_PumpEvents(); SDL_PumpEvents();
	return true;
}

void InputTask::Update()
{
	SDL_PumpEvents();
	oldButtons=buttons;
	buttons=SDL_GetRelativeMouseState(&dX, &dY);
	memcpy((unsigned char*)(*oldKeys), (unsigned char*)(*keys), sizeof(unsigned char)*keyCount);
	unsigned char *tempKeys=SDL_GetKeyState(&keyCount);
	memcpy((unsigned char*)(*keys), tempKeys, sizeof(unsigned char)*keyCount);
}

void InputTask::Stop()
{
	*keys=0;
	*oldKeys=0;
}