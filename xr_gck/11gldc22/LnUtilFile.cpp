// Implementation of the CLcFileSys class.
//
////////////////////////////////////////////////////////////////////////////////

#if _MSC_VER >= 1200
#pragma warning(disable : 4996)
#endif

#include <direct.h>
#include <sys/stat.h>
#include <time.h>

#include "LnUtil.h"



DWORD LcFile_Err()
{
	LPVOID	lpMsgBuf;
	DWORD	dMsgId = GetLastError();

	FormatMessage(
			FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS
		,	NULL
		,	dMsgId
		,	MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT)	// Default language
		,	(LPTSTR) &lpMsgBuf
		,	0,NULL
		);

	printf("%s\n", (char*)lpMsgBuf);
//	MessageBox(0, (LPCTSTR)lpMsgBuf, "Err", 0);
	// Free the buffer.
	LocalFree( lpMsgBuf );

	return dMsgId;
}


void LcFile_Err(DWORD dError)
{
	LPVOID	lpMsgBuf;

	FormatMessage(
			FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS
		,	NULL
		,	dError
		,	MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT)	// Default language
		,	(LPTSTR) &lpMsgBuf
		,	0,NULL
		);

	printf("%s\n", (char*)lpMsgBuf);
//	MessageBox(0, (LPCTSTR)lpMsgBuf, "Err", 0);
	// Free the buffer.
	LocalFree( lpMsgBuf );

	//	case NO_ERROR:						break;
	//	case ERROR_FILE_NOT_FOUND:			break;
	//	case ERROR_PATH_NOT_FOUND:			break;
	//	case ERROR_TOO_MANY_OPEN_FILES:		break;
	//	case ERROR_ACCESS_DENIED:			break;
	//	case ERROR_INVALID_HANDLE:			break;
	//	case ERROR_BAD_FORMAT:				break;
	//	case ERROR_INVALID_ACCESS:			break;
	//	case ERROR_INVALID_DRIVE:			break;
	//	case ERROR_CURRENT_DIRECTORY:		break;
	//	case ERROR_NOT_SAME_DEVICE:			break;
	//	case ERROR_NO_MORE_FILES:			break;
	//	case ERROR_WRITE_PROTECT:			break;
	//	case ERROR_BAD_UNIT:				break;
	//	case ERROR_NOT_READY:				break;
	//	case ERROR_BAD_COMMAND:				break;
	//	case ERROR_CRC:						break;
	//	case ERROR_BAD_LENGTH:				break;
	//	case ERROR_SEEK:					break;
	//	case ERROR_NOT_DOS_DISK:			break;
	//	case ERROR_SECTOR_NOT_FOUND:		break;
	//	case ERROR_WRITE_FAULT:				break;
	//	case ERROR_READ_FAULT:				break;
	//	case ERROR_SHARING_VIOLATION:		break;
	//	case ERROR_LOCK_VIOLATION:			break;
	//	case ERROR_WRONG_DISK:				break;
	//	case ERROR_SHARING_BUFFER_EXCEEDED:	break;
	//	case ERROR_HANDLE_EOF:				break;
	//	case ERROR_HANDLE_DISK_FULL:		break;
	//	case ERROR_DUP_NAME:				break;
	//	case ERROR_BAD_NETPATH:				break;
	//	case ERROR_NETWORK_BUSY:			break;
	//	case ERROR_DEV_NOT_EXIST:			break;
	//	case ERROR_ADAP_HDW_ERR:			break;
	//	case ERROR_BAD_NET_RESP:			break;
	//	case ERROR_UNEXP_NET_ERR:			break;
	//	case ERROR_BAD_REM_ADAP:			break;
	//	case ERROR_NO_SPOOL_SPACE:			break;
	//	case ERROR_NETNAME_DELETED:			break;
	//	case ERROR_NETWORK_ACCESS_DENIED:	break;
	//	case ERROR_BAD_DEV_TYPE:			break;
	//	case ERROR_BAD_NET_NAME:			break;
	//	case ERROR_TOO_MANY_NAMES:			break;
	//	case ERROR_SHARING_PAUSED:			break;
	//	case ERROR_REQ_NOT_ACCEP:			break;
	//	case ERROR_FILE_EXISTS:				break;
	//	case ERROR_CANNOT_MAKE:				break;
	//	case ERROR_ALREADY_ASSIGNED:		break;
	//	case ERROR_INVALID_PASSWORD:		break;
	//	case ERROR_NET_WRITE_FAULT:			break;
	//	case ERROR_DISK_CHANGE:				break;
	//	case ERROR_DRIVE_LOCKED:			break;
	//	case ERROR_BUFFER_OVERFLOW:			break;
	//	case ERROR_DISK_FULL:				break;
	//	case ERROR_NO_MORE_SEARCH_HANDLES:	break;
	//	case ERROR_INVALID_TARGET_HANDLE:	break;
	//	case ERROR_INVALID_CATEGORY:		break;
	//	case ERROR_INVALID_NAME:			break;
	//	case ERROR_INVALID_LEVEL:			break;
	//	case ERROR_NO_VOLUME_LABEL:			break;
	//	case ERROR_NEGATIVE_SEEK:			break;
	//	case ERROR_SEEK_ON_DEVICE:			break;
	//	case ERROR_DIR_NOT_ROOT:			break;
	//	case ERROR_DIR_NOT_EMPTY:			break;
	//	case ERROR_LABEL_TOO_LONG:			break;
	//	case ERROR_BAD_PATHNAME:			break;
	//	case ERROR_LOCK_FAILED:				break;
	//	case ERROR_BUSY:					break;
	//	case ERROR_INVALID_ORDINAL:			break;
	//	case ERROR_ALREADY_EXISTS:			break;
	//	case ERROR_INVALID_EXE_SIGNATURE:	break;
	//	case ERROR_BAD_EXE_FORMAT:			break;
	//	case ERROR_FILENAME_EXCED_RANGE:	break;
	//	case ERROR_META_EXPANSION_TOO_LONG:	break;
	//	case ERROR_DIRECTORY:				break;
	//	case ERROR_OPERATION_ABORTED:		break;
	//	case ERROR_IO_INCOMPLETE:			break;
	//	case ERROR_IO_PENDING:				break;
	//	case ERROR_SWAPERROR:				break;
}



void LcFile_GetAllFiles(vector<string >* pOut, char* sPath)
{
	char sSearchPath[2048];
	_finddata_t fd;
	long handle;

	int result=1;

	sprintf(sSearchPath,"%s/*.*", sPath);
	handle=_findfirst(sSearchPath,&fd);

	if (handle == -1)
		return;

	while (result != -1)
	{
		char sDate[128];
		char sYear[32];
		char sMon[32];
		char sDay[32];
		char sTime[32];

		if( (fd.attrib & _A_ARCH) && (fd.name[0] !='.'))
		{
			char sT[4096];
			strcpy(sDate, LcFile_SimpleStrDateTime(sYear, sMon, sDay, sTime, fd.time_write ) );
			sprintf(sT, "F %s %s %s %s  %10d %s %s", sYear, sMon, sDay, sTime, fd.size, sPath, fd.name);
			pOut->push_back(sT);
		}

		result=_findnext(handle,&fd);

	}

	_findclose(handle);

}


void LcFile_GetAllFolders(vector<string >* pOut, char* sPath)
{
	char sSearchPath[2048];
	_finddata_t fd;
	long handle;

	int result=1;

	sprintf(sSearchPath,"%s/*.*", sPath);
	handle=_findfirst(sSearchPath,&fd);

	if (handle == -1)
		return;

	while (result != -1)
	{
		char sDate[128];
		char sYear[32];
		char sMon[32];
		char sDay[32];
		char sTime[32];

		if( (fd.attrib & _A_SUBDIR) && (fd.name[0] !='.'))
		{
			char sT[4096];
			strcpy(sDate, LcFile_SimpleStrDateTime(sYear, sMon, sDay, sTime, fd.time_write ) );
			sprintf(sT, "D %s %s %s %s  %10d %s %s", sYear, sMon, sDay, sTime, fd.size, sPath, fd.name);
			pOut->push_back(sT);
		}

		result=_findnext(handle,&fd);

	}

	_findclose(handle);

}



void LcFile_GetAllFilesFromSubDir(vector<string >* pOut, char* sPath)
{
	char sSearchPath[2048];
	_finddata_t fd;
	long handle;

	int result=1;

	sprintf(sSearchPath,"%s/*.*", sPath);
	handle=_findfirst(sSearchPath,&fd);

	if (handle == -1)
		return;

	while (result != -1)
	{
		if(fd.name[0] !='.')
		{
			if((fd.attrib & _A_SUBDIR))
			{
				char sSearchFolder[2048];

				sprintf(sSearchFolder, "%s/%s", sPath, fd.name);
				LcFile_GetAllFilesFromSubDir(pOut, sSearchFolder);
			}
			else if((fd.attrib & _A_ARCH))
			{
				char sDate[128];
				char sYear[32];
				char sMon[32];
				char sDay[32];
				char sTime[32];

				char sT[4096];
				strcpy(sDate, LcFile_SimpleStrDateTime(sYear, sMon, sDay, sTime, fd.time_write ) );
				sprintf(sT, "F %s %s %s %s  %10d %s %s", sYear, sMon, sDay, sTime, fd.size, sPath, fd.name);
				pOut->push_back(sT);
			}
		}

		result=_findnext(handle,&fd);

	}

	_findclose(handle);

}


void LcFile_GetAllFoldersFromSubDir(vector<string >* pOut, char* sPath)
{
	char sSearchPath[2048];
	_finddata_t fd;
	long handle;

	int result=1;

	sprintf(sSearchPath,"%s/*.*", sPath);
	handle=_findfirst(sSearchPath,&fd);

	if (handle == -1)
		return;

	while (result != -1)
	{
		if(fd.name[0] !='.')
		{
			if((fd.attrib & _A_SUBDIR))
			{
				char sDate[128];
				char sYear[32];
				char sMon[32];
				char sDay[32];
				char sTime[32];

				char sT[4096];
				strcpy(sDate, LcFile_SimpleStrDateTime(sYear, sMon, sDay, sTime, fd.time_write ) );
				sprintf(sT, "D %s %s %s %s  %10d %s %s", sYear, sMon, sDay, sTime, fd.size, sPath, fd.name);
				pOut->push_back(sT);


				char sSearchFolder[2048];
				sprintf(sSearchFolder, "%s/%s", sPath, fd.name);
				LcFile_GetAllFoldersFromSubDir(pOut, sSearchFolder);
			}
		}

		result=_findnext(handle,&fd);

	}

	_findclose(handle);

}



void LcFile_GetAllFilesAndFoldersFromSubDir(vector<string >* pOut, char* sPath)
{
	char sSearchPath[2048];
	_finddata_t fd;
	long handle;

	int result=1;

	sprintf(sSearchPath,"%s/*.*", sPath);
	handle=_findfirst(sSearchPath,&fd);

	if (handle == -1)
		return;

	while (result != -1)
	{
		char sDate[128];
		char sYear[32];
		char sMon[32];
		char sDay[32];
		char sTime[32];

		if(fd.name[0] !='.')
		{
			char sT[4096];
			strcpy(sDate, LcFile_SimpleStrDateTime(sYear, sMon, sDay, sTime, fd.time_write ) );

			if((fd.attrib & _A_SUBDIR))		// Directory
				sprintf(sT, "D %s %s %s %s  %10d %s %s", sYear, sMon, sDay, sTime, fd.size, sPath, fd.name);
			else if((fd.attrib & _A_ARCH))	// File
				sprintf(sT, "F %s %s %s %s  %10d %s %s", sYear, sMon, sDay, sTime, fd.size, sPath, fd.name);

			pOut->push_back(sT);

			if((fd.attrib & _A_SUBDIR))
			{
				char sSearchFolder[2048];

				sprintf(sSearchFolder, "%s/%s", sPath, fd.name);
				LcFile_GetAllFilesAndFoldersFromSubDir(pOut, sSearchFolder);
			}
		}

		result=_findnext(handle,&fd);

	}

	_findclose(handle);

}



void LcFile_Finds(vector<string >* pOut, char* sPath, char* sFile)
{
	char		sSearchPath[2048];
	_finddata_t	fd;
	long		handle;
	int			result=1;

	sprintf(sSearchPath,"%s/*.*", sPath);
	handle=_findfirst(sSearchPath,&fd);

	if (handle == -1)
		return;

	while (result != -1)
	{
		if(fd.name[0] !='.')
		{
			if((fd.attrib & _A_SUBDIR))
			{
				char sSearchFolder[2048];

				sprintf(sSearchFolder, "%s/%s", sPath, fd.name);
				LcFile_Finds(pOut, sSearchFolder, sFile);
			}

			else
			{
				if(0== _stricmp(fd.name, sFile))
				{
					char sYear[32];
					char sMon[32];
					char sDay[32];
					char sTime[32];

					LcFile_SimpleStrDateTime(sYear, sMon, sDay, sTime, fd.time_write);

					char sT[4096];
					sprintf(sT, "%s %s %s %s  %10d %s %s", sYear, sMon, sDay, sTime, fd.size, sPath, fd.name);
					pOut->push_back(sT);
				}
			}
		}

		result=_findnext(handle,&fd);

	}

	_findclose(handle);

}




void LcFile_FindSameExt(vector<string >* pOut, char* sPath, char* sExt)
{
	char		sSearchPath[2048];
	_finddata_t	fd;
	long		handle;
	int			result=1;


	char		tPath[1024]={0};
	if(sPath)
	{
		strcpy(tPath, sPath);
	}

	int iLen = strlen(tPath);

	if(iLen>0 && ('/' == tPath[iLen-1] || '\\' == tPath[iLen-1]))
	{
		tPath[iLen-1] =0;
	}


	sprintf(sSearchPath,"%s/*.*", tPath);
	handle=_findfirst(sSearchPath,&fd);

	if (handle == -1)
		return;

	while (result != -1)
	{
		if(fd.name[0] !='.')
		{
			if((fd.attrib & _A_SUBDIR))
			{
				char sSearchFolder[1024]={0};

				sprintf(sSearchFolder, "%s/%s", tPath, fd.name);
				LcFile_FindSameExt(pOut, sSearchFolder, sExt);
			}

			else
			{
				char* p = strrchr(fd.name, '.');

				if(0== _stricmp(p, sExt))
				{
					char sT[1024]={0};
					sprintf(sT, "%s/%s", tPath, fd.name);
					pOut->push_back(sT);
				}
			}
		}

		result=_findnext(handle,&fd);

	}

	_findclose(handle);

}






INT LcFile_FileFind(char* sFile)
{
	HANDLE hFile = CreateFile( sFile, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL );

    if( INVALID_HANDLE_VALUE != hFile )
    {
        CloseHandle( hFile );
        return 1;
    }

	return -1;
}



// 파일이 없을 경우 -1을 리턴한다.
INT LcFile_FileSize(char* sFile)
{
	int				hr;
	struct _stat	st;

	hr= _stat(sFile, &st );

	if(!hr)
	{
//		File size: st.st_size
//		Drive    : st.st_dev + 'A'
//		Modified : ctime( &st.st_atime)
		return st.st_size;
	}

	return -1;
}




long LcFile_FileSize(FILE* &fp)
{
	long lCur = ftell(fp);
	long lSize =0;
	long lB =0, lE =0;

	fseek(fp, 0, SEEK_SET);		lB = ftell(fp);
	fseek(fp, 0, SEEK_END);		lE = ftell(fp);

	lSize = lE-lB;

	fseek(fp, lCur, SEEK_SET);

	return lSize;
}





DWORD LcFile_GetAttrib(char* sFile)
{
	_finddata_t	fd;
	long		handle;
	int			result=1;
	DWORD		dAtt=0;

	handle=_findfirst(sFile,&fd);

	if (handle == -1)
		return 0;


	dAtt = ('.' == fd.name[0])? 0 : dAtt = fd.attrib;

	_findclose(handle);

	return dAtt;
}



INT LcFile_GetEntry(TfileEntry* pData, char* sFullPathFile)
{
	_finddata_t	fd;
	long		handle;
	int			result=1;

	handle=_findfirst(sFullPathFile,&fd);

	if (handle == -1)
		return -1;


	char sYear[32];
	char sMon[32];
	char sDay[32];
	char sTime[32];
	char* pStr;

	memset(pData, 0, sizeof TfileEntry);


	LcFile_SimpleStrDateTime(sYear, sMon, sDay, sTime, fd.time_write);


	pData->dAtt = fd.attrib;
	pData->dLen = fd.size;

	pData->iYea	= atoi(sYear);
	pData->iMon	= atoi(sMon);
	pData->iDay	= atoi(sDay);

	pData->iHou = atoi(sTime+0);
	pData->iMin = atoi(sTime+3);
	pData->iSec = atoi(sTime+5);

	strcpy(pData->name, fd.name);
	pStr = strstr(sFullPathFile, pData->name);

	int	iLen = pStr - sFullPathFile;

	strncpy(pData->path, sFullPathFile, iLen);

	if(iLen)
		pData->path[iLen-1] =0;

	_findclose(handle);

	return 1;
}




void LcFile_GetDrive( char const *pPath, char *pDrive)
{
	if(pPath == NULL || pDrive == NULL)
		return;

	_splitpath (pPath, pDrive, NULL, NULL, NULL);
}

void LcFile_GetDir(char const *pPath, char *pDir)
{
	if(pPath == NULL || pDir == NULL)
		return;

	_splitpath (pPath, NULL, pDir, NULL, NULL);

}

void LcFile_GetName(char const *pPath, char *pName)
{
	if(pPath == NULL || pName == NULL)
		return;

	_splitpath (pPath, NULL, NULL, pName, NULL);
}

void LcFile_GetExt(char const *pPath, char *pExt)
{
	if(pPath == NULL || pExt == NULL)
		return;

	_splitpath (pPath, NULL, NULL, NULL, pExt);
}


void LcFile_GetDriveAndDir(char const *pPath, char *pDriveDir)
{
	char Drive[MAX_PATH];
	char Dir[MAX_PATH];

	if(pPath == NULL || pDriveDir == NULL)
		return;

	_splitpath (pPath, Drive, Dir, NULL, NULL);
	_makepath (pDriveDir, Drive, Dir, NULL, NULL);
}

void LcFile_GetNameAndExt(char const *pPath, char *pNameExt)
{
	char Name[MAX_PATH];
	char Ext[MAX_PATH];

	if(pPath == NULL || pNameExt == NULL)
		return;

	_splitpath (pPath, NULL, NULL, Name, Ext);
	_makepath (pNameExt, NULL, NULL, Name, Ext);
}

void LcFile_SetExt(char const *pSrcFile, char const *pExt, char *pDestFile)
{
	char Drive[MAX_PATH];
	char Dir[MAX_PATH];
	char Name[MAX_PATH];

	if(pSrcFile == NULL || pExt == NULL || pDestFile == NULL)
		return;

	_splitpath (pSrcFile, Drive, Dir, Name, NULL);
	_makepath (pDestFile, Drive, Dir, Name, pExt);
}




INT LcFile_XCopy(char *pSrc, char *pDst)
{
	INT				hr=TRUE;
	HANDLE			hSrch;
	WIN32_FIND_DATA wfd;

	char sFileName[MAX_PATH];
	char WildCard[MAX_PATH];
	char sSrcFile[MAX_PATH];
	char sDstFile[MAX_PATH];

	strcpy(WildCard,pSrc);
	strcat(WildCard, "/*.*");


	CreateDirectory(pDst,NULL);

	hSrch=FindFirstFile(WildCard,&wfd);

	if (hSrch == INVALID_HANDLE_VALUE)
		return 0;

	while (hr)
	{
		wsprintf(sFileName,"%s/%s",pSrc,wfd.cFileName);

		// 서브 디렉토리가 발견되면 서브 디렉토리를 복사 한다.

		if (wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{
			if (wfd.cFileName[0]!='.')
			{
				wsprintf(sSrcFile,"%s/%s",pSrc,wfd.cFileName);
				wsprintf(sDstFile,"%s/%s",pDst,wfd.cFileName);
				LcFile_XCopy(sSrcFile, sDstFile);
			}
		}
		else
		{
			wsprintf(sDstFile,"%s/%s",pDst,wfd.cFileName);
			CopyFile(sFileName,sDstFile,FALSE);
		}

		hr=FindNextFile(hSrch,&wfd);
	}

	FindClose(hSrch);

	return 1;
}



INT LcFile_RemoveAll(char *pSrc, char *pDst)
{
	INT				hr=TRUE;
	HANDLE			hSrch;
	WIN32_FIND_DATA wfd;

	char sFileName[MAX_PATH];
	char WildCard[MAX_PATH];
	char sSrcFile[MAX_PATH];
	char sDstFile[MAX_PATH];

	strcpy(WildCard,pSrc);
	strcat(WildCard, "/*.*");


	hSrch=FindFirstFile(WildCard,&wfd);

	if (hSrch == INVALID_HANDLE_VALUE)
		return 0;

	while (hr)
	{
		wsprintf(sFileName,"%s/%s",pSrc, wfd.cFileName);

		if (wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{
			if (wfd.cFileName[0]!='.')
			{
				memset(sSrcFile, 0, sizeof sDstFile);
				memset(sDstFile, 0, sizeof sDstFile);
				wsprintf(sSrcFile,"%s/%s",pSrc,wfd.cFileName);
				wsprintf(sDstFile,"%s/%s",pDst,wfd.cFileName);
				LcFile_RemoveAll(sSrcFile, sDstFile);
			}
		}
		else
		{
			memset(sDstFile, 0, sizeof sDstFile);
			wsprintf(sDstFile,"%s/%s",pDst,wfd.cFileName);
			hr = DeleteFile(sFileName);
		}


		hr=FindNextFile(hSrch,&wfd);
	}

	FindClose(hSrch);
	RemoveDirectory(pSrc);

	return 1;
}





INT LcFile_CmtlFileOpen(														// Common Control File Open
							HWND hWnd											// HWND Owner
						,	char* sFileFull										// File Full Path Name
						,	char* sFilePath										// File Path
						,	char* sFileName										// File Name
						,	char* sName											// Name
						,	char* sFileExtn										// File Extension
						,	CONST char* sFilter									// Filter
						,	CONST char* sDefExt									// Default Extension
						,	CONST char* sTitle									// Common Control Title
						,	CONST char* sInitPath								// Init Path
						)
{
	OPENFILENAME OFN={0};

	char	sCurrentDir[MAX_PATH];
	char	szFileName[MAX_PATH];

	char	sFileAll[MAX_PATH]={0};
	char	InitDir[MAX_PATH];

	GetCurrentDirectory(MAX_PATH, sCurrentDir);

	if(NULL == sInitPath)
		sprintf(InitDir, "%s", sCurrentDir);

	else if(0 == _stricmp(sInitPath, "system"))
	{
		GetSystemDirectory(InitDir, sizeof InitDir);
	}
	else if(0 == _stricmp(sInitPath, "windows"))
	{
		GetWindowsDirectory(InitDir, sizeof InitDir);
	}
	else
	{
		memset(InitDir, 0, sizeof InitDir);
		strcpy(InitDir, sInitPath);
	}

	OFN.lStructSize		= sizeof(OPENFILENAME);
	OFN.hwndOwner		= hWnd;
	OFN.lpstrFilter		= sFilter;
	OFN.lpstrFile		= sFileAll;
	OFN.nMaxFile		= MAX_PATH;

	OFN.lpstrTitle		= sTitle;
	OFN.lpstrFileTitle	= szFileName;
	OFN.nMaxFileTitle	= MAX_PATH;
	OFN.lpstrDefExt		= sDefExt;
	OFN.lpstrInitialDir = InitDir;

	if( !GetOpenFileName(&OFN))
	{
		SetCurrentDirectory(sCurrentDir);
		return -1;
	}

	if(!strlen(OFN.lpstrFile))
	{
		SetCurrentDirectory(sCurrentDir);
		return -1;
	}


	SetCurrentDirectory(sCurrentDir);

	if(sFileFull)
		strcpy(sFileFull, sFileAll);

	if(sFilePath)
	{
		strcpy(sFilePath, sFileAll);

		int iLen = strlen(sFilePath);

		for(INT i=iLen; i>0; --i)
		{
			if('\\' == sFilePath[i] || '/' == sFilePath[i])
				break;

			sFilePath[i] = 0;
		}
	}

	if(sFileName)
	{
		strcpy(sFileName, szFileName);
	}

	// 파일명 옵셋: OFN.nFileOffset
	// 확장자 옵셋:	OFN.nFileExtension;

	if(sName)
	{
		sName[0] = 0;

		if(OFN.nFileOffset)
		{
			strcpy(sName, sFileAll + OFN.nFileOffset);
		}

		if(OFN.nFileExtension)
		{
			*(sName + OFN.nFileExtension-OFN.nFileOffset-1) = 0;
		}
	}

	if(sFileExtn)
	{
		sFileExtn[0] = 0;

		if(OFN.nFileExtension)
			strcpy(sFileExtn, sFileAll + OFN.nFileExtension);
	}

	return 1;
}





char* LcFile_SimpleStrDateTime(char* sYear, char* sMon, char* sDay, char* sTime, time_t lTime)
{
	static char sTimeA[256];

	char C[64];
	char Y[16];
	char M[16];
	char D[16];
	char T[32];

	strcpy(C, ctime( &lTime ) );
	sscanf(C, "%*s %*s %*s %s", T);

	strftime( C, sizeof C, "%Y %m %d", gmtime(&lTime) );
	sscanf(C, "%s %s %s", Y, M, D);


	if(sYear)
		strcpy(sYear, Y);

	if(sMon)
		strcpy(sMon, M);

	if(sDay)
		strcpy(sDay, D);

	if(sTime)
		strcpy(sTime, T);


	memset(sTimeA, 0, sizeof sTimeA);
	sprintf(sTimeA, "%s %s %s %s", Y, M, D, T);

	return sTimeA;
}






HANDLE LcFile_fopen(char* sFile, char* sT)
{
	// rt, rb, wt, wb만 지원하자.

	HANDLE hFile = NULL;

	DWORD					dAccess		= 0;
	DWORD					dShare		= 0;
	LPSECURITY_ATTRIBUTES	pSecurity	= NULL;
	DWORD					dDispostion	= 0;
	DWORD					dFlagsAtt	= 0;
	HANDLE					hTempFile	= NULL;


	char sUsage[4];

	strcpy(sUsage, sT);

	strcat(sUsage, " ");

	// 1. Desired Access
	if('r' == sUsage[0])		dAccess = GENERIC_READ;
	else if('w' == sUsage[0])	dAccess = GENERIC_WRITE;
	else if('a' == sUsage[0])	dAccess = GENERIC_READ | GENERIC_WRITE;


	// 2. Share Mode
	if('r' == sUsage[0])		dShare = FILE_SHARE_READ;
	else if('w' == sUsage[0])	dShare = FILE_SHARE_WRITE;
	else if('a' == sUsage[0])	dShare = FILE_SHARE_READ | FILE_SHARE_WRITE;

	//3. Security Attributes
	pSecurity=NULL;

	// 4. Creation Deposition
	if(GENERIC_READ == dAccess)							dDispostion = OPEN_EXISTING;
	else if(GENERIC_WRITE == dAccess)					dDispostion = CREATE_ALWAYS;
	else if((GENERIC_READ | GENERIC_WRITE) == dAccess)	dDispostion = OPEN_ALWAYS;


	// 5. Flags and Attributes
	dFlagsAtt = FILE_ATTRIBUTE_NORMAL;	// The file has no other attributes set.

	// 6. Template File
	hTempFile = NULL;

	hFile = CreateFile(
			sFile						// pointer to name of the file
		,	dAccess						// access (read-write) mode
		,	dShare						// share mode
		,	pSecurity					// pointer to security attributes
		,	dDispostion					// how to create
		,	dFlagsAtt					// file attributes
		,	hTempFile					// handle to file with attributes to  copy
		);

	if(INVALID_HANDLE_VALUE == hFile)
		return hFile;

	// Append Mode이면 파일 포인터를 끝으로 옮긴다.
	if('a' == sUsage[0] && '+' == sUsage[1])
		LcFile_fseekEnd(hFile);

	return hFile;
}


INT	LcFile_fclose(HANDLE hFile)
{
	if(NULL == hFile)
		return -1;

	LcFile_feof();

	return CloseHandle(hFile) ? 1: -1;
}


DWORD LcFile_fsize(HANDLE hFile)
{
	DWORD size= ::GetFileSize(hFile,NULL);

	return size;
}


INT	LcFile_fread(HANDLE hFile, void* p, INT bufSize, INT nCount)
{
	DWORD iSize = bufSize * nCount;
	DWORD dRead=0;
	DWORD dError;

	BOOL hr = ReadFile(hFile, p, iSize, &dRead, NULL);

	if(0==hr)
	{
		// hr==-1 is End of File
		dError = GetLastError();

		if(NO_ERROR != dError)
		{
			LcFile_Err(dError);
		}
	}

	return 0==hr ? -1: dRead;
}



INT	LcFile_fwrite(HANDLE hFile, void* p, INT bufSize, INT nCount)
{
	DWORD iSize = bufSize * nCount;
	DWORD dWritten=0;
	DWORD dError;

	INT hr = WriteFile(hFile, p, iSize, &dWritten, NULL);

	if(0==hr)
	{
		// hr==-1 is End of File
		dError = GetLastError();

		if(NO_ERROR != dError)
		{
			LcFile_Err(dError);
		}
	}

	return 0==hr ? -1: dWritten;
}


INT LcFile_fprintf(HANDLE hFile, char *format,...)
{
	va_list ap;
	char s[2048];

	if (NULL == format)
		return -1;

	va_start(ap, format);
	vsprintf((char *)s, format, ap);
	va_end(ap);


	DWORD dWritten=0;
	DWORD iSize = strlen(s);
	INT hr = WriteFile(hFile, s, iSize, &dWritten, NULL);

	return 0==hr ? -1: dWritten;
}


INT LcFile_fgets(HANDLE hFile, char* sLine, int iMax, BOOL bFillEndLineZero)
{
	INT		hr = 0;

	INT		i=0;
	INT		nTotal=0;
	DWORD	dRead=0;

	for(i=0; i<iMax; ++i)
	{
		hr = ReadFile(hFile, &sLine[i], 1, &dRead, NULL);

		nTotal +=dRead;

		// Fail
		if(0==hr)
		{
			hr = LcFile_Err();
			hr = -1;
			break;
		}

		// End of File
		if (hr && 0== dRead)
		{
			hr = -1;
			break;
		}

		// End of Line
//		if('\n' == sLine[i] || '\r' == sLine[i] || '\n\r' == sLine[i] || '\r\n' == sLine[i])
		// window의 텍스트 파일은 '\r\n'값이 들어오더라...
		if('\n' == sLine[i])
		{
			hr = 1;
			sLine[i+1] =0;
			break;
		}
	}

	if(bFillEndLineZero)
	{
		if(i>0)
		{
			if('\r' == sLine[i-1])
				sLine[i-1] =0;
			else
				sLine[i] =0;
		}
	}

	return nTotal? nTotal*hr: hr;
}

DWORD LcFile_fseek(HANDLE hFile, LONG lOff, DWORD nFrom/*FILE_BEGIN, FILE_CURRENT, FILE_END*/)
{
	DWORD dError;
	DWORD dFilePos = ::SetFilePointer(hFile, lOff, NULL, nFrom);

	if(0xFFFFFFFF ==dFilePos)
	{
		dError = GetLastError();

		if(NO_ERROR != dError)
			LcFile_Err(dError);
	}


	return dFilePos;
}

DWORD	LcFile_fseekBegin(HANDLE hFile)
{
	return LcFile_fseek(hFile, 0, FILE_BEGIN);
}

DWORD	LcFile_fseekEnd(HANDLE hFile)
{
	return LcFile_fseek(hFile, 0, FILE_END);
}



void LcFile_fsetLength(HANDLE hFile, DWORD dwNewLen)
{
	DWORD dError=0;
	LcFile_fseek(hFile, (LONG)dwNewLen, FILE_BEGIN);

	if(!::SetEndOfFile(hFile))
	{
		dError = GetLastError();

		if(NO_ERROR != dError)
			LcFile_Err(dError);
	}

}

DWORD LcFile_fgetLength(HANDLE hFile)
{
	DWORD dwLen, dwCur;
	dwCur = LcFile_ftell(hFile);
	dwLen = LcFile_fseekEnd(hFile);

	LcFile_fseek(hFile, dwCur, FILE_BEGIN);

	return dwLen;
}


DWORD LcFile_ftell(HANDLE hFile)
{
	DWORD dError;
	DWORD dFilePos = ::SetFilePointer(hFile, 0, NULL, FILE_CURRENT);

	if(0xFFFFFFFF ==dFilePos)
	{
		dError = GetLastError();

		if(NO_ERROR != dError)
		{
			LcFile_Err(dError);
		}
	}


	return dFilePos;
}


BOOL LcFile_feof(HANDLE hFile)
{
	static HANDLE	hFtmp = INVALID_HANDLE_VALUE;
	static DWORD	dFsize = 0;

	if(INVALID_HANDLE_VALUE == hFile)
	{
		hFtmp = INVALID_HANDLE_VALUE;
		dFsize = 0;
		return FALSE;
	}

	if(hFtmp != hFile)
	{
		hFtmp	= hFile;
		dFsize = ::GetFileSize(hFile,NULL);
	}

	DWORD dFilePos = ::SetFilePointer(hFile, 0, NULL, FILE_CURRENT);

	if(dFilePos>=dFsize)
		return TRUE;

	return FALSE;
}







void	LcFile_ReadFileLine(HANDLE hFile, char* sLine)
{
	static char sTmp[512];

	INT nSize;
	INT nBgn, nEnd;
	INT i;

	memset(sTmp,0, sizeof(sTmp));
	LcFile_fgets(hFile, sTmp, 512);

	nSize = strlen(sTmp);

	for(i=0; i<nSize; ++i)
	{
		if('\t' == sTmp[i] || ',' == sTmp[i])
			sTmp[i] =' ';

		if('\n' == sTmp[i] || '\r' == sTmp[i])
			sTmp[i] = 0;
	}

	nBgn=0;
	nEnd= strlen(sTmp);

	if(0 == nEnd)
	{
		sLine[0] =0;
		return;
	}

	for(i=0; i<=nEnd; ++i)
	{
		if(' ' != sTmp[i])
		{
			nBgn =i;
			break;
		}
	}

	strncpy(sLine, sTmp + nBgn, nEnd-nBgn);
	sLine[nEnd-nBgn] = 0;
}




void LcFile_ReadFileLine(FILE *fp, char* sLine)
{
	static char sTmp[512];

	INT nSize;
	INT nBgn, nEnd;
	INT i;

	memset(sTmp,0, sizeof(sTmp));
	fgets(sTmp, 512, fp);

	nSize = strlen(sTmp);

	for(i=0; i<nSize; ++i)
	{
		if('\t' == sTmp[i] || ',' == sTmp[i])
			sTmp[i] =' ';

		if('\n' == sTmp[i] || '\r' == sTmp[i])
			sTmp[i] = 0;
	}

	nBgn=0;
	nEnd= strlen(sTmp);

	if(0 == nEnd)
	{
		sLine[0] =0;
		return;
	}

	for(i=0; i<=nEnd; ++i)
	{
		if(' ' != sTmp[i])
		{
			nBgn =i;
			break;
		}
	}

	strncpy(sLine, sTmp + nBgn, nEnd-nBgn);
	sLine[nEnd-nBgn] = 0;
}


