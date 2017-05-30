// GenPacket.c
// 1452334 Bonan Ruan
// Mon May 15 08:01:32 2017

#ifndef _MY_PUBLIC_H
#define _MY_PUBLIC_H
#include "Public.h"
#endif

extern const char HTML_PNG_PREFIX[];
extern const char WEB_HAVE[];
extern const char WEB_HAVENT[];
extern const char WEB_LOGOUT[];
/*------------------------------------> Mobile Interact */

Status GenHeader(struct Header *header, \
		char lType, char dataLen)
{
	header->hType = SC_H;
	header->lType = lType;
	header->len = dataLen + sizeof(struct Header);
	header->dataLen = dataLen;

	return OK;
}

Status GenLogOut(struct SC_Response *response)
{
	GenHeader(&(response->header), LOGOUT_L, \
			sizeof(struct SC_Response) - sizeof(struct Header));

	response->err = htonl(ERR_NONE);

	return OK;
}

Status GenLogIn(struct SC_Response *response, int retNum)
{
	GenHeader(&(response->header), LOGIN_L, \
			sizeof(struct SC_Response) - sizeof(struct Header));

	response->err = htonl(retNum);

	return OK;
}

Status GenSignUp(struct SC_Response *response, int retNum)
{
	GenHeader(&(response->header), SIGNUP_L, \
			sizeof(struct SC_Response) - sizeof(struct Header));

	response->err = htonl(retNum);

	return OK;
}

Status GenQRIden(struct SC_Response *response, int retNum)
{
	GenHeader(&(response->header), QRIDEN_L, \
			sizeof(struct SC_Response) - sizeof(struct Header));

	response->err = htonl(retNum);
	return OK;
}

/*------------------------------------> Web Interact */

Status GenWebLogged(char *buf, int *len, char *username)
{
	sprintf(buf, WEB_HAVE, username);
	*len = strlen(buf);

	return OK;
}

Status GenWegLogOut(char *buf, int *len, char *username)
{
	sprintf(buf, WEB_LOGOUT, username);
	*len = strlen(buf);

	return OK;
}

Status GenImage(struct WFD *wfd, \
		int index, char *buf, int *len, char *path)
{
	sprintf(buf, "%s", HTML_PNG_PREFIX);
	int tmpLen = strlen(buf);

	char qrPath[QRCODE_PATH_LEN] = {0};
	sprintf(qrPath, "%s%s" QR_SUFFIX, path, wfd->arr[index].ip);
	printf("In GenImage qrPath: %s\n", qrPath);
	int fd = open(qrPath, O_RDONLY);
	if(fd == -1){
		GenQRCode(wfd->arr[index].ip, qrPath);
		if((fd = open(qrPath, O_RDONLY)) == -1){
			perror("open");
		}
	}
	*len = read(fd, buf + tmpLen, SENDBUF_LEN / 2);
	if(*len <= 0){
		perror("read");
		close(fd);
		*len = tmpLen;
		return ERROR;
	}
	*len += tmpLen;

	close(fd);
	return OK;
}

Status GenWebPage(struct WFD *wfd, int index, \
		char *buf, int *len, char *path)
{
	char qrPath[QRCODE_PATH_LEN] = {0};
	sprintf(qrPath, "%s%s" QR_SUFFIX, path, wfd->arr[index].ip);
	sprintf(buf, WEB_HAVENT, qrPath);
	*len = strlen(buf);

	return OK;
}

