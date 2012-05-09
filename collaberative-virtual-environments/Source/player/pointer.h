#ifndef CPOINTER_H
#define CPOINTER_H

//#include "player.h"

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
		//NetworkMoveClent( m_vPosition, 0 );
	}
	void	MoveLeft(vec3 camLeft){
		m_vPosition += camLeft;

		// ###########
		// # NETWORK #
		// ###########
		//NetworkMoveClent( m_vPosition, 0 );
	}
	void	MoveTo(vec3 position){
		m_vPosition = position;

		// ###########
		// # NETWORK #
		// ###########
		//NetworkMoveClent( m_vPosition, 0 );
	}

	std::string		m_owner;
	vec3			m_vColor;
	vec3			m_vPosition;
	int				m_areaOfInfluence;
};

#endif // POINTER_H
