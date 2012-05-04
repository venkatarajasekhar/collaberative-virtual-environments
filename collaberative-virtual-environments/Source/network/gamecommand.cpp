#include "gamecommand.h"
#include <assert.h>
#include <iostream>


void CommandQueue::Append( GamePacket *p )
{
	GamePacket *add = new GamePacket();

	add->next = NULL;
	if (this->first == NULL) {
		this->first = add;
	} else {
		this->last->next = add;
	}
	this->last = add;
	this->count++;
}

GamePacket *CommandQueue::Pop()
{
	if( this->first == NULL ) return NULL;
	GamePacket *front = this->first;
	this->first = this->first->next;
	this->count--;
	return front;
}

GamePacket *CommandQueue::Peek()
{
	return this->first;
}

void CommandQueue::Free()
{
	GamePacket *p;
	while ( ( p = this->Pop()) != NULL )
	{
		if( p != NULL ) free( p );
	}
	assert(this->count == 0);
}
