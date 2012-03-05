#ifndef SCENETASK_H
#define SCENETASK_H

#include "../kernel/Kernel.h"
#include "../scenes/SceneManager.h"

/// The Scene Task contains all logic and drawing for the scene manager.
class SceneTask : public ITask
{
public:
	bool Start();
	void Update();
	void Stop();
	void OnResume();

	void Idle();
	bool ResizeWindow(int newWidth = NULL, int newHeight = NULL);
	void Keyboard();

private:

};

#endif