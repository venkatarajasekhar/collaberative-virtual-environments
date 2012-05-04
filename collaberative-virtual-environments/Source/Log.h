// Log.h: interface for the CLog class.
//
//////////////////////////////////////////////////////////////////////

#ifndef LOG_H
#define LOG_H

#include "console.h"
#include <fstream>
#include <stdarg.h>

const unsigned char LOG_APP			=  1;
const unsigned char LOG_CLIENT		=  2;
const unsigned char LOG_SERVER		=  4;
const unsigned char LOG_USER		=  8;
const unsigned char LOG_PROFILER	= 16;
const unsigned char LOG_MASTER		= 32;

#define MAX_LOG_STRINGS 256

class CLog
{
public:
	CLog();

	std::ofstream appLog;
	std::ofstream clientLog;
	std::ofstream serverLog;
	std::ofstream profilerLog;

	std::string logStrings[MAX_LOG_STRINGS];
	bool LoadStrings();

public:
	static CLog &Get();

	bool Init();
	void Shutdown();

	void Write(int target, bool printToScreen, const char *msg, ...);
	void Write(int target, bool printToScreen, unsigned long msgID, ...);
	std::string String(int target, unsigned long msgID, ...);
};

#endif // LOG_H
