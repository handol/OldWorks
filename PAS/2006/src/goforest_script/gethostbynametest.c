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

double elapsedTime(struct timeval stp, struct timeval etp);

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
	char host[1024], filename[128];
	char hostlist[128][128];
	char *tmp;
	unsigned char ipaddr[128];
	FILE *fp;
	int hostcount;
	int loopcount;
	int errorcount = 0;

	struct in_addr  addr;
	struct hostent  *he;

	if( argc > 2 ) {
		strcpy(filename, argv[1]);
		loopcount = atoi(argv[2]);
	} else if ( argc > 1 ) {
		strcpy(filename, argv[1]);
		loopcount = 100000;
	} else {
		strcpy(filename, "host.list");
		loopcount = 100000;
	}

	fp = fopen(argv[1], "r");
	if( fp == (FILE *)NULL ) {
		fprintf(stderr,"Error!! Couldnot read %s ", filename);
		exit(1);
	}

	i = 0;
	while( (tmp = fgets(host, 1024, fp)) != (char *)NULL) {
		host[strlen(host)-1] = (char)NULL;
		if( strlen(host) < 3 ) continue;
		strcpy(hostlist[i++], host);
//		fprintf(stderr,"%s\n", host);
	}
	if( i == 0  ) {
		fprintf(stderr,"Error!! hostname is not exist\n");
		exit(1);
	}
	hostcount = i;

	/* signal handling */
	signal(SIGALRM, SIG_IGN);

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

	for( i = 0; i < loopcount ; i++) {
		flag = gettimeofday(&stp, NULL);
		/*
		for(i = 0 ; i < 10; i++ ) {
			fprintf(stderr,"%d ", i+1);
			sleep(1);
		}
		sleep(10);
		*/

		strcpy(host, hostlist[i%hostcount]);

		memset(ipaddr, 0, sizeof(ipaddr));

//		fprintf(stderr,"%s\n", host);

		if ((addr.s_addr = inet_addr(host)) == -1) {
			ualarm(1000, 1000);
			if (!(he = (struct hostent *)gethostbyname(host))) {
				flag = gettimeofday(&etp, NULL);
				fprintf(stderr,"Error!! gethostbyname(%s)[errno=%d](Elapsedtime: %.6f)\n"
					, host, errno, elapsedTime(stp, etp));
//				memset(ipaddr, 0, sizeof(ipaddr));
				memset(ipaddr, 0, 128);
				errorcount++;
				ualarm(0, 0);
				usleep(1000000);
				errno = 0;
				continue;
			}
			memcpy(ipaddr, he->h_addr, he->h_length);
//			fprintf(stderr,"[%s -> %d.%d.%d.%d]\n", host, ipaddr[0],ipaddr[1],ipaddr[2],ipaddr[3]);
		}
		ualarm(0, 0);

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

		if( (i % 100) == 0 ) {
			fprintf(stderr,"[%s -> %d.%d.%d.%d]\n", host, ipaddr[0],ipaddr[1],ipaddr[2],ipaddr[3]);
			fprintf(stderr,"[%d] = [%.06f]\n", i,  elapsedtime);
		}

		if( elapsedtime > 1.0 ) {
			fprintf(stderr,"[%s -> %d.%d.%d.%d]\n", host, ipaddr[0],ipaddr[1],ipaddr[2],ipaddr[3]);
			fprintf(stderr,"[%.06f]\n",  elapsedtime);
		}

		usleep(200000);

	} /* for */

	fprintf(stderr," The End [%d][error=%d]\n\n", loopcount, errorcount);
	exit(0);
}

/* calculate elapsed time 
*/
double elapsedTime( struct timeval stp, struct timeval etp)
{
	struct timeval elapsed;
	double elapsedtime;

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

	return(elapsedtime);
}
