


#if defined(_MSC_VER)
  #pragma comment(lib, "lgpwn_b22_.lib")
  #pragma comment(lib, "lgpwn_c22_.lib")
  #pragma comment(lib, "lgpwn_x22_.lib")
  #pragma comment(lib, "lgpwn_t22_.lib")
  #pragma comment(lib, "zlib125_.lib")
#endif


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <io.h>

#include "D:\_cygwin\home\Administrator\glc220\rc_cmm/03zlib125/contrib/minizip/unzip.h"
#include "D:\_cygwin\home\Administrator\glc220\/rc_cmm/03zlib125/contrib/minizip/zip.h"

#include <LcType.h>
#include <LcBase.h>
#include <LcAux.h>


#define MEDIA_DIR	"../../media/"


enum
{
	LZIP_NO_COMPRESSION      =    0,
	LZIP_BEST_SPEED          =    1,
	LZIP_BEST_COMPRESSION    =    9,
	LZIP_DEFAULT_COMPRESSION = (-1),
};



//compressed buffer: zipped size : unsigned long, orginal size: unsigned long, zip data: unsigned byte



int Zip_CompressBufferToBuffer(unsigned char** OutZip		  // output zip data
							,  const unsigned char* Src		  // input src data
							,  const unsigned long  SrcSize	  // input src size
							,  int level	 = LZIP_DEFAULT_COMPRESSION // zip level
								)
{
	int		hr = Z_ERRNO;

	unsigned char*	org = (unsigned char*)Src;
	unsigned long	orgL= SrcSize;

	unsigned char*	zipAll = NULL;
	unsigned char*	zip= NULL;
	unsigned long	zipL= 0;


	zipL = compressBound(orgL);
	zipAll  = (unsigned char*) malloc(zipL + sizeof(unsigned long)*2 );

	zip  = zipAll + sizeof(unsigned long) * 2;

	hr = compress2(zip, &zipL, org, orgL, level);
	if(Z_OK != hr)
		goto END;


	zip  = zipAll;
	memcpy(zip, &zipL, sizeof(unsigned long) );zip += sizeof(unsigned long);
	memcpy(zip, &orgL, sizeof(unsigned long) );


END:
	if(Z_OK != hr)
	{
		if(zipAll)
			free(zipAll);

		return -1;
	}


	if(OutZip)
		*OutZip = zipAll;

	return hr;
}




int Zip_CompressToBufferFromFile(unsigned char** OutZip		  // output zip data
								, const char*    sOrg         // source file name
								, int level	 = LZIP_DEFAULT_COMPRESSION // zip level
								)
{
	int		hr = Z_ERRNO;

	FILE*	fO = NULL;

	unsigned char*	org = NULL;
	unsigned long	orgL= 0;

	unsigned char*	zip = NULL;
	unsigned long	zipL= 0;


	fO = fopen(sOrg, "rb");
	if(NULL == fO)
		goto END;

	fseek(fO, 0, SEEK_END);
	orgL = ftell(fO);

	if(0 == orgL)
		goto END;


	org = (unsigned char*) malloc(orgL);

	fseek(fO, 0, SEEK_SET);
	fread(org, 1, orgL, fO);


	hr = Zip_CompressBufferToBuffer(&zip, org, orgL, level); 

END:
	if(fO)
		fclose(fO);

	if(org)
		free(org);

	if(0 > hr)
		return hr;



	if(OutZip)
		*OutZip = zip;


	return hr;
}





int Zip_CompressFileToFile(							// Compress SrcFile to DstFile
						char* sZip					// Output Compress Data File Name
						, char* sOrg				// Source File Name
						)
{
	int		hr = Z_ERRNO;

	FILE*	fZ = NULL;


	unsigned char*	zipAll = NULL;
	unsigned char*	zip = NULL;
	unsigned long	zipL= 0;

	unsigned char*	org = NULL;
	unsigned long	orgL= 0;



	fZ = fopen(sZip, "wb");
	if(NULL == fZ)
		goto END;


	hr = Zip_CompressToBufferFromFile(&zipAll, sOrg);
	if(0> hr)
		goto END;


	fwrite(sOrg , 1, strlen(sOrg), fZ);
	fseek(fZ, 160, SEEK_SET);

	zip = zipAll;
	memcpy(&zipL, zip, sizeof(unsigned long));	zip += sizeof(unsigned long);
	memcpy(&orgL, zip, sizeof(unsigned long));	zip += sizeof(unsigned long);

	fwrite(&zipL, 1, sizeof(unsigned long), fZ);
	fwrite(&orgL, 1, sizeof(unsigned long), fZ);
	fwrite(  zip, 1, zipL, fZ);

END:
	if(fZ)
		fclose(fZ);

	if(zipAll)
		free(zipAll);

	return hr;
}




int Zip_DeCompressBufferToBuffer( unsigned char**     OutUnzip	  // output unzip data
								, unsigned char*      InputZiped
								)
{
	int		hr = Z_ERRNO;

	unsigned char*	orgAll = NULL;
	unsigned char*	org = NULL;
	unsigned long	orgL= 0;

	unsigned char*	zip = InputZiped;
	unsigned long	zipL= 0;


	memcpy(&zipL, zip, sizeof(unsigned long));	zip += sizeof(unsigned long);
	memcpy(&orgL, zip, sizeof(unsigned long));	zip += sizeof(unsigned long);


	orgAll = (unsigned char*)malloc( orgL + sizeof(unsigned long) );

	org = orgAll;
	memcpy(orgAll, &orgL, sizeof(unsigned long) );	org += sizeof(unsigned long);

	hr = uncompress(org, &orgL, zip, zipL);
	if(Z_OK != hr)
		goto END;

END:
	if(0>hr)
	{
		if(orgAll)
			free(orgAll);
		
		return hr;
	}


	if(OutUnzip)
		*OutUnzip = orgAll;

	return hr;
}




int Zip_DeCompressToBufferFromFile(unsigned char** OutUnZip	  // output zip data
								,  const char*    sZip        // source file name
								)
{
	int		hr = Z_ERRNO;

	FILE*	fZ = NULL;
	char	sOrgFile[160]={0};


	unsigned char*	orgAll = NULL;
	unsigned char*	org = NULL;
	unsigned long	orgL= 0;

	unsigned char*	zip = NULL;
	unsigned long	zipL= 0;


	fZ = fopen(sZip, "rb");
	if(NULL == fZ)
		goto END;



	fread(sOrgFile, 1, 160, fZ);
	fseek(fZ, 160, SEEK_SET);

	fread(&zipL, 1, sizeof(unsigned long), fZ);
	fread(&orgL, 1, sizeof(unsigned long), fZ);

	zip    = (unsigned char*)malloc(zipL);
	orgAll = (unsigned char*)malloc(orgL + sizeof(unsigned long) );

	org = orgAll;
	memcpy(orgAll, &orgL, sizeof(unsigned long) );	org += sizeof(unsigned long);

	fread(zip, 1, zipL, fZ);
	fclose(fZ);
	fZ = NULL;


	hr = uncompress(org, &orgL, zip, zipL);
	if(Z_OK != hr)
		goto END;


END:
	if(0>hr)
	{
		if(orgAll)
			free(orgAll);
		
		return hr;
	}


	if(fZ)
		fclose(fZ);

	if(zip)
		free(zip);

	if(OutUnZip)
		*OutUnZip = orgAll;

	return hr;
}



int Zip_DeCompressOneFile(							// Compress SrcFile to DstFile
						char* sOrg					// Output Compress Data File Name
						, char* sZip				// Source File Name
						)
{
	int		hr = Z_ERRNO;

	FILE*	fO = NULL;
	FILE*	fZ = NULL;
	char	sOrgFile[160]={0};


	unsigned char*	org = NULL;
	unsigned long	orgL= 0;

	unsigned char*	zip = NULL;
	unsigned long	zipL= 0;


	fZ = fopen(sZip, "rb");
	if(NULL == fZ)
		goto END;


	fO = fopen(sOrg, "wb");
	if(NULL == fO)
		goto END;


	fread(sOrgFile, 1, 160, fZ);
	fseek(fZ, 160, SEEK_SET);

	fread(&zipL, 1, sizeof(unsigned long), fZ);
	fread(&orgL, 1, sizeof(unsigned long), fZ);

	zip = (unsigned char*)malloc(zipL);
	org = (unsigned char*)malloc(orgL);

	fread(zip, 1, zipL, fZ);
	fclose(fZ);
	fZ = NULL;


	hr = uncompress(org, &orgL, zip, zipL);
	if(Z_OK != hr)
		goto END;

	fwrite(org, 1, orgL, fO);

END:
	if(fO)
		fclose(fO);

	if(fZ)
		fclose(fZ);

	if(org)
		free(org);

	if(zip)
		free(zip);

	return hr;
}



int main(int argc, char* argv[])
{
//	char	sSrc[260]="bg.png";
	char	sSrc[260]=MEDIA_DIR"page_03.lua";
	char	sDst[260]=MEDIA_DIR"t.gpz";
	char	sDst2[260]=MEDIA_DIR"t.tga";

	// 압축
	if(LC_FAILED(Zip_CompressFileToFile(sDst, sSrc)))
		return LC_EFAIL;

	if(LC_FAILED(Zip_DeCompressOneFile(sDst2, sDst)))
		return LC_EFAIL;

//	// 압축 파일을 특정 폴더에 풀기
//	if(LC_FAILED(DgtLzo_DeCompressFile((char*)"../", (char*)"t.gpz")))
//		return LC_EFAIL;
//
//
//	unsigned char* sbuf;
//	unsigned long  lbuf;
//
//	// 압축 파일에서 데이터 직접 가져오기
//	if(LC_FAILED(DgtLzo_DeCompressFromFile(&sbuf, &lbuf, (char*)"t.gpz")))
//		return LC_EFAIL;
//
//
//	// Debugging...
//	FILE* fp = fopen("tt.txt", "wt");
//	fwrite(sbuf, sizeof(unsigned char), lbuf, fp);
//	free(sbuf);
//	fclose(fp);
//
//	char sLine[512];
//	fp = fopen("tt.txt", "rt");
//
//	while(!feof(fp))
//	{
//		fgets(sLine, 512, fp);
//		printf("%s", sLine);
//	}
//
//	fclose(fp);


	return LC_OK;
}


