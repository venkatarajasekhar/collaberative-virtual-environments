#ifndef USING_CNE
#define USING_CNE
#endif
//#include "engine.h"
#include <time.h>
#include "Log.h"
#include "network\network_core\config.h"

/*
CMMPointer< CSettingVariable<std::string> > CLog::appLogName=0;
CMMPointer< CSettingVariable<std::string> > CLog::clientLogName=0;
CMMPointer< CSettingVariable<std::string> > CLog::serverLogName=0;
*/

CLog::CLog()
{

}

CLog &CLog::Get()
{
	static CLog log;
	return log;
}

bool CLog::Init()
{
	appLog.open("applog.txt");
	clientLog.open("clntlog.txt");
	serverLog.open("srvrlog.txt");
	profilerLog.open("profilerlog.txt");
	//user errors get logged to client

	//load the strings file
	if(!LoadStrings())return false;

	return true;
}

void CLog::Shutdown()
{
	appLog.close();
	clientLog.close();
	serverLog.close();
	profilerLog.close();
}

void CLog::Write(int target, bool printToScreen, const char *msg, ...)
{
	va_list args; va_start(args,msg);
	char szBuf[ SEND_SP ];
	vsprintf_s(szBuf,msg,args);

	time_t _time;
	struct tm *time_struct;
	char time_buffer[ 80 ];

	time( &_time );
	time_struct = localtime( &_time );
	strftime( time_buffer, 80, "%d/%m/%Y %H:%M:%S ", time_struct  );	

	//CConsole::GetSingleton().cOutput( COLOR_LOG, szBuf );
	if( printToScreen ) printf( "%s\n", szBuf );

	if(target&LOG_APP)
	{
		appLog << time_buffer << szBuf << "\n";
#ifdef _DEBUG
		appLog.flush();
#endif
	}
	if(target&LOG_CLIENT)
	{
		clientLog << time_buffer << szBuf << "\n";
#ifdef _DEBUG
		clientLog.flush();
#endif
	}
	if(target&LOG_SERVER)
	{
		serverLog << time_buffer << szBuf << "\n";
#ifdef _DEBUG
		serverLog.flush();
#endif
	}
	if(target&LOG_USER)
	{
#ifdef WIN32
		//MessageBox(NULL,szBuf,"Message",MB_OK);
#else
#error User-level logging is not implemented for this platform.
#endif
	}
	if(target&LOG_PROFILER)
	{
		profilerLog << time_buffer << szBuf << "\n";
#ifdef _DEBUG
		profilerLog.flush();
#endif
	}
	va_end(args);
}

void CLog::Write(int target, bool printToScreen, unsigned long msgID, ...)
{
	va_list args; va_start(args, msgID);
	char szBuf[ SEND_SP ];
	vsprintf_s(szBuf,logStrings[msgID].c_str(),args);
	Write(target, printToScreen, szBuf);
	va_end(args);
}

std::string CLog::String(int target, unsigned long msgID, ...)
{
	va_list args; va_start(args, msgID);
	char szBuf[ SEND_SP ];
	vsprintf_s(szBuf,logStrings[msgID].c_str(),args);
	std::string sText = szBuf;
	va_end(args);

	return sText;
}

//#ifdef WIN32
////under Win32, the strings get read in from a string table resource
//bool CLog::LoadStrings()
//{
//	for(unsigned long i=0;i<MAX_LOG_STRINGS;i++)
//	{
//		char szBuf[ SEND_SP ];
//		if(!LoadString(GetModuleHandle("engine"),i,szBuf,1024))break; //returning 0 means no more strings
//		logStrings[i]=szBuf;
//	}
//	return true;
//}
//
//#else
//other platforms load the strings in from strings.txt
bool CLog::LoadStrings()
{
	std::ifstream in("strings.txt");
	if(!in.is_open())return false;

	unsigned long index=0;

	while(!in.eof())
	{
		char szBuf[ SEND_SP ];
		in.getline(szBuf, SEND_SP );
		logStrings[index++]=szBuf;
	}

	return true;
}

//#endif