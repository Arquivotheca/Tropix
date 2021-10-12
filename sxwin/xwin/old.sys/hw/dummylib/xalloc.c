/* $XFree86: xc/programs/Xserver/hw/xfree86/dummylib/xalloc.c,v 1.5 2006/01/08 22:50:30 dawes Exp $ */

#include <X11/X.h>
#include "os.h"
#include "xf86.h"
#include "xf86Priv.h"

/*
 * Utility functions required by libxf86_os, the loader, and others. 
 */

pointer
Xalloc(unsigned long n)
{
    if (!n)
	n = 1;
    return malloc(n);
}

pointer
Xrealloc(pointer p, unsigned long n)
{
    if (!n)
	n = 1;
    return realloc(p, n);
}

pointer
Xcalloc(unsigned long n)
{
    pointer r;

    r = Xalloc(n);
    memset(r, 0, n);
    return r;
}

pointer
XNFalloc(unsigned long n)
{
    pointer r;

    r = Xalloc(n);
    if (!r)
	FatalError("XNFalloc failed\n");
    return r;
   
}

pointer
XNFrealloc(pointer p, unsigned long n)
{
    pointer r;

    r = Xrealloc(p, n);
    if (!r)
	FatalError("XNFrealloc failed\n");
    return r;
   
}

pointer
XNFcalloc(unsigned long n)
{
    pointer r;

    r = Xcalloc(n);
    if (!r)
	FatalError("XNFcalloc failed\n");
    return r;
   
}

void
Xfree(pointer p)
{
    free(p);
}

char *
Xstrdup(const char *s)
{
    char *sd;

    if (s == NULL)
	return NULL;

    sd = (char *)Xalloc(strlen(s) + 1);
    if (sd != NULL)
	strcpy(sd, s);
    return sd;
}

char *
XNFstrdup(const char *s)
{
    char *sd;

    if (s == NULL)
	return NULL;

    sd = (char *)XNFalloc(strlen(s) + 1);
    strcpy(sd, s);
    return sd;
}

int
Xasprintf(char **ret, const char *format, ...)
{
    char *s;
    va_list args;
    int status;

    if (!ret || !format)
	return -1;

#ifdef HAS_ASPRINTF
    va_start(args, format);
    status = vasprintf(&s, format, args);
    va_end(args);
    if (status != -1 && s) {
	*ret = Xstrdup(s);
	free(s);
	if (!*ret)
	    status = -1;
    } else
	*ret = NULL;
    return status;
#else
#define TMP_SIZE 4000
    s = xcalloc(1, TMP_SIZE);
    if (!s) {
	*ret = NULL;
	return -1;
    }
    va_start(args, format);
    status = vsnprintf(s, TMP_SIZE, format, args);
    va_end(args);
    if (status > TMP_SIZE - 1)
	status = TMP_SIZE - 1;
    if (status < TMP_SIZE - 1) {
	*ret = xrealloc(s, status + 1);
	if (!*ret) {
	    xfree(s);
	    status = -1;
	}
    } else
	*ret = s;
    return status;
#endif
}

