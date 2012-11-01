#include "MyLog.h"

int main(int argc, char *argv[])
{
	if( argc < 3 )
	{
		printf( "Usage : hexdump [Source File] [Dest File]\n" );
		return 0;
	}

	FILE *fpSource = NULL;
	FILE *fpDest = NULL;

	fpSource = fopen( argv[1], "r" );
	if( fpSource == NULL )
	{
		printf( "Source File open error\n" );
		return 0;
	}

/*	fpDest = fopen( argv[2], "wt+" );
	if( fpDest == NULL )
	{
		printf( "Dest File open error\n" );
		return 0;
	}
*/
	// MyLog 객체 생성
	MyLog Log;
	Log.open((char*)"./", argv[2]);

	while( !feof(fpSource) )
	{
		// 한 라인을 담을 임시 버퍼
		char szTemp[1024] = "\0";

		size_t nRead = fread( szTemp, 1, sizeof(szTemp), fpSource );
		if( nRead <= 0 )
			break;

		Log.hexdump( LVL_DEBUG, szTemp, strlen(szTemp) );
	}

	// Release
	fclose( fpSource );
	Log.close();

	return 0;
}
