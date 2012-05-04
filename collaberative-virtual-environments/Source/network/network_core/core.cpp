#include "core.h"

bool NetworkCoreInitialize() {
	WSADATA WsaData;
	int err_code = WSAStartup( MAKEWORD( 2, 0 ), &WsaData ) != NO_ERROR;
	switch( err_code ) {
	case WSASYSNOTREADY:
		CLog::Get().Write( LOG_CLIENT, true, "[udp core] The underlying network subsystem is not ready for network communication." );
		return false; break;
	case WSAVERNOTSUPPORTED:
		CLog::Get().Write( LOG_CLIENT, true, "[udp core] The version of Windows Sockets support requested is not provided by this particular Windows Sockets implementation." );
		return false; break;
	case WSAEINPROGRESS:
		CLog::Get().Write( LOG_CLIENT, true, "[udp core] A blocking Windows Sockets 1.1 operation is in progress." );
		return false; break;
	case WSAEPROCLIM:
		CLog::Get().Write( LOG_CLIENT, true, "[udp core] A limit on the number of tasks supported by the Windows Sockets implementation has been reached." );
		return false; break;
	case WSAEFAULT:
		CLog::Get().Write( LOG_CLIENT, true, "[udp core] The lpWSAData parameter is not a valid pointer." );
		return false; break;
	default:
		CLog::Get().Write( LOG_CLIENT, false, "[udp core] Winsock Initialised successfully." );
		break;
	}
	return true;
}

void NetworkCoreShutdown() {
	CLog::Get().Write( LOG_CLIENT, false, "[udp core] Winsock closed." );
	WSACleanup();
}

