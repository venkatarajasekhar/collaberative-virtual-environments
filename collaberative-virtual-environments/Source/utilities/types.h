/**
 * This header contains miscellaneous helper type defs, structs and macros.
 */

#ifndef TYPES_H
#define TYPES_H

#include <GL\glew.h>

#include <ctime>
#include <sstream>
#include <iomanip>

#include "math.h"

struct Point
{
	int x, y;
};

struct Pointf
{
	float x, y;
};

namespace OMB
{
	struct Rectangle
	{
		int x, y;
		unsigned int width, height;
	};
}

struct Colour
{
	unsigned int r, g, b;
};

// If Win32 then we can use this to get the current directory with appendment
#ifdef WIN32
#define WIN32_LEAN_AND_MEAN
#include <WinSock2.h> // Has to go here to prevent windows.h conflicts.
#include <Windows.h>
#define GET_PATH(appendment, string_name) \
	char szPath[MAX_PATH+1]; \
	GetCurrentDirectory(MAX_PATH, szPath); \
	strcat_s(szPath, appendment); \
	std::string string_name = szPath;
#endif

// Custom Asser function which gives more detail when assert occurs
#if defined( _DEBUG )
extern bool CustomAssertFunction( bool, char*, int, char*, bool* );

#define Assert( exp, description ) \
   {  static bool ignoreAlways = false; \
      if( !ignoreAlways ) { \
         if( CustomAssertFunction( (int)(exp), description, \
                                   __LINE__, __FILE__, &ignoreAlways ) )  \
         { _asm { int 3 } } \
      } \
   }

#else
	#define Assert( exp, description )
#endif

#ifndef max
	#define max(a, b)            (((a) > (b)) ? (a) : (b))
#endif

#ifndef min
	#define min(a, b)            (((a) < (b)) ? (a) : (b))
#endif

//inline float clamp(float val, float minVal, float maxVal)
//{
//	return min(max(val, minVal), maxVal);
//}

struct frect
{
	float x1, y1, x2, y2;
	frect(){x1=y1=x2=y1=0;}
	frect(float ux1, float uy1, float ux2, float uy2){x1=ux1; y1=uy1; x2=ux2; y2=uy2; Normalize();}
	void Normalize(){if(x1>x2)std::swap(x1, x2); if(y1>y2)std::swap(y1, y2);}
	frect &operator =(const frect &rc){x1=rc.x1; y1=rc.y1; x2=rc.x2; y2=rc.y2; return *this;}
};

//float inline pow(float base, int power)
//{
//	if(power==1)return base;
//	if(power==0)return 1;
//	return base*pow(base, power-1);
//}

float inline frand(int precis=4)
{
	int val=(int)pow(10.0, precis);
	return (rand()%val)/(float)val;
}

void inline InitRandomNumbers()
{
	srand((unsigned int)time(0));
}

template<class T>
T qAbs(T v)
{
	return (v>0)?v:-v;
}

class rgba 
{ 
public:
	float r, g, b, a; 
	rgba(){r=g=b=a=0.0f;}
	rgba(float red, float green, float blue, float alpha){r=red; g=green; b=blue; a=alpha;}
	rgba &operator =(rgba &c){r=c.r; g=c.g; b=c.b; a=c.a; return *this;}
};

inline std::string toString(float x, int precision = 1)
{  
	std::ostringstream o;  
	o << std::fixed << std::setprecision(precision) << x;  
	return o.str();  
}

//// Used to create a string to be displayed on screen
//inline void printString(void *font, const char* str){	
//	while (*str != '\0')	
//		glutBitmapCharacter(font, *str++);
//}


// Quality of text to display
enum TextQuality
{
	Solid, 
	Shaded, 
	Blended
};

inline void glEnable2D()
{
	int vPort[4];
  
	glGetIntegerv(GL_VIEWPORT, vPort);
  
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
  
	glOrtho(0, vPort[2], 0, vPort[3], -1, 1);
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();

	glDisable(GL_DEPTH_TEST);
}

inline void glDisable2D()
{
	glEnable(GL_DEPTH_TEST);

	glMatrixMode(GL_PROJECTION);
	glPopMatrix();   

	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();	
}


/*
 * Compute lookup table of cos and sin values forming a cirle
 *
 * Notes:
 *    It is the responsibility of the caller to free these tables
 *    The size of the table is (n+1) to form a connected loop
 *    The last entry is exactly the same as the first
 *    The sign of n can be flipped to get the reverse loop
 */

static void myCircleTable(double **sint,double **cost,const int n)
{
    int i;

    /* Table size, the sign of n flips the circle direction */

    const int size = abs(n);

    /* Determine the angle between samples */

    const double angle = 2*M_PI/(double)( ( n == 0 ) ? 1 : n );

    /* Allocate memory for n samples, plus duplicate of first entry at the end */

    *sint = (double *) calloc(sizeof(double), size+1);
    *cost = (double *) calloc(sizeof(double), size+1);

    /* Bail out if memory allocation fails, fgError never returns */

    if (!(*sint) || !(*cost))
    {
        free(*sint);
        free(*cost);
    }

    /* Compute cos and sin around the circle */

    (*sint)[0] = 0.0;
    (*cost)[0] = 1.0;

    for (i=1; i<size; i++)
    {
        (*sint)[i] = sin(angle*i);
        (*cost)[i] = cos(angle*i);
    }

    /* Last sample is duplicate of the first */

    (*sint)[size] = (*sint)[0];
    (*cost)[size] = (*cost)[0];
}


/*
 * Draws a solid sphere
 */
inline void mySolidSphere(GLdouble radius, GLint slices, GLint stacks)
{
    int i,j;

    /* Adjust z and radius as stacks are drawn. */

    double z0,z1;
    double r0,r1;

    /* Pre-computed circle */

    double *sint1,*cost1;
    double *sint2,*cost2;

    myCircleTable(&sint1,&cost1,-slices);
    myCircleTable(&sint2,&cost2,stacks*2);

    /* The top stack is covered with a triangle fan */

    z0 = 1.0;
    z1 = cost2[(stacks>0)?1:0];
    r0 = 0.0;
    r1 = sint2[(stacks>0)?1:0];

    glBegin(GL_TRIANGLE_FAN);

        glNormal3d(0,0,1);
        glVertex3d(0,0,radius);

        for (j=slices; j>=0; j--)
        {
            glNormal3d(cost1[j]*r1,        sint1[j]*r1,        z1       );
            glVertex3d(cost1[j]*r1*radius, sint1[j]*r1*radius, z1*radius);
        }

    glEnd();

    /* Cover each stack with a quad strip, except the top and bottom stacks */

    for( i=1; i<stacks-1; i++ )
    {
        z0 = z1; z1 = cost2[i+1];
        r0 = r1; r1 = sint2[i+1];

        glBegin(GL_QUAD_STRIP);

            for(j=0; j<=slices; j++)
            {
                glNormal3d(cost1[j]*r1,        sint1[j]*r1,        z1       );
                glVertex3d(cost1[j]*r1*radius, sint1[j]*r1*radius, z1*radius);
                glNormal3d(cost1[j]*r0,        sint1[j]*r0,        z0       );
                glVertex3d(cost1[j]*r0*radius, sint1[j]*r0*radius, z0*radius);
            }

        glEnd();
    }

    /* The bottom stack is covered with a triangle fan */

    z0 = z1;
    r0 = r1;

    glBegin(GL_TRIANGLE_FAN);

        glNormal3d(0,0,-1);
        glVertex3d(0,0,-radius);

        for (j=0; j<=slices; j++)
        {
            glNormal3d(cost1[j]*r0,        sint1[j]*r0,        z0       );
            glVertex3d(cost1[j]*r0*radius, sint1[j]*r0*radius, z0*radius);
        }

    glEnd();

    /* Release sin and cos tables */

    free(sint1);
    free(cost1);
    free(sint2);
    free(cost2);
}


#endif
