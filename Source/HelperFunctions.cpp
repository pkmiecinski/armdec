#include "HelperFunctions.h"


WRes MyCreateDir(const UInt16* name)
{
#ifdef USE_WINDOWS_FILE

	return CreateDirectoryW((LPCWSTR)name, NULL) ? 0 : GetLastError();

#else

	CBuf buf;
	WRes res;
	Buf_Init(&buf);
	RINOK(Utf16_To_Char(&buf, name MY_FILE_CODE_PAGE_PARAM));

	res =
#ifdef _WIN32
		_mkdir((const char*)buf.data)
#else
		mkdir((const char*)buf.data, 0777)
#endif
		== 0 ? 0 : errno;
	Buf_Free(&buf, &g_Alloc);
	return res;

#endif
}

WRes OutFile_OpenUtf16(CSzFile* p, const UInt16* name)
{
#ifdef USE_WINDOWS_FILE
	return OutFile_OpenW(p, (LPCWSTR)name);
#else
	CBuf buf;
	WRes res;
	Buf_Init(&buf);
	RINOK(Utf16_To_Char(&buf, name MY_FILE_CODE_PAGE_PARAM));
	res = OutFile_Open(p, (const char*)buf.data);
	Buf_Free(&buf, &g_Alloc);
	return res;
#endif
}


/*
int Buf_EnsureSize(CBuf* dest, size_t size)
{
	if (dest->size >= size)
		return 1;
	Buf_Free(dest, &g_Alloc);
	return Buf_Create(dest, size, &g_Alloc);
}

SRes Utf16_To_Char(CBuf* buf, const UInt16* s
#ifndef _USE_UTF8
	, UINT codePage
#endif
)
{
	unsigned len = 0;
	for (len = 0; s[len] != 0; len++);

#ifndef _USE_UTF8
	{
		unsigned size = len * 3 + 100;
		if (!Buf_EnsureSize(buf, size))
			return SZ_ERROR_MEM;
		{
			buf->data[0] = 0;
			if (len != 0)
			{
				char defaultChar = '_';
				BOOL defUsed;
				unsigned numChars = 0;
				numChars = WideCharToMultiByte(codePage, 0, (LPCWSTR)s, len, (char*)buf->data, size, &defaultChar, &defUsed);
				if (numChars == 0 || numChars >= size)
					return SZ_ERROR_FAIL;
				buf->data[numChars] = 0;
			}
			return SZ_OK;
		}
	}
#else
	return Utf16_To_Utf8Buf(buf, s, len);
#endif
}




void GetAttribString(UInt32 wa, BoolInt isDir, char* s)
{
#ifdef USE_WINDOWS_FILE
	s[0] = (char)(((wa & FILE_ATTRIBUTE_DIRECTORY) != 0 || isDir) ? 'D' : '.');
	s[1] = (char)(((wa & FILE_ATTRIBUTE_READONLY) != 0) ? 'R' : '.');
	s[2] = (char)(((wa & FILE_ATTRIBUTE_HIDDEN) != 0) ? 'H' : '.');
	s[3] = (char)(((wa & FILE_ATTRIBUTE_SYSTEM) != 0) ? 'S' : '.');
	s[4] = (char)(((wa & FILE_ATTRIBUTE_ARCHIVE) != 0) ? 'A' : '.');
	s[5] = 0;
#else
	s[0] = (char)(((wa & (1 << 4)) != 0 || isDir) ? 'D' : '.');
	s[1] = 0;
#endif
}

void UInt64ToStr(UInt64 value, char* s, int numDigits)
{
	char temp[32];
	int pos = 0;
	do
	{
		temp[pos++] = (char)('0' + (unsigned)(value % 10));
		value /= 10;
	} while (value != 0);

	for (numDigits -= pos; numDigits > 0; numDigits--)
		*s++ = ' ';

	do
		*s++ = temp[--pos];
	while (pos);
	*s = '\0';
}

char* UIntToStr(char* s, unsigned value, int numDigits)
{
	char temp[16];
	int pos = 0;
	do
		temp[pos++] = (char)('0' + (value % 10));
	while (value /= 10);

	for (numDigits -= pos; numDigits > 0; numDigits--)
		*s++ = '0';

	do
		*s++ = temp[--pos];
	while (pos);
	*s = '\0';
	return s;
}

 void UIntToStr_2(char* s, unsigned value)
{
	s[0] = (char)('0' + (value / 10));
	s[1] = (char)('0' + (value % 10));
}
*/