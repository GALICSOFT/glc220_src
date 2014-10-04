// Implementation of the LcFile class.
//
////////////////////////////////////////////////////////////////////////////////

#if defined(_MSC_VER) && _MSC_VER >= 1400
  #pragma warning(disable: 4996)
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <LcType.h>

#include "LcAux.h"
#include "LcBase.h"


class CLcFile : public ILC_File
{
protected:
	enum
	{
		RSC_FILE =0,
		RSC_BUF	   ,

		IO_R   = 0x1,
		IO_W   = 0x2,
		IO_RW  = 0x3,
	};

	INT		m_Type;			// Type

	FILE*	m_Fp;			// file pointer
	char*	m_Fmode;		// F mode

	char*	m_Bgn;			// Begin pointer of buffer
	char*	m_Eoz;			// End of z point
	char*	m_Cur;			// Current pointer

	INT		m_Size;			// buffer size
	INT		m_Mode;			// Open Mode


public:
	CLcFile();
	virtual ~CLcFile();

	virtual INT		Query(char* sCmd, LC_HANDLE pData);
	virtual INT		Release();

	virtual INT		ReadByte (char* buf, INT size);
	virtual INT		WriteByte(char* buf, INT size);
	virtual INT		ReadLine(char* buf, INT size);
	virtual INT		Seek(INT offset, INT origin);

	virtual	INT		GetSize()	{	return m_Size;	}
	virtual INT		GetMode()	{	return m_Mode;	}

	virtual INT		Tell()
	{
		if(RSC_FILE == m_Type)
			return (INT)ftell(m_Fp);

		return (INT)( (L_PTR)(m_Cur - m_Bgn) );
	}

	virtual INT		Eof()
	{
		if(RSC_FILE == m_Type)
			return feof(m_Fp);

		return m_Cur >= m_Eoz;
	}


public:
	INT		Create(LC_HANDLE sFile, INT size, char* mode);
	void	Destroy();

};


CLcFile::CLcFile()
{
	m_Type	= -1;

	m_Fp	= NULL;
	m_Fmode	= NULL;

	m_Bgn	= NULL;
	m_Eoz	= NULL;
	m_Size	= 0;
	m_Mode	= -1;
	m_Cur	= NULL;
}


CLcFile::~CLcFile()
{
	Destroy();
}


INT CLcFile::Query(char* sCmd, LC_HANDLE pData)
{
	return LC_EFAIL;
}


INT CLcFile::Release()
{
	INT hr = LC_OK;

	delete this;

	// AFEW: Decrease reference count.
	hr = LcVar_DecRefCountObject();

	return hr;
}


void CLcFile::Destroy()
{
	if(m_Fp)
	{
		fclose(m_Fp);
		m_Fp = NULL;
	}

	if(m_Type == RSC_BUF && m_Bgn)
		free(m_Bgn);

	m_Bgn	= NULL;
	m_Eoz	= NULL;
	m_Cur	= NULL;

	m_Size	= 0;
	m_Mode	= 0;
}


INT CLcFile::Create(LC_HANDLE sFile, INT size, char* mode)
{
	// save the buffer, size, mode
	m_Bgn	= (char*)sFile;
	m_Size	= size;
	m_Eoz	= m_Bgn + size;
	m_Cur	= m_Bgn;
	m_Fmode	= mode;

	if     (0== m_Size)	m_Type = RSC_FILE;
	else if(0 < m_Size)	m_Type = RSC_BUF;
	else				return -1;

	if(0 >=size && NULL == mode)
		return -1;


	m_Fmode = mode;
	if(0 == LcStr_Stricmp("r+", m_Fmode) || 0 == LcStr_Stricmp("w+", m_Fmode))
		m_Mode = IO_RW;

	else if('w' == m_Fmode[0])
		m_Mode = IO_W;

	else if('r' == m_Fmode[0])
		m_Mode = IO_R;
	else
		return -1;


	// open file or buffer
	if(RSC_FILE == m_Type)
	{
		m_Fp = fopen(m_Bgn, m_Fmode);
		if(NULL == m_Fp)
			return -1;

		// save the file size
		fseek(m_Fp, 0, SEEK_END);
		m_Size = ftell(m_Fp);
		fseek(m_Fp, 0, SEEK_SET);
		
		LOGI("File::Create: %s\n", m_Bgn);
	}
	else if(RSC_BUF == m_Type)
	{
		LOGI("File::Create: %d\n", m_Size);
		if(0 == m_Size)
		{
			LOGI("File::Create::Err:: 0x%p - size: %d\n", m_Bgn, m_Size);
			return -1;
		}
	}

	return 0;
}


INT CLcFile::ReadByte(char* buf, INT size)
{
	if(RSC_FILE == m_Type)
	{
		INT hr = 0;

		hr = fread(buf, 1, size, m_Fp);
		return hr;
	}


	INT remain = (INT)(m_Eoz - m_Cur);

	if(remain<size)
		size = remain;


	if(0>=size)
		return -1;


	memcpy(buf, m_Cur, size);
	m_Cur += size;

	return size;
}


INT CLcFile::WriteByte(char* buf, INT size)
{
	if(RSC_FILE == m_Type)
	{
		INT hr = 0;

		hr =  fwrite(buf, 1, size, m_Fp);
		return hr;
	}

	if(Eof())
		return -1;


	INT remain = (INT)(m_Eoz - m_Cur);

	if(remain<size)
		size = remain;


	if(0>=size)
		return -1;


	memcpy(m_Cur, buf, size);
	m_Cur += size;

	return size;
}


INT CLcFile::ReadLine(char* buf, INT size)
{
	INT isize = size;
	char* rd = m_Cur;
	INT  rdb = -1;

	if(RSC_FILE == m_Type)
	{
		rd = fgets(buf, size, m_Fp);

		if(NULL == rd)
		{
			*buf = 0;
			return rdb;
		}

		for( ; rd<(buf + size); ++rd)
		{
			if('\n' == *rd || '\r' == *rd)
			{
				*rd = '\0';
				break;
			}
		}

		rdb = (INT)(rd - buf);
		return rdb;
	}



	while( --isize && !Eof() )
	{
		if('\r' == *m_Cur)
			++m_Cur;

		if('\n' == *m_Cur)
			break;

		*(buf++) = *(m_Cur++);
	}

	//if('\n' != *m_Cur)
	//{
	//	while( !Eof() )
	//	{
	//		if('\r' == *m_Cur)
	//			++m_Cur;
	//
	//		if('\n' == *m_Cur)
	//			break;
	//
	//		++m_Cur;
	//	}
	//}


	*buf = '\0';
	rdb = (INT)(m_Cur-rd);


	if(Eof() )
		return -1;

	++m_Cur;

	return rdb;
}


INT CLcFile::Seek(INT offset, INT origin)
{
	if(RSC_FILE == m_Type)
		return fseek(m_Fp, offset, origin)? -1: 0;

	if(SEEK_SET == origin)
		m_Cur = m_Bgn + offset;
	else if(SEEK_END == origin)
		m_Cur = m_Bgn + (m_Size + offset);
	else if(SEEK_CUR == origin)
		m_Cur += offset;

	if(Eof() )
		m_Cur = m_Eoz;

	return 0;
}


INT LcFile_CreateFile(char*     sCmd       //
                    , PLC_FILE* pOut		// Output instance
                    , LC_HANDLE sFile		// file name or buffer address
                    , INT       size		// if it'll be used to file size must be zero. buffer then size is greather than zero
                    , char*     mode		// read write mode. w, r, w+, r+ rt, wt, rb, wb. not support the append 'a' or 'a+'
                    )
{
	INT			hr = LC_OK;
	CLcFile*	pObj = NULL;

	*pOut		= NULL;
	pObj		= new CLcFile;

	hr = pObj->Create(sFile, size, mode);
	if(LC_FAILED(hr))
	{
		delete pObj;
		return hr;
	}

	LcVar_IncRefCountObject();
	*pOut = pObj;
	return LC_OK;
}



INT LcFile_Open(char* sCmd, PLC_FILE* pOut, char* sFile, char* sMode)
{
	PLC_FILE pObj = NULL;
	INT		 hr   = -1;

	*pOut = NULL;

	char  sMedia[LC_MAX_PATH2]={0};
	char* tsave = strstr(sFile, (char*)"save");

	hr = LcSys_ResourcePath(sMedia, sFile);
	if(tsave)														// if there is "save" in prefix of file name then copy to document folder to be able to read and write.
	{
		char*	sApp = (char*)LcSys_AppFolder();					// find document folder
		char	Src [LC_MAX_PATH2]={0};

		strcpy(Src, sMedia);										// Copy file name

#if defined(__APPLE__) || defined(__IPHONE__) || defined(__ANDROID__)
		LcStr_ReplacePath(sMedia, sFile, "", sApp, NULL, NULL);		// rename dest file
#endif


		if(LC_FAILED(LcFile_Info(sMedia, NULL, NULL, NULL)))
		{
#if defined(_MSC_VER)
			return LC_CANNT_FIND_RESOURCE;

#elif defined(__APPLE__) || defined(__IPHONE__)
			hr = LcFile_XcopyFileToFile(sMedia, Src);
			if(LC_FAILED(hr))
				LOGE("LscFile::ILhLst::Alloc:: File Copy Err: %s\n", sMedia);

#elif defined(__ANDROID__)
			BYTE*		buf = NULL;
			ULONG		len = 0;
			const char*	sApkFile = LcSys_AndroidApkFile(NULL);

			hr = Lzip_UnzipbyName(&buf, &len, sApkFile, Src, LC_TRUE);
			hr = LcFile_XcopyBufferToFile(sMedia, buf, len);
			if(LC_FAILED(hr))
				LOGE("LscFile::ILhLst::Alloc:: File Copy Err: %s\n", sMedia);
#endif
		}


		hr = LcFile_CreateFile(sCmd, &pObj,sMedia,0,sMode);		// create from file
		if(LC_FAILED(hr))
		{
			LOGE("LcFile_Open Failed:: %s, %s\n", sMedia, sMode);
			return LC_CANNT_FIND_RESOURCE;
		}

		*pOut = pObj;
		return LC_OK;
	}


	BYTE*		buf = NULL;
	ULONG		len = 0;

#if defined(__ANDROID__)
	const char*	sApkFile = LcSys_AndroidApkFile(NULL);

	hr = Lzip_UnzipbyName(&buf, &len, sApkFile, sMedia, LC_TRUE);
	if(LC_FAILED(hr))
		return LC_CANNT_FIND_RESOURCE;

#else
	buf = (BYTE*)sMedia;
#endif


	LOGI("LcFile_Open:: %s\n", sFile);
	hr = LcFile_CreateFile(sCmd, &pObj, buf, len, sMode);
	if(LC_FAILED(hr))
		return LC_CANNT_FIND_RESOURCE;

	*pOut = pObj;
	return LC_OK;
}
