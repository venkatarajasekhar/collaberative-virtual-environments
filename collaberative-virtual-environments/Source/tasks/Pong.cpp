#include <SDL.h>

#include "Pong.h"
#include "videoupdate.h"
#include "inputtask.h"
#include "GlobalTimer.h"

#include "../kernel/Kernel.h"
#include "../utilities/types.h"

bool CPongTask::Start()
{
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0.0f, 1.0f, 1.0f, 0.0f, -1.0f, 1.0f);

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glShadeModel(GL_SMOOTH);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	paddleWidth=0.1f; paddleHeight=0.01f;
	p1pos=p2pos=0.5f;
	ballX=0.5f; ballY=0.5f;
	ballVX=(float)(rand()%20-10)/20;
	ballVY=(float)(rand()%20-10)/20;
	ballSize=0.01f;

	return true;
}

void CPongTask::Update()
{
	glClear(GL_COLOR_BUFFER_BIT);
	if(InputTask::mouseDown(SDL_BUTTON_RIGHT))Kernel::GetSingleton().KillAllTasks();

	if(InputTask::keyDown(SDLK_1))
		Kernel::GetSingleton().SuspendTask((ITask*)this);

	glBegin(GL_QUADS);
	{
		glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
				
		//draw the ball
		glVertex2f(ballX-ballSize, ballY-ballSize);
		glVertex2f(ballX+ballSize, ballY-ballSize);
		glVertex2f(ballX+ballSize, ballY+ballSize);
		glVertex2f(ballX-ballSize, ballY+ballSize);
				
		//paddles
		glVertex2f(p2pos-paddleWidth, 0);
		glVertex2f(p2pos+paddleWidth, 0);
		glVertex2f(p2pos+paddleWidth, paddleHeight);
		glVertex2f(p2pos-paddleWidth, paddleHeight);

		glVertex2f(p1pos-paddleWidth, 1-paddleHeight);
		glVertex2f(p1pos+paddleWidth, 1-paddleHeight);
		glVertex2f(p1pos+paddleWidth, 1);
		glVertex2f(p1pos-paddleWidth, 1);
	}
	glEnd();

	p1pos+=((float)InputTask::dX)/200.0f;
	if(p1pos<paddleWidth)p1pos=paddleWidth;
	if(p1pos>1-paddleWidth)p1pos=1-paddleWidth;

	ballX+=ballVX*GlobalTimer::dT; ballY+=ballVY*GlobalTimer::dT;
	if(ballX<ballSize)ballVX=qAbs(ballVX);
	if(ballX>1-ballSize)ballVX=-qAbs(ballVX);
	if(ballY<ballSize+paddleHeight)
	{
		if((ballX>p2pos-paddleWidth)&&(ballX<p2pos+paddleWidth))
		{
			ballVY=qAbs(ballVY);
		}else{
			//CKernel::GetSingleton().KillAllTasks();
		}
	}
	if(ballY>1-ballSize-paddleHeight)
	{
		if((ballX>p1pos-paddleWidth)&&(ballX<p1pos+paddleWidth))
		{
			ballVY=-qAbs(ballVY);
		}else{
			//CKernel::GetSingleton().KillAllTasks();
		}
	}

	if(ballX>p2pos)p2pos+=0.5f*GlobalTimer::dT;
	if(ballX<p2pos)p2pos-=0.5f*GlobalTimer::dT;
	if(p2pos<paddleWidth)p2pos=paddleWidth;
	if(p2pos>1-paddleWidth)p2pos=1-paddleWidth;


}

void CPongTask::OnResume()
{
	gluLookAt( 0, 0, 0, 
			   0, 0, -1, 
			   0, 1, 0);
};

void CPongTask::Stop()
{

};