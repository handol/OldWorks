/* echo_cli.c
This is the client program for echo_ser.c

Algorithm outline:
a. Open a TCP socket.
b. Initialize server socket address structure
c. Connect to server
d. Send user input to server
e. Display server response on screen
f. Notify server of intent to close
g. Close socket.

This program also illustrates:
a. Use of gethostbyname() to get some infor. on intended server.
b. Use of bcopy() to copy an element into a structure.
c. Use of shutdow() to notify server of closing connection.

To compile:       gcc -o echo_cli echo_cli.c -lsocket -lnsl 
Command syntax:   echo_cli  server_name (either DNS or IP)
*/

#include <stdio.h>
#include <unistd.h> 
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#define  SER_PORT  8080     /* arbitrary; official echo server port is 7 */

main(int argc, char *argv[])
{
	struct linger ling;
	int cli_sock, ser_len, port_number;
	size_t nread;
	char buf[BUFSIZ];
	struct sockaddr_in  ser;
	struct hostent *ser_info;
	int port = 0;

	if( argc > 2 ) 
		port = atoi(argv[2]);

	if( port <= 0 ) port = SER_PORT;

	fprintf(stderr,"argv[1] : %s port : %s\n\n", argv[1], argv[2]);
	fprintf(stderr,"host : %s port : %d\n\n", argv[1], port);

	/* to open a socket */
	if ((cli_sock = socket(AF_INET, SOCK_STREAM, 0)) == -1)  {
		fprintf(stderr, "Unable to create socket for client\n");
		exit(1);
	}

	/* to initialize server sockaddr_in structure */
	ser_len = sizeof(ser);            /* to get ser length */
	bzero((char *)&ser, ser_len);     /* to zero out struct */
	ser.sin_family      = AF_INET;    /* Internet domain */
	ser.sin_port = htons(port);   /* echo server port number */

	if ((ser_info = gethostbyname(argv[1])) == (struct hostent *)NULL) {
		fprintf(stderr, "unknown server\n");
		exit(2);
	}
	bcopy((char *)ser_info->h_addr, (char *)&ser.sin_addr, ser_info->h_length);

	/* to connect to server */
	if ((connect(cli_sock, (struct sockaddr *)&ser, ser_len)) == -1) {
		fprintf(stderr, "can't connect to  server\n");
		exit(3);
	}

#ifdef OOO
	nread = read(STDIN_FILENO, buf, BUFSIZ);  /* get input from stdin */
	send(cli_sock, buf, nread, 0);            /* send data to server */
	nread = recv(cli_sock, buf, BUFSIZ, 0);   /* read back data */
	write(STDOUT_FILENO, buf, nread);         /* display it on screen */
#endif

	ling.l_onoff  = 1;    /* 대기여부 결정. 0 이면 대기 안함 */
	ling.l_linger = 4;    /* 대기시간 결정. 0 이면 대기 안함 */

	setsockopt(cli_sock, SOL_SOCKET, SO_LINGER, (const char *) &ling, sizeof(struct linger));

//	shutdown(cli_sock, SHUT_RDWR);            /* notify server of closing */
	fprintf(stderr,"before close()\n");
//	close(cli_sock);
	fprintf(stderr,"after close()\n");
}  /* main */


