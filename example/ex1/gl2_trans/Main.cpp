// Implementation of the CMain class.
//
////////////////////////////////////////////////////////////////////////////////


#include "_StdAfx.h"


CMain::CMain()
{
}


int	CMain::Init()
{
	return LC_OK;
}

int	CMain::Destroy()
{
	return LC_OK;
}

int	CMain::FrameMove()
{
#if defined(_WIN32)
	LcSys_Sleep(10);
#endif


	static int c=0;
	++c;

	//	if(c>20*60*20)
	if(c>8*60*2)
	{
		printf("Game Process Terminated\n");
		return LC_EFAIL;
	}


	static int n = 0;
	++n;

	if(n>5)
	{
		printf("FPS: %3.f\n", m_fFPS);
		n = 0;
	}

	return LC_OK;
}



int	CMain::Render()
{
	// Rendering vertex data
	static float pCube[] =
	{
		-10,-10,-10,  +10,-10,-10, +10,+10,-10, -10,+10,-10,
		-10,-10,+10,  +10,-10,+10, +10,+10,+10, -10,+10,+10
	};



	// Setting Rendering pipeline
	glShadeModel(GL_SMOOTH);
	glEnable( GL_DEPTH_TEST);
	glEnable( GL_CULL_FACE );

	glClearColor(0.9f, 1.0f, 1.0f, 1.0f);


	// Clear Color buffer and Depth Buffer.
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);




	// set up projection matrix in 3d pipeline
	float	temp[16]={0};
	float	width, height;
	glGetFloatv(GL_VIEWPORT, temp);		// get viewport to set the screen width and height.
	width = temp[2];
	height= temp[3];


	float	Asp = (float)width/height;
	float	Near = 1.F;
	float	Far	 = 1000.F;
	float	Fov  = 45.F;

	float	xmin, xmax, ymin, ymax;

	ymax = Near * (float)tan(Fov* 3.1415962 / 360.0);
	ymin = -ymax;

	xmin = ymin * Asp;
	xmax = ymax * Asp;

	glMatrixMode(GL_PROJECTION);					// specify which matrix is the current matrix.
	glLoadIdentity();								// replace the current matrix by the identity matrix
	glFrustumf(xmin, xmax, ymin, ymax, Near, Far);	// multiply the current matrix by the perspective matrix.



	glMatrixMode(GL_MODELVIEW);						// specify which matrix is the current matrix to Model-View Matrix.
	glLoadIdentity();

	//push(copy) the current matrix stack
	glPushMatrix();
	{

		static int c =0;
		++c;

		glTranslatef(0.F, 0.F, -50.F);				// multiply the current matrix by a translation matrix
		glRotatef((float)(c), 0.F,0.F,1.F);			// multiply the current matrix by a rotation matrix
		glRotatef((float)(c), 1.F,0.F,0.F);
		glRotatef((float)(c), 0.F,1.F,0.F);


		//      7 +----------+ 6
		//       /|         /|
		//      / |        / |
		//     /  |       /  |
		//  3 +----------+ 2 |
		//    |   |      |   |
		//    | 4 +------|---+ 5
		//    |  /       |  /
		//    | /        | /
		//    |/         |/
		//  0 +----------+ 1

		static WORD front[]  = {2,1,3,0};
		static WORD back[]   = {5,6,4,7}; //back face
		static WORD top[]    = {6,2,7,3}; //top face
		static WORD bottom[] = {1,5,0,4}; //bottom face
		static WORD left[]   = {3,0,7,4}; //left face
		static WORD right[]  = {6,5,2,1}; //right face



		//Enable the vertices array
		glEnableClientState(GL_VERTEX_ARRAY);

		glVertexPointer(3, GL_FLOAT, 0, pCube);

		glColor4f(1,0,0,1);	glDrawElements(GL_TRIANGLE_STRIP, 4, GL_UNSIGNED_SHORT, front);
		glColor4f(0,1,0,1);	glDrawElements(GL_TRIANGLE_STRIP, 4, GL_UNSIGNED_SHORT, back);
		glColor4f(0,0,1,1);	glDrawElements(GL_TRIANGLE_STRIP, 4, GL_UNSIGNED_SHORT, top);
		glColor4f(1,1,0,1);	glDrawElements(GL_TRIANGLE_STRIP, 4, GL_UNSIGNED_SHORT, bottom);
		glColor4f(0,1,1,1);	glDrawElements(GL_TRIANGLE_STRIP, 4, GL_UNSIGNED_SHORT, left);
		glColor4f(1,0,1,1);	glDrawElements(GL_TRIANGLE_STRIP, 4, GL_UNSIGNED_SHORT, right);

		glDisableClientState(GL_VERTEX_ARRAY);

		glColor4f(1,1,1,1);
	}

	glPopMatrix();

	return LC_OK;
}



