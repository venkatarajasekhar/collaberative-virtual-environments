#ifndef PONG_H
#define PONG_H

#include <GL/glew.h>
#include <GL/gl.h>
#include <GL/glu.h>

#include "../kernel/Kernel.h"

/// The pong game task contains the functions and logic for pong.  Used as an example.
class CPongTask : public ITask, public Singleton<CPongTask>
{
public:
	float p1pos, p2pos;
	float paddleWidth, paddleHeight;
	float ballX, ballY, ballVX, ballVY;
	float ballSize;

	bool Start();
	void Update();
	void Stop();
	void OnResume();

	//AUTO_SIZE;
};

#endif