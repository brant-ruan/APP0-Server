// Database.c
// 1452334 Bonan Ruan
// Wed May 24 15:22:11 2017

#ifndef _MY_PUBLIC_H
#define _MY_PUBLIC_H
#include "Public.h"
#endif

#define QUERY_LEN	256

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
	"SELECT count(username) FROM User WHERE username = '%s' AND password = '%s';";
const char QUERY_SIGNUP[] = "SELECT count(username) FROM User WHERE username = '%s';";
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
Status DBSignUp(MYSQL *mysql, char *username, char *password)
{
	int queryRes = 0;
	MYSQL_RES *mysqlRes;
	MYSQL_ROW row;

	char query[QUERY_LEN];
	sprintf(query, QUERY_SIGNUP, username);
	queryRes = mysql_query(mysql, query);
	if(queryRes){
		printf("MySQL error: %s\n", mysql_error(mysql));
	}
	printf("signup query: %s\n", query);
	mysqlRes = mysql_store_result(mysql);
	if(mysqlRes){
		if((unsigned long)mysql_num_rows(mysqlRes) > 0){
			row = mysql_fetch_row(mysqlRes);
			printf("row[0]: %s\n", row[0]);
			if(atoi(row[0]) > 0){
				printf("User existed\n");
				mysql_free_result(mysqlRes);
				return NO; // username existed
			}
		}
	}
	else{
		printf("mysql_store_result error: %s\n", mysql_error(mysql));
	}

	mysql_free_result(mysqlRes);
	sprintf(query, QUERY_ADDUSER, username, password);
	printf("insert query: %s\n", query);
	queryRes = mysql_query(mysql, query);
	if(queryRes){
		printf("MySQL error: %s\n", mysql_error(mysql));
	}

	return YES;
}

/*
 * If no such user, return NO
 * else return YES
 * */
Status DBLogIn(MYSQL *mysql, char *username, char *password)
{
	int queryRes = 0;
	MYSQL_RES *mysqlRes;
	MYSQL_ROW row;
	char query[QUERY_LEN];
	sprintf(query, QUERY_LOGIN, username, password);
	queryRes = mysql_query(mysql, query);
	printf("query: %s\n", query);
	if(queryRes){
		printf("MySQL error: %s\n", mysql_error(mysql));
	}
	mysqlRes = mysql_store_result(mysql);
	if(mysqlRes){
		if((unsigned long)mysql_num_rows(mysqlRes) > 0){
			row = mysql_fetch_row(mysqlRes);
			printf("row[0]: %s\n", row[0]);
			if(atoi(row[0]) > 0){
				mysql_free_result(mysqlRes);
				return YES; // username existed
			}
			else{
				printf("No such user\n");
				return NO;
			}
		}
	}
	else{
		printf("mysql_store_result error: %s\n", mysql_error(mysql));
	}

	mysql_free_result(mysqlRes);
	return NO;
}
