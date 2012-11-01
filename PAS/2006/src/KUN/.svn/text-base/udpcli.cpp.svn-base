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
#include <string.h>
#include "udpcli.h"


void udpcli::init()
{
//msg = (message_t *)malloc(sizeof(message_t));

	bzero((char*)&serv_addr,sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = inet_addr(SERV_HOST_ADDR);
	serv_addr.sin_port = htons(SERV_UDP_PORT);

	if((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
		printf("client : can't open datagram socket");

	bzero((char*)&cli_addr,sizeof(cli_addr));
	cli_addr.sin_family = AF_INET;

	cli_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	cli_addr.sin_port = htons(0);

	if (bind(sockfd,(struct sockaddr *)&cli_addr, sizeof(cli_addr))<0)
		fprintf(stderr, "udpcli for pasmonitor on logsvr or KUNLOG bind() failed");
	return;
}


void udpcli::send(char *host, int port, int count)
{
	memset(&msg, 0x00, sizeof(message_t));
	strcpy(msg.host, host);
	msg.port = port;
	msg.count = count;

//printf("I will send %s\n",msg->host);
//printf("& :[%s]\n", msg->data);
	if (sendto(sockfd, (char *)&msg, sizeof(message_t),0,
		(struct sockaddr *) &serv_addr,sizeof(serv_addr))!= sizeof(message_t))
		fprintf(stderr, "udpcli for pasmonitor on logsvr or KUNLOG: sendtp() failed");
}


void udpcli::close()
{
	::close(sockfd);
}
