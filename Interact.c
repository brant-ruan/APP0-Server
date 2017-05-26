// Interact.c
// 1452334 Bonan Ruan
// Wed May 24 15:53:20 2017

#ifndef _MY_PUBLIC_H
#define _MY_PUBLIC_H
#include "Public.h"
#endif

const char MOBILE_USER_PATH[] = "./MobileLogin/";
const char WEB_USER_PATH[] = "./WebLogin/";
const char QRCODE_PATH[] = "./QRCode/";

const char DB_IP = "localhost";
const char DB_PORT = 0;
const char DB_NAME = "APP0";
const char DB_USER = "root";
const char DB_PASSWD = "root123";

Status Interact(int *mobFd, int *webFd)
{
	int i, j, tmp;
	char userPath[USER_PATH_LEN] = {0};
	// Initialize database object
	MYSQL *mysql;
	if(DBInit(&mysql, DB_IP, DB_PORT, DB_USER, DB_PASSWD, DB_NAME) == ERROR){
		perror("DBInit");
		return ERROR;
	}

	// fds for mobile connections and web connections
	struct MFD mfd;
//	int wfd[WFD_NUM];
	MFDInit(&mfd);
//	FdInit(wfd, WFD_NUM);
	MFDAdd(&mfd, *mobFd);
//	wfd[0] = *webFd;

	// initialize structures for epoll
	int epFd;
	struct epoll_event ev;
	struct epoll_event evlist[MFD_NUM];
	if((epFd = epoll_create(1)) == -1){
		perror("epoll_create");
		DBClose(&mysql);
		return ERROR;
	}
	ev.events = EPOLLIN;
	ev.data.fd = mfd.arr[0].fd; // listen fd
	if(epoll_ctl(epFd, EPOLL_CTL_ADD, mfd.arr[0].fd, &ev) == -1){
		perror("epoll_ctl");
		goto LABEL_ERR;
	}
	// epoll begins...
	struct sockaddr_in cAddr; // client addr
	socklen_t cliLen;
	bzero(&cAddr, sizeof(cAddr));
	int ready = 0;
	while(mfd->num > 0){
		printf("New day new chance\n");
		ready = epoll_wait(epFd, evlist, mfd->num, -1);
		if(ready == -1){
			if(errno == EINTR) // interrupted by signal
				continue;
			else{
				perror("epoll_wait");
				goto LABEL_ERR;
			}
		}
		printf("Look! %d little fish comes\n", ready);
		for(j = 0; i < ready; j++){
			printf("fd=%d events: %s%s%s\n", evlist[j].data.fd, \
					(evlist[j].events & EPOLLIN) ? "EPOLLIN " : "", \
					(evlist[j].events & EPOLLOUT) ? "EPOLLOUT " : "", \
					(evlist[j].events & EPOLLHUP) ? "EPOLLHUP " : "", \
					(evlist[j].events & EPOLLERR) ? "EPOLLERR " : "");
			if(evlist[j].data.fd == mfd->arr[0].fd){ // listen socket
				if(evlist[j].events & EPOLLIN){ // fish comes
					cliLen = sizeof(cAddr);
					tmp = accept(mfd.arr[0].fd, (struct sockaddr *)&cAddr, &cliLen);
					if(tmp == -1){
						if(errno == EAGAIN || errno == EWOULDBLOCK)
							continue;
						else{
							perror("accept");
						}
					}
					MFDAdd(&mfd, tmp);
					// add fish into basket
					ev.events = EPOLLIN | EPOLLOUT;
					ev.data.fd = tmp; // listen fd
					if(epoll_ctl(epFd, EPOLL_CTL_ADD, tmp, &ev) == -1){
						perror("epoll_ctl");
					}
				}
				// nothing to do currently
			}
			else{
				tmp = MFDIndex(&mfd, evlist[j].data.fd);
				struct SC_Reponse response;
				if((evlist[j].events & EPOLLIN) \
					&& !(mfd.arr[tmp].flag & (LOGIN_L | SIGNUP_L \
											| LOGOUT_L | QRIDEN_L))){
					struct Header header;
					int len = ReadN(&(evlist[j].data.fd), \
							(char *)&header, sizeof(struct Header));
					if(len == ERROR){
						if(close(evlist[j].data.fd) == -1){
							perror("close");
						}
						MFDDel(&mfd, tmp, MOBILE_USER_PATH);
					}
					if(header.hType != CS_H){
						printf("header.hType invalid\n");
					}
					switch(header.lType){
					case LOGIN_L:
						struct CS_LogIn logIn;
						len = ReadN(&(evlist[j].data.fd), \
								(char *)&logIn, sizeof(struct CS_LogIn));
						if(len == ERROR){
							if(close(evlist[j].data.fd) == -1){
								perror("close");
							}
							MFDDel(&mfd, tmp, MOBILE_USER_PATH);
						}
						if(HaveLogged(&logIn) == YES){
							GenLogIn(&response, ERR_ALREADY);
						}
						else if(DBLogIn(logIn.username, logIn.password) == YES){
							GenLogIn(&response, evlist[j].data.fd);
							MobileUp(&mfd, tmp, &logIn);
						}
						else{
							GenLogIn(&response, ERR_WRONG);
						}
						mfd.arr[tmp].flag |= LOGIN_L;
						break;
					case SIGNUP_L:
						struct CS_SignUp signUp;
						mfd.arr[tmp].flag |= SIGNUP_L;
						break;
					case QRIDEN_L:
						struct CS_QRIden qrIden;
						mfd.arr[tmp].flag |= QRIDEN_L;

						break;
					case LOGOUT_L:
						if(mfd.arr[tmp].state == MOBILE_UP){
							GenLogOut(&response);
							mfd.arr[tmp].flag |= LOGOUT_L;
						}
						break;
					default:
						printf("header.lType invalid\n");
						break;
					}
				}
				if((evlist[j].events & EPOLLOUT) && (mfd.arr[tmp].flag != 0)){
					int len;
					len = SendN(evlist[j].data.fd, \
							(char *)response, sizeof(struct SC_Response));
					if(len == ERROR || (mfd.arr[tmp].flag & LOGOUT_L)){
						if(close(evlist[j].data.fd) == -1){
							perror("close");
						}
						MFDDel(&mfd, tmp, MOBILE_USER_PATH);
					}
					mfd.arr[tmp].flag = 0;
				}
				else if((evlist[j].events & (EPOLLHUP | EPOLLERR)) \
						&& !(evlist[j].events & EPOLLIN)){
					printf("One fish leaves\n");
					if(close(evlist[j].data.fd) == -1){
						perror("close");
					}
					MFDDel(&mfd, tmp, MOBILE_USER_PATH);
				}
			}
		}
	}

	close(epFd);
	DBClose(&mysql);
	return OK;

LABEL_ERR:
	close(epFd);
	DBClose(&mysql);
	return ERROR;
}
