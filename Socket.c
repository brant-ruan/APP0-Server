// Socket.c
// 1452334 Bonan Ruan
// Wed May 24 09:10:43 2017

#ifndef _MY_PUBLIC_H
#define _MY_PUBLIC_H
#include "Public.h"
#endif

/*
 * Configure server socket
 * 		If error occurs, this function will close socket automatically
 * */
Status SSocket(int *fd, char *ip, u_short port)
{
	// server socket
	*fd = socket(AF_INET, SOCK_STREAM, 0);
	if(*fd == -1){
		perror("socket");
		return ERROR;
	}
	// set nonblocking
	int val;
	if((val = fcntl(*fd, F_GETFL, 0)) < 0){
		close(*fd);
		perror("fcntl");
		return ERROR;
	}
	if(fcntl(*fd, F_SETFL, val | O_NONBLOCK) < 0){
		close(*fd);
		perror("fcntl");
		return ERROR;
	}
	// reuse
	int opt;
	if(setsockopt(*fd, SOL_SOCKET, SO_REUSEADDR, \
			(char *)&opt, sizeof(opt)) < 0){
		perror("setsockopt");
		close(*fd);
		return ERROR;
	}
	// bind
	struct sockaddr_in svrAddr;
	bzero(&svrAddr, sizeof(svrAddr));
	svrAddr.sin_family = AF_INET;
	if(ip == NULL){
		svrAddr.sin_addr.s_addr = INADDR_ANY;
	}
	else{
		inet_pton(AF_INET, ip, (void *)&(svrAddr.sin_addr));
	}
	svrAddr.sin_port = htons(port);
	if(bind(*fd, (struct sockaddr *)&svrAddr, sizeof(svrAddr)) == -1){
		perror("bind");
		close(*fd);
		return ERROR;
	}
	// listen
	if(listen(*fd, CLIENT_NUM_MAX) == -1){
		perror("listen");
		close(*fd);
		return ERROR;
	}

	return OK;
}

int RecvN(int *fd, char *buf, int len)
{
	int haveRecvd = 0;
	int onceRecvd = 0;
	while(haveRecvd < len){
		if((onceRecvd = read(*fd, buf + haveRecvd, len - haveRecvd)) <= 0){
			perror("read");
			if(onceRecvd == 0 || (errno != EAGAIN && errno != EINTR)){
				return ERROR; // 0 means the peer close socket
			}
		}
		haveRecvd += onceRecvd;
	}

	return len;
}

/* Used by my webServer */
int RecvNN(int *fd, char *buf, int len)
{
	int haveRecvd = 0;
	int onceRecvd = 0;
	while(haveRecvd < len){
		printf("haveRecvd: %d\n", haveRecvd);
		if((onceRecvd = read(*fd, buf + haveRecvd, len - haveRecvd)) <= 0){
			perror("read");
			if(onceRecvd == 0 || (errno != EAGAIN && errno != EINTR)){
				return ERROR; // 0 means the peer close socket
			}
			else if(errno == EAGAIN || errno == EINTR){
				printf("go out\n");
				break;
			}
		}
		haveRecvd += onceRecvd;
	}

	return len;
}

int SendN(int *fd, char *sendBuf, int len)
{
	int haveSend = 0;
	int onceSend = 0;
	while(haveSend < len){
		if((onceSend = write(*fd, sendBuf + haveSend, len - haveSend)) <= 0){
			perror("write");
			if(errno != EAGAIN && errno != EINTR){
				return ERROR;
			}
		}
		printf("sending\n");
		haveSend += onceSend;
	}

	return len;
}
