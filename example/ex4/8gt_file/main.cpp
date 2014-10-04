


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

#include <LcType.h>
#include <LcBase.h>
#include <LcAux.h>
#include <LcStdev.h>
#include <LcxMedia.h>
#include <LctEntity.h>


int main(int argc, char** argv)
{
	INT hr = 0;

	PLC_FILE	pObj = NULL;

	FILE* fp = fopen("tex.csv", "r");
	int size = 0;
	char* sbuf = NULL;

	if(NULL == fp)
		return 0;

	fseek(fp, 0, SEEK_END);
	size = ftell(fp);
	fseek(fp, 0, SEEK_SET);


	sbuf = new char[size*2];
	memset(sbuf, 0, size * 2);
	fread(sbuf, 1, size, fp);

	fclose(fp);

	hr = LcFile_CreateFile(NULL, &pObj, sbuf, size, "r");


	while(1)
	{
		char sline[256]={0};
		int r = pObj->ReadLine(sline, 256);

		if( 0> r)
			break;

		printf("%s\n", sline);
	}

	delete [] sbuf;


	return 0;
}
