/* gamecommand.h 01/05/12 Matt Allan */

#ifndef NETWORK_GAMECOMMAND
#define NETWORK_GAMECOMMAND

#define NULL 0

enum Commands{
	CMD_something,	
	CMD_nextcommand
};

struct GameCommandContainer {
	unsigned int dfsfsd;
	//TileIndex tile;                  ///< tile command being executed on.
	unsigned int p1;                       ///< parameter p1.
	unsigned int p2;                       ///< parameter p2.
	//uint32 cmd;                      ///< command being executed.
	//CommandCallback *callback;       ///< any callback function executed upon successful completion of the command.
	//char text[32 * MAX_CHAR_LENGTH]; ///< possible text sent for name changes etc, in bytes including '\0'.
};

struct GamePacket : GameCommandContainer {
	GamePacket *next;		///< the next command packet (if in queue)
	//	team;		///< company that is executing the command
	unsigned int frame;		///< the frame in which this packet is executed
	bool my_cmd;			///< did the command originate from "me"
};

/** A queue of CommandPackets. */
class CommandQueue {
	GamePacket *first;		///< The first packet in the queue.
	GamePacket *last;		///< The last packet in the queue; only valid when first != NULL.
	unsigned int count;		///< The number of items in the queue.

public:
	/** Initialise the command queue. */
	CommandQueue() : first(NULL), last(NULL) {}
	/** Clear the command queue. */
	~CommandQueue() { this->Free(); }
	void Append(GamePacket *p);
	GamePacket *Pop();
	GamePacket *Peek();
	void Free();
	/** Get the number of items in the queue. */
	unsigned int Count() const { return this->count; }
};

void NetworkDistributeCommands();
void NetworkExecuteLocalCommandQueue();
void NetworkFreeLocalCommandQueue();
//void NetworkSyncCommandQueue(NetworkClientSocket *cs);


#endif /* NETWORK_GAMECOMMAND */