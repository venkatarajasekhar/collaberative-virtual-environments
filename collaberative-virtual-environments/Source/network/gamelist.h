/* gamelist.h 12/02/12 Matt Allan */

#ifndef NETWORK_GAME_LIST_H
#define NETWORK_GAME_LIST_H

/* Includes. */
#include <stdlib.h>
#include "base.h"
#include "..\pool\pool.h"

typedef unsigned int NetworkGameListIndex;
struct NetworkGameList;
typedef Pool<NetworkGameList, NetworkGameListIndex, 8, 256> NetworkGameListPool;
extern NetworkGameListPool _networkgamelist_pool;

struct NetworkGameList : NetworkGameListPool::PoolItem<&_networkgamelist_pool> {
	NetworkGameInfo info;		///< Game server information.
	Network_Address address;	///< The address of this server.
	bool online;				///< Is the server online (False if not ).
	unsigned char retries;		///< Number of retries (to stop re-querying)
	NetworkGameList() {}
	~NetworkGameList() {}
};

#define FOR_ALL_GAMELIST_FROM(var, start) FOR_ALL_ITEMS_FROM( NetworkGameList, gamelist_index, var, start )
#define FOR_ALL_GAMELIST( var ) FOR_ALL_GAMELIST_FROM( var, 0 )

void NetworkGameListPrint();

#endif /* NETWORK_GAME_LIST_H */