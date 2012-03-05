#ifndef VIDEOUPDATE_H
#define VIDEOUPDATE_H

#include <gl\glew.h>

#include "../kernel/Kernel.h"

struct frect;

class VideoUpdate : public ITask  
{
public:
	VideoUpdate();
	virtual ~VideoUpdate();

	// Settings
	static int scrWidth, scrHeight, scrBPP, fullScrn;
	static bool scrResized;

	bool Start();
	void Update();
	void Stop();

protected:
	int flags;
	int screenWidth, screenHeight, screenBPP;
	bool fullScreen;
};

#endif
