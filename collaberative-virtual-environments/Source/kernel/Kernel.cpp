#include <SDL.h>
#include <algorithm>

#include "../utilities/types.h"

#include "Kernel.h"

Kernel::Kernel()
{
	SDL_Init(0);
}

Kernel::~Kernel()
{
	SDL_Quit();
}

int Kernel::Execute()
{

	while(!taskList.empty())
	{
		{
			std::list< ITask* >::iterator it, thisIt;
			for(it=taskList.begin();it!=taskList.end();)
			{
				ITask *t=(*it);
				++it;
				if(!t->canKill)t->Update();
			}
			//loop again to remove dead tasks
			for(it=taskList.begin();it!=taskList.end();)
			{
				ITask *t=(*it);
				thisIt=it; ++it;
				if(t->canKill)
				{
					t->Stop();
					taskList.erase(thisIt);
					t=0;
				}
			}
		}
	}

	return 0;
}

bool Kernel::AddTask(ITask* t)
{
	if(!t->Start())return false;

	//keep the order of priorities straight
	std::list< ITask* >::iterator it;
	for(it=taskList.begin(); it!=taskList.end(); it++)
	{
		ITask* &comp=(*it);
		if(comp->priority>t->priority)break;
	}
	taskList.insert(it, t);
	return true;
}

void Kernel::SuspendTask(ITask* t)
{
	//check that this task is in our list - we don't want to suspend a task that isn't running
	if(std::find(taskList.begin(), taskList.end(), t)!=taskList.end())
	{
		t->OnSuspend();
		taskList.remove(t);
		pausedTaskList.push_back(t);
	}
}

void Kernel::ResumeTask(ITask* t)
{
	if(std::find(pausedTaskList.begin(), pausedTaskList.end(), t)!=pausedTaskList.end())
	{
		t->OnResume();
		pausedTaskList.remove(t);
		//keep the order of priorities straight
		std::list< ITask* >::iterator it;
		for(it=taskList.begin();it!=taskList.end();it++)
		{
			ITask* &comp=(*it);
			if(comp->priority>t->priority)break;
		}
		taskList.insert(it, t);
	}
}

void Kernel::RemoveTask(ITask* t)
{
	//assert(t.isValid() && "Tried to remove a null task.");
	if(std::find(taskList.begin(), taskList.end(), t)!=taskList.end())
	{
		t->canKill=true;
		return;
	}
	//check the suspended list
	std::list<ITask* >::iterator it;
	if((it=std::find(pausedTaskList.begin(), pausedTaskList.end(), t))!=taskList.end())
	{
		//kill the task immediately
		t->Stop();
		pausedTaskList.erase(it);
		return;
	}

}

void Kernel::KillAllTasks()
{
	for(std::list< ITask* >::iterator it=taskList.begin();it!=taskList.end();it++)
	{
		(*it)->canKill=true;
	}
	//kill all paused tasks right now
	for(std::list< ITask* >::iterator it=pausedTaskList.begin();it!=pausedTaskList.end();it++)
	{
		(*it)->Stop();
	}
	pausedTaskList.clear();
}