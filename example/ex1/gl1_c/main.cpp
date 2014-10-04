

#if defined(_MSC_VER)
  #pragma comment(linker, "/subsystem:console")

  #pragma comment(lib, "lgpwn_b22_.lib")
  #pragma comment(lib, "lgpwn_c22_.lib")
  #pragma comment(lib, "lgpwn_x22_.lib")
  #pragma comment(lib, "lgpwn_t22_.lib")

  #pragma comment(lib, "ftf243m_.lib")
  #pragma comment(lib, "ImgJpg_.lib")
  #pragma comment(lib, "png144_.lib")
  #pragma comment(lib, "zlib125_.lib")
  #pragma comment(lib, "al11s_.lib")

  #pragma comment(lib, "opengl32.lib")
  #pragma comment(lib, "winmm.lib")
  #pragma comment(lib, "dxguid.lib")
#endif


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <gles/gl.h>

#include <LcType.h>
#include <LcBase.h>
#include <LcMath.h>
#include <LcAux.h>


LC_HWND			m_hWnd	= NULL;		// Window Handle
PLC_DEVICE		m_pDev = NULL;		// Device pointer


INT LC_CALLBACK Run(LC_HANDLE);



INT InitDevice()
{
	// Init GLC Library
	LcSys_Init();

	printf("LcSys_Init\n");

	// Set Main procedure Function Pointer
	LcSys_SetMainRunFunc(Run);

	// Create Window
	m_hWnd = LcSys_CreateWindow(NULL, (char*)"LC_Tutorial", 640, 480);
	if(NULL == m_hWnd)
	{
		printf("LcSys_CreateWindow Failed\n");
		return LC_EFAIL;
	}

	// Create GLC Rendering Device
	if(LC_FAILED(LcDev_CreateDevice(NULL, &m_pDev, m_hWnd, NULL)))
	{
		printf("LcDev_CreateDevice Failed\n");
		return LC_EFAIL;
	}


	return LC_OK;
}




// rendering 3d Model
void Render()
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
	GLfloat vertexArray[9] = {  -1.F,-1.F,0.F,     1.F,-1.F,0.F,     0.F,1.F,0.F };

	// color buffer
	GLfloat colorArray[12] = { 1.F,0.F,0.F,1.F,  0.F,1.F,0.F,1.F,   0.F,0.F,1.F,0.F};

	// index buffer
	GLushort indices[3] = {0, 1, 2};


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
	glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_SHORT, indices);

	// you can use glDrawArrays() function without index data
	// GL_TRIANGLES = primitive type, 0 = start index 3 = number of items (vertices) to draw
	//glDrawArrays(GL_TRIANGLES, 0,3);


	// disable client-side capability
	glDisableClientState(GL_COLOR_ARRAY);
	glDisableClientState(GL_VERTEX_ARRAY);
}



// Main Program
int main(int argc, char* argv[])
{
	if(LC_FAILED(InitDevice()))
		goto End;

	// Run Program
	while(1)
	{
		if(LC_FAILED(LcSys_Run()))
			break;

		// Flipping
		m_pDev->Present(0,0,0,0);
	}


End:
	m_pDev->Release();		// Release Device

	// Release Window
	LcSys_DestroyWindow(&m_hWnd);

	// Release GLC
	LcSys_Close();

	return LC_OK;
}


INT LC_CALLBACK Run(LC_HANDLE)
{
#if defined(_WIN32)
	LcSys_Sleep(1);
#endif

	static int frames = 500;

	if(0 == --frames)
		return LC_EFAIL;


	Render();

	return LC_OK;
}


