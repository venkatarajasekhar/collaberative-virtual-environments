#include <GL/glew.h>

#include "Frustum.h"

/*
vec3 Frustum::Intersect2D(int id1, int id2) const {
	vec3 p;
//	p.y = (-frustum[id2][3]/frustum[id2][0] + frustum[id1][3]/frustum[id1][0]) / (-frustum[id2][2]/frustum[id2][0] + frustum[id1][2]/frustum[id1][0]);

//	p.x = (-frustum[id1][2] * p.y - frustum[id1][3])/frustum[id1][0];
	return p;
}
*/
bool Frustum::ContainsPoint(const vec3& point) const
{
   for(int p = 0; p < 6; p++)	// For all planes
	  // If the point is outside of any of the planes
      if(	m_tFrustumPlanes[p][0] * point.x +
			m_tFrustumPlanes[p][1] * point.y +
			m_tFrustumPlanes[p][2] * point.z +
			m_tFrustumPlanes[p][3] <= 0)
         return false;

   return true;
}

int Frustum::ContainsSphere(const vec3& center, float radius) const
{
	// For all planes
	for(int p = 0; p < 6; p++)
	{
		// Distance between center circle and plane
		float t =	m_tFrustumPlanes[p][0] * center.x +
					m_tFrustumPlanes[p][1] * center.y +
					m_tFrustumPlanes[p][2] * center.z +
					m_tFrustumPlanes[p][3];

		if( t < -radius)
			return FRUSTUM_OUT;

		if((float)fabs(t) < radius)
			return FRUSTUM_INTERSECT;
	}
	return FRUSTUM_IN;
}

int Frustum::ContainsBoundingBox(const BoundingBox& bbox) const
{
	vec3 tCorners[8] = {	vec3(bbox.min.x, bbox.min.y, bbox.min.z),
							vec3(bbox.max.x, bbox.min.y, bbox.min.z),
							vec3(bbox.min.x, bbox.max.y, bbox.min.z),
							vec3(bbox.min.x, bbox.min.y, bbox.max.z),
							vec3(bbox.max.x, bbox.max.y, bbox.min.z),
							vec3(bbox.min.x, bbox.max.y, bbox.max.z),
							vec3(bbox.max.x, bbox.min.y, bbox.max.z),
							vec3(bbox.max.x, bbox.max.y, bbox.max.z)	};
	int iTotalIn = 0;

	for(int p=0; p<6; p++)
	{
		int iInCount = 8;
		int iPtIn = 1;

		for(int c=0; c<8; c++)
		{
			// Test all points
			float side =	m_tFrustumPlanes[p][0] * tCorners[c].x +
							m_tFrustumPlanes[p][1] * tCorners[c].y +
							m_tFrustumPlanes[p][2] * tCorners[c].z +
							m_tFrustumPlanes[p][3];
			if(side < 0) {
				iPtIn = 0;
				iInCount--;
			}
		}

		// All the points outside
		if(iInCount == 0)
			return FRUSTUM_OUT;

		iTotalIn += iPtIn;
	}

	// If all points are in
	if(iTotalIn == 6)
		return FRUSTUM_IN;

	return FRUSTUM_INTERSECT;

}

void Frustum::Extract(const vec3& eye)
{
	m_EyePos = eye;


	glGetFloatv( GL_MODELVIEW_MATRIX, m_mtxMV.mat );	// Get MV matrix
	m_mtxMV.inverse(m_mtxMVinv);						// Inverse MV matrix
	glGetFloatv( GL_PROJECTION_MATRIX, m_mtxProj.mat );	// Get projection matrix


	float t;

	// Combine both my multiplying them
	m_mtxMVProj = m_mtxProj * m_mtxMV;


	/* Extract RIGHT plane */
	m_tFrustumPlanes[0][0] = m_mtxMVProj[ 3] - m_mtxMVProj[ 0];
	m_tFrustumPlanes[0][1] = m_mtxMVProj[ 7] - m_mtxMVProj[ 4];
	m_tFrustumPlanes[0][2] = m_mtxMVProj[11] - m_mtxMVProj[ 8];
	m_tFrustumPlanes[0][3] = m_mtxMVProj[15] - m_mtxMVProj[12];

	/* Calculate normals */
	t = sqrt( m_tFrustumPlanes[0][0] * m_tFrustumPlanes[0][0] + m_tFrustumPlanes[0][1] * m_tFrustumPlanes[0][1] + m_tFrustumPlanes[0][2] * m_tFrustumPlanes[0][2] );
	m_tFrustumPlanes[0][0] /= t;
	m_tFrustumPlanes[0][1] /= t;
	m_tFrustumPlanes[0][2] /= t;
	m_tFrustumPlanes[0][3] /= t;

	/* Extract LEFT plane */
	m_tFrustumPlanes[1][0] = m_mtxMVProj[ 3] + m_mtxMVProj[ 0];
	m_tFrustumPlanes[1][1] = m_mtxMVProj[ 7] + m_mtxMVProj[ 4];
	m_tFrustumPlanes[1][2] = m_mtxMVProj[11] + m_mtxMVProj[ 8];
	m_tFrustumPlanes[1][3] = m_mtxMVProj[15] + m_mtxMVProj[12];

	/* Calculate normals */
	t = sqrt( m_tFrustumPlanes[1][0] * m_tFrustumPlanes[1][0] + m_tFrustumPlanes[1][1] * m_tFrustumPlanes[1][1] + m_tFrustumPlanes[1][2] * m_tFrustumPlanes[1][2] );
	m_tFrustumPlanes[1][0] /= t;
	m_tFrustumPlanes[1][1] /= t;
	m_tFrustumPlanes[1][2] /= t;
	m_tFrustumPlanes[1][3] /= t;

	/* Extract bottom plane */
	m_tFrustumPlanes[2][0] = m_mtxMVProj[ 3] + m_mtxMVProj[ 1];
	m_tFrustumPlanes[2][1] = m_mtxMVProj[ 7] + m_mtxMVProj[ 5];
	m_tFrustumPlanes[2][2] = m_mtxMVProj[11] + m_mtxMVProj[ 9];
	m_tFrustumPlanes[2][3] = m_mtxMVProj[15] + m_mtxMVProj[13];

	/* Calculate normals */
	t = sqrt( m_tFrustumPlanes[2][0] * m_tFrustumPlanes[2][0] + m_tFrustumPlanes[2][1] * m_tFrustumPlanes[2][1] + m_tFrustumPlanes[2][2] * m_tFrustumPlanes[2][2] );
	m_tFrustumPlanes[2][0] /= t;
	m_tFrustumPlanes[2][1] /= t;
	m_tFrustumPlanes[2][2] /= t;
	m_tFrustumPlanes[2][3] /= t;

	/* Extract TOP plane */
	m_tFrustumPlanes[3][0] = m_mtxMVProj[ 3] - m_mtxMVProj[ 1];
	m_tFrustumPlanes[3][1] = m_mtxMVProj[ 7] - m_mtxMVProj[ 5];
	m_tFrustumPlanes[3][2] = m_mtxMVProj[11] - m_mtxMVProj[ 9];
	m_tFrustumPlanes[3][3] = m_mtxMVProj[15] - m_mtxMVProj[13];

	/* Calculate normals */
	t = sqrt( m_tFrustumPlanes[3][0] * m_tFrustumPlanes[3][0] + m_tFrustumPlanes[3][1] * m_tFrustumPlanes[3][1] + m_tFrustumPlanes[3][2] * m_tFrustumPlanes[3][2] );
	m_tFrustumPlanes[3][0] /= t;
	m_tFrustumPlanes[3][1] /= t;
	m_tFrustumPlanes[3][2] /= t;
	m_tFrustumPlanes[3][3] /= t;

	/* Extract DISTANT plane */
	m_tFrustumPlanes[4][0] = m_mtxMVProj[ 3] - m_mtxMVProj[ 2];
	m_tFrustumPlanes[4][1] = m_mtxMVProj[ 7] - m_mtxMVProj[ 6];
	m_tFrustumPlanes[4][2] = m_mtxMVProj[11] - m_mtxMVProj[10];
	m_tFrustumPlanes[4][3] = m_mtxMVProj[15] - m_mtxMVProj[14];

	/* Calculate normals */
	t = sqrt( m_tFrustumPlanes[4][0] * m_tFrustumPlanes[4][0] + m_tFrustumPlanes[4][1] * m_tFrustumPlanes[4][1] + m_tFrustumPlanes[4][2] * m_tFrustumPlanes[4][2] );
	m_tFrustumPlanes[4][0] /= t;
	m_tFrustumPlanes[4][1] /= t;
	m_tFrustumPlanes[4][2] /= t;
	m_tFrustumPlanes[4][3] /= t;

	/* Extract the NEAR plane */
	m_tFrustumPlanes[5][0] = m_mtxMVProj[ 3] + m_mtxMVProj[ 2];
	m_tFrustumPlanes[5][1] = m_mtxMVProj[ 7] + m_mtxMVProj[ 6];
	m_tFrustumPlanes[5][2] = m_mtxMVProj[11] + m_mtxMVProj[10];
	m_tFrustumPlanes[5][3] = m_mtxMVProj[15] + m_mtxMVProj[14];

	/* Calculate normals */
	t = sqrt( m_tFrustumPlanes[5][0] * m_tFrustumPlanes[5][0] + m_tFrustumPlanes[5][1] * m_tFrustumPlanes[5][1] + m_tFrustumPlanes[5][2] * m_tFrustumPlanes[5][2] );
	m_tFrustumPlanes[5][0] /= t;
	m_tFrustumPlanes[5][1] /= t;
	m_tFrustumPlanes[5][2] /= t;
	m_tFrustumPlanes[5][3] /= t;
	
}


