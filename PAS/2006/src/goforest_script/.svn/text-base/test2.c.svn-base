#include <errno.h>
#include <fcntl.h>
#include <netdb.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

/*
* main()
*/
int main(int argc, char **argv)
{
	extern int errno;
	struct timeval stp, etp;
	struct timeval elapsed;
	double elapsedtime;
	int i, flag;
	char host[1024];
	unsigned char ipaddr[128];

	struct in_addr  addr;
	struct hostent  *he;

	if( argc > 1 ) 
		strcpy(host, argv[1]);
	else
		strcpy(host, "www.anydic.com");

	/* signal handling */
//	signal(SIGALRM, SIG_IGN);

#ifdef TEST
	ualarm(5000000, 500000);
	flag = gettimeofday(&stp, NULL);
	/*
	for(i = 0 ; i < 10; i++ ) {
		fprintf(stderr,"%d ", i+1);
		sleep(1);
	}
	*/
	sleep(10);

	flag = gettimeofday(&etp, NULL);

	if( etp.tv_usec >= stp.tv_usec ) {
		elapsed.tv_usec = etp.tv_usec - stp.tv_usec;
		elapsed.tv_sec = etp.tv_sec - stp.tv_sec;
		elapsedtime = elapsed.tv_sec + (double)elapsed.tv_usec/1000000;
	} else {
		elapsed.tv_usec = (etp.tv_usec + 1)- stp.tv_usec;
		elapsed.tv_sec = etp.tv_sec - stp.tv_sec - 1;
		elapsedtime = elapsed.tv_sec + (double)elapsed.tv_usec/1000000;
	}
	fprintf(stderr," The End [%d sec %06d microseconds][%.06f][%d]\n", elapsed.tv_sec, elapsed.tv_usec, elapsedtime, errno);
	errno = 0;
#endif

//	ualarm(10000, 10000);

	for( i = 0; i < 100000 ; i++) {
		flag = gettimeofday(&stp, NULL);
	/*
		for(i = 0 ; i < 10; i++ ) {
			fprintf(stderr,"%d ", i+1);
			sleep(1);
		}
		sleep(10);
	*/

		memset(&addr, 0, sizeof(addr));

		if ((addr.s_addr = inet_addr(host)) == -1) {
			if (!(he = (struct hostent *)gethostbyname(host))) {
				fprintf(stderr," The End [%d]\n\n", errno);
				exit(1);
			}
			memcpy(ipaddr, he->h_addr, he->h_length);
//			fprintf(stderr,"[%s -> %d.%d.%d.%d]\n", host, ipaddr[0],ipaddr[1],ipaddr[2],ipaddr[3]);
		}

		flag = gettimeofday(&etp, NULL);

		if( etp.tv_usec >= stp.tv_usec ) {
			elapsed.tv_usec = etp.tv_usec - stp.tv_usec;
			elapsed.tv_sec = etp.tv_sec - stp.tv_sec;
			elapsedtime = elapsed.tv_sec + (double)elapsed.tv_usec/1000000;
		} else {
//			fprintf(stderr,"[%06d, %06d]\n", etp.tv_usec, stp.tv_usec);
			elapsed.tv_usec = (etp.tv_usec + 1000000)- stp.tv_usec;
			elapsed.tv_sec = etp.tv_sec - stp.tv_sec - 1;
			elapsedtime = elapsed.tv_sec + (double)elapsed.tv_usec/1000000;
		}
//		fprintf(stderr," The End [%d sec %06d microseconds][%.06f][%d]\n", elapsed.tv_sec, elapsed.tv_usec, elapsedtime, errno);

		if( (i % 100) == 0 )
			fprintf(stderr,"[%d] = [%.06f][%d]\n", i,  elapsedtime);

		if( elapsedtime > 1.0 ) 
			fprintf(stderr,"[%.06f]\n",  elapsedtime);

		usleep(200000);

	} /* for */

	fprintf(stderr," The End [%d]\n\n", errno);
	exit(0);
}

