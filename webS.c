// webS.c
// 1452334 Bonan Ruan
// Mon May 15 22:41:56 2017

#ifndef _MY_PUBLIC_H
#define _MY_PUBLIC_H
#include "Public.h"
#endif

const u_short WEB_PORT = 8001;
const char *imgNeedle = ".png HTTP/1.1";
const char *outNeedle = "GET /logout HTTP/1.1";

extern const char WEB_USER_PATH[];
extern const char QRCODE_PATH[];

void WFDInit(struct WFD *wfd)
{
	int i;
	for(i = 0; i < WFD_NUM; i++){
		wfd->arr[i].fd = -1;
	}

	wfd->num = 0;
}

void WFDAdd(struct WFD *wfd, int fd, char *ip)
{
	if(wfd->num < WFD_NUM){
		wfd->arr[wfd->num].fd = fd;
		wfd->arr[wfd->num].flag = 0;
		sprintf(wfd->arr[wfd->num].ip, "%s", ip);
		wfd->num += 1;
	}
	else
		printf("Fish comes but I got enough...\n");
}

void WFDSub(struct WFD *wfd, int index)
{
	if(wfd->num > 1)
	{
		if(index < wfd->num - 1){
			memcpy(&(wfd->arr[index]), &(wfd->arr[index + 1]), \
					sizeof(wfd->arr[index]) * (wfd->num - 1 - index));
		}
		wfd->num -= 1;
	}
}

int WFDIndex(struct WFD *wfd, int fd)
{
	int i;
	for(i = 0; i < wfd->num; i++){
		if(wfd->arr[i].fd == fd)
			return i;
	}
	return -1;
}

void WFDDel(struct WFD *wfd, int index, char *path)
{
	char userPath[USER_PATH_LEN] = {0};
	if(index > 0){
		sprintf(userPath, "%s%s", path, \
				wfd->arr[index].ip);
		//printf("del %s\n", userPath);
		// currently, do not del the ID-Card when one socket exits
	//	unlink(userPath);
		WFDSub(wfd, index);
	}
}

Status WebHaveLogged(struct WFD *wfd, int index, char *path, char *username)
{
	char userPath[USER_PATH_LEN] = {0};
	sprintf(userPath, "%s%s", path, \
			wfd->arr[index].ip);
	printf("check webUserPath: %s\n", userPath);
	int fd = open(userPath, O_RDONLY);
	if(fd == -1){
		return NO;
	}
	if(read(fd, username, USERNAME_LEN) <= 0){
		perror("read\n");
	}
	close(fd);
	return YES;
}

/* because there is a '\0' in buf,
 * so len is currently not used*/
Status AskImage(char *buf)
{
	if(strcasestr(buf, imgNeedle) == NULL){
		return NO;
	}
	return YES;
}

Status AskLogOut(char *buf)
{
	if(strcasestr(buf, outNeedle) == NULL){
		return NO;
	}
	return YES;
}

Status WebLogOut(struct WFD *wfd, int index, char *path)
{
	char userPath[USER_PATH_LEN] = {0};
	sprintf(userPath, "%s%s", path, wfd->arr[index].ip);
	unlink(userPath);

	return OK;
}

Status main(int argc, char **argv)
{
	int j = 0;
	int len = 0;
	int tmp = 0;
	int webFd = 0;
	if(argc < 2){
		return ERROR;
	}
	if(SSocket(&webFd, argv[1], WEB_PORT) == ERROR){
		perror("SSocket");
		return ERROR;
	}

	struct WFD wfd;
	WFDInit(&wfd);
	WFDAdd(&wfd, webFd, argv[1]);

	int epFd;
	struct epoll_event ev;
	struct epoll_event evlist[WFD_NUM];
	if((epFd = epoll_create(1)) == -1){
		perror("epoll_create");
		return ERROR;
	}

	ev.events = EPOLLIN;
	ev.data.fd = wfd.arr[0].fd; // listen fd
	if(epoll_ctl(epFd, EPOLL_CTL_ADD, wfd.arr[0].fd, &ev) == -1){
		perror("epoll_ctl");
		goto LABEL_ERR;
	}

	struct sockaddr_in cAddr; // client addr
	socklen_t cliLen;
	bzero(&cAddr, sizeof(cAddr));
	int ready = 0;

	char recvBuf[RECVBUF_LEN] = {0};
	char sendBuf[SENDBUF_LEN] = {0};
	int sendLen = 0;
	while(wfd.num > 0){
		ready = epoll_wait(epFd, evlist, wfd.num, -1);
	//	printf("wait here\n");
		if(ready == -1){
			if(errno == EINTR) // interrupted by signal
				continue;
			else{
				perror("epoll_wait");
				goto LABEL_ERR;
			}
		}
		for(j = 0; j < ready; j++){
			if(evlist[j].data.fd == wfd.arr[0].fd){
				if(evlist[j].events & EPOLLIN){
					cliLen = sizeof(cAddr);
					tmp = accept(wfd.arr[0].fd, (struct sockaddr *)&cAddr, &cliLen);
					if(tmp == -1){
						if(errno == EAGAIN || errno == EWOULDBLOCK)
							continue;
						else{
							perror("accept");
						}
					}
					// set nonblocking
					int val;
					if((val = fcntl(tmp, F_GETFL, 0)) < 0){
						close(tmp);
						perror("fcntl");
						continue;
					}
					if(fcntl(tmp, F_SETFL, val | O_NONBLOCK) < 0){
						close(tmp);
						perror("fcntl");
						continue;
					}

					WFDAdd(&wfd, tmp, inet_ntoa(cAddr.sin_addr));
					// add fish into basket
					ev.events = EPOLLIN | EPOLLOUT;
					ev.data.fd = tmp; // listen fd
					if(epoll_ctl(epFd, EPOLL_CTL_ADD, tmp, &ev) == -1){
						perror("epoll_ctl");
					}
					printf("one comes\n");
				}
			}
			else{
	//			printf("wfd.num: %d\n", wfd.num);
				tmp = WFDIndex(&wfd, evlist[j].data.fd);
		//		printf("here\n");
				if((evlist[j].events & EPOLLIN) && (wfd.arr[tmp].flag == 0)){
					int len;
					printf("before RecvNN\n");
					len = RecvNN(&(evlist[j].data.fd), recvBuf, RECVBUF_LEN);
					printf("after RecvNN\n");
					if(len == ERROR){
						if(close(evlist[j].data.fd) == -1){
							perror("close");
						}
						epoll_ctl(epFd, EPOLL_CTL_DEL, evlist[j].data.fd, NULL);
						WFDDel(&wfd, tmp, WEB_USER_PATH);
						continue; // this socket is ok
					}
					recvBuf[len] = '\0';
					char username[USERNAME_LEN + 1] = {0};
					if(WebHaveLogged(&wfd, tmp, WEB_USER_PATH, username) == YES){
						printf("generate web logged packet\n");
						if(AskLogOut(recvBuf) == YES){
							WebLogOut(&wfd, tmp, WEB_USER_PATH);
							GenWegLogOut(sendBuf, &sendLen, username);
						}
						else
							GenWebLogged(sendBuf, &sendLen, username);
					}
					else{
						printf("web not logged\n");
						if(AskImage(recvBuf) == YES){
							if(GenImage(&wfd, tmp, sendBuf, &sendLen, QRCODE_PATH) == ERROR){
								perror("GenImage");
							}
						}
						else{
							GenWebPage(&wfd, tmp, sendBuf, &sendLen, QRCODE_PATH);
						}
					}
					wfd.arr[tmp].flag = 1;
				}
				if((evlist[j].events & EPOLLOUT) && (wfd.arr[tmp].flag == 1)){
					int len;
					len = SendN(&(evlist[j].data.fd), \
							sendBuf, sendLen);
					if(len == ERROR){
						if(close(evlist[j].data.fd) == -1){
							perror("close");
						}
						epoll_ctl(epFd, EPOLL_CTL_DEL, evlist[j].data.fd, NULL);
						WFDDel(&wfd, tmp, WEB_USER_PATH);
					}
					wfd.arr[tmp].flag = 0;
				}
				else if((evlist[j].events & (EPOLLHUP | EPOLLERR)) \
						&& !(evlist[j].events & EPOLLIN)){
					printf("One fish leaves\n");
					if(close(evlist[j].data.fd) == -1){
						perror("close");
					}
					epoll_ctl(epFd, EPOLL_CTL_DEL, evlist[j].data.fd, NULL);
					WFDDel(&wfd, tmp, WEB_USER_PATH);
				}
			}
		}
	}
	close(webFd);
	close(epFd);
	return OK;

LABEL_ERR:
	close(webFd);
	close(epFd);
	return ERROR;
}
