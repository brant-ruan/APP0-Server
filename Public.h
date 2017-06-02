// Public.h
// 1452334 Bonan Ruan
// Wed May 24 09:01:23 2017

// -----------------------------> [include]
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <time.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <mysql.h>
#include <fcntl.h>
#include <sys/epoll.h>
// -----------------------------> [define]
#define	ERROR		-1
#define	OK			0

#define YES			1
#define NO			0

#define CLIENT_NUM_MAX	500
#define IP_LEN		16

#define MFD_NUM		501
#define WFD_NUM		MFD_NUM

#define MOBILE_UP	1
#define MOBILE_DOWN	0

#define USER_PATH_LEN	36
#define QRCODE_PATH_LEN	48

#define RECVBUF_LEN	1024
#define SENDBUF_LEN	2048

#define  QR_SUFFIX "_qr.png"
// -----------------------------> [struct]
#define SC_H	0x11
#define CS_H	0x91
#define LOGIN_L		0x01 // log in
#define SIGNUP_L	0x02 // sign up
#define	QRIDEN_L	0x04 // qrcode identification
#define LOGOUT_L	0x08 // log out

struct Header{
	u_char hType; // main type: server/client
	u_char lType; // sub type: protocol type
	u_char len; // 4 ~ 255
	u_char dataLen; // 0 ~ 251
};

#define PASSWORD_LEN	32
#define USERNAME_LEN	16
struct CS_LogIn{
//	struct Header header;
	char username[USERNAME_LEN];
	char password[PASSWORD_LEN]; // password md5
};

struct CS_SignUp{
//	struct Header header;
	char username[USERNAME_LEN];
	char password[PASSWORD_LEN]; // password md5
};

// #define TOKEN_LEN		32
struct CS_QRIden{
//	struct Header header;
	char IP[IP_LEN];
//	char username[USERNAME_LEN];
//	char password[PASSWORD_LEN];
};

struct CS_LogOut{
//	struct Header header;
};

#define ERR_NONE		0 		// that means operation succeeded
#define ERR_DUP			0x1	 	// user exists when signing up
#define ERR_ALREADY		0x2		// user already logged in
#define ERR_WRONG		-1		// username or password is wrong (or both)
// if err > 2, it is current fd used by server for this socket
struct SC_Response{
	struct Header header;
	int err;
};

struct MobArr{
	int fd;
	int flag;
	int state;
	char name[USERNAME_LEN + 1];
};

struct MFD{
	struct MobArr arr[MFD_NUM];
	int num;
};

struct WebArr{
	int fd;
	int flag;
	char ip[IP_LEN + 1];
};

struct WFD{
	struct WebArr arr[WFD_NUM];
	int num;
};
// -----------------------------> [typedef]
typedef int Status;
typedef unsigned char u_char;
typedef unsigned short u_short;
typedef unsigned int u_int;


