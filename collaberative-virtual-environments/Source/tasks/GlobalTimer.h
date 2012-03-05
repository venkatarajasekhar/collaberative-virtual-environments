#ifndef GLOBALTIMER_H
#define GLOBALTIMER_H

#include "../kernel/Kernel.h"

class GlobalTimer : public ITask
{
public:
	GlobalTimer();
	virtual ~GlobalTimer();
	
	static float dT;
	static unsigned long lastFrameIndex;
	static unsigned long thisFrameIndex;
	static float totalTime;	///< Total time in seconds.

	bool Start();
	void Update();
	void Stop();

	void Reset();

	static float GetTime();

	static inline unsigned long GlobalTimer::TimeToIndex(float t) { return (unsigned long)(t*1000.0f); }
	static inline float GlobalTimer::IndexToTime(unsigned long i) { return ((float)i)/1000.0f;	      }
};

#endif
