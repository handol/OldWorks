/*
 * Copyright (c) 2000-2002 KT Freetel Co., Ltd. All rights reserved.
 * File       :
 * Date       :
 * Author     : un-hae choi(cloudsea@magicn.com)
 * Description:
 *
 * Revisions  :
 *
 */

#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <signal.h>
#include <stdlib.h>
#include <strings.h>
#include <unistd.h>

#define KUN

#ifdef KUN
#define SERV_UDP_PORT 8090
#define SERV_HOST_ADDR "192.168.208.98"
#else
#define SERV_UDP_PORT 8090
#define SERV_HOST_ADDR "128.134.98.69"
#endif

struct message_t
{
	char host[40];
	int port;
	int count;
};

class udpcli
{
	message_t msg;

	public:
		int sockfd;
		struct sockaddr_in serv_addr,cli_addr;

		void init();
		void send(char *host, int port, int count);
		void close();

};
