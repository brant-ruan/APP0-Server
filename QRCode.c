// QRCode.c
// 1452334 Bonan Ruan
// Wed May 24 14:29:54 2017

#ifndef _MY_PUBLIC_H
#define _MY_PUBLIC_H
#include "Public.h"
#endif

#define EN_IP_LEN	32
#define CMD_LEN		128

Status Encrypt(char *dst, char *src);

/*
 * Server generates data for QRcode:
 * 		Content: WebClientIP XOR "Hello, WebClient"
 *
 * Client sends data to Server for identification:
 * 		Content: struct CS_QRIden
 * */

const char XOR_KEY[] = "Hello, WebClient"; // 16 byte, the same as IP
const char ALPHA[] = "46CdeFGhIJK1mn0pQR5tuVwxYz";

/*
 * QRCode will be generated in QRCODE_PATH, named as "xxx.xxx.xxx.xxx_qr.png"
 * */
Status GenQRCode(char *IP, char *path)
{
	char enIP[EN_IP_LEN + 1] = {0};
	Encrypt(enIP, IP);
	char enCmd[CMD_LEN + 1] = {0};
	printf("In GenQRCode: %s\n", path);
	sprintf(enCmd, "qr \"%s\" > %s", enIP, path);
	printf("cmd: %s\n", enCmd);
	system(enCmd);

	return OK;
}

/*
 * '.' '0' '1' '2' '3' '4' '5' '6' '7' '8' '9'
 * 												% 26 =
 * 20	22	23	24	25	0	1	2	3	4	5
 * */
/* src must be a string end with '\0' */
Status Encrypt(char *dst, char *src)
{
	while(*src){
		*dst = ALPHA[(*src) % 26];
		src++;
		dst++;
	}

	return OK;
}
