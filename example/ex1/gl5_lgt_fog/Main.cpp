// Implementation of the CMain class.
//
////////////////////////////////////////////////////////////////////////////////


#include "_StdAfx.h"


#define MEDIA_DIR	"../../media/"


FixedMesh* LoadMeshFromFile(const char *filename);


CMain::CMain()
{
	m_pMesh	= NULL;
}


int	CMain::Init()
{
	//Load our scene file
	m_pMesh = LoadMeshFromFile(MEDIA_DIR"model/scene.gsd");

	return LC_OK;
}

int	CMain::Destroy()
{
	 //delete mesh data
	free(m_pMesh->Geometry);
	free(m_pMesh->Indices);
	free(m_pMesh->Normals);
	free(m_pMesh->TexCoord);
	free(m_pMesh);

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




	// set up projection matrix in 3d pipeline
	// set up the Projection matrix
	SetPerspective();


	glMatrixMode(GL_MODELVIEW);						// specify which matrix is the current matrix to Model-View Matrix.
	glLoadIdentity();


	//fog color
	LCXCOLOR fogColor(0, 0.7F, 0.9F, 0);

	glFogfv(GL_FOG_COLOR, (const GLfloat*)&fogColor);
	glFogf(GL_FOG_MODE, GL_LINEAR);
	glFogf(GL_FOG_START, 20);
	glFogf(GL_FOG_END,  100);

	glEnable(GL_FOG);



	// Lighting Position
	static GLfloat Rot=0;
	LCXVECTOR4	vcLight(0, 10, 0, 0);
	LCXCOLOR	amb(0.0F, 1.0F, 1.0F, 1.F);
	LCXCOLOR	dif(1.f, 1.0f, 0.f, 1);

	Rot += 0.04f;
	vcLight.x = 30*(GLfloat)sin(Rot);
	vcLight.z = 30*(GLfloat)cos(Rot);


	glLightfv(GL_LIGHT0, GL_AMBIENT,  (const GLfloat*)&amb);
	glLightfv(GL_LIGHT0, GL_DIFFUSE,  (const GLfloat*)&dif);
	glLightfv(GL_LIGHT0, GL_POSITION, (const GLfloat*)&vcLight);
	glEnable(GL_LIGHT0);						//Enable Light0 (switch on it)

	glEnable(GL_LIGHTING);						//Enable lighting computations

	// Set default OpenGL ES material settings
	LCXCOLOR mat_amb(0.2f, 0.2f, 0.2f, 0.2f);
	LCXCOLOR mat_dif(0.8f, 0.8f, 0.8f, 0.8f);
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, (const GLfloat*)&mat_amb);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, (const GLfloat*)&mat_dif);




	glPushMatrix();
	{
		// Set Model position
		static GLfloat rotation = 0;
		rotation +=0.4f;

		glTranslatef(0, -5, -60);
		glRotatef(30, 1, 0, 0);
		glRotatef(rotation, 0, 1, 0);

		glEnableClientState(GL_VERTEX_ARRAY);
		glVertexPointer(3, GL_FLOAT, 0, m_pMesh->Geometry);

		// Normals are needed for the lighting computations
		glEnableClientState(GL_NORMAL_ARRAY);
		glNormalPointer(GL_FLOAT, 0, m_pMesh->Normals);

		glDrawElements(GL_TRIANGLES,m_pMesh->indexCounter,GL_UNSIGNED_SHORT,m_pMesh->Indices);

		glDisableClientState(GL_VERTEX_ARRAY);
		glDisableClientState(GL_NORMAL_ARRAY);
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






//Structure to hold the data readed from the file
struct GenericObjectData
{
  char Name[128];
  char ParentName[128];
  unsigned int *Indices;
  GLfloat *Geometry;
  GLfloat *Normals;
  GLfloat *TexCoord;
  unsigned long iC;
  unsigned long vC;
};

//GSD file header
struct GSDHeader
{
  char id[32];
  char version[16];
  int numberOfSubObjects;
};


FixedMesh* LoadMeshFromFile(const char *filename)
{
	GenericObjectData o;
	FixedMesh* mesh= (FixedMesh*)malloc(sizeof(struct FixedMesh));
	unsigned int i;

	struct GSDHeader header;
	FILE *meshFile = fopen(filename,"rb");

	if(!meshFile)
		return NULL;

	/*The header holds a brief description of the file, the version number, and the number of meshes
	that are stored in the file. This type of files are thought for static meshes only*/
	fread(&header,sizeof(struct GSDHeader),1,meshFile);

	//Check if there is at least one object
	if(header.numberOfSubObjects < 1)
		return NULL;

	// we only will use the first object, so we won't iterate over the others, if they exist
	fread(o.Name,sizeof(char)*128,1,meshFile); //read the object name
	fread(o.ParentName,sizeof(char)*128,1,meshFile); //Read the name of the parent object (useful for hierarchies)
	fread(&o.iC,sizeof(unsigned long),1,meshFile); //read the number of vertex indices
	fread(&o.vC,sizeof(unsigned long),1,meshFile); //read the number of vertices

	//allocate enough space for the indices and the GLshort version of them
	o.Indices = (unsigned int *)malloc(sizeof(unsigned int)*(o.iC));
	mesh->Indices = (short *)malloc(sizeof(GLshort)*(o.iC));
	fread(o.Indices, sizeof(unsigned int) * o.iC,1,meshFile); // read all indices

	//allocate enough space for the vertices and the GLfloat version of them
	o.Geometry = (GLfloat *)malloc(sizeof(GLfloat)*(o.vC * 3));
	mesh->Geometry = (GLfloat *)malloc(sizeof(GLfloat)*(o.vC * 3));
	fread(o.Geometry,o.vC * 3 * sizeof(GLfloat),1,meshFile); //read all vertices (1 vertex = 3 floats)

	//allocate enough space for the texture coordinates and the GLfloat version of them
	o.TexCoord = (GLfloat *)malloc(sizeof(GLfloat)*(o.vC * 2));
	mesh->TexCoord = (GLfloat *)malloc(sizeof(GLfloat)*(o.vC * 2));
	fread(o.TexCoord,o.vC * 2 * sizeof(GLfloat),1,meshFile);//read all texcoords (1 tex coord = 2 floats)

	//allocate enough space for the normals and the GLfloat version of them
	o.Normals=(GLfloat *)malloc(sizeof(GLfloat)*(o.vC * 3));
	mesh->Normals = (GLfloat *)malloc(sizeof(GLfloat)*(o.vC * 3));
	fread(o.Normals,o.vC * 3* sizeof(GLfloat),1,meshFile);//read all normals (1 normal = 3 floats)
	fclose(meshFile); //Do not need the file opened anymore

	// Convert data to optimized data types for OpenGL ES (GLfloat and GLshort)
	for(i=0;i<o.vC * 3;i++)
	{
		mesh->Geometry[i]= (o.Geometry[i]);
		mesh->Normals[i] = (o.Normals[i]);
	}

	for(i=0;i<o.vC * 2;i++)
		mesh->TexCoord[i] = (o.TexCoord[i]);

	for(i=0;i<o.iC;i++)
		mesh->Indices[i] = (GLshort)o.Indices[i];

	mesh->indexCounter = (GLshort)o.iC;
	mesh->vertexCounter= (GLshort)o.vC;

	//delete original values, we will use only the optimized ones
	free(o.Indices);
	free(o.Geometry);
	free(o.Normals);
	free(o.TexCoord);

	return mesh;
}


