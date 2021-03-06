/* $XFree86: xc/programs/Xserver/hw/xfree86/common/xf86Option.c,v 1.37 2005/10/14 15:16:33 tsi Exp $ */
/*
 * Copyright (c) 1998-2005 by The XFree86 Project, Inc.
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
/*
 * Copyright ? 2003, 2004, 2005 David H. Dawes.
 * Copyright ? 2003, 2004, 2005 X-Oz Technologies.
 * All rights reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 * 
 *  1. Redistributions of source code must retain the above copyright
 *     notice, this list of conditions, and the following disclaimer.
 *
 *  2. Redistributions in binary form must reproduce the above
 *     copyright notice, this list of conditions and the following
 *     disclaimer in the documentation and/or other materials provided
 *     with the distribution.
 * 
 *  3. The end-user documentation included with the redistribution,
 *     if any, must include the following acknowledgment: "This product
 *     includes software developed by X-Oz Technologies
 *     (http://www.x-oz.com/)."  Alternately, this acknowledgment may
 *     appear in the software itself, if and wherever such third-party
 *     acknowledgments normally appear.
 *
 *  4. Except as contained in this notice, the name of X-Oz
 *     Technologies shall not be used in advertising or otherwise to
 *     promote the sale, use or other dealings in this Software without
 *     prior written authorization from X-Oz Technologies.
 *
 * THIS SOFTWARE IS PROVIDED ``AS IS'' AND ANY EXPRESSED OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL X-OZ TECHNOLOGIES OR ITS CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY,
 * OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT
 * OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
 * BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
 * OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/*
 * Author: David Dawes <dawes@xfree86.org>
 *
 * This file includes public option handling functions.
 */

#include <stdlib.h>
#include <ctype.h>
#include <X11/X.h>
#include "os.h"
#include "xf86.h"
#include "xf86Xinput.h"
#include "xf86Optrec.h"

static Bool ParseOptionValue(int scrnIndex, pointer options, OptionInfoPtr p,
			     Bool markUsed);

/*
 * xf86CollectOptions collects the options from each of the config file
 * sections used by the screen and puts the combined list in pScrn->options.
 * This function requires that the following have been initialised:
 *
 *	pScrn->confScreen
 *	pScrn->Entities[i]->device
 *	pScrn->display
 *	pScrn->monitor
 *
 * The extraOpts parameter may optionally contain a list of additional options
 * to include.
 *
 * The order of precedence for options is:
 *
 *   extraOpts, display, confScreen, monitor, device
 */

void
xf86CollectOptions(ScrnInfoPtr pScrn, pointer extraOpts)
{
    XF86OptionPtr tmp;
    XF86OptionPtr extras = (XF86OptionPtr)extraOpts;
    GDevPtr device;
    
    int i;

    pScrn->options = NULL;

    for (i=pScrn->numEntities - 1; i >= 0; i--) {
	device = xf86GetDevFromEntity(pScrn->entityList[i],
					pScrn->entityInstanceList[i]);
	if (device && device->options) {
	    tmp = xf86optionListDup(device->options);
	    if (pScrn->options)
		xf86optionListMerge(pScrn->options,tmp);
	    else
		pScrn->options = tmp;
	}
    }
    if (pScrn->monitor->options) {
	tmp = xf86optionListDup(pScrn->monitor->options);
	if (pScrn->options)
	    pScrn->options = xf86optionListMerge(pScrn->options, tmp);
	else
	    pScrn->options = tmp;
    }
    if (pScrn->confScreen->options) {
	tmp = xf86optionListDup(pScrn->confScreen->options);
	if (pScrn->options)
	    pScrn->options = xf86optionListMerge(pScrn->options, tmp);
	else
	    pScrn->options = tmp;
    }
    if (pScrn->display->options) {
	tmp = xf86optionListDup(pScrn->display->options);
	if (pScrn->options)
	    pScrn->options = xf86optionListMerge(pScrn->options, tmp);
	else
	    pScrn->options = tmp;
    }
    if (extras) {
	tmp = xf86optionListDup(extras);
	if (pScrn->options)
	    pScrn->options = xf86optionListMerge(pScrn->options, tmp);
	else
	    pScrn->options = tmp;
    }
}

/*
 * xf86CollectInputOptions collects the options for an InputDevice.
 * This function requires that the following has been initialised:
 *
 *	pInfo->conf_idev
 *
 * The extraOpts parameter may optionally contain a list of additional options
 * to include.
 *
 * The order of precedence for options is:
 *
 *   extraOpts, pInfo->conf_idev->extraOptions,
 *   pInfo->conf_idev->commonOptions, defaultOpts
 */

void
xf86CollectInputOptions(InputInfoPtr pInfo, const char **defaultOpts,
			pointer extraOpts)
{
    XF86OptionPtr tmp;
    XF86OptionPtr extras = (XF86OptionPtr)extraOpts;

    pInfo->options = NULL;
    if (defaultOpts) {
	pInfo->options = xf86OptionListCreate(defaultOpts, -1, 0);
    }
    if (pInfo->conf_idev->commonOptions) {
	tmp = xf86optionListDup(pInfo->conf_idev->commonOptions);
	if (pInfo->options)
	    pInfo->options = xf86optionListMerge(pInfo->options, tmp);
	else
	    pInfo->options = tmp;
    }
    if (pInfo->conf_idev->extraOptions) {
	tmp = xf86optionListDup(pInfo->conf_idev->extraOptions);
	if (pInfo->options)
	    pInfo->options = xf86optionListMerge(pInfo->options, tmp);
	else
	    pInfo->options = tmp;
    }
    if (extras) {
	tmp = xf86optionListDup(extras);
	if (pInfo->options)
	    pInfo->options = xf86optionListMerge(pInfo->options, tmp);
	else
	    pInfo->options = tmp;
    }
}

/* Created for new XInput stuff -- essentially extensions to the parser	*/

static int
LookupIntOption(pointer optlist, const char *name, int deflt, Bool markUsed)
{
    OptionInfoRec o;

    o.name = name;
    o.type = OPTV_INTEGER;
    if (ParseOptionValue(-1, optlist, &o, markUsed))
	deflt = o.value.num;
    return deflt;
}


static double
LookupRealOption(pointer optlist, const char *name, double deflt,
		 Bool markUsed)
{
    OptionInfoRec o;

    o.name = name;
    o.type = OPTV_REAL;
    if (ParseOptionValue(-1, optlist, &o, markUsed))
	deflt = o.value.realnum;
    return deflt;
}


static char *
LookupStrOption(pointer optlist, const char *name, char *deflt, Bool markUsed)
{
    OptionInfoRec o;

    o.name = name;
    o.type = OPTV_STRING;
    if (ParseOptionValue(-1, optlist, &o, markUsed))
        deflt = o.value.str;
    if (deflt)
	return xstrdup(deflt);
    else
	return NULL;
}


static int
LookupBoolOption(pointer optlist, const char *name, int deflt, Bool markUsed)
{
    OptionInfoRec o;

    o.name = name;
    o.type = OPTV_BOOLEAN;
    if (ParseOptionValue(-1, optlist, &o, markUsed))
	deflt = o.value.bool;
    return deflt;
}

/* These xf86Set* functions are intended for use by non-screen specific code */

int
xf86SetIntOption(pointer optlist, const char *name, int deflt)
{
    return LookupIntOption(optlist, name, deflt, TRUE);
}


double
xf86SetRealOption(pointer optlist, const char *name, double deflt)
{
    return LookupRealOption(optlist, name, deflt, TRUE);
}


char *
xf86SetStrOption(pointer optlist, const char *name, char *deflt)
{
    return LookupStrOption(optlist, name, deflt, TRUE);
}


int
xf86SetBoolOption(pointer optlist, const char *name, int deflt)
{
    return LookupBoolOption(optlist, name, deflt, TRUE);
}

/*
 * These are like the Set*Option functions, but they don't mark the options
 * as used.
 */
int
xf86CheckIntOption(pointer optlist, const char *name, int deflt)
{
    return LookupIntOption(optlist, name, deflt, FALSE);
}


double
xf86CheckRealOption(pointer optlist, const char *name, double deflt)
{
    return LookupRealOption(optlist, name, deflt, FALSE);
}


char *
xf86CheckStrOption(pointer optlist, const char *name, char *deflt)
{
    return LookupStrOption(optlist, name, deflt, FALSE);
}


int
xf86CheckBoolOption(pointer optlist, const char *name, int deflt)
{
    return LookupBoolOption(optlist, name, deflt, FALSE);
}

/*
 * addNewOption() has the required property of replacing the option value
 * if the option is already present.
 */
pointer
xf86ReplaceIntOption(pointer optlist, const char *name, const int val)
{
    char *tmp;

    xasprintf(&tmp, "%i", val);
    if (tmp)
	return xf86AddNewOption(optlist, name, tmp);
    else
	return NULL;
}

pointer
xf86ReplaceRealOption(pointer optlist, const char *name, const double val)
{
    char *tmp;

    xasprintf(&tmp, "%f", val);
    if (tmp)
	return xf86AddNewOption(optlist, name, tmp);
    else
	return NULL;
}

pointer
xf86ReplaceBoolOption(pointer optlist, const char *name, const Bool val)
{
    return xf86AddNewOption(optlist,name,val?"True":"False");
}

pointer
xf86ReplaceStrOption(pointer optlist, const char *name, const char* val)
{
      return xf86AddNewOption(optlist,name,val);
}

pointer
xf86AddNewOption(pointer head, const char *name, const char *val)
{
    /* XXX These should actually be allocated in the parser library. */
    char *tmp = strdup(val);
    char *tmp_name = strdup(name);

    return xf86addNewOption(head, tmp_name, tmp);
}


pointer
xf86NewOption(char *name, char *value)
{
    return xf86newOption(name, value);
}


pointer
xf86NextOption(pointer list)
{
    return xf86nextOption(list);
}

pointer
xf86OptionListCreate(const char **options, int count, int used)
{
	return xf86optionListCreate(options, count, used);
}

pointer
xf86OptionListMerge(pointer head, pointer tail)
{
	return xf86optionListMerge(head, tail);
}

pointer
xf86OptionListDup(pointer opt)
{
	return xf86optionListDup(opt);
}

void
xf86OptionListFree(pointer opt)
{
	xf86optionListFree(opt);
}

char *
xf86OptionName(pointer opt)
{
	return xf86optionName(opt);
}

char *
xf86OptionValue(pointer opt)
{
	return xf86optionValue(opt);
}

void
xf86OptionListReport(pointer parm)
{
    XF86OptionPtr opts = parm;

    while(opts) {
	if (xf86optionValue(opts))
	    xf86ErrorFVerb(5, "\tOption \"%s\" \"%s\"\n",
			    xf86optionName(opts), xf86optionValue(opts));
	else
	    xf86ErrorFVerb( 5, "\tOption \"%s\"\n", xf86optionName(opts));
	opts = xf86nextOption(opts);
    }
}

void
xf86OptionListPrint(int verb, MessageType from, const char *prefix,
		    pointer parm)
{
    XF86OptionPtr opts = parm;
    const char *p;

    p = EMPTYIFNULL(prefix);
    while (opts) {
	if (xf86optionValue(opts))
	    xf86MsgVerb(from, verb, "%sOption \"%s\" \"%s\"\n", p,
			xf86optionName(opts), xf86optionValue(opts));
	else
	    xf86MsgVerb(from, verb, "%sOption \"%s\"\n", p,
			xf86optionName(opts));
	opts = xf86nextOption(opts);
    }
}

pointer
xf86FindOption(pointer options, const char *name)
{
    return xf86findOption(options, name);
}


char *
xf86FindOptionValue(pointer options, const char *name)
{
    return xf86findOptionValue(options, name);
}


void
xf86MarkOptionUsed(pointer option)
{
    if (option != NULL)
	((XF86OptionPtr)option)->opt_used = TRUE;
}


void
xf86MarkOptionUsedByName(pointer options, const char *name)
{
    XF86OptionPtr opt;

    opt = xf86findOption(options, name);
    if (opt != NULL)
	opt->opt_used = TRUE;
}

Bool
xf86CheckIfOptionUsed(pointer option)
{
    if (option != NULL)
	return ((XF86OptionPtr)option)->opt_used;
    else
	return FALSE;
}

Bool
xf86CheckIfOptionUsedByName(pointer options, const char *name)
{
    XF86OptionPtr opt;

    opt = xf86findOption(options, name);
    if (opt != NULL)
	return opt->opt_used;
    else
	return FALSE;
}

void
xf86ShowUnusedOptionsVerb(int scrnIndex, pointer options, int verb)
{
    XF86OptionPtr opt = options;

    while (opt) {
	if (opt->opt_name && !opt->opt_used) {
	    xf86DrvMsgVerb(scrnIndex, X_WARNING, verb,
			   "Option \"%s\" is not used\n", opt->opt_name);
	}
	opt = opt->list.next;
    }
}

void
xf86ShowUnusedOptions(int scrnIndex, pointer options)
{
    xf86ShowUnusedOptionsVerb(scrnIndex, options, X_LOG_DEFAULT_VERB);
}


static Bool
GetBoolValue(OptionInfoPtr p, const char *s)
{
    if (*s == '\0') {
	p->value.bool = TRUE;
    } else {
	if (xf86NameCmp(s, "1") == 0)
	    p->value.bool = TRUE;
	else if (xf86NameCmp(s, "on") == 0)
	    p->value.bool = TRUE;
	else if (xf86NameCmp(s, "true") == 0)
	    p->value.bool = TRUE;
	else if (xf86NameCmp(s, "yes") == 0)
	    p->value.bool = TRUE;
	else if (xf86NameCmp(s, "0") == 0)
	    p->value.bool = FALSE;
	else if (xf86NameCmp(s, "off") == 0)
	    p->value.bool = FALSE;
	else if (xf86NameCmp(s, "false") == 0)
	    p->value.bool = FALSE;
	else if (xf86NameCmp(s, "no") == 0)
	    p->value.bool = FALSE;
	else
	    return FALSE;
    }
    return TRUE;
}

static Bool
ParseOptionValue(int scrnIndex, pointer options, OptionInfoPtr p,
		 Bool markUsed)
{
    char *s, *end;
    Bool wasUsed = FALSE;

    if ((s = xf86findOptionValue(options, p->name)) != NULL) {
	if (markUsed) {
	    wasUsed = xf86CheckIfOptionUsedByName(options, p->name);
	    xf86MarkOptionUsedByName(options, p->name);
	}
	switch (p->type) {
	case OPTV_INTEGER:
	    if (*s == '\0') {
		xf86DrvMsg(scrnIndex, X_WARNING,
			   "Option \"%s\" requires an integer value\n",
			   p->name);
		p->found = FALSE;
	    } else {
		p->value.num = strtoul(s, &end, 0);
		if (*end == '\0') {
		    p->found = TRUE;
		} else {
		    xf86DrvMsg(scrnIndex, X_WARNING,
			       "Option \"%s\" requires an integer value\n",
			        p->name);
		    p->found = FALSE;
		}
	    }
	    break;
	case OPTV_STRING:
	    if (*s == '\0') {
		xf86DrvMsg(scrnIndex, X_WARNING,
			   "Option \"%s\" requires a string value\n",
			   p->name);
		p->found = FALSE;
	    } else {
		p->value.str = s;
		p->found = TRUE;
	    }
	    break;
	case OPTV_ANYSTR:
	    p->value.str = s;
	    p->found = TRUE;
	    break;
	case OPTV_REAL:	
	    if (*s == '\0') {
		xf86DrvMsg(scrnIndex, X_WARNING,
			   "Option \"%s\" requires a floating point value\n",
			   p->name);
		p->found = FALSE;
	    } else {
		p->value.realnum = strtod(s, &end);
		if (*end == '\0') {
		    p->found = TRUE;
		} else {
		    xf86DrvMsg(scrnIndex, X_WARNING,
			    "Option \"%s\" requires a floating point value\n",
			    p->name);
		    p->found = FALSE;
		}
	    }
	    break;
	case OPTV_BOOLEAN:
	    if (GetBoolValue(p, s)) {
		p->found = TRUE;
	    } else {
		xf86DrvMsg(scrnIndex, X_WARNING,
			   "Option \"%s\" requires a boolean value\n", p->name);
		p->found = FALSE;
	    }
	    break;
	case OPTV_FREQ:	
	    if (*s == '\0') {
		xf86DrvMsg(scrnIndex, X_WARNING,
			   "Option \"%s\" requires a frequency value\n",
			   p->name);
		p->found = FALSE;
	    } else {
		double freq = strtod(s, &end);
		int    units = 0;

		if (end != s) {
		    p->found = TRUE;
		    if (!xf86NameCmp(end, "Hz"))
			units = 1;
		    else if (!xf86NameCmp(end, "kHz") ||
			     !xf86NameCmp(end, "k"))
			units = 1000;
		    else if (!xf86NameCmp(end, "MHz") ||
			     !xf86NameCmp(end, "M"))
			units = 1000000;
		    else {
			xf86DrvMsg(scrnIndex, X_WARNING,
			    "Option \"%s\" requires a frequency value\n",
			    p->name);
			p->found = FALSE;
		    }
		    if (p->found)
			freq *= (double)units;
		} else {
		    xf86DrvMsg(scrnIndex, X_WARNING,
			    "Option \"%s\" requires a frequency value\n",
			    p->name);
		    p->found = FALSE;
		}
		if (p->found) {
		    p->value.freq.freq = freq;
		    p->value.freq.units = units;
		}
	    }
	    break;
	case OPTV_NONE:
	    /* Should never get here */
	    p->found = FALSE;
	    break;
	}
	if (p->found && markUsed) {
	    int verb = 2;
	    if (wasUsed)
		verb = 4;
	    xf86DrvMsgVerb(scrnIndex, X_CONFIG, verb, "Option \"%s\"", p->name);
	    if (!(p->type == OPTV_BOOLEAN && *s == 0)) {
		xf86ErrorFVerb(verb, " \"%s\"", s);
	    }
	    xf86ErrorFVerb(verb, "\n");
	}
    } else if (p->type == OPTV_BOOLEAN) {
	/* Look for matches with options with or without a "No" prefix. */
	char *n, *newn;
	OptionInfoRec opt;

	n = xf86NormalizeName(p->name);
	if (!n) {
	    p->found = FALSE;
	    return FALSE;
	}
	if (strncmp(n, "no", 2) == 0) {
	    newn = n + 2;
	} else {
	    xfree(n);
	    n = xalloc(strlen(p->name) + 2 + 1);
	    if (!n) {
		p->found = FALSE;
		return FALSE;
	    }
	    strcpy(n, "No");
	    strcat(n, p->name);
	    newn = n;
	}
	if ((s = xf86findOptionValue(options, newn)) != NULL) {
	    if (markUsed)
		xf86MarkOptionUsedByName(options, newn);
	    if (GetBoolValue(&opt, s)) {
		p->value.bool = !opt.value.bool;
		p->found = TRUE;
	    } else {
		xf86DrvMsg(scrnIndex, X_WARNING,
			   "Option \"%s\" requires a boolean value\n", newn);
		p->found = FALSE;
	    }
	} else {
	    p->found = FALSE;
	}
	if (p->found && markUsed) {
	    xf86DrvMsgVerb(scrnIndex, X_CONFIG, 2, "Option \"%s\"", newn);
	    if (*s != 0) {
		xf86ErrorFVerb(2, " \"%s\"", s);
	    }
	    xf86ErrorFVerb(2, "\n");
	}
	xfree(n);
    } else {
	p->found = FALSE;
    }
    return p->found;
}


void
xf86ProcessOptions(int scrnIndex, pointer options, OptionInfoPtr optinfo)
{
    OptionInfoPtr p;

    for (p = optinfo; p->name != NULL; p++) {
	ParseOptionValue(scrnIndex, options, p, TRUE);
    }
}


OptionInfoPtr
xf86TokenToOptinfo(const OptionInfoRec *table, int token)
{
    const OptionInfoRec *p, *match = NULL, *set = NULL;

    if (!table) {
	ErrorF("xf86TokenToOptinfo: table is NULL\n");
	return NULL;
    }

    for (p = table; p->token >= 0; p++) {
	if (p->token == token) {
	    match = p;
	    if (p->found)
		set = p;
	}
    }

    if (set)
	return (OptionInfoPtr)set;
    else if (match)
	return (OptionInfoPtr)match;
    else
	return NULL;
}


const char *
xf86TokenToOptName(const OptionInfoRec *table, int token)
{
    const OptionInfoRec *p;

    p = xf86TokenToOptinfo(table, token);
    return p->name;
}


Bool
xf86IsOptionSet(const OptionInfoRec *table, int token)
{
    OptionInfoPtr p;

    p = xf86TokenToOptinfo(table, token);
    return (p && p->found);
}


char *
xf86GetOptValString(const OptionInfoRec *table, int token)
{
    OptionInfoPtr p;

    p = xf86TokenToOptinfo(table, token);
    if (p && p->found)
	return p->value.str;
    else
	return NULL;
}


Bool
xf86GetOptValInteger(const OptionInfoRec *table, int token, int *value)
{
    OptionInfoPtr p;

    p = xf86TokenToOptinfo(table, token);
    if (p && p->found) {
	*value = p->value.num;
	return TRUE;
    } else
	return FALSE;
}


Bool
xf86GetOptValULong(const OptionInfoRec *table, int token, unsigned long *value)
{
    OptionInfoPtr p;

    p = xf86TokenToOptinfo(table, token);
    if (p && p->found) {
	*value = p->value.num;
	return TRUE;
    } else
	return FALSE;
}


Bool
xf86GetOptValReal(const OptionInfoRec *table, int token, double *value)
{
    OptionInfoPtr p;

    p = xf86TokenToOptinfo(table, token);
    if (p && p->found) {
	*value = p->value.realnum;
	return TRUE;
    } else
	return FALSE;
}


Bool
xf86GetOptValFreq(const OptionInfoRec *table, int token,
		  OptFreqUnits expectedUnits, double *value)
{
    OptionInfoPtr p;

    p = xf86TokenToOptinfo(table, token);
    if (p && p->found) {
	if (p->value.freq.units > 0) {
	    /* Units give, so the scaling is known. */
	    switch (expectedUnits) {
	    case OPTUNITS_HZ:
		*value = p->value.freq.freq;
		break;
	    case OPTUNITS_KHZ:
		*value = p->value.freq.freq / 1000.0;
		break;
	    case OPTUNITS_MHZ:
		*value = p->value.freq.freq / 1000000.0;
		break;
	    }
	} else {
	    /* No units given, so try to guess the scaling. */
	    switch (expectedUnits) {
	    case OPTUNITS_HZ:
		*value = p->value.freq.freq;
		break;
	    case OPTUNITS_KHZ:
		if (p->value.freq.freq > 1000.0)
		    *value = p->value.freq.freq / 1000.0;
		else
		    *value = p->value.freq.freq;
		break;
	    case OPTUNITS_MHZ:
		if (p->value.freq.freq > 1000000.0)
		    *value = p->value.freq.freq / 1000000.0;
		else if (p->value.freq.freq > 1000.0)
		    *value = p->value.freq.freq / 1000.0;
		else
		    *value = p->value.freq.freq;
	    }
	}
	return TRUE;
    } else
	return FALSE;
}


Bool
xf86GetOptValBool(const OptionInfoRec *table, int token, Bool *value)
{
    OptionInfoPtr p;

    p = xf86TokenToOptinfo(table, token);
    if (p && p->found) {
	*value = p->value.bool;
	return TRUE;
    } else
	return FALSE;
}


Bool
xf86ReturnOptValBool(const OptionInfoRec *table, int token, Bool def)
{
    OptionInfoPtr p;

    p = xf86TokenToOptinfo(table, token);
    if (p && p->found) {
	return p->value.bool;
    } else
	return def;
}


int
xf86NameCmp(const char *s1, const char *s2)
{
    return xf86nameCompare(s1, s2);
}

char *
xf86NormalizeName(const char *s)
{
    char *ret, *q;
    const char *p;

    if (s == NULL)
	return NULL;

    ret = xnfalloc(strlen(s) + 1);
    for (p = s, q = ret; *p != 0; p++) {
	switch (*p) {
	case '_':
	case ' ':
	case '\t':
	    continue;
	default:
	    if (isupper(*p))
		*q++ = tolower(*p);
	    else
		*q++ = *p;
	}
    }
    *q = '\0';
    return ret;
}
