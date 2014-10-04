


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



int Zip_CompressToBufferFromFile(unsigned char** OutZip		  // output zip data
								, unsigned long* OutZipSize	  // output zip size
								, unsigned long* OutSrcSize	  // output orginal size
								, char*          sOrg         // source file name
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
	fclose(fO);
	fO = NULL;

	zipL = compressBound(orgL);
	zip  = (unsigned char*) malloc(zipL);

	hr = compress2(zip, &zipL, org, orgL, level);
	if(Z_OK != hr)
		goto END;


END:
	if(fO)
		fclose(fO);

	if(org)
		free(org);


	if(OutZip)
		*OutZip = zip;

	if(OutZipSize)
		*OutZipSize = zipL;

	if(OutSrcSize)
		*OutSrcSize = orgL;


	return hr;
}





int Zip_CompressOneFile(							// Compress SrcFile to DstFile
						char* sZip					// Output Compress Data File Name
						, char* sOrg				// Source File Name
						)
{
	int		hr = Z_ERRNO;

	FILE*	fZ = NULL;


	unsigned char*	zip = NULL;
	unsigned long	zipL= 0;
	unsigned long	orgL= 0;


	fZ = fopen(sZip, "wb");
	if(NULL == fZ)
		goto END;


	hr = Zip_CompressToBufferFromFile(&zip, &zipL, &orgL, sOrg);
	if(0> hr)
		goto END;

	
	fwrite(&orgL, 1, sizeof(unsigned long), fZ);
	fwrite(&zipL, 1, sizeof(unsigned long), fZ);
	fwrite(sOrg , 1, strlen(sOrg), fZ);

	fseek(fZ, 260, SEEK_SET);
	fwrite(zip, 1, zipL, fZ);

END:
	if(fZ)
		fclose(fZ);

	if(zip)
		free(zip);

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


	fread(&orgL, 1, sizeof(unsigned long), fZ);
	fread(&zipL, 1, sizeof(unsigned long), fZ);

	org = (unsigned char*)malloc(orgL);
	zip = (unsigned char*)malloc(zipL);

//	fseek(fZ, 64, SEEK_SET);
	fread(zip, 1, zipL, fZ);

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
	if(LC_FAILED(Zip_CompressOneFile(sDst, sSrc)))
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


