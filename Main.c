// Main.c
// 1452334 Bonan Ruan
// Wed May 24 09:01:19 2017

#ifndef _MY_PUBLIC_H
#define _MY_PUBLIC_H
#include "Public.h"
#endif

const u_short MOBILE_PORT	= 10001;
// const char SERVER_IP[]		= "192.168.137.230";

Status main(int argc, char **argv)
{
	if(argc < 2){
		return ERROR;
	}
	int mobFd = 0;
	int webFd = 0;
	// configure nonblocking socket
	// listen for mobile
	if(SSocket(&mobFd, argv[1], MOBILE_PORT) == ERROR){
		perror("SSocket");
		return ERROR;
	}
#if 0
	// listen for Web
	if(SSocket(&webFd, argv[1], WEB_PORT) == ERROR){
		perror("SSocket");
		return ERROR;
	}
#endif
	// Interact
	if(Interact(&mobFd, &webFd) == ERROR){
		perror("Interact");
		close(mobFd);
		close(webFd);
		return ERROR;
	}

	close(mobFd);
//	close(webFd);
	return OK;
}
