#include <stdio.h>
#include <string.h>
#include "clients.h"
#include "..\Log.h"
#include "..\strings.h"

unsigned int NetworkClientsFindName( char *name )
{
	NetworkClients *nc;
	FOR_ALL_CLIENTS( nc )
	{
		if( strcmp( name, nc->client_name ) == 0 ) return nc->client_id;
	}
	return -1;
}

void NetworkClientsPrintClients()
{
	unsigned int _online = 0, _offline = 0;
	NetworkClients *nc;
	FOR_ALL_CLIENTS( nc )
	{
		if( nc->client_status == STATUS_ACTIVE || nc->client_status == STATUS_AUTHORISED )
		{
			_online++;
		}else{
			_offline++;
		}
	}

	printf( "\n" );
	CLog::Get().Write( LOG_CLIENT, true, Connect_String( "Clients list: (online: %i, offline: %i)", _online, _offline ) );
	CLog::Get().Write( LOG_CLIENT, true, "ID	Ping	IP Address	Name" );
	
	FOR_ALL_CLIENTS( nc )
	{
		//if( nc->client_status == STATUS_ACTIVE || nc->client_status == STATUS_AUTHORISED )
		//{
			CLog::Get().Write( LOG_CLIENT, true, Connect_String( "%i	%i	%i.%i.%i.%i	%s", nc->client_id, nc->ping_time, nc->client_address.GetA(), nc->client_address.GetB(), nc->client_address.GetC(), nc->client_address.GetD(), nc->client_name ) );
		//}
	}
}

void Draw()
{
	NetworkClients *nc;
	FOR_ALL_CLIENTS( nc )
	{
		if( nc->client_status == STATUS_ACTIVE || nc->client_status == STATUS_AUTHORISED )
		{
			/* Add draw code here. */
		}
	}
}