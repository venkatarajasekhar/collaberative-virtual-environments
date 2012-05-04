#include "console.h"
#include "network\clients.h"
#include "network\network.h"
#include "strings.h"
#include "md5.h"

//extern bool quit_app;

/* Thread our input console on the constructor. */
CConsole::CConsole()
{
	hOut = GetStdHandle(STD_OUTPUT_HANDLE);
	StartCInputFunc();
}

CConsole::~CConsole()
{
}

/* Input handler. */
void CConsole::cInput( void* )
{
	for( ; ; )
	{

		// Set up input buffer
		char buffer[ MAX_MESSAGE_SIZE ];

		// Get console input
		gets_s(buffer);
	
		// Convert the buffer to string for manipulation
		std::string line = buffer;
	
		// Find First space in buffer
		int findfst = line.find( " ", 0 );

		// Assign Variables to each part of the message
		std::string cmd = line.substr( 0, findfst );
		std::transform( cmd.begin(), cmd.end(), cmd.begin(), ::tolower );
		std::string newbuffer = line.substr( findfst + 1 );

	///// Console Commands /////

		// Help Command
		if( cmd == "menu" ) {
			printf("\nConsole Commands: \n");
			printf("	Menu				Shows this help menu.\n");
			printf("	cls				Clears this screen.\n");
			printf("	quit				Closes the application.\n");
			printf("	Status				Network Information.\n");
			//if( CNetManager::GetSingleton().isOnline() == false ) {
				printf("\nLogin Server Commands:\n");
				printf("	Login <username> <password>	Connect to Login Server.\n");
			//}
			//if( CNetManager::GetSingleton().isHosting() == false ) {
				printf("\nClient Connect Commands: \n");
				//if( CNetManager::GetSingleton().isOnline() == true ) {
					printf("	Servers				Shows a list of online servers.\n");
					printf("	Join <id>			Connect by ID (From server List).\n");
					printf("	Join <id> <pass>		Connect with password.\n");
				//}
				printf("	Connect <ip> <port>		Connect Using Address and Port.\n");
				printf("	Connect <ip> <port> <pass>	Connect with password.\n");
				printf("	Host				Host a game.\n");
			//}
			//if( CNetManager::GetSingleton().isConnected() == true ) {
				printf("\nClient Commands: \n");
				printf("	Clients				Shows a list of online clients.\n");
				printf("	Close				Close the current game.\n");
			//}
			//if( CNetManager::GetSingleton().isHosting() == true ) {
				printf("\nServer Commands\n");
				printf("	Stop		Closes the current host session\n");
				printf("	Hostname <hostname>	Set server name and Host a game.\n");
				printf("	Password		Change server password\n");
				printf("	kick <user>		Kicks the user from chat.\n");
			//}
			//if( CNetManager::GetSingleton().isConnected() == true || CNetManager::GetSingleton().isHosting() == true ) {
				printf("\nIn Game Options\n");
				printf("	Say <message>		Send a message to all users\n");
				printf("	Msg <user> <message>	Send a message to an individual user\n");
				printf("	Name <name>		Change display name.\n\n");
			//}
		}

		// Clear Screen
		if( cmd == "cls" ) {
			system("cls");
		}

		// Shutdown server
		if( cmd == "quit" ) {
			printf("this function has been disabled.\n");
			//NetworkDisconnect();
			//quit_app = true;
		}

		// Network Status
		if( cmd == "st" ) {
			NetworkStatus();
		}


	///// Main Server Commands /////

		if( cmd == "login" )
		{
			// DO ERROR HANDLING FIRST
			int findspc = newbuffer.find( " ", 0 );	
			// There must be a space within this command to seperate the user from the password
			if( findspc != -1 )
			{	
				// split user and pass from newbuffer
				std::string user = newbuffer.substr( 0, findspc );
				std::string pass = md5( newbuffer.substr( findspc + 1 ) );
				NetworkSignin( &user[0], &pass[0] );
			}
		}

	///// Client Commands /////

		// List All Clients
		if( cmd == "clients" ) {
			NetworkClientsPrintClients();
		}

		// Send Message to All Users
		if( cmd == "say" ) {
			NetworkClientSendChat( NETWORK_ACTION_CHAT, TYPE_BROADCAST, 0, &newbuffer[0] );
		}

		// Send private message
		if( cmd == "msg" ) {
		
			// DO ERROR HANDLING FIRST
			int findspc = newbuffer.find( " ", 0 );
		
			// There must be a space within this command to seperate the user from the message
			if( findspc != -1 ) {
			
				// split user and message from newbuffer
				std::string user = newbuffer.substr( 0, findspc );
				std::string message = newbuffer.substr( findspc + 1 );

	/*			int namefound = CNetworkClients::GetSingleton().FindName( &user[ 0 ] );
				if( namefound >= 0 ) {
					NetworkClientSendChat( NETWORK_ACTION_CHAT, TYPE_CLIENT, namefound, &message[0] );
				}else{
					printf( "[console] user not found.\n" );
				}*/
			}
		}

		// Change display name
		if( cmd == "name" ) {
			NetworkUpdateClientName( &newbuffer[0] );
			/* ^^^^^ HEAP ERROR, WORKS FINE IN DEBUGGING MODE BUT NOT EXTERNALY ^^^^^ */
		}

		// Connect to server
		if( cmd == "join" ) {
			Network_Address _local( 192, 168, 1, 102, 30000 );
			NetworkJoinGame( _local, 3, "Hero Armstrong", "James0101" );
		}

		// Connect to server
		if( cmd == "connect" ) {

			/* Count the number of space in the string as this will determine what the user has entered. */
			unsigned char spaces = 0;
			for ( int counter = 0; newbuffer[ counter ]!= '\0'; counter++)
				if (newbuffer[ counter ] == ' ' && newbuffer[ counter + 1 ] != ' ')
					spaces++;

			/* What will be our order be as is port important,
			 * ip, port, serverpassword, teampassword.*/
			unsigned short _port = 30000;			///< Set our server port number.
			Network_Address _local( 0, _port );		///< Set our server address.
			char *_serverpassword = new char[ 32 ];	///< Set our server password.
			std::string _server;					///< Temp string for server address.
			std::string _tmpport;					///< Temp string for port number.
			std::string _tmppass;					///< Temp string for password.

			switch( spaces ) {
			case 2:

				/* Copy out the password from the input string */
				_tmppass = md5( newbuffer.substr( newbuffer.find_last_of( " " ) + 1, newbuffer.size() ) );
				strecpy( _serverpassword, _tmppass.c_str(), lastof( _tmppass ) );
			case 1:

				/* Copy out the port number from the input string */
				_tmpport = newbuffer.substr( newbuffer.find_first_of( " " ) + 1, newbuffer.find_last_of( " " ) - newbuffer.find_first_of( " " ) - 1 );
				_port = atoi( _tmpport.c_str() );
			case 0:

				/* Connect to the server */
				_server = newbuffer.substr( 0,  newbuffer.find_first_of( " " ) );
				_local.SetAddress( ConvertIPtoInt( (char*)_server.c_str() ), _port );
				NetworkJoinGame( _local, 3, _serverpassword, "" );
				break;
			default:
				printf( "Too many parameters. Use 'connect <ip> <port> <password>'\n" );
				break;
			}
		}

		// Close
		if( cmd == "close" ) {
			NetworkDisconnect();
		}
	
		// List Servers
		if( cmd == "servers" ) {
			NetworkRequestGameList();
		}

	///// Server Commands /////

		// Host Server
		if( cmd == "host" ) {
			NetworkServerStart();
		}

		// HostName
		if( cmd == "hostname" ) {
			NetworkHostName( &newbuffer[0] );
		}

		// Server Password
		if( cmd == "password" ) {
			NetworkServerPassword( newbuffer );
		}

		// Kick Users
		if( cmd == "kick" ) {
			//NetworkServerKickClient( &&CLIENT_NAME&& );
		}

		// Stop Hosting
		if( cmd == "stop" ) {
			NetworkDisconnect();
		}

		// Send File
		if( cmd == "file" ) {

		}

		// Restart server
		if( cmd == "restart" )
		{
			NetworkServerRestart();
		}
	}
}

void CConsole::cOutput( int color, char* msg, ... ) {
	va_list args; va_start(args,msg);
	char szBuf[1024];
	vsprintf_s(szBuf,msg,args);
	
	switch( color ) {
		case COLOR_RED: {
			SetConsoleTextAttribute(hOut, FOREGROUND_RED | FOREGROUND_INTENSITY);
			break; }
		case COLOR_GREEN: {
			SetConsoleTextAttribute(hOut, FOREGROUND_GREEN | FOREGROUND_INTENSITY);
			break; }
		case COLOR_BLUE: {
			SetConsoleTextAttribute(hOut, FOREGROUND_BLUE | FOREGROUND_INTENSITY);
			break; }
		case COLOR_YELLOW: {
			SetConsoleTextAttribute(hOut, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY);
			break; }
		case COLOR_CYAN: {
			SetConsoleTextAttribute(hOut, FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY);
			break; }
		case COLOR_MAGENTA: {
			SetConsoleTextAttribute(hOut, FOREGROUND_BLUE | FOREGROUND_RED | FOREGROUND_INTENSITY);
			break; }
		case COLOR_WHITE: {
			SetConsoleTextAttribute(hOut, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY);
			break; }
	}
	std::cout << szBuf << std::endl;
	SetConsoleTextAttribute(hOut, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY);
	va_end(args);
}