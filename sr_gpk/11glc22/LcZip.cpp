// Implementation of the Zip...
//
////////////////////////////////////////////////////////////////////////////////

#if defined(_MSC_VER) && _MSC_VER >= 1400
  #pragma warning(disable: 4996)
#endif


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../../rc_cmm/03zlib125/minizip/unzip.h"
#include "../../rc_cmm/03zlib125/minizip/zip.h"

#include <LcType.h>

#include "LcAux.h"



int Lzip_CompressBufferToBuffer(  BYTE**      OutZip	// output zip data
								, const BYTE* Src		// input src data
								, const ULONG SrcSize	// input src size
								, int         level		// zip level
								)
{
	int		hr = Z_ERRNO;

	BYTE*	org = (BYTE*)Src;
	ULONG	orgL= SrcSize;

	BYTE*	zipT= NULL;
	BYTE*	zip = NULL;
	ULONG	zipL= 0;


	zipL = compressBound(orgL);
	zipT = (BYTE*) malloc(zipL + sizeof(ULONG)*2 );

	zip  = zipT + sizeof(ULONG) * 2;

	hr = compress2(zip, &zipL, org, orgL, level);
	if(Z_OK != hr)
		goto END;


	zip  = zipT;

	zipL += sizeof(ULONG) * 2;
	memcpy(zip, &zipL, sizeof(ULONG) );

	zip += sizeof(ULONG);
	memcpy(zip, &orgL, sizeof(ULONG) );


END:
	if(Z_OK != hr)
	{
		if(zipT)
			free(zipT);

		return -1;
	}


	if(OutZip)
		*OutZip = zipT;

	return hr;
}



int Lzip_CompressToBufferFromFile( BYTE**      OutZip	// output zip data
								,  const char* sOrg		// source file name
								,  int         level	// zip level
								)
{
	int		hr = Z_ERRNO;

	FILE*	fO = NULL;

	BYTE*	org = NULL;
	ULONG	orgL= 0;

	BYTE*	zip = NULL;



	fO = fopen(sOrg, "rb");
	if(NULL == fO)
		goto END;

	fseek(fO, 0, SEEK_END);
	orgL = ftell(fO);

	if(0 == orgL)
		goto END;


	org = (BYTE*) malloc(orgL);

	fseek(fO, 0, SEEK_SET);
	fread(org, 1, orgL, fO);


	hr = Lzip_CompressBufferToBuffer(&zip, org, orgL, level);

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



int Lzip_CompressFileToFile(				// Compress SrcFile to DstFile
							  char* sZip	// Output Compress Data File Name
							, char* sOrg	// Source File Name
						)
{
	int		hr = Z_ERRNO;

	FILE*	fZ = NULL;


	BYTE*	zipT= NULL;
	ULONG	zipL= 0;






	fZ = fopen(sZip, "wb");
	if(NULL == fZ)
		goto END;


	hr = Lzip_CompressToBufferFromFile(&zipT, sOrg);
	if(0> hr)
		goto END;


	fwrite(sOrg , 1, strlen(sOrg), fZ);
	fseek(fZ, LC_DEFAULT_NAME, SEEK_SET);

	memcpy(&zipL, zipT, sizeof(ULONG));
	fwrite( zipT, 1, zipL, fZ);

END:
	if(fZ)
		fclose(fZ);

	if(zipT)
		free(zipT);

	return hr;
}



int Lzip_DeCompressBufferToBuffer(BYTE** OutUnzip		  // output unzip data
								, BYTE*  InputZiped
								)
{
	int		hr = Z_ERRNO;

	BYTE*	orgT = NULL;
	BYTE*	org = NULL;
	ULONG	orgL= 0;

	BYTE*	zip = InputZiped;
	ULONG	zipL= 0;


	memcpy(&zipL, zip, sizeof(ULONG));	zip += sizeof(ULONG);
	memcpy(&orgL, zip, sizeof(ULONG));	zip += sizeof(ULONG);


	orgT = (BYTE*)malloc( orgL + sizeof(ULONG) );

	org = orgT;

	memcpy(orgT, &orgL, sizeof(ULONG) );
	org += sizeof(ULONG);

	hr = uncompress(org, &orgL, zip, zipL);
	if(Z_OK != hr)
		goto END;

END:
	if(0>hr)
	{
		if(orgT)
			free(orgT);

		return hr;
	}


	if(OutUnzip)
		*OutUnzip = orgT;

	return hr;
}



int Lzip_DeCompressBufferToBuffer(BYTE** OutUnzip		// output unzip data with no output size
								, ULONG* OutSize		// input src size
								, BYTE*  InputZiped		// Input zipped data
								)
{
	int		hr = Z_ERRNO;

	BYTE*	org = NULL;
	ULONG	orgL= 0;

	BYTE*	zip = InputZiped;
	ULONG	zipL= 0;


	memcpy(&zipL, zip, sizeof(ULONG));	zip += sizeof(ULONG);
	memcpy(&orgL, zip, sizeof(ULONG));	zip += sizeof(ULONG);


	org = (BYTE*)malloc( orgL );



	hr = uncompress(org, &orgL, zip, zipL);
	if(Z_OK != hr)
		goto END;

END:
	if(0>hr)
	{
		if(org)
			free(org);

		return hr;
	}


	if(OutUnzip)
		*OutUnzip = org;

	if(OutSize)
		*OutSize = orgL;

	return hr;
}



int Lzip_DeCompressToBufferFromFile(  BYTE**      OutUnZip		// output zip data
									, char*       sOrgFile		// original file name
									, const char* sZip			// source file name
									)
{
	int		hr = Z_ERRNO;

	FILE*	fZ = NULL;

	BYTE*	orgT= NULL;
	BYTE*	org = NULL;
	ULONG	orgL= 0;

	BYTE*	zip = NULL;
	ULONG	zipL= 0;


	fZ = fopen(sZip, "rb");
	if(NULL == fZ)
		goto END;


	if(sOrgFile)
		fread(sOrgFile, 1, LC_DEFAULT_NAME, fZ);


	fseek(fZ, LC_DEFAULT_NAME, SEEK_SET);
	fread(&zipL, 1, sizeof(ULONG), fZ);
	fread(&orgL, 1, sizeof(ULONG), fZ);

	zip  = (BYTE*)malloc(zipL);
	orgT = (BYTE*)malloc(orgL + sizeof(ULONG) );

	org = orgT;

	memcpy(orgT, &orgL, sizeof(ULONG) );
	org += sizeof(ULONG);


	fread(zip, 1, zipL, fZ);
	fclose(fZ);
	fZ = NULL;


	hr = uncompress(org, &orgL, zip, zipL);
	if(Z_OK != hr)
		goto END;


END:
	if(0>hr)
	{
		if(orgT)
			free(orgT);

		return hr;
	}


	if(fZ)
		fclose(fZ);

	if(zip)
		free(zip);

	if(OutUnZip)
		*OutUnZip = orgT;

	return hr;
}



int Lzip_DeCompressFileToFile( char* sOrg			// Output DeCompress File Name
							 , const char* sZip		// zipped File Name
								)
{
	int		hr = Z_ERRNO;

	FILE*	fO = NULL;
	char	sOrgFile[LC_DEFAULT_NAME]={0};


	BYTE*	orgT= NULL;
	BYTE*	org = NULL;
	ULONG	orgL= 0;


	hr = Lzip_DeCompressToBufferFromFile(&orgT, sOrgFile, sZip);

	if(0>hr)
		goto END;



	fO = fopen(sOrg, "wb");
	if(NULL == fO)
		goto END;


	org = orgT;

	memcpy(&orgL, org, sizeof(ULONG));
	org += sizeof(ULONG);

	fwrite(org, 1, orgL, fO);

END:
	if(fO)
		fclose(fO);

	if(orgT)
		free(orgT);


	return hr;
}





int Lzip_UnzipbyName(							// Decompress buffer from zip file by finding name
					   BYTE**      bufOut		// DeCompress Data
					 , ULONG*      lsize		// DeCompress Data Size
					 , const char* ZipFile		// Compressed zip file
					 , const char* FindingName	// to finding file Name
					 , int         bIgnorePath	// Ignore the path in zip file
					 )
{

	char	tFile[LC_MAX_PATH ]={0};
	char	ext  [LC_MAX_EXT  ]={0};
	char	FindingFile[LC_MAX_PATH]={0};


	int		bFind = LC_EFAIL;
	int		hr = UNZ_OK;
	unzFile	fp = NULL;

	int		len = 0;
	char	buf[1024]={0};
	char*	s1  = NULL;



	*bufOut	= NULL;
	*lsize  = 0;

	fp= unzOpen(ZipFile);	// open the zip file
	if(!fp)
		return -1;


	hr = unzGoToFirstFile(fp);		// go to the first compressed file
	if(UNZ_OK != hr)
		goto END;



	if(bIgnorePath)
	{
		LcStr_ReplacePath(FindingFile, FindingName, "", "", NULL, NULL);
	}
	else
	{
		strcpy(FindingFile, FindingName);
	}



	while(UNZ_END_OF_LIST_OF_FILE != hr)
	{
		char			zFile[260]={0};
		unz_file_info   zInfo={0};

		// get the zInfo and file Name
		hr = unzGetCurrentFileInfo(fp, &zInfo, zFile, 260, NULL, 0, NULL, 0);


		if(UNZ_OK == hr)
		{
			if(bIgnorePath)
			{
				LcStr_ReplacePath(tFile, zFile, "", "", NULL, NULL);
				hr = LcStr_Stricmp(tFile, FindingFile);
			}
			else
			{
				hr = LcStr_Stricmp(zFile, FindingFile);
			}


			if(0 == hr)
			{
				hr = unzOpenCurrentFile(fp);
				if(UNZ_OK == hr)
				{
					*lsize  = zInfo.uncompressed_size;
					*bufOut	= (BYTE*)malloc(*lsize);

					s1 = (char*)(*bufOut);

					while(1)
					{
						len = unzReadCurrentFile(fp, buf, 1024);
						if(0 >= len)
							break;

						memcpy(s1, buf, len);
						s1 += len;
					}

					bFind = LC_OK;
					unzCloseCurrentFile(fp);
				}

				break;
			}
		}

		hr = unzGoToNextFile(fp);
	}

END:
	if(fp)
		hr = unzCloseCurrentFile(fp);

	if(fp)
		hr= unzClose(fp);

	fp= NULL;

	return bFind;
}


