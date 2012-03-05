#ifndef INTERPOL_H
#define INTERPOL_H

#include <vector>

#include "../../utilities/math.h"

class CInterpol 
{
protected:
	std::vector<vec2> ctrlpoints;	// Vector of control points

public:
	CInterpol();
	CInterpol(const std::vector<vec2>& controlpoints);
	virtual ~CInterpol();

	virtual vec2 getpoint(float t)=0;

};
#endif 
