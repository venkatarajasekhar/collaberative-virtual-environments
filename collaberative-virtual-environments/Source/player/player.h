#ifndef PLAYER_H
#define PLAYER_H

#include "../utilities/math.h"
#include "pointer.h"

class CPointer;

/**
 * Player Class
 * Used to handle all player drawing and logic.
 */
class Player
{
public:
	enum STATE{ IDLE, EDITING, BLAH };

	Player();
	~Player();

	void	Draw();
	void	Update();
	void	EditAoi(int delta);
	void	setAoi(int aoi){ m_pointer.m_areaOfInfluence = aoi;}

	std::string		m_szName;
	vec3			m_vPosition;
	vec3			m_vPointerPosition;
	vec3			m_vColor;
	STATE			m_state;
	CPointer		m_pointer;
};

#endif	// PLAYER_H
