


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
#include <LcAux.h>

#include <LctEntity.h>


int main(int argc, char** argv)
{
	INT hr = 0;
	PLCT_DBCSV	pObj = NULL;



	INT		dlm = ',';

	FILE* fp = fopen("data/tb_player_base_data.csv", "r");
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




	//hr = LctDB_CreateCsv(NULL, &pObj, "data/tb_player_base_data.csv");
	hr = LctDB_CreateCsv(NULL, &pObj, sbuf, &size, &dlm);
	if(LC_FAILED(hr))
		return -1;

	ILCT_DBcsv::TCSV_CELL	cell;
	ILCT_DBcsv::TCSV_REC	rec;

	int i, j;
	INT nRec=0, nField=0;


	

	hr = pObj->GetAttrib(LCTDB_RECORD_NUM, &nRec);
	hr = pObj->GetAttrib(LCTDB_FIELD_NUM, &nField);


	for(i=0; i<nRec; ++i)
	{
		for(j=0; j<nField; ++j)
		{
			hr = pObj->Select(&cell, i, j);

			if     (LCTDB_BOOL  == cell.t)
				printf("%d, ", (int)cell.b);

			else if(LCTDB_SMALL == cell.t)
				printf("%d, ", cell.n);

			else if(LCTDB_FLOAT == cell.t)
				printf("%f, ", cell.f);

			else if(LCTDB_BIG   == cell.t)
				printf("%lf, ", cell.d);

			else if(LCTDB_TEXT  == cell.t)
				printf("%s, ", cell.s);
			else
				printf("not defined");
		}

		printf("0\n");
	}


	SAFE_RELEASE(	pObj);
	return 0;
}



