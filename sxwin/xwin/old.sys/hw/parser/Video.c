/* $XFree86: xc/programs/Xserver/hw/xfree86/parser/Video.c,v 1.15 2005/01/26 05:31:50 dawes Exp $ */
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
 * Copyright (c) 1997-2005 by The XFree86 Project, Inc.
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
 * Copyright ? 2004, 2005 X-Oz Technologies.
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
 * 
 */

/* View/edit this file with tab stops set to 4 */

#include "xf86Parser.h"
#include "xf86tokens.h"
#include "Configint.h"

extern LexRec val;

static xf86ConfigSymTabRec VideoPortTab[] =
{
	{ENDSUBSECTION, "endsubsection"},
	{IDENTIFIER, "identifier"},
	{OPTION, "option"},
	{-1, ""},
};

#define CLEANUP xf86freeVideoPortList

XF86ConfVideoPortPtr
xf86parseVideoPortSubSection (char *name)
{
	int has_ident = FALSE;
	int token;
	parsePrologue (XF86ConfVideoPortPtr, XF86ConfVideoPortRec)

	ptr->vp_identifier = name;

	while ((token = xf86getToken (VideoPortTab)) != ENDSUBSECTION)
	{
		switch (token)
		{
		case COMMENT:
			ptr->vp_comment = xf86addComment(ptr->vp_comment, val.str);
			break;
		case IDENTIFIER:
			if (xf86getSubToken (&(ptr->vp_comment)) != STRING)
				Error (QUOTE_MSG, "Identifier");
			if (has_ident == TRUE)
				Error (MULTIPLE_MSG, "Identifier");
			TestFree(ptr->vp_identifier);
			ptr->vp_identifier = val.str;
			has_ident = TRUE;
			break;
		case OPTION:
			ptr->vp_option_lst = xf86parseOption(ptr->vp_option_lst);
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
	printf ("VideoPort subsection parsed\n");
#endif

	return ptr;
}

#undef CLEANUP

static xf86ConfigSymTabRec VideoAdaptorTab[] =
{
	{ENDSECTION, "endsection"},
	{IDENTIFIER, "identifier"},
	{VENDOR, "vendorname"},
	{BOARD, "boardname"},
	{BUSID, "busid"},
	{DRIVER, "driver"},
	{OPTION, "option"},
	{SUBSECTION, "subsection"},
	{-1, ""},
};

#define CLEANUP xf86freeVideoAdaptorList

XF86ConfVideoAdaptorPtr
xf86parseVideoAdaptorSection (void)
{
	int has_ident = FALSE;
	int token;

	parsePrologue (XF86ConfVideoAdaptorPtr, XF86ConfVideoAdaptorRec)

	while ((token = xf86getToken (VideoAdaptorTab)) != ENDSECTION)
	{
		switch (token)
		{
		case COMMENT:
			ptr->va_comment = xf86addComment(ptr->va_comment, val.str);
			break;
		case IDENTIFIER:
			if (xf86getSubToken (&(ptr->va_comment)) != STRING)
				Error (QUOTE_MSG, "Identifier");
			ptr->va_identifier = val.str;
			if (has_ident == TRUE)
				Error (MULTIPLE_MSG, "Identifier");
			has_ident = TRUE;
			break;
		case VENDOR:
			if (xf86getSubToken (&(ptr->va_comment)) != STRING)
				Error (QUOTE_MSG, "Vendor");
			ptr->va_vendor = val.str;
			break;
		case BOARD:
			if (xf86getSubToken (&(ptr->va_comment)) != STRING)
				Error (QUOTE_MSG, "Board");
			ptr->va_board = val.str;
			break;
		case BUSID:
			if (xf86getSubToken (&(ptr->va_comment)) != STRING)
				Error (QUOTE_MSG, "BusID");
			ptr->va_busid = val.str;
			break;
		case DRIVER:
			if (xf86getSubToken (&(ptr->va_comment)) != STRING)
				Error (QUOTE_MSG, "Driver");
			ptr->va_driver = val.str;
			break;
		case OPTION:
			ptr->va_option_lst = xf86parseOption(ptr->va_option_lst);
			break;
		case SUBSECTION:
			if (xf86getSubToken (&(ptr->va_comment)) != STRING)
				Error (QUOTE_MSG, "SubSection");
			{
				{
					XF86ConfVideoPortPtr p =
						xf86parseVideoPortSubSection(val.str);
					if (!p) {
						CLEANUP(ptr);
						return NULL;
					} else {
						ptr->va_port_lst =
							(XF86ConfVideoPortPtr)xf86addListItem((glp)ptr->va_port_lst, (glp)p);
					}
				}
			}
			break;

		case EOF_TOKEN:
			Error (UNEXPECTED_EOF_MSG, NULL);
			break;
		default:
			Error (INVALID_KEYWORD_MSG, xf86tokenString ());
			break;
		}
	}

	if (!has_ident)
		Error (NO_IDENT_MSG, NULL);

#ifdef DEBUG
	printf ("VideoAdaptor section parsed\n");
#endif

	return ptr;
}

void
xf86printVideoAdaptorSection (FILE * cf, XF86ConfVideoAdaptorPtr ptr)
{
	XF86ConfVideoPortPtr pptr;

	while (ptr)
	{
		fprintf (cf, "Section \"VideoAdaptor\"\n");
		if (ptr->va_comment)
			fprintf (cf, "%s", ptr->va_comment);
		if (ptr->va_identifier)
			fprintf (cf, "\tIdentifier  \"%s\"\n", ptr->va_identifier);
		if (ptr->va_vendor)
			fprintf (cf, "\tVendorName  \"%s\"\n", ptr->va_vendor);
		if (ptr->va_board)
			fprintf (cf, "\tBoardName   \"%s\"\n", ptr->va_board);
		if (ptr->va_busid)
			fprintf (cf, "\tBusID       \"%s\"\n", ptr->va_busid);
		if (ptr->va_driver)
			fprintf (cf, "\tDriver      \"%s\"\n", ptr->va_driver);
		xf86printOptionList(cf, ptr->va_option_lst, 1);
		for (pptr = ptr->va_port_lst; pptr; pptr = pptr->list.next)
		{
			fprintf (cf, "\tSubSection \"VideoPort\"\n");
			if (pptr->vp_comment)
				fprintf (cf, "%s", pptr->vp_comment);
			if (pptr->vp_identifier)
				fprintf (cf, "\t\tIdentifier \"%s\"\n", pptr->vp_identifier);
			xf86printOptionList(cf, pptr->vp_option_lst, 2);
			fprintf (cf, "\tEndSubSection\n");
		}
		fprintf (cf, "EndSection\n\n");
		ptr = ptr->list.next;
	}

}

void
xf86freeVideoAdaptorList (XF86ConfVideoAdaptorPtr ptr)
{
	XF86ConfVideoAdaptorPtr prev;

	while (ptr)
	{
		TestFree (ptr->va_identifier);
		TestFree (ptr->va_vendor);
		TestFree (ptr->va_board);
		TestFree (ptr->va_busid);
		TestFree (ptr->va_driver);
		TestFree (ptr->va_fwdref);
		TestFree (ptr->va_comment);
		xf86freeVideoPortList (ptr->va_port_lst);
		xf86optionListFree (ptr->va_option_lst);
		prev = ptr;
		ptr = ptr->list.next;
		xf86conffree (prev);
	}
}

void
xf86freeVideoPortList (XF86ConfVideoPortPtr ptr)
{
	XF86ConfVideoPortPtr prev;

	while (ptr)
	{
		TestFree (ptr->vp_identifier);
		TestFree (ptr->vp_comment);
		xf86optionListFree (ptr->vp_option_lst);
		prev = ptr;
		ptr = ptr->list.next;
		xf86conffree (prev);
	}
}

XF86ConfVideoAdaptorPtr
xf86findVideoAdaptor (const char *ident, XF86ConfVideoAdaptorPtr p)
{
	while (p)
	{
		if (xf86nameCompare (ident, p->va_identifier) == 0)
			return (p);

		p = p->list.next;
	}
	return (NULL);
}
