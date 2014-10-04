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


	float fps = (float)this->GetFPS();

	if(n>5)
	{
		printf("FPS: %3.f\n", fps);
		n = 0;
	}


	return LC_OK;
}



int	CMain::Render()
{
	// specify clear values(red, green, blue, alpha) for the color buffer
	glClearColor(0.9f, 1.0f, 1.0f, 1.0f);

	// Clear Color buffer and Depth Buffer.
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);





	// Setting Rendering pipeline
	glShadeModel(GL_SMOOTH);		// select smooth shading model.
	glDisable( GL_DEPTH_TEST);		// disable depth test
	glDisable( GL_CULL_FACE );		// disalbe culling face




	// Vertex Stream data
	// position buffer
	GLfloat vertexArray[12] = {-0.9F,-0.9F,0.F,   0.9F,-0.9F,0.F,   0.9F,0.9F,0.F ,   -0.9F,0.9F,0.F };

	// color buffer
	GLfloat colorArray[16] = { 1.F,0.F,0.F,1.F,  0.F,1.F,0.F,1.F,   0.F,0.F,1.F,0.F,   1.F,0.F,1.F,0.F};

	// index buffer
	GLushort indices[4] = {0, 1, 2, 3};


	// Position
	//Enable the vertex stream for position
	glEnableClientState(GL_VERTEX_ARRAY);

	// binding the position array to the vertex stream in pipeline
	//3 = XYZ coordinates, GL_FLOAT = data type of positon array, 0 = 0 stride bytes
	glVertexPointer(3, GL_FLOAT, 0, vertexArray);


	// Color
	//Enable the vertex stream for diffuse color
	glEnableClientState(GL_COLOR_ARRAY);

	// binding the color array to the vertex stream in pipeline
 	// 4 = RGBA format, GL_FLOAT = data type of color array, 0 = 0 stide  bytes
	glColorPointer(4, GL_FLOAT, 0, colorArray);



	// Draw with index
	// GL_TRIANGLES = primitive type, 3 = index number, GL_UNSIGNED_BYTE= index data type
	glDrawElements(GL_TRIANGLE_FAN, 4, GL_UNSIGNED_SHORT, indices);

	// you can use glDrawArrays() function without index data
	// GL_TRIANGLES = primitive type, 0 = start index 3 = number of items (vertices) to draw
	//glDrawArrays(GL_TRIANGLES, 0,3);


	// disable client-side capability
	glDisableClientState(GL_COLOR_ARRAY);
	glDisableClientState(GL_VERTEX_ARRAY);


	return LC_OK;
}



