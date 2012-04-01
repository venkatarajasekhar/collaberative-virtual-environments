#include "../graphics/SingleCamera.h"

#include "player.h"

Player::Player()
{
	m_state				= IDLE;
	m_szName			= "Player";
	m_vColor			= vec3( 1.f, 1.0f, 1.0f );
	m_vPosition			= vec3( .0f, .0f, .0f );
	m_pointer.m_owner   = m_szName;
	m_pointer.m_vColor	= m_vColor;
	m_vPointerPosition	= m_pointer.m_vPosition;

	m_pointer.m_areaOfInfluence = 1;
}

Player::~Player()
{}

void Player::Draw()
{
	m_pointer.Draw();
}

void Player::Update()
{
	m_vPosition = Camera::GetSingleton().getEye();
	m_vPointerPosition	= m_pointer.m_vPosition;

	m_pointer.Update();
}

void Player::EditAoi(int delta)
{
	m_pointer.EditAoi(delta);
}