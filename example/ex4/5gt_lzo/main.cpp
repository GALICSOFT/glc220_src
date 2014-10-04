


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

#include <LctEntity.h>


#define MEDIA_DIR	"../../media/"



int main(int argc, char* argv[])
{
//	char	sSrc[260]="bg.png";
	char	sSrc[260]=MEDIA_DIR"bg512x256.tga";
	char	sDst[260]=MEDIA_DIR"t.gpz";
	char	sDst2[260]=MEDIA_DIR"t.tga";

	// 압축
	if(LC_FAILED(Lzip_CompressFileToFile(sDst, sSrc)))
		return LC_EFAIL;

	if(LC_FAILED(Lzip_DeCompressFileToFile(sDst2, sDst)))
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


