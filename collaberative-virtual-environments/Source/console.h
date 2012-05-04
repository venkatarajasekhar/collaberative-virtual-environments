/* console.h 11/09/11 Matt Allan */

#ifndef CONSOLE_H
#define CONSOLE_H

//#include <process.h>	// Threading
#include <windows.h>
#include <iostream>
#include <algorithm>
#include <string>
#include "utilities\singleton.h"


#define MAX_MESSAGE_SIZE 2048

// String Colours
#define COLOR_RED		1 // Error Messages
#define COLOR_GREEN		2 // Login Server information
#define COLOR_BLUE		3 // Messaging
#define COLOR_YELLOW	4 // Server information
#define COLOR_CYAN		5 // Private Message
#define COLOR_MAGENTA	6 // Log
#define COLOR_WHITE		7 // Standard Messaging

#define COLOR_ERROR		COLOR_RED
#define COLOR_LOGIN		COLOR_GREEN
#define COLOR_SERVER	COLOR_YELLOW
#define COLOR_MESSAGING	COLOR_YELLOW
#define COLOR_PRIVATE	COLOR_CYAN
#define COLOR_STANDARD	COLOR_WHITE
#define COLOR_LOG		COLOR_MAGENTA

class CConsole: public Singleton<CConsole> {
private:
	HANDLE hOut;

public:
	CConsole();
	~CConsole();

	void cInput( void* );
	void cOutput( int color, char* msg, ... );
	
	// Threading of the Console
	static UINT ThreadFunc(LPVOID param) {
		CConsole* This = (CConsole*)param;
		This->cInput(0); // call a member function
		return 0;
	}
	void StartCInputFunc() {
		::CreateThread(0,0,(LPTHREAD_START_ROUTINE)ThreadFunc,
		(LPVOID)this,0,0);
	}


};
#endif