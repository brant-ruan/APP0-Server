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

const char DB_IP[] = "localhost";
const char DB_PORT = 0;
const char DB_NAME[] = "APP0";
const char DB_USER[] = "root";
const char DB_PASSWD[] = "root123";

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
	while(mfd.num > 0){
		ready = epoll_wait(epFd, evlist, mfd.num, -1);
		if(ready == -1){
			if(errno == EINTR) // interrupted by signal
				continue;
			else{
				perror("epoll_wait");
				goto LABEL_ERR;
			}
		}
		for(j = 0; j < ready; j++){
		//	printf("fd=%d events: %s%s%s\n", evlist[j].data.fd, \
					(evlist[j].events & EPOLLIN) ? "EPOLLIN " : "", \
					(evlist[j].events & EPOLLOUT) ? "EPOLLOUT " : "", \
					(evlist[j].events & EPOLLHUP) ? "EPOLLHUP " : "", \
					(evlist[j].events & EPOLLERR) ? "EPOLLERR " : "");
			if(evlist[j].data.fd == mfd.arr[0].fd){ // listen socket
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
				printf("mfd.num: %d\n", mfd.num);
				tmp = MFDIndex(&mfd, evlist[j].data.fd);
				printf("tmp: %d\n", tmp);
				struct SC_Response response;
				struct CS_LogIn logIn;
				struct CS_SignUp signUp;
				struct CS_QRIden qrIden;
				char password[PASSWORD_LEN + 1] = {0};
				if((evlist[j].events & EPOLLIN) \
					&& !(mfd.arr[tmp].flag & (LOGIN_L | SIGNUP_L \
											| LOGOUT_L | QRIDEN_L))){
					struct Header header;
					int len = RecvN(&(evlist[j].data.fd), \
							(char *)&header, sizeof(struct Header));
					if(len == ERROR){
						if(close(evlist[j].data.fd) == -1){
							perror("close");
						}
						epoll_ctl(epFd, EPOLL_CTL_DEL, evlist[j].data.fd, NULL);
						MFDDel(&mfd, tmp, MOBILE_USER_PATH);
						continue; // this socket is ok
					}
					if(header.hType != CS_H){
						printf("header.hType invalid\n");
					}
					switch(header.lType){
					case LOGIN_L:
						len = RecvN(&(evlist[j].data.fd), \
								(char *)&logIn, sizeof(struct CS_LogIn));
						if(len == ERROR){
							if(close(evlist[j].data.fd) == -1){
								perror("close");
							}
							epoll_ctl(epFd, EPOLL_CTL_DEL, evlist[j].data.fd, NULL);
							MFDDel(&mfd, tmp, MOBILE_USER_PATH);
						}
						snprintf(mfd.arr[tmp].name, USERNAME_LEN, "%s", logIn.username);
						snprintf(password, PASSWORD_LEN, "%s", logIn.password);
						printf("username: %s\n", mfd.arr[tmp].name);
						printf("password: %s\n", password);
						printf("logining\n");
						if(HaveLogged(mfd.arr[tmp].name, MOBILE_USER_PATH) == YES){
							GenLogIn(&response, ERR_ALREADY);
						}
						else if(DBLogIn(mysql, mfd.arr[tmp].name, password) == YES){
							GenLogIn(&response, evlist[j].data.fd);
							MobileUp(&mfd, tmp, MOBILE_USER_PATH);
						}
						else{
							GenLogIn(&response, ERR_WRONG);
						}
						mfd.arr[tmp].flag |= LOGIN_L;
						break;
					case SIGNUP_L:
						len = RecvN(&(evlist[j].data.fd), \
								(char *)&signUp, sizeof(struct CS_SignUp));
						if(len == ERROR){
							if(close(evlist[j].data.fd) == -1){
								perror("close");
							}
							epoll_ctl(epFd, EPOLL_CTL_DEL, evlist[j].data.fd, NULL);
							MFDDel(&mfd, tmp, MOBILE_USER_PATH);
						}
						snprintf(mfd.arr[tmp].name, USERNAME_LEN, "%s", signUp.username);
						snprintf(password, PASSWORD_LEN, "%s", signUp.password);
						printf("signup name: %s\n", mfd.arr[tmp].name);
						printf("signup password: %s\n", password);
						if(HaveLogged(mfd.arr[tmp].name, MOBILE_USER_PATH) == YES){
							GenSignUp(&response, ERR_ALREADY);
						}
						else if(DBSignUp(mysql, mfd.arr[tmp].name, password) != YES){
							perror("DBSignUp");
							GenSignUp(&response, ERR_DUP);
						}
						else{
							GenSignUp(&response, evlist[j].data.fd);
							MobileUp(&mfd, tmp, MOBILE_USER_PATH);
						}
						mfd.arr[tmp].flag |= SIGNUP_L;
						break;
					case QRIDEN_L:
						len = RecvN(&(evlist[j].data.fd), \
								(char *)&qrIden, sizeof(struct CS_QRIden));
						if(len == ERROR){
							if(close(evlist[j].data.fd) == -1){
								perror("close");
							}
							epoll_ctl(epFd, EPOLL_CTL_DEL, evlist[j].data.fd, NULL);
							MFDDel(&mfd, tmp, MOBILE_USER_PATH);
						}
						printf("recv qrcode\n");
						if(mfd.arr[tmp].state == MOBILE_DOWN){
							printf("mobile down\n");
							GenQRIden(&response, ERR_WRONG);
						}
						else{
							printf("set web logged in\n");
							char ip[IP_LEN + 1] = {0};
							strncpy(ip, qrIden.IP, IP_LEN);
							if(QRLogIn(mfd.arr[tmp].name, ip, WEB_USER_PATH) == ERROR){
								GenQRIden(&response, ERR_WRONG);
							}
							else{
								GenQRIden(&response, ERR_NONE);
							}
						}
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
					printf("Before send\n");
					len = SendN(&(evlist[j].data.fd), \
							(char *)&response, sizeof(struct SC_Response));
					printf("err: %d\n", response.err);
	//				printf("After send\n");
					if(len == ERROR || (mfd.arr[tmp].flag & LOGOUT_L)){
						if(close(evlist[j].data.fd) == -1){
							perror("close");
						}
						epoll_ctl(epFd, EPOLL_CTL_DEL, evlist[j].data.fd, NULL);
						MFDDel(&mfd, tmp, MOBILE_USER_PATH);
						printf("now mfd.num: %d\n", mfd.num);
					}
					mfd.arr[tmp].flag = 0;
				}
				else if((evlist[j].events & (EPOLLHUP | EPOLLERR)) \
						&& !(evlist[j].events & EPOLLIN)){
					printf("One fish leaves\n");
					if(close(evlist[j].data.fd) == -1){
						perror("close");
					}
					epoll_ctl(epFd, EPOLL_CTL_DEL, evlist[j].data.fd, NULL);
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
