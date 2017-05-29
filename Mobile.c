// Mobile.c
// 1452334 Bonan Ruan
// Mon May 15 05:26:38 2017

#ifndef _MY_PUBLIC_H
#define _MY_PUBLIC_H
#include "Public.h"
#endif

void MFDInit(struct MFD *mfd)
{
	int i;
	for(i = 0; i < MFD_NUM; i++){
		mfd->arr[i].fd = -1;
	}

	mfd->num = 0;
}

void MFDAdd(struct MFD *mfd, int fd)
{
	if(mfd->num < MFD_NUM){
		mfd->arr[mfd->num].fd = fd;
		mfd->arr[mfd->num].state = MOBILE_DOWN;
		mfd->arr[mfd->num].flag = 0;
		mfd->num += 1;
	}
	else
		printf("Fish comes but I got enough...\n");
}

void MFDSub(struct MFD *mfd, int index)
{
	if(mfd->num > 1)
	{
		if(index < mfd->num - 1){
			memcpy(&(mfd->arr[index]), &(mfd->arr[index + 1]), \
					sizeof(mfd->arr[index]) * (mfd->num - 1 - index));
		}
		mfd->num -= 1;
	}
}

int MFDIndex(struct MFD *mfd, int fd)
{
	int i;
	for(i = 0; i < mfd->num; i++){
		if(mfd->arr[i].fd == fd)
			return i;
	}
	return -1;
}

void MFDDel(struct MFD *mfd, int index, char *path)
{
	char userPath[USER_PATH_LEN] = {0};
	if(index > 0){
		sprintf(userPath, "%s%s", path, \
				mfd->arr[index].name);
		//printf("del %s\n", userPath);
		unlink(userPath);
		MFDSub(mfd, index);
	}
}

Status MobileUp(struct MFD *mfd, int index, char *path)
{
	mfd->arr[index].state = MOBILE_UP;
	char userPath[USER_PATH_LEN] = {0};
	sprintf(userPath, "%s%s", path, mfd->arr[index].name);
	int fd;
	// create a file named with username
	while((fd = open(userPath, O_RDWR | O_CREAT)) == -1){
		perror("open");
	}
	close(fd);

	return OK;
}

Status HaveLogged(char *username, char *path)
{
	char userPath[USER_PATH_LEN] = {0};

	sprintf(userPath, "%s%s", path, username);
	int fd = open(userPath, O_RDONLY);
	if(fd == -1){
		return NO;
	}

	close(fd);
	return YES;
}
