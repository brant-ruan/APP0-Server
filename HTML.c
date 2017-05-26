// HTML.c
// 1452334 Bonan Ruan
// Wed May 24 15:21:26 2017

#ifndef _MY_PUBLIC_H
#define _MY_PUBLIC_H
#include "Public.h"
#endif

#define HTML_PREFIX \
	"<!DOCTYPE html>\n" \
	"<html>\n" \
	  "<head>\n" \
		"<title>Welcome</title>\n" \
	  "</head>\n" \
	  "<body>\n" \
	    "<center>\n"

#define HTML_SUFFIX \
	    "</center>\n" \
	  "</body>\n" \
	"</html>\n"

const char WEB_HAVENT[] = HTML_PREFIX "<img src=\"%s\">" HTML_SUFFIX;

const char WEB_HAVE[] = HTML_PREFIX "<h1>Hello, %s</h1>" HTML_SUFFIX;

