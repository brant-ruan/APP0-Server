// HTML.c
// 1452334 Bonan Ruan
// Wed May 24 15:21:26 2017

#ifndef _MY_PUBLIC_H
#define _MY_PUBLIC_H
#include "Public.h"
#endif

#define HTML_TEXT_PREFIX \
	"HTTP/1.1 200 OK\r\n" \
	"Content-Type: text/html; charset=UTF-8\r\n\r\n" \
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

const char HTML_PNG_PREFIX[] = "HTTP/1.1 200 OK\r\n" \
	"Content-Type: image/png\r\n\r\n";

const char WEB_HAVENT[] = HTML_TEXT_PREFIX "<h2>Where there is a will, there is a QRCode :)</h2><img src=\"%s\"></img>" HTML_SUFFIX;

const char WEB_HAVE[] = HTML_TEXT_PREFIX "<h1>Hello, %s</h1>" HTML_SUFFIX;

const char WEB_LOGOUT[] = HTML_TEXT_PREFIX "<h1>%s, see you again.</h1>" HTML_SUFFIX;

