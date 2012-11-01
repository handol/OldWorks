#include "dnslookup.h"

extern "C" void sig_SIGSTOP(int arg);
extern "C" void sig_SIGUSR1(int arg);

bool run = true;
bool seelog = false;

URL_LIST lookupURL;
void logWrite( const char *filename, const char *log )
{
	FILE *fp = NULL;
	fp = fopen( filename, "w+" );
	if( fp == NULL )
		return;

	int nWrite = fprintf( fp, "%s", log );
	if( nWrite < 0 )
	{
		fclose(fp);
		return;
	}

	fclose(fp);
}

bool loadURLlist( const char *filename )
{
	FILE *fp = NULL;
	fp = fopen( filename, "r" );
	if( fp == NULL )
	{
		printf( "%s File Open Error\n", filename );
		return false;
	}

	while( feof(fp) != EOF )
	{
		char buff[1024] = "\0";

		char *eof = fgets( buff, sizeof(buff), fp );
		if( eof == NULL )
			break;

		if( buff[strlen(buff)-1] == '\n' )
			buff[strlen(buff)-1] = '\0';

		lookupURL.push_back( buff );
	}

	fclose( fp );

	return true;
}

void sig_SIGSTOP(int arg)
{
	run = false;

	printf("Shutting down...\n");
}

void sig_SIGUSR1(int arg)
{
	if( seelog )
		seelog = false;

	else seelog = true;
}

int main(int argc, char *argv[])
{
	if( argc < 3 )
	{
		printf("usage : dnslookup [millisec] [URL list File]\n");
		return 0;
	}

	sigset( SIGSTOP, sig_SIGSTOP );
	sigset( SIGUSR1, sig_SIGUSR1 );

	timeval startTime, endTime;

	bool bSuccess = loadURLlist( argv[2] );
	if( bSuccess == false )
	{
		printf("wrong URL list file\n");
		return 0;
	}

	URL_LIST_ITER iter = lookupURL.begin();

	while(run)
	{
		if( iter == lookupURL.end() )
			iter = lookupURL.begin();

		if( iter == lookupURL.end() )
		{
			printf("URL list file error\n");
			break;
		}

		string strURL = *iter;
		iter++;

		memset( &startTime, 0x00, sizeof(timeval) );
		memset( &endTime, 0x00, sizeof(timeval) );

		gettimeofday( &startTime, NULL );

		struct hostent* lphost = NULL;
		lphost = gethostbyname( strURL.c_str() );

		gettimeofday( &endTime, NULL );

		int delayedsec = endTime.tv_sec - startTime.tv_sec;
//		float delayedusec = (float)(endTime.tv_usec - startTime.tv_usec)/1000000;
		double delayedusec = (double)(endTime.tv_usec - startTime.tv_usec)/1000000;

		struct tm *currtime = localtime( (time_t *)&startTime );

		struct in_addr addr;

		if( lphost )
			memcpy( &addr, lphost->h_addr, 4 );

		if( delayedsec >= 1 )
		{
			char buff[512] = "\0";
			char filename[64] = "\0";
			sprintf( buff, "[%d-%02d-%02d %02d:%02d:%02d] Delayed sec [%d] usec[%f] lookup[%s] IP[%s]\n", currtime->tm_year+1900, currtime->tm_mon+1, currtime->tm_mday, currtime->tm_hour, currtime->tm_min, currtime->tm_sec, delayedsec, delayedusec, strURL.c_str(), inet_ntoa(addr) );

			sprintf( filename, "%d%02d%02d.log", currtime->tm_year+1900, currtime->tm_mon+1, currtime->tm_mday );
			logWrite( filename, buff );
		}

		if( seelog )
			printf( "[%d-%02d-%02d %02d:%02d:%02d] Delayed sec [%d] usec[%f] lookup[%s] IP[%s]\n", currtime->tm_year+1900, currtime->tm_mon+1, currtime->tm_mday, currtime->tm_hour, currtime->tm_min, currtime->tm_sec, delayedsec, delayedusec, strURL.c_str(), inet_ntoa(addr) );

		int udelay = atoi( argv[1] );
		udelay *= 1000;
		usleep( udelay );
	}

	return 0;
}
