/*

Copyright 1987, 1998  The Open Group

Permission to use, copy, modify, distribute, and sell this software and its
documentation for any purpose is hereby granted without fee, provided that
the above copyright notice appear in all copies and that both that
copyright notice and this permission notice appear in supporting
documentation.

The above copyright notice and this permission notice shall be included
in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE OPEN GROUP BE LIABLE FOR ANY CLAIM, DAMAGES OR
OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
OTHER DEALINGS IN THE SOFTWARE.

Except as contained in this notice, the name of The Open Group shall
not be used in advertising or otherwise to promote the sale, use or
other dealings in this Software without prior written authorization
from The Open Group.


Copyright 1987 by Digital Equipment Corporation, Maynard, Massachusetts,
Copyright 1994 Quarterdeck Office Systems.

                        All Rights Reserved

Permission to use, copy, modify, and distribute this software and its
documentation for any purpose and without fee is hereby granted,
provided that the above copyright notice appear in all copies and that
both that copyright notice and this permission notice appear in
supporting documentation, and that the names of Digital and
Quarterdeck not be used in advertising or publicity pertaining to
distribution of the software without specific, written prior
permission.

DIGITAL AND QUARTERDECK DISCLAIM ALL WARRANTIES WITH REGARD TO THIS
SOFTWARE, INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND
FITNESS, IN NO EVENT SHALL DIGITAL BE LIABLE FOR ANY SPECIAL, INDIRECT
OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS
OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE
OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE
OR PERFORMANCE OF THIS SOFTWARE.

*/

/*
 * Copyright (c) 1997-2006 by The XFree86 Project, Inc.
 * All rights reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject
 * to the following conditions:
 *
 *   1.  Redistributions of source code must retain the above copyright
 *       notice, this list of conditions, and the following disclaimer.
 *
 *   2.  Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer
 *       in the documentation and/or other materials provided with the
 *       distribution, and in the same place and form as other copyright,
 *       license and disclaimer information.
 *
 *   3.  The end-user documentation included with the redistribution,
 *       if any, must include the following acknowledgment: "This product
 *       includes software developed by The XFree86 Project, Inc
 *       (http://www.xfree86.org/) and its contributors", in the same
 *       place and form as other third-party acknowledgments.  Alternately,
 *       this acknowledgment may appear in the software itself, in the
 *       same form and location as other such third-party acknowledgments.
 *
 *   4.  Except as contained in this notice, the name of The XFree86
 *       Project, Inc shall not be used in advertising or otherwise to
 *       promote the sale, use or other dealings in this Software without
 *       prior written authorization from The XFree86 Project, Inc.
 *
 * THIS SOFTWARE IS PROVIDED ``AS IS'' AND ANY EXPRESSED OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE XFREE86 PROJECT, INC OR ITS CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY,
 * OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT
 * OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
 * BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
 * OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/* $XFree86: xc/programs/Xserver/os/log.c,v 1.14 2006/03/06 16:06:23 dawes Exp $ */

#include <X11/Xos.h>
#include <stdio.h>
#include <time.h>
#include <sys/stat.h>
#include <stdarg.h>
#include <stdlib.h>	/* for malloc() */
#include <errno.h>

#include "site.h"
#include "opaque.h"

#ifdef DDXOSVERRORF
void (*OsVendorVErrorFProc)(const char *, va_list args) = NULL;
#endif

static FILE *logFile = NULL;
static Bool logFlush = FALSE;
static Bool logSync = FALSE;
static int logVerbosity = DEFAULT_LOG_VERBOSITY;
static int logFileVerbosity = DEFAULT_LOG_FILE_VERBOSITY;

/* Buffer to information logged before the log file is opened. */
static char *saveBuffer = NULL;
static int bufferSize = 0, bufferUnused = 0, bufferPos = 0;
static Bool needBuffer = TRUE;

/* Prefix strings for log messages. */
#ifndef X_UNKNOWN_STRING
#define X_UNKNOWN_STRING		"(\?\?)"
#endif
#ifndef X_PROBE_STRING
#define X_PROBE_STRING			"(--)"
#endif
#ifndef X_CONFIG_STRING
#define X_CONFIG_STRING			"(**)"
#endif
#ifndef X_DEFAULT_STRING
#define X_DEFAULT_STRING		"(==)"
#endif
#ifndef X_CMDLINE_STRING
#define X_CMDLINE_STRING		"(++)"
#endif
#ifndef X_NOTICE_STRING
#define X_NOTICE_STRING			"(!!)"
#endif
#ifndef X_ERROR_STRING
#define X_ERROR_STRING			"(EE)"
#endif
#ifndef X_WARNING_STRING
#define X_WARNING_STRING		"(WW)"
#endif
#ifndef X_INFO_STRING
#define X_INFO_STRING			"(II)"
#endif
#ifndef X_NOT_IMPLEMENTED_STRING
#define X_NOT_IMPLEMENTED_STRING	"(NI)"
#endif

/*
 * LogInit is called to start logging to a file.  It is also called (with
 * NULL arguments) when logging to a file is not wanted.  It must always be
 * called, otherwise log messages will continue to accumulate in a buffer.
 *
 * %s, if present in the fname or backup strings, is expanded to the display
 * string.
 */

const char *
LogInit(const char *fname, const char *backup)
{
    char *logFileName = NULL;

#ifdef	TROPIX
	if (fname != NULL)
		logFileName = strdup (fname);
	else
		logFileName = "<stderr>";

	logFile = stderr;

	if (saveBuffer && bufferSize > 0)
	{
		fwrite (saveBuffer, bufferPos, 1, logFile);
		fflush (logFile);
	}
#else
    if (fname && *fname) {
	/* xalloc() can't be used yet. */
	logFileName = malloc(strlen(fname) + strlen(display) + 1);
	if (!logFileName)
	    FatalError("Cannot allocate space for the log file name\n");
	sprintf(logFileName, fname, display);

	if (backup && *backup) {
	    struct stat buf;

	    if (!stat(logFileName, &buf) && S_ISREG(buf.st_mode)) {
		char *suffix;
		char *oldLog;

		oldLog = malloc(strlen(logFileName) + strlen(backup) +
				strlen(display) + 1);
		suffix = malloc(strlen(backup) + strlen(display) + 1);
		if (!oldLog || !suffix)
		    FatalError("Cannot allocate space for the log file name\n");
		sprintf(suffix, backup, display);
		sprintf(oldLog, "%s%s", logFileName, suffix);
		free(suffix);
#ifdef __UNIXOS2__
		remove(oldLog);
#endif
		if (rename(logFileName, oldLog) == -1) {
		    FatalError("Cannot move old log file (\"%s\" to \"%s\"\n",
			       logFileName, oldLog);
		}
		free(oldLog);
	    }
	}
	if ((logFile = fopen(logFileName, "w")) == NULL)
	    FatalError("Cannot open log file \"%s\"\n", logFileName);
	setvbuf(logFile, NULL, _IONBF, 0);

	/* Flush saved log information. */
	if (saveBuffer && bufferSize > 0) {
	    fwrite(saveBuffer, bufferPos, 1, logFile);
	    fflush(logFile);
	    fsync(fileno(logFile));
	}
    }
#endif	TROPIX

    /*
     * Unconditionally free the buffer, and flag that the buffer is no longer
     * needed.
     */
    if (saveBuffer && bufferSize > 0) {
	free(saveBuffer);	/* Must be free(), not xfree() */
	saveBuffer = NULL;
	bufferSize = 0;
    }
    needBuffer = FALSE;

    return logFileName;
}

void
LogClose()
{
    if (logFile) {
	fclose(logFile);
	logFile = NULL;
    }
}

Bool
LogSetParameter(LogParameter param, int value)
{
    switch (param) {
    case XLOG_FLUSH:
	logFlush = value ? TRUE : FALSE;
	return TRUE;
    case XLOG_SYNC:
	logSync = value ? TRUE : FALSE;
	return TRUE;
    case XLOG_VERBOSITY:
	logVerbosity = value;
	return TRUE;
    case XLOG_FILE_VERBOSITY:
	logFileVerbosity = value;
	return TRUE;
    default:
	return FALSE;
    }
}

/* This function does the actual log message writes. */

void
LogVWrite(int verb, const char *f, va_list args)
{
    static char tmpBuffer[1024];
    int len = 0;

    /*
     * Since a va_list can only be processed once, write the string to a
     * buffer, and then write the buffer out to the appropriate output
     * stream(s).
     */
    if (verb < 0 || logFileVerbosity >= verb || logVerbosity >= verb) {
	vsnprintf(tmpBuffer, sizeof(tmpBuffer), f, args);
	len = strlen(tmpBuffer);
    }
#ifndef	TROPIX
    if ((verb < 0 || logVerbosity >= verb) && len > 0)
	fwrite(tmpBuffer, len, 1, stderr);
#else
    if (logFile == NULL && (verb < 0 || logVerbosity >= verb) && len > 0)
	fwrite(tmpBuffer, len, 1, stderr);
#endif	TROPIX
    if ((verb < 0 || logFileVerbosity >= verb) && len > 0) {
	if (logFile) {
	    fwrite(tmpBuffer, len, 1, logFile);
	    if (logFlush || logSync) {
		fflush(logFile);
#ifndef	TROPIX
		if (logSync)
		    fsync(fileno(logFile));
#endif	TROPIX
	    }
	} else if (needBuffer) {
	    /*
	     * Note, this code is used before OsInit() has been called, so
	     * xalloc() and friends can't be used.
	     */
	    if (len > bufferUnused) {
		bufferSize += 1024;
		bufferUnused += 1024;
		if (saveBuffer)
		    saveBuffer = realloc(saveBuffer, bufferSize);
		else
		    saveBuffer = malloc(bufferSize);
		if (!saveBuffer)
		    FatalError("realloc() failed while saving log messages\n");
	    }
	    bufferUnused -= len;
	    memcpy(saveBuffer + bufferPos, tmpBuffer, len);
	    bufferPos += len;
	}
    }
}

void
LogWrite(int verb, const char *f, ...)
{
    va_list args;

    va_start(args, f);
    LogVWrite(verb, f, args);
    va_end(args);
}

void
LogVMessageVerb(MessageType type, int verb, const char *format, va_list args)
{
    const char *s  = X_UNKNOWN_STRING;
    char *tmpBuf = NULL;

    /* Ignore verbosity for X_ERROR */
    if (logVerbosity >= verb || logFileVerbosity >= verb || type == X_ERROR) {
	switch (type) {
	case X_PROBED:
	    s = X_PROBE_STRING;
	    break;
	case X_CONFIG:
	    s = X_CONFIG_STRING;
	    break;
	case X_DEFAULT:
	    s = X_DEFAULT_STRING;
	    break;
	case X_CMDLINE:
	    s = X_CMDLINE_STRING;
	    break;
	case X_NOTICE:
	    s = X_NOTICE_STRING;
	    break;
	case X_ERROR:
	    s = X_ERROR_STRING;
	    if (verb > 0)
		verb = 0;
	    break;
	case X_WARNING:
	    s = X_WARNING_STRING;
	    break;
	case X_INFO:
	    s = X_INFO_STRING;
	    break;
	case X_NOT_IMPLEMENTED:
	    s = X_NOT_IMPLEMENTED_STRING;
	    break;
	case X_UNKNOWN:
	    s = X_UNKNOWN_STRING;
	    break;
	case X_NONE:
	    s = NULL;
	    break;
	}

	/*
	 * Prefix the format string with the message type.  We do it this way
	 * so that LogVWrite() is only called once per message.
	 */
	if (s) {
	    tmpBuf = malloc(strlen(format) + strlen(s) + 1 + 1);
	    /* Silently return if malloc fails here. */
	    if (!tmpBuf)
		return;
	    sprintf(tmpBuf, "%s ", s);
	    strcat(tmpBuf, format);
	    LogVWrite(verb, tmpBuf, args);
	    free(tmpBuf);
	} else
	    LogVWrite(verb, format, args);
    }
}

/* Log message with verbosity level specified. */
void
LogMessageVerb(MessageType type, int verb, const char *format, ...)
{
    va_list ap;

    va_start(ap, format);
    LogVMessageVerb(type, verb, format, ap);
    va_end(ap);
}

/* Log a message with the standard verbosity level of X_LOG_DEFAULT_VERB. */
void
LogMessage(MessageType type, const char *format, ...)
{
    va_list ap;

    va_start(ap, format);
    LogVMessageVerb(type, X_LOG_DEFAULT_VERB, format, ap);
    va_end(ap);
}

#ifndef AUDIT_PREFIX
#define AUDIT_PREFIX "AUDIT: %s: %ld %s: "
#endif
#ifndef AUDIT_TIMEOUT
#define AUDIT_TIMEOUT ((CARD32)(120 * 1000)) /* 2 mn */
#endif

static int nrepeat = 0;
static int oldlen = -1;
static OsTimerPtr auditTimer = NULL;

void 
FreeAuditTimer(void)
{
    if (auditTimer != NULL) {
	/* Force output of pending messages */
	TimerForce(auditTimer);
	TimerFree(auditTimer);
	auditTimer = NULL;
    }
}

static char *
AuditPrefix(void)
{
    time_t tm;
    char *autime, *s;
    char *tmpBuf;
    int len;

    time(&tm);
    autime = ctime(&tm);
    if ((s = strchr(autime, '\n')))
	*s = '\0';
    if ((s = strrchr(argvGlobal[0], '/')))
	s++;
    else
	s = argvGlobal[0];
    len = strlen(AUDIT_PREFIX) + strlen(autime) + 10 + strlen(s) + 1;
    tmpBuf = malloc(len);
    if (!tmpBuf)
	return NULL;
    snprintf(tmpBuf, len, AUDIT_PREFIX, autime, (unsigned long)getpid(), s);
    return tmpBuf;
}

void
AuditF(const char * f, ...)
{
    va_list args;

    va_start(args, f);

    VAuditF(f, args);
    va_end(args);
}

static CARD32
AuditFlush(OsTimerPtr timer, CARD32 now, pointer arg)
{
    char *prefix;

    if (nrepeat > 0) {
	prefix = AuditPrefix();
	ErrorF("%slast message repeated %d times\n",
	       prefix != NULL ? prefix : "", nrepeat);
	nrepeat = 0;
	if (prefix != NULL)
	    free(prefix);
	return AUDIT_TIMEOUT;
    } else {
	/* if the timer expires without anything to print, flush the message */
	oldlen = -1;
	return 0;
    }
}

void
VAuditF(const char *f, va_list args)
{
    char *prefix;
    char buf[1024];
    int len;
    static char oldbuf[1024];

    prefix = AuditPrefix();
    len = vsnprintf(buf, sizeof(buf), f, args);

    if (len == oldlen && strcmp(buf, oldbuf) == 0) {
	/* Message already seen */
	nrepeat++;
    } else {
	/* new message */
	if (auditTimer != NULL)
	    TimerForce(auditTimer);
	ErrorF("%s%s", prefix != NULL ? prefix : "", buf);
	strlcpy(oldbuf, buf, sizeof(oldbuf));
	oldlen = len;
	nrepeat = 0;
	auditTimer = TimerSet(auditTimer, 0, AUDIT_TIMEOUT, AuditFlush, NULL);
    }
    if (prefix != NULL)
	free(prefix);
}

void
FatalError(const char *f, ...)
{
    va_list args;
    static Bool beenhere = FALSE;

    if (beenhere)
	ErrorF("\nFatalError re-entered, aborting\n");
    else
	ErrorF("\nFatal server error:\n");

    va_start(args, f);
    VErrorF(f, args);
    va_end(args);
    ErrorF("\n");
#ifdef DDXOSFATALERROR
    if (!beenhere)
	OsVendorFatalError();
#endif
#ifdef ABORTONFATALERROR
    abort();
#endif
    if (!beenhere) {
	beenhere = TRUE;
	AbortServer(0);
    } else
	abort();
    /*NOTREACHED*/
}

void
VErrorF(const char *f, va_list args)
{
#ifdef DDXOSVERRORF
    if (OsVendorVErrorFProc)
	OsVendorVErrorFProc(f, args);
    else
	LogVWrite(-1, f, args);
#else
    LogVWrite(-1, f, args);
#endif
}

void
ErrorF(const char * f, ...)
{
    va_list args;

    va_start(args, f);
    VErrorF(f, args);
    va_end(args);
}

/* A perror() workalike. */

#ifndef NEED_STRERROR
#ifdef SYSV
#if !defined(ISC) || defined(ISC202) || defined(ISC22)
#define NEED_STRERROR
#endif
#endif
#endif

#if defined(NEED_STRERROR) && !defined(strerror)
extern char *sys_errlist[];
extern int sys_nerr;
#define strerror(n) \
	((n) >= 0 && (n) < sys_nerr) ? sys_errlist[(n)] : "unknown error"
#endif

void
Error(char *str)
{
    char *err = NULL;
    int saveErrno = errno;

    if (str) {
	err = malloc(strlen(strerror(saveErrno)) + strlen(str) + 2 + 1);
	if (!err)
	    return;
	sprintf(err, "%s: ", str);
	strcat(err, strerror(saveErrno));
	LogWrite(-1, err);
    } else
	LogWrite(-1, strerror(saveErrno));
}

const char *
LogTypeToTxt(MessageType type)
{
    switch (type) {
    case X_PROBED:
	return "probed";
    case X_CONFIG:
	return "from config file";
    case X_DEFAULT:
	return "default setting";
    case X_CMDLINE:
	return "from command line";
    case X_NOTICE:
	return "notice";
    case X_INFO:
	return "informational";
    case X_WARNING:
	return "warning";
    case X_ERROR:
	return "error";
    case X_NOT_IMPLEMENTED:
	return "not implemented";
    case X_UNKNOWN:
	return "unknown";
    case X_NONE:
	return "none";
    }
    return "";
}

#define LINEBREAK(i) (((i) == X_DEFAULT || (i) == X_WARNING) ? "\n\t" : " ")

void
LogPrintMarkers()
{
    MessageType i;
    /* Show what the message marker symbols mean. */
    ErrorF("Markers: ");
    for (i = X_PROBED; i <= X_NOT_IMPLEMENTED; i++)
	if (i != X_NONE)
	    LogMessageVerb(i, -1, "%s,%s", LogTypeToTxt(i), LINEBREAK(i));
    LogMessageVerb(X_UNKNOWN, -1, "unknown.\n");
}

