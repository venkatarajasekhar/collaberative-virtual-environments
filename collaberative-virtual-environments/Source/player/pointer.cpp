#include "../utilities/ResourceManager.h"

#include "pointer.h"

CPointer::CPointer()
{}

void CPointer::Draw()
{
	SINGLETON_GET( ResourceManager, res )

	glPushAttrib(GL_ENABLE_BIT);
	// TODO : ADD SHADER AND AOI RING!
	glColor3f(m_vColor.r, m_vColor.g, m_vColor.b);
	glPushMatrix();
		glTranslatef(m_vPosition.x, m_vPosition.y, m_vPosition.z);
		glScalef(m_areaOfInfluence, 2.0f, m_areaOfInfluence);
		res.getMesh("pointy.3d")->Draw();
	glPopMatrix();

	glPopAttrib();
}

void CPointer::Update()
{
}


void CPointer::EditAoi(int delta)
{

	m_areaOfInfluence += delta;

	if(m_areaOfInfluence < 1)
		m_areaOfInfluence = 1;
}