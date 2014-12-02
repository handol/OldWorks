// utfconv.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "stdlib.h"
#include "windows.h"
//#include "iconv.h"


int Utf8ToANSI(char* ToCode)
{

// BSTR wide_char;
	 char* wide_char;
 char* multi_char;
 int length;

 

// Get length of the Wide Char buffer
 length = MultiByteToWideChar(
      CP_UTF8, 
      0, 
      ToCode, 
      strlen(ToCode)+1,   
      NULL,     
      NULL    
      );

 wide_char = SysAllocStringLen(NULL, length);

 

// Change UTF-8 to Unicode (UTF-16)

 MultiByteToWideChar(
      CP_UTF8, 
      0, 
      ToCode, 
      strlen(ToCode)+1,  
      wide_char, 
      length    
      );

 

// Get length of the multi byte buffer 

 length = WideCharToMultiByte(
      CP_ACP, 
      0, 
      wide_char, 
      -1, 
      NULL, 
      0, 
      NULL, 
      NULL
      );

 

 multi_char = (char*)malloc(length);

 

// Change from unicode to mult byte

 WideCharToMultiByte(
      CP_ACP, 
      0, 
      wide_char, 
      -1, 
      multi_char, 
      length,    

      NULL, 
      NULL
      );

 

 memset(ToCode, 0x00, sizeof(ToCode));
 strncpy(ToCode, multi_char, length);

 return 0;
}



int  decode_utf8( LPCSTR utf8str )
{
int size = MultiByteToWideChar(CP_UTF8, 0, utf8str, -1, NULL, 0);

LPWSTR wStr = new WCHAR[size];

MultiByteToWideChar(CP_UTF8, 0, utf8str, -1, wStr, size);

USES_CONVERSION;

printf("%s\n", wStr);
//CString str = W2CT(wStr);

delete[] wStr;

//return str;
}

int main(int argc, char* argv[])
{
	printf("Hello World!\n");
	return 0;
}

