#include <stdio.h>
#include <string.h>

#include "7z.h"
#include "7zAlloc.h"
#include "7zFile.h"
#include "7zCrc.h"
#include "7zBuf.h"
#include "HelperFunctions.h"

#define ARCHNAME "TestPackFoldersNoPass.7z"
#define kInputBufSize ((size_t)1 << 18)

#define ENABLE_EXTRACT_LOGS  1 

static const ISzAlloc g_Alloc = { SzAlloc, SzFree };


enum _eDecFileNotFound
{
	eDecOk,
	eDecFileNotFound

}DecRetVal;



int decompress(char* archName)
{
	ISzAlloc allocImp;
	ISzAlloc allocTempImp;

	allocImp = g_Alloc;
	allocTempImp = g_Alloc;

	CFileInStream archiveStream;
	CLookToRead2 lookStream;

	SRes res;
	CSzArEx db;
	UInt16* temp = NULL;
	size_t tempSize = 0;

	UInt32 blockIndex = 0xFFFFFFFF; /* it can have any value before first call (if outBuffer = 0) */
	Byte* outBuffer = 0; /* it must be 0 before first call for each new archive. */
	size_t outBufferSize = 0;  /* it can have any value before first call (if outBuffer = 0) */


	if (InFile_Open(&archiveStream.file, archName))
	{
		printf("E: Cannot find archive file\n");
		return eDecFileNotFound;
	}

	FileInStream_CreateVTable(&archiveStream);
	LookToRead2_CreateVTable(&lookStream, False);

	lookStream.buf = NULL;

	res = SZ_OK;

	{
		lookStream.buf = (Byte*)ISzAlloc_Alloc(&allocImp, kInputBufSize);
		if (!lookStream.buf)
			res = SZ_ERROR_MEM;
		else
		{
			lookStream.bufSize = kInputBufSize;
			lookStream.realStream = &archiveStream.vt;
			LookToRead2_Init(&lookStream);
		}
	}

	CrcGenerateTable();

	SzArEx_Init(&db);

	if (res == SZ_OK)
	{
		res = SzArEx_Open(&db, &lookStream.vt, &allocImp, &allocTempImp);
	}

	if (res == SZ_OK)
	{
		UInt32 i;

		UInt32 blockIndex = 0xFFFFFFFF; /* it can have any value before first call (if outBuffer = 0) */
		Byte* outBuffer = 0; /* it must be 0 before first call for each new archive. */
		size_t outBufferSize = 0;  /* it can have any value before first call (if outBuffer = 0) */

		for (i = 0; i < db.NumFiles; i++)
		{
			size_t offset = 0;
			size_t outSizeProcessed = 0;
			// const CSzFileItem *f = db.Files + i;
			size_t len;
			unsigned isDir = SzArEx_IsDir(&db, i);

			len = SzArEx_GetFileNameUtf16(&db, i, NULL);

			if (len > tempSize)
			{
				SzFree(NULL, temp);
				tempSize = len;
				temp = (UInt16*)SzAlloc(NULL, tempSize * sizeof(temp[0]));
				if (!temp)
				{
					res = SZ_ERROR_MEM;
					break;
				}
			}

			SzArEx_GetFileNameUtf16(&db, i, temp);

			res = SzArEx_Extract(&db, &lookStream.vt, i,
					&blockIndex, &outBuffer, &outBufferSize,
					&offset, &outSizeProcessed,
					&allocImp, &allocTempImp);

			CSzFile outFile;
			size_t processedSize;
			size_t j;
			UInt16* name = (UInt16*)temp;
			const UInt16* destPath = (const UInt16*)name;

			for (j = 0; name[j] != 0; j++)
				if (name[j] == '/')
				{
					name[j] = 0;
					MyCreateDir(name);
					name[j] = CHAR_PATH_SEPARATOR;

				}

			if (isDir)
			{
				MyCreateDir(destPath);
				continue;
			}
			else if (OutFile_OpenUtf16(&outFile, destPath))
			{
				printf("E: can not open output file\n");
				res = SZ_ERROR_FAIL;
				break;
			}

			processedSize = outSizeProcessed;

			if (File_Write(&outFile, outBuffer + offset, &processedSize) != 0 || processedSize != outSizeProcessed)
			{
				printf("E: can not write output file\n");
				res = SZ_ERROR_FAIL;
				break;
			}

			if (File_Close(&outFile))
			{
				printf("E: can not close output file\n");
				res = SZ_ERROR_FAIL;
				break;
			}
			
		}

		ISzAlloc_Free(&allocImp, outBuffer);
		SzFree(NULL, temp);
		SzArEx_Free(&db, &allocImp);
		ISzAlloc_Free(&allocImp, lookStream.buf);

		return eDecOk;
	}
}


int  main(int argc, char* argv[])
{
	decompress((char*)ARCHNAME);
}