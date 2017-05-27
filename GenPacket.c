// GenPacket.c
// 1452334 Bonan Ruan
// Mon May 15 08:01:32 2017

#ifndef _MY_PUBLIC_H
#define _MY_PUBLIC_H
#include "Public.h"
#endif

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
