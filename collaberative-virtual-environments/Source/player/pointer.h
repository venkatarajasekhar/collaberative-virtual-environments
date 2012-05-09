#ifndef CPOINTER_H
#define CPOINTER_H

//#include "player.h"
#include "..\network\Network.h"

/**
 * The Pointer
 * Highlights the area of the terrain
 * to be modified.
 */
class CPointer
{
public:
	CPointer();

	void	Draw();
	void	Update();
	void	EditAoi(int delta);

	void	MoveForward(vec3 camForwad){
		m_vPosition += camForwad; 

		// ###########
		// # NETWORK #
		// ###########
		Network::GetSingletonPtr(  )->sendCoordPacket( m_vPosition );
	}
	void	MoveLeft(vec3 camLeft){
		m_vPosition += camLeft;

		// ###########
		// # NETWORK #
		// ###########
		Network::GetSingletonPtr(  )->sendCoordPacket( m_vPosition );
	}
	void	MoveTo(vec3 position){
		m_vPosition = position;
		
		// ###########
		// # NETWORK #
		// ###########
		Network::GetSingletonPtr(  )->sendCoordPacket( m_vPosition );
	}

	std::string		m_owner;
	vec3			m_vColor;
	vec3			m_vPosition;
	int				m_areaOfInfluence;
};

#endif // POINTER_H
