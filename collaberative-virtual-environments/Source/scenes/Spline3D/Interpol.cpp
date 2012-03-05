#include "Interpol.h"

CInterpol::CInterpol()
{
}

CInterpol::~CInterpol()
{
	ctrlpoints.clear();
}

	
CInterpol::CInterpol(const std::vector<vec2>& controlpoints)
{
	int nbcontrolpoints = (int)controlpoints.size();
	ctrlpoints.resize(nbcontrolpoints);

	// The control points are sorted in ascending order
	int j;

	float xtmp;

	int nbpoints=1;
	ctrlpoints[0]=controlpoints[0];

	for(int i=1;i<nbcontrolpoints;i++)
	{
		j=0;
		xtmp=controlpoints[i].x;
		
		while(j<nbpoints && xtmp>ctrlpoints[j].x)
			j++;

		for(int k=nbpoints;k>j;k--)
		{
			ctrlpoints[k]=ctrlpoints[k-1];
		}

		ctrlpoints[j]=controlpoints[i];

		nbpoints++;
	}

}
