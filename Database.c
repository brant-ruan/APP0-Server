// Database.c
// 1452334 Bonan Ruan
// Wed May 24 15:22:11 2017

#ifndef _MY_PUBLIC_H
#define _MY_PUBLIC_H
#include "Public.h"
#endif

/*
 * Database	: APP0
 * Table	: User
 * Col1		: username char(16) PRIMARY KEY NOT NULL
 * Col2		: password char(32) NOT NULL
 *
 * */

/*
 * Here you must pay attention to SQL injection...
 * So, how to prevent it?
 * */
const char QUERY_LOGIN[] = \
	"SELECT count(username) FROM User WHERE username = %s AND password = %s;";
const char QUERY_SIGNUP[] = "SELECT count(username) FROM User WHERE username = %s;";
const char QUERY_ADDUSER[] = "INSERT INTO User values('%s', '%s');";

/* Initialize MySQL object */
Status MySQLInit(MYSQL **mysql, char *dbIP, u_short dbPort, \
		char *dbUser, char *dbPasswd, char *dbName)
{
	if((*mysql = mysql_init(NULL)) == NULL){
		perror("mysql_init");
		return ERROR;
	}
	if(mysql_real_connect(*mysql, dbIP, dbUser, \
			dbPasswd, dbName, dbPort, NULL, 0) == NULL){
		perror("mysql_real_connect");
		return ERROR;
	}
	mysql_set_character_set(*mysql, "gbk");

	return OK;
}

Status DBInit(void *dbObj, char *dbIP, u_short dbPort, \
		char *dbUser, char *dbPasswd, char *dbName)
{
	if(MySQLInit((MYSQL **)dbObj, dbIP, dbPort, dbUser, dbPasswd, dbName) == ERROR){
		perror("MySQLInit");
		return ERROR;
	}
	return OK;
}

/* Close MySQL object */
Status MySQLClose(MYSQL **mysql)
{
	mysql_close(*mysql);
	return OK;
}

Status DBClose(void *dbObj)
{
	MySQLClose((MYSQL **)dbObj);

	return OK;
}

/*
 * If username existed, return NO
 * else add user into database, return YES
 * */
Status DBSignUp(char *username, char *password)
{
	return YES;
}

/*
 * If no such user, return NO
 * else return YES
 * */
Status DBLogIn(char *username, char *password)
{

	return YES;
}
