/* $XFree86: xc/programs/Xserver/hw/xfree86/parser/Flags.c,v 1.28 2006/08/09 20:53:15 dawes Exp $ */
/* 
 * 
 * Copyright (c) 1997  Metro Link Incorporated
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"), 
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 * THE X CONSORTIUM BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF
 * OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 * 
 * Except as contained in this notice, the name of the Metro Link shall not be
 * used in advertising or otherwise to promote the sale, use or other dealings
 * in this Software without prior written authorization from Metro Link.
 * 
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


/* View/edit this file with tab stops set to 4 */

#include "xf86Parser.h"
#include "xf86tokens.h"
#include "Configint.h"
#include <math.h>

extern LexRec val;

static xf86ConfigSymTabRec ServerFlagsTab[] =
{
	{ENDSECTION, "endsection"},
	{IDENTIFIER, "identifier"},
	{NOTRAPSIGNALS, "notrapsignals"},
	{DONTZAP, "dontzap"},
	{DONTZOOM, "dontzoom"},
	{DISABLEVIDMODE, "disablevidmodeextension"},
	{ALLOWNONLOCAL, "allownonlocalxvidtune"},
	{DISABLEMODINDEV, "disablemodindev"},
	{MODINDEVALLOWNONLOCAL, "allownonlocalmodindev"},
	{ALLOWMOUSEOPENFAIL, "allowmouseopenfail"},
	{OPTION, "option"},
	{BLANKTIME, "blanktime"},
	{STANDBYTIME, "standbytime"},
	{SUSPENDTIME, "suspendtime"},
	{OFFTIME, "offtime"},
	{DEFAULTLAYOUT, "defaultserverlayout"},
	{-1, ""},
};

#define CLEANUP xf86freeFlagsList

XF86ConfFlagsPtr
xf86parseFlagsSection (void)
{
	int has_ident = FALSE;
	int token;
	parsePrologue (XF86ConfFlagsPtr, XF86ConfFlagsRec)

	while ((token = xf86getToken (ServerFlagsTab)) != ENDSECTION)
	{
		int hasvalue = FALSE;
		int strvalue = FALSE;
		int tokentype;
		switch (token)
		{
		case COMMENT:
			ptr->flg_comment = xf86addComment(ptr->flg_comment, val.str);
			break;
		case IDENTIFIER:
			if (xf86getSubToken (&(ptr->flg_comment)) != STRING)
				Error (QUOTE_MSG, "Identifier");
			if (has_ident)
				Error (MULTIPLE_MSG, "Identifier");
			ptr->flg_identifier = xf86configStrdup(val.str);
			has_ident = TRUE;
			break;
			/* 
			 * These old keywords are turned into standard generic options.
			 * We fall through here on purpose.
			 */
		case DEFAULTLAYOUT:
			strvalue = TRUE;
		case BLANKTIME:
		case STANDBYTIME:
		case SUSPENDTIME:
		case OFFTIME:
			hasvalue = TRUE;
		case NOTRAPSIGNALS:
		case DONTZAP:
		case DONTZOOM:
		case DISABLEVIDMODE:
		case ALLOWNONLOCAL:
		case DISABLEMODINDEV:
		case MODINDEVALLOWNONLOCAL:
		case ALLOWMOUSEOPENFAIL:
			{
				int i = 0;
				while (ServerFlagsTab[i].token != -1)
				{
					char *tmp;

					if (ServerFlagsTab[i].token == token)
					{
						char *valstr = NULL;
						tmp = ServerFlagsTab[i].name;
						if (hasvalue)
						{
							tokentype = xf86getSubToken(&(ptr->flg_comment));
							if (strvalue) {
								if (tokentype != STRING)
									Error (QUOTE_MSG, tmp);
								valstr = xf86configStrdup(val.str);
							} else {
								if (tokentype != NUMBER)
									Error (NUMBER_MSG, tmp);
								xf86configAsprintf(&valstr, "%d", val.num);
							}
						}
						ptr->flg_option_lst = xf86addNewOption
							(ptr->flg_option_lst, tmp, valstr);
						xf86conffree(valstr);
					}
					i++;
				}
			}
			break;
		case OPTION:
			ptr->flg_option_lst = xf86parseOption(ptr->flg_option_lst);
			break;

		case EOF_TOKEN:
			Error (UNEXPECTED_EOF_MSG, NULL);
			break;
		default:
			Error (INVALID_KEYWORD_MSG, xf86tokenString ());
			break;
		}
	}

#ifdef DEBUG
	printf ("Flags section parsed\n");
#endif

	return ptr;
}

#undef CLEANUP

void
xf86printServerFlagsSection (FILE * f, XF86ConfFlagsPtr ptr)
{
	if ((!ptr) || (!ptr->flg_option_lst))
		return;
	while (ptr) {
		fprintf (f, "Section \"ServerFlags\"\n");
		if (ptr->flg_comment)
			fprintf (f, "%s", ptr->flg_comment);
		if (ptr->flg_identifier)
			fprintf (f, "\tIdentifier   \"%s\"\n", ptr->flg_identifier);
		xf86printOptionList(f, ptr->flg_option_lst, 1);
		fprintf (f, "EndSection\n\n");
		ptr = ptr->list.next;
	}
}

static XF86OptionPtr
addNewOption2 (XF86OptionPtr head, const char *name, const char *val, int used)
{
	XF86OptionPtr new, old = NULL;
	

	/* Don't allow duplicates */
 	if (head != NULL && (old = xf86findOption(head, name)) != NULL) {
 		new = old;
		xf86conffree(old->opt_name);
		TestFree(old->opt_val);
		TestFree(old->opt_comment);
 	} else {
		new = xf86confcalloc (1, sizeof (XF86OptionRec));
 		new->list.next = NULL;
 	}
 	new->opt_name = xf86configStrdup(name);
 	new->opt_val = xf86configStrdup(val);
 	new->opt_used = used;
	
  	if (old == NULL)
		return ((XF86OptionPtr) xf86addListItem ((glp) head, (glp) new));
 	else 
 		return head;
}

/*
 * Duplicate name/value strings with config's alloc.  This is for internal
 * and public use.
 */
XF86OptionPtr
xf86addNewOption (XF86OptionPtr head, const char *name, const char *val)
{
	return addNewOption2(head, name, val, 0);
}

void
xf86freeFlagsList (XF86ConfFlagsPtr ptr)
{
	XF86ConfFlagsPtr prev;

	while (ptr) {
		xf86optionListFree (ptr->flg_option_lst);
		TestFree(ptr->flg_comment);
		TestFree(ptr->flg_identifier);
		prev = ptr;
		ptr = ptr->list.next;
		xf86conffree (prev);
	}
}

XF86OptionPtr
xf86optionListDup (XF86OptionPtr opt)
{
	XF86OptionPtr newopt = NULL;

	while (opt)
	{
		newopt = xf86addNewOption(newopt, xf86configStrdup(opt->opt_name), 
					  xf86configStrdup(opt->opt_val));
		newopt->opt_used = opt->opt_used;
		if (opt->opt_comment)
			newopt->opt_comment = xf86configStrdup(opt->opt_comment);
		opt = opt->list.next;
	}
	return newopt;
}

void
xf86optionListFree (XF86OptionPtr opt)
{
	XF86OptionPtr prev;

	while (opt)
	{
		TestFree (opt->opt_name);
		TestFree (opt->opt_val);
		TestFree (opt->opt_comment);
		prev = opt;
		opt = opt->list.next;
		xf86conffree (prev);
	}
}

char *
xf86optionName(XF86OptionPtr opt)
{
	if (opt)
		return opt->opt_name;
	return 0;
}

char *
xf86optionValue(XF86OptionPtr opt)
{
	if (opt)
		return opt->opt_val;
	return 0;
}

XF86OptionPtr
xf86newOption(const char *name, const char *value)
{
	XF86OptionPtr opt;

	opt = xf86confcalloc(1, sizeof (XF86OptionRec));
	if (!opt)
		return NULL;

	opt->opt_used = 0;
	opt->list.next = 0;
	opt->opt_name = xf86configStrdup(name);
	opt->opt_val = xf86configStrdup(value);

	return opt;
}

XF86OptionPtr
xf86nextOption(XF86OptionPtr list)
{
	if (!list)
		return NULL;
	return list->list.next;
}

/*
 * This function searches the given option list for the named option and
 * returns a pointer to the option rec if found.  The last occurrence of the
 * named option is returned.  If not found, it returns NULL.
 */

XF86OptionPtr
xf86findOption (XF86OptionPtr list, const char *name)
{
	while (list)
	{
		if (xf86nameCompare (list->opt_name, name) == 0)
			return (list);
		list = list->list.next;
	}
	return (NULL);
}

/*
 * this function searches the given option list for the named option. If
 * found and the option has a parameter, a pointer to the parameter is
 * returned.  If the option does not have a parameter an empty string is
 * returned.  If the option is not found, a NULL is returned.
 */

char *
xf86findOptionValue (XF86OptionPtr list, const char *name)
{
	XF86OptionPtr p = xf86findOption (list, name);

	if (p)
	{
		if (p->opt_val)
			return (p->opt_val);
		else
			return "";
	}
	return (NULL);
}

XF86OptionPtr
xf86optionListCreate( const char **options, int count, int used )
{
	XF86OptionPtr p = NULL;
	char *t1, *t2;
	int i;

	if (count == -1)
	{
		for (count = 0; options[count]; count++)
			;
	}
	if( (count % 2) != 0 )
	{
		fprintf( stderr, "xf86optionListCreate: count must be an even number.\n" );
		return (NULL);
	}
	for (i = 0; i < count; i += 2)
	{
		/* can't use strdup because it calls malloc */
		t1 = xf86confmalloc (sizeof (char) *
				(strlen (options[i]) + 1));
		strcpy (t1, options[i]);
		t2 = xf86confmalloc (sizeof (char) *
				(strlen (options[i + 1]) + 1));
		strcpy (t2, options[i + 1]);
		p = addNewOption2 (p, t1, t2, used);
	}

	return (p);
}

/* the 2 given lists are merged. If an option with the same name is present in
 * both, the option from the user list - specified in the second argument -
 * is used. The end result is a single valid list of options. Duplicates
 * are freed, and the original lists are no longer guaranteed to be complete.
 */
XF86OptionPtr
xf86optionListMerge (XF86OptionPtr head, XF86OptionPtr tail)
{
	XF86OptionPtr a, b, ap = NULL, bp = NULL;

	a = tail;
	b = head;
	while (tail && b) {
		if (xf86nameCompare (a->opt_name, b->opt_name) == 0) {
			if (b == head)
				head = a;
			else
				bp->list.next = a;
			if (a == tail)
				tail = a->list.next;
			else
				ap->list.next = a->list.next;
			a->list.next = b->list.next;
			b->list.next = NULL;
			xf86optionListFree (b);
			b = a->list.next;
			bp = a;
			a = tail;
			ap = NULL;
		} else {
			ap = a;
			if (!(a = a->list.next)) {
				a = tail;
				bp = b;
				b = b->list.next;
				ap = NULL;
			}
		}
	}

	if (head) {
		for (a = head; a->list.next; a = a->list.next)
			;
		a->list.next = tail;
	} else 
		head = tail;

	return (head);
}

char *
xf86uLongToString(unsigned long i)
{
	char *s = NULL;

	xf86configAsprintf(&s, "%lu", i);
	return s;
}

void
xf86debugListOptions(XF86OptionPtr Options)
{
	while (Options) {
		ErrorF("Option: %s Value: %s\n",Options->opt_name,Options->opt_val);
		Options = Options->list.next;
	}
}

XF86OptionPtr
xf86parseOption(XF86OptionPtr head)
{
	XF86OptionPtr option;
	char *name, *value = NULL, *comment = NULL;
	const char *comment2 = NULL;
	int token;

	if ((token = xf86getSubToken(&comment)) != STRING) {
		xf86parseError(BAD_OPTION_MSG, NULL);
		if (comment)
			xf86conffree(comment);
		return (head);
	}

	name = xf86configStrdup(val.str);
	if ((token = xf86getSubToken(&comment)) == STRING) {
		value = xf86configStrdup(val.str);
		if ((token = xf86getToken(NULL)) == COMMENT)
			comment2 = val.str;
		else
			xf86unGetToken(token);
	} else if (token == COMMENT)
		comment2 = val.str;
	else
		xf86unGetToken(token);

	head = xf86addNewOption(head, name, value);
	option = xf86findOption(head, name);
	xf86conffree(name);
	TestFree(value);
	option->opt_comment = comment;
	if (comment2)
		option->opt_comment = xf86addComment(option->opt_comment, comment2);

	return (head);
}

void
xf86printOptionList(FILE *fp, XF86OptionPtr list, int tabs)
{
	int i;

	if (!list)
		return;
	while (list) {
		for (i = 0; i < tabs; i++)
			fputc('\t', fp);
		if (list->opt_val)
			fprintf(fp, "Option	    \"%s\" \"%s\"", list->opt_name, list->opt_val);
		else
			fprintf(fp, "Option	    \"%s\"", list->opt_name);
		if (list->opt_comment)
			fprintf(fp, "%s", list->opt_comment);
		else
			fputc('\n', fp);
		list = list->list.next;
	}
}
