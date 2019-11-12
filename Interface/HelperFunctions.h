
#include "7z.h"
#include "7zAlloc.h"
#include "7zFile.h"
#include "7zCrc.h"
#include "7zBuf.h"



WRes MyCreateDir(const UInt16* name);

WRes OutFile_OpenUtf16(CSzFile* p, const UInt16* name);

/*
int Buf_EnsureSize(CBuf* dest, size_t size);
SRes Utf16_To_Char(CBuf* buf, const UInt16* s
#ifndef _USE_UTF8
	, UINT codePage
#endif
);
void GetAttribString(UInt32 wa, BoolInt isDir, char* s);
void UInt64ToStr(UInt64 value, char* s, int numDigits);
char* UIntToStr(char* s, unsigned value, int numDigits);
void UIntToStr_2(char* s, unsigned value);
*/