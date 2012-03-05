// Kernel.h: interface for the Kernel class.

#ifndef KERNEL_H
#define KERNEL_H

#include <list>

#include "../utilities/singleton.h"

class ITask;

SINGLETON_BEGIN( Kernel )
public:
	Kernel();
	virtual ~Kernel();

	int Execute();

	bool AddTask(		ITask* t);
	void SuspendTask(	ITask* t);
	void ResumeTask(	ITask* t);
	void RemoveTask(	ITask* t);
	void KillAllTasks();

protected:
	std::list< ITask* > taskList;
	std::list< ITask* > pausedTaskList;
SINGLETON_END()

class ITask
{
public:
	ITask(){canKill=false;priority=5000;}
	virtual bool Start()=0;
	virtual void OnSuspend(){};
	virtual void Update()=0;
	virtual void OnResume(){};
	virtual void Stop()=0;

	bool canKill;
	long priority;
};

#endif // KERNEL_H
