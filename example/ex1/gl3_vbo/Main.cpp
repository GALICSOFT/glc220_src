// Implementation of the CMain class.
//
// VBO와 관련된 함수
//
//	- glBindBuffer
//	- glBufferData
//	- glBufferSubData
//	- glDeleteBuffers
//	- glGenBuffers
//
////////////////////////////////////////////////////////////////////////////////


#include "_StdAfx.h"


CMain::CMain()
{
}


int	CMain::Init()
{
	////////////////////////////////////////////////////////////////////////////

	// VBO(Vertex Buffer Object) 1개를 생성
	glGenBuffers(1, &m_Vtx);


	// 시스템 메모리에 정점 데이터 생성
	GLfloat vertices[] =
	{
		-10,-10,-10,  +10,-10,-10, +10,+10,-10, -10,+10,-10,
		-10,-10,+10,  +10,-10,+10, +10,+10,+10, -10,+10,+10
	};

	for(UINT i=0; i< sizeof(vertices)/sizeof(vertices[0]); ++i)
		vertices[i] = vertices[i];


	// VBO binding 시작
	glBindBuffer(GL_ARRAY_BUFFER,m_Vtx);

	// 시스템 메모리에 저장된 정점 데이터를 VBO에 binding
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);


	// 정점 데이터가 VBO에 설정되어 있어서 malloc, new로 생성된 메모리를
	// 여기서 release해도 된다.
	// 정점 버퍼 release...
	//

	// VBO binding release
	glBindBuffer(GL_ARRAY_BUFFER, 0);




	// IBO(Index Buffer Object) 생성 ← IBO도 VBO 임. 구분을 위해서 임으로 이름을 정함.

	// 6면에 대한 IBO 6 개를 생성
	glGenBuffers(6, &m_Idx[0]);


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


	// Index 데이터
	WORD front [] = {2,1,3,0};	// front face
	WORD back  [] = {5,6,4,7};	// back face
	WORD top   [] = {6,2,7,3};	// top face
	WORD bottom[] = {1,5,0,4};	// bottom face
	WORD left  [] = {3,0,7,4};	// left face
	WORD right [] = {6,5,2,1};	// right face

	// IBO와 인덱스 데이터 binding
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_Idx[0]);	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(front ), front , GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_Idx[1]);	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(back  ), back  , GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_Idx[2]);	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(top   ), top   , GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_Idx[3]);	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(bottom), bottom, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_Idx[4]);	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(left  ), left  , GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_Idx[5]);	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(right ), right , GL_STATIC_DRAW);

	// IBO binding release
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	return LC_OK;
}

int	CMain::Destroy()
{
	// VBO release
	if(m_Vtx)
	{
		glDeleteBuffers(1, &m_Vtx);
		m_Vtx =0;
	}

	// IBO release
	if(m_Idx[0])
	{
		glDeleteBuffers(6, &m_Idx[0]);
		memset(m_Idx, 0, 6 * sizeof(LC_IB) );
	}



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
	// Setting Rendering pipeline
	glShadeModel(GL_SMOOTH);
	glEnable( GL_DEPTH_TEST);
	glEnable( GL_CULL_FACE );

	glClearColor(0.9f, 1.0f, 1.0f, 1.0f);


	// Clear Color buffer and Depth Buffer.
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


	SetPerspective();


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



		//Bind vertex buffer
		glBindBuffer(GL_ARRAY_BUFFER, m_Vtx);
		glVertexPointer(3, GL_FLOAT, 0, 0);
		glEnableClientState(GL_VERTEX_ARRAY);

		// bind Index buffer
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_Idx[0]);	glColor4f(1,0,0,1);		glDrawElements(GL_TRIANGLE_STRIP, 4, GL_UNSIGNED_SHORT, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_Idx[1]);	glColor4f(0,1,0,1);		glDrawElements(GL_TRIANGLE_STRIP, 4, GL_UNSIGNED_SHORT, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_Idx[2]);	glColor4f(0,0,1,1);		glDrawElements(GL_TRIANGLE_STRIP, 4, GL_UNSIGNED_SHORT, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_Idx[3]);	glColor4f(1,1,0,1);		glDrawElements(GL_TRIANGLE_STRIP, 4, GL_UNSIGNED_SHORT, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_Idx[4]);	glColor4f(0,1,1,1);		glDrawElements(GL_TRIANGLE_STRIP, 4, GL_UNSIGNED_SHORT, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_Idx[5]);	glColor4f(1,0,1,1);		glDrawElements(GL_TRIANGLE_STRIP, 4, GL_UNSIGNED_SHORT, 0);


		glDisableClientState(GL_VERTEX_ARRAY);


		glColor4f(1,1,1,1);


		// bind 0
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}

	glPopMatrix();

	return LC_OK;
}



void CMain::SetPerspective()
{
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


	glMatrixMode(GL_MODELVIEW);
}
