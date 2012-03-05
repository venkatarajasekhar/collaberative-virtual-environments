#include "SceneSimple.h"
#include "../utilities/ResourceManager.h"
#include "../utilities/VarManager.h"
#include "../graphics/SingleCamera.h"
#include "Spline3D/SplineGL.h"

void SceneSimple::Init()
{
	SINGLETON_GET(ResourceManager, res);

	m_pMyTex = (CTexture2D*)res.LoadResource(ResourceManager::TEXTURE2D, "rocks_diffuse.jpg");

	m_fAngle = 0.0f;

	// Set up cam traj
	m_pCamEyeTraj->AddPoint(vec3(0.0f, 0.0f, 0.0f));
	m_pCamEyeTraj->AddPoint(vec3(5.0f, 5.0f, 0.0f));
	m_pCamEyeTraj->AddPoint(vec3(5.0f, 0.0f, 5.0f));
	m_pCamEyeTraj->BuildSplines(true);

	m_pCamLookAtTraj->AddPoint(vec3(0.0f, 0.0f, 0.0f));
	m_pCamLookAtTraj->AddPoint(vec3(-5.0f, -5.0f, 0.0f));
	m_pCamLookAtTraj->AddPoint(vec3(-5.0f, 0.0f, -5.0f));
	m_pCamLookAtTraj->BuildSplines(true);
}

void SceneSimple::Destroy()
{
	ISceneBase::Destroy();
	// Nothing to destroy here, res manager automatically handles resources
}

// Helper update
void SceneSimple::Idle(float fElapsedTime)
{
	// We take into account time elapsed since the previous frame
	// and change data over time
	m_fAngle += 10.0f*fElapsedTime;
}

void SceneSimple::Reset()
{
	// Reset the camera position
	Camera& cam = Camera::GetSingleton();
	cam.setEye(vec3(10.0f, 0.0f, 10.0f));
	cam.setAngle(RADIANS(225.f), RADIANS(90.f));
}

void SceneSimple::Render()
{
	SINGLETON_GET(ResourceManager, res);
	SINGLETON_GET(VarManager, var);

	DrawAxes();

	glColor3f(1.0f, 0.0f, 0.0f);
	//DrawTraj();

	glPushMatrix();
	glRotatef(m_fAngle, 0.0f, 1.0f, 0.0f);


	vec4 white(1.0f, 1.0f, 1.0f, 1.0f);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	vec4 pos(0.0, 0.0, 1.0, 0.0);
	glLightfv(GL_LIGHT0, GL_POSITION, pos);
	glLightfv(GL_LIGHT0, GL_AMBIENT, white);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, white);
	glLightfv(GL_LIGHT0, GL_SPECULAR, white);

	/* FYI. Here we will activate our texture
	 * We could access our texture via:
		 Texture2D* pTex = res.getTexture2D("rocks_diffuse.jpg");
	 * and this would avoid having a member class variable to access the resource.
	 */

	// Bind the texture slot 0
	m_pMyTex->Bind(0);

	// Draw an object
	glColor3f(1.0f, 0.0f, 0.0f);
	mySolidTeapot( 5 );

	// Unbind the texture
	m_pMyTex->Unbind(0);

	glPopMatrix();
}

//// ----------------------------- TEAPOT! -------IGNORE!---GLUT!-------{
void SceneSimple::myTeapot( GLint grid, GLdouble scale, GLenum type )
{
#if TARGET_HOST_WINCE
		int i, numV=sizeof(strip_vertices)/4, numI=sizeof(strip_normals)/4;
#else
    double p[4][4][3], q[4][4][3], r[4][4][3], s[4][4][3];
    long i, j, k, l;
#endif

	glPushAttrib( GL_ENABLE_BIT | GL_EVAL_BIT );
    glEnable( GL_AUTO_NORMAL );
    glEnable( GL_NORMALIZE );
    glEnable( GL_MAP2_VERTEX_3 );
    glEnable( GL_MAP2_TEXTURE_COORD_2 );

    glPushMatrix();
    glRotated( 270.0, 1.0, 0.0, 0.0 );
    glScaled( 0.5 * scale, 0.5 * scale, 0.5 * scale );
    glTranslated( 0.0, 0.0, -1.5 );

int patchData[][16] =
{
	{ 102, 103, 104, 105,   4,   5,   6,   7,   8,   9,  10,  11,  12,  13,  14,  15 }, /* rim	*/
	{  12,  13,  14,  15,  16,  17,  18,  19,  20,  21,  22,  23,  24,  25,  26,  27 }, /* body   */
	{  24,  25,  26,  27,  29,  30,  31,  32,  33,  34,  35,  36,  37,  38,  39,  40 },
	{  96,  96,  96,  96,  97,  98,  99, 100, 101, 101, 101, 101,   0,   1,   2,   3 }, /* lid	*/
	{   0,   1,   2,   3, 106, 107, 108, 109, 110, 111, 112, 113, 114, 115, 116, 117 },
	{ 118, 118, 118, 118, 124, 122, 119, 121, 123, 126, 125, 120,  40,  39,  38,  37 }, /* bottom */
	{  41,  42,  43,  44,  45,  46,  47,  48,  49,  50,  51,  52,  53,  54,  55,  56 }, /* handle */
	{  53,  54,  55,  56,  57,  58,  59,  60,  61,  62,  63,  64,  28,  65,  66,  67 },
	{  68,  69,  70,  71,  72,  73,  74,  75,  76,  77,  78,  79,  80,  81,  82,  83 }, /* spout  */
	{  80,  81,  82,  83,  84,  85,  86,  87,  88,  89,  90,  91,  92,  93,  94,  95 }
};
	
double cpData[][3] =
{
	{0.2, 0, 2.7}, {0.2, -0.112, 2.7}, {0.112, -0.2, 2.7}, {0,
	-0.2, 2.7}, {1.3375, 0, 2.53125}, {1.3375, -0.749, 2.53125},
	{0.749, -1.3375, 2.53125}, {0, -1.3375, 2.53125}, {1.4375,
	0, 2.53125}, {1.4375, -0.805, 2.53125}, {0.805, -1.4375,
	2.53125}, {0, -1.4375, 2.53125}, {1.5, 0, 2.4}, {1.5, -0.84,
	2.4}, {0.84, -1.5, 2.4}, {0, -1.5, 2.4}, {1.75, 0, 1.875},
	{1.75, -0.98, 1.875}, {0.98, -1.75, 1.875}, {0, -1.75,
	1.875}, {2, 0, 1.35}, {2, -1.12, 1.35}, {1.12, -2, 1.35},
	{0, -2, 1.35}, {2, 0, 0.9}, {2, -1.12, 0.9}, {1.12, -2,
	0.9}, {0, -2, 0.9}, {-2, 0, 0.9}, {2, 0, 0.45}, {2, -1.12,
	0.45}, {1.12, -2, 0.45}, {0, -2, 0.45}, {1.5, 0, 0.225},
	{1.5, -0.84, 0.225}, {0.84, -1.5, 0.225}, {0, -1.5, 0.225},
	{1.5, 0, 0.15}, {1.5, -0.84, 0.15}, {0.84, -1.5, 0.15}, {0,
	-1.5, 0.15}, {-1.6, 0, 2.025}, {-1.6, -0.3, 2.025}, {-1.5,
	-0.3, 2.25}, {-1.5, 0, 2.25}, {-2.3, 0, 2.025}, {-2.3, -0.3,
	2.025}, {-2.5, -0.3, 2.25}, {-2.5, 0, 2.25}, {-2.7, 0,
	2.025}, {-2.7, -0.3, 2.025}, {-3, -0.3, 2.25}, {-3, 0,
	2.25}, {-2.7, 0, 1.8}, {-2.7, -0.3, 1.8}, {-3, -0.3, 1.8},
	{-3, 0, 1.8}, {-2.7, 0, 1.575}, {-2.7, -0.3, 1.575}, {-3,
	-0.3, 1.35}, {-3, 0, 1.35}, {-2.5, 0, 1.125}, {-2.5, -0.3,
	1.125}, {-2.65, -0.3, 0.9375}, {-2.65, 0, 0.9375}, {-2,
	-0.3, 0.9}, {-1.9, -0.3, 0.6}, {-1.9, 0, 0.6}, {1.7, 0,
	1.425}, {1.7, -0.66, 1.425}, {1.7, -0.66, 0.6}, {1.7, 0,
	0.6}, {2.6, 0, 1.425}, {2.6, -0.66, 1.425}, {3.1, -0.66,
	0.825}, {3.1, 0, 0.825}, {2.3, 0, 2.1}, {2.3, -0.25, 2.1},
	{2.4, -0.25, 2.025}, {2.4, 0, 2.025}, {2.7, 0, 2.4}, {2.7,
	-0.25, 2.4}, {3.3, -0.25, 2.4}, {3.3, 0, 2.4}, {2.8, 0,
	2.475}, {2.8, -0.25, 2.475}, {3.525, -0.25, 2.49375},
	{3.525, 0, 2.49375}, {2.9, 0, 2.475}, {2.9, -0.15, 2.475},
	{3.45, -0.15, 2.5125}, {3.45, 0, 2.5125}, {2.8, 0, 2.4},
	{2.8, -0.15, 2.4}, {3.2, -0.15, 2.4}, {3.2, 0, 2.4}, {0, 0,
	3.15}, {0.8, 0, 3.15}, {0.8, -0.45, 3.15}, {0.45, -0.8,
	3.15}, {0, -0.8, 3.15}, {0, 0, 2.85}, {1.4, 0, 2.4}, {1.4,
	-0.784, 2.4}, {0.784, -1.4, 2.4}, {0, -1.4, 2.4}, {0.4, 0,
	2.55}, {0.4, -0.224, 2.55}, {0.224, -0.4, 2.55}, {0, -0.4,
	2.55}, {1.3, 0, 2.55}, {1.3, -0.728, 2.55}, {0.728, -1.3,
	2.55}, {0, -1.3, 2.55}, {1.3, 0, 2.4}, {1.3, -0.728, 2.4},
	{0.728, -1.3, 2.4}, {0, -1.3, 2.4}, {0, 0, 0}, {1.425,
	-0.798, 0}, {1.5, 0, 0.075}, {1.425, 0, 0}, {0.798, -1.425,
	0}, {0, -1.5, 0.075}, {0, -1.425, 0}, {1.5, -0.84, 0.075},
	{0.84, -1.5, 0.075}
};

static double tex[2][2][2] =
{
	{ {0.0, 0.0}, {1.0, 0.0} },
	{ {0.0, 1.0}, {1.0, 1.0} }
};


#if TARGET_HOST_WINCE
    glRotated( 90.0, 1.0, 0.0, 0.0 );
    glBegin( GL_TRIANGLE_STRIP );

    for( i = 0; i < numV-1; i++ )
    {
        int vidx = strip_vertices[i],
            nidx = strip_normals[i];

        if( vidx != -1 )
        {
            glNormal3fv( normals[nidx]  );
            glVertex3fv( vertices[vidx] );
        }
        else
        {
            glEnd();
            glBegin( GL_TRIANGLE_STRIP );
        }
    }

    glEnd();
#else
    for (i = 0; i < 10; i++) {
      for (j = 0; j < 4; j++) {
        for (k = 0; k < 4; k++) {
          for (l = 0; l < 3; l++) {
            p[j][k][l] = cpData[patchData[i][j * 4 + k]][l];
            q[j][k][l] = cpData[patchData[i][j * 4 + (3 - k)]][l];
            if (l == 1)
              q[j][k][l] *= -1.0;
            if (i < 6) {
              r[j][k][l] =
                cpData[patchData[i][j * 4 + (3 - k)]][l];
              if (l == 0)
                r[j][k][l] *= -1.0;
              s[j][k][l] = cpData[patchData[i][j * 4 + k]][l];
              if (l == 0)
                s[j][k][l] *= -1.0;
              if (l == 1)
                s[j][k][l] *= -1.0;
            }
          }
        }
      }

      glMap2d(GL_MAP2_TEXTURE_COORD_2, 0.0, 1.0, 2, 2, 0.0, 1.0, 4, 2,
        &tex[0][0][0]);
      glMap2d(GL_MAP2_VERTEX_3, 0.0, 1.0, 3, 4, 0.0, 1.0, 12, 4,
        &p[0][0][0]);
      glMapGrid2d(grid, 0.0, 1.0, grid, 0.0, 1.0);
      glEvalMesh2(type, 0, grid, 0, grid);
      glMap2d(GL_MAP2_VERTEX_3, 0.0, 1.0, 3, 4, 0.0, 1.0, 12, 4,
        &q[0][0][0]);
      glEvalMesh2(type, 0, grid, 0, grid);
      if (i < 6) {
        glMap2d(GL_MAP2_VERTEX_3, 0.0, 1.0, 3, 4, 0.0, 1.0, 12, 4,
          &r[0][0][0]);
        glEvalMesh2(type, 0, grid, 0, grid);
        glMap2d(GL_MAP2_VERTEX_3, 0.0, 1.0, 3, 4, 0.0, 1.0, 12, 4,
          &s[0][0][0]);
        glEvalMesh2(type, 0, grid, 0, grid);
      }
    }
#endif  /* TARGET_HOST_WINCE */

    glPopMatrix();
    glPopAttrib();
}


/*
 * Renders a beautiful wired teapot...
 */
void SceneSimple::myWireTeapot( GLdouble size )
{
    myTeapot( 10, size, GL_LINE );
}

/*
 * Renders a beautiful filled teapot...
 */
void SceneSimple::mySolidTeapot( GLdouble size )
{
    myTeapot( 7, size, GL_FILL );
}