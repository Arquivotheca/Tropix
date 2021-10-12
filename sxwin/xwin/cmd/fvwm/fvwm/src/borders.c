/*
 ****************************************************************
 *								*
 *			src/borders.c				*
 *								*
 *	Desenha as bordas das janelas				*
 *								*
 *	Versão	3.0.0, de 11.09.97				*
 *		4.4.0, de 20.03.03				*
 *								*
 *	Módulo: cmd/fvwm					*
 *		X Windows - Clientes				*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright (c) 2003 TROPIX Technologies Ltd.	*
 * 								*
 ****************************************************************
 */

/****************************************************************************
 * This module is all original code 
 * by Rob Nation 
 * Copyright 1993, Robert Nation
 *     You may use this code for any purpose, as long as the original
 *     copyright remains in the source code and all documentation
 ****************************************************************************/


/***********************************************************************
 *
 * fvwm window border drawing code
 *
 ***********************************************************************/

#include "../global.h"

#include <stdio.h>
#include <signal.h>
#include <string.h>

#include "fvwm.h"
#include "menus.h"
#include "misc.h"
#include "parse.h"
#include "screen.h"
#include "module.h"

#ifdef SHAPE
#include <X11/extensions/shape.h>
#endif

void DrawLinePattern(Window win,
                     GC ReliefGC,
                     GC ShadowGC,
                     int num_coords,
                     int *x_coord, 
                     int *y_coord,
                     int *line_style,
                     int th);

/* macro to change window background color/pixmap */
#define ChangeWindowColor(window) {\
        if(NewColor)\
        {\
          XChangeWindowAttributes(dpy,window,valuemask, &attributes);\
          XClearWindow(dpy,window);\
        }\
      }

extern Window PressedW;
XGCValues Globalgcv;
ulong Globalgcm;
/****************************************************************************
 *
 * Redraws the windows borders
 *
 ****************************************************************************/
void SetBorder (FVWMWIN *t, Bool onoroff,Bool force,Bool Mapped, 
		Window expose_win)
{
  int y, i, x;
  GC ReliefGC,ShadowGC;
  Pixel BorderColor,BackColor;
  Pixmap BackPixmap,TextColor;
  Bool NewColor = False;
  XSetWindowAttributes attributes;
  ulong valuemask;
  static unsigned int corners[4];
  Window w;

  corners[0] = TOP_HILITE | LEFT_HILITE;
  corners[1] = TOP_HILITE | RIGHT_HILITE;
  corners[2] = BOTTOM_HILITE | LEFT_HILITE;
  corners[3] = BOTTOM_HILITE | RIGHT_HILITE;
  
  if(!t)
    return;
  
  if (onoroff) 
  {
    /* don't re-draw just for kicks */
    if((!force)&&(Scr.Hilite == t))
      return;
      
      
    if(Scr.Hilite != t)
      NewColor = True;
      
    /* make sure that the previously highlighted window got unhighlighted */
    if((Scr.Hilite != t)&&(Scr.Hilite != NULL))
      SetBorder(Scr.Hilite,False,False,True,None);
      
    /* set the keyboard focus */
    if((Mapped)&&(t->flags&MAPPED)&&(Scr.Hilite != t))
      w = t->w;
    else if((t->flags&ICONIFIED)&&
            (Scr.Hilite !=t)&&(!(t->flags &SUPPRESSICON)))
      w = t->icon_w;
    Scr.Hilite = t;
      
    TextColor = Scr.HiColors.fore;
    BackPixmap= Scr.gray_pixmap;
    BackColor = Scr.HiColors.back;
    ReliefGC = Scr.HiReliefGC;
    ShadowGC = Scr.HiShadowGC;
    BorderColor = Scr.HiRelief.back;
  }
  else
  {
    /* don't re-draw just for kicks */
    if((!force)&&(Scr.Hilite != t))
      return;
      
    if(Scr.Hilite == t)
    {
      Scr.Hilite = NULL;
      NewColor = True;
    }
      
    TextColor =t->TextPixel;
    BackPixmap = Scr.light_gray_pixmap;
    if(t->flags & STICKY)
      BackPixmap = Scr.sticky_gray_pixmap;
    BackColor = t->BackPixel;
    Globalgcv.foreground = t->ReliefPixel;
    Globalgcm = GCForeground;
    XChangeGC(dpy,Scr.ScratchGC1,Globalgcm,&Globalgcv); 
    ReliefGC = Scr.ScratchGC1;
      
    Globalgcv.foreground = t->ShadowPixel;
    XChangeGC(dpy,Scr.ScratchGC2,Globalgcm,&Globalgcv); 
    ShadowGC = Scr.ScratchGC2;
    BorderColor = t->ShadowPixel;
  }
  
  if(t->flags & ICONIFIED)
  {
    DrawIconWindow(t);
    return;
  }
  
  valuemask = CWBorderPixel;
  attributes.border_pixel = BorderColor;
  if(Scr.d_depth < 2)
  {
    attributes.background_pixmap = BackPixmap;
    valuemask |= CWBackPixmap;
  }
  else
  {
    attributes.background_pixel = BackColor;
    valuemask |= CWBackPixel;
  }
  
  if(t->flags & (TITLE|BORDER))
  {
    XSetWindowBorder(dpy,t->Parent,BorderColor);
    XSetWindowBorder(dpy,t->frame,BorderColor);
  }
  if(t->flags & TITLE)
  {
    Picture pbf;
    int a;

    ChangeWindowColor(t->title_w);
    for(i=0;i<Scr.nr_left_buttons;i++)
    {
      if(t->left_w[i] != None)
      {
	int button=i*2+1;
        ChangeWindowColor(t->left_w[i]);
        if(flush_expose(t->left_w[i])||(expose_win == t->left_w[i])||
           (expose_win == None))
        {	
          RelieveWindow(t,t->left_w[i],0,0,t->title_height,
                        t->title_height,
                        (PressedW==t->left_w[i]?ShadowGC:ReliefGC), 
                        (PressedW==t->left_w[i]?ReliefGC:ShadowGC), 
                        BOTTOM_HILITE);

	  switch (Scr.button_style[button]) {

	  case SimpleButton:
	    /* draw nothing */
	    break;
	    
	  case NormalButton:
	    DrawLinePattern(t->left_w[i],ReliefGC,ShadowGC,
                          Scr.left_num_coords[i],
                          Scr.left_x_coords[i], Scr.left_y_coords[i],
                          Scr.left_line_style[i],
                          t->title_height);
	    break;

	  case PixmapButton:
	  case FullPixmapButton:
	    if (PressedW==t->left_w[i]) 
	      pbf=*Scr.pixmap_button[button].down;
	    else
	      pbf=*Scr.pixmap_button[button].up;
	    
	    /* clear area in case of transparency artifacts */
	    if (Scr.button_style[button] == FullPixmapButton) 
	    {
	      /* button face max width/height */
	      a = t->title_height;
	      x=(a - pbf.width) / 2; if (x < 0) x = 0; 
	      y=(a - pbf.height) / 2; if (y < 0) y = 0;
	      if (pbf.width > a) pbf.width = a; 
	      if (pbf.height > a) pbf.height = a;
	      XClearArea(dpy, t->left_w[i], 0, 0, a + 2, a + 2, False);
	    }
	    else
	    {
	      /* button face max width/height */
	      a = t->title_height - 2;
	      x=(a - pbf.width) / 2 + 1; if (x < 1) x = 1; 
	      y=(a - pbf.height) / 2 + 1; if (y < 1) y = 1;
	      if (pbf.width >= a) pbf.width = a - 1; 
	      if (pbf.height >= a) pbf.height = a - 1;
	      XClearArea(dpy, t->left_w[i], 1, 1, a - 1, a - 1, False);
	    }

	    /* set transparency mask */
	    XSetClipMask(dpy, Scr.ButtonMaskGC, pbf.mask);
	    XSetClipOrigin(dpy, Scr.ButtonMaskGC, x, y);

	    /* draw the new button face */
	    XCopyArea(dpy, pbf.picture, t->left_w[i], Scr.ButtonMaskGC,
		      0, 0, pbf.width, pbf.height, x, y);
	    break;

	  default:
	    {
	      fvwm_msg(ERR,"SetBorder",
                       "Internal inconsistency: unknown button type [%d]",
                       button);
	    }
	    break;
	  }
	}
      }
    }
    for(i=0;i<Scr.nr_right_buttons;i++)
    {
      if(t->right_w[i] != None)
      {
	int button=2*(i+1)%10;
        ChangeWindowColor(t->right_w[i]);
        if(flush_expose(t->right_w[i])||(expose_win==t->right_w[i])||
           (expose_win == None))
        {
          GC sgc,rgc;
	  
          sgc=ShadowGC;
          rgc=ReliefGC;
          if((t->flags & MWMButtons)&&(!i)&&(t->flags&MAXIMIZED))
	  {
	    sgc = ReliefGC;
            rgc = ShadowGC;
          }
          RelieveWindow(t,t->right_w[i],0,0,t->title_height,
                        t->title_height,
                        (PressedW==t->right_w[i]
                         ?ShadowGC:ReliefGC),
                        (PressedW==t->right_w[i]
                         ?ReliefGC:ShadowGC), 
                        BOTTOM_HILITE);

	  switch (Scr.button_style[button]) {
	    
	  case SimpleButton:
	    /* draw nothing */
	    break;

	  case NormalButton:
	    DrawLinePattern(t->right_w[i],rgc,sgc,
			    Scr.right_num_coords[i],
			    Scr.right_x_coords[i], Scr.right_y_coords[i],
			    Scr.right_line_style[i],
			    t->title_height);
	    break;
	    
	  case PixmapButton:
	  case FullPixmapButton:
	    if (PressedW==t->right_w[i]) 
	      pbf=*Scr.pixmap_button[button].down;
	    else
	      pbf=*Scr.pixmap_button[button].up;
	    
	    /* clear area in case of transparency artifacts */
	    if (Scr.button_style[button] == FullPixmapButton) 
	    {
	      /* button face max width/height */
	      a = t->title_height;
	      x=(a - pbf.width) / 2; if (x < 0) x = 0; 
	      y=(a - pbf.height) / 2; if (y < 0) y = 0;
	      if (pbf.width > a) pbf.width = a; 
	      if (pbf.height > a) pbf.height = a;
	      XClearArea(dpy, t->right_w[i], 0, 0, a + 2, a + 2, False);
	    }
	    else
	    {
	      /* button face max width/height */
	      a = t->title_height - 2;
	      x=(a - pbf.width) / 2 + 1; if (x < 1) x = 1; 
	      y=(a - pbf.height) / 2 + 1; if (y < 1) y = 1;
	      if (pbf.width >= a) pbf.width = a - 1; 
	      if (pbf.height >= a) pbf.height = a - 1;
	      XClearArea(dpy, t->right_w[i], 1, 1, a - 1, a - 1, False);
	    }

	    /* set transparency mask */
	    XSetClipMask(dpy, Scr.ButtonMaskGC, pbf.mask);
	    XSetClipOrigin(dpy, Scr.ButtonMaskGC, x, y);

	    /* draw the new button face */
	    XCopyArea(dpy, pbf.picture, t->right_w[i], Scr.ButtonMaskGC,
		      0, 0, pbf.width, pbf.height, x, y);
	    break;

	  default:
	    {
	      fvwm_msg(ERR,"SetBorder",
                       "Internal inconsistency: unknown button type [%d]",
                       button);
	    }
	    break;
	  }
        }
      } 
    }
    SetTitleBar(t,onoroff, False);
    
  }
      
  if(t->flags & BORDER)
  {
    /* draw relief lines */
    y= t->frame_height - 2*t->corner_width;
    x = t->frame_width-  2*t->corner_width +t->bw;
      
    for(i=0;i<4;i++)
    {
      ChangeWindowColor(t->sides[i]);
      if((flush_expose (t->sides[i]))||(expose_win == t->sides[i])||
         (expose_win == None))
      {
        GC sgc,rgc;
	      
        sgc=ShadowGC;
        rgc=ReliefGC;
        if(!(t->flags & MWMButtons)&&(PressedW == t->sides[i]))
        {
          sgc = ReliefGC;
          rgc = ShadowGC;
        }
        /* index    side
         * 0        TOP
         * 1        RIGHT
         * 2        BOTTOM
         * 3        LEFT
         */
	      
        RelieveWindow(t,t->sides[i],0,0,
                      ((i%2)?t->boundary_width:x),
                      ((i%2)?y:t->boundary_width),
                      rgc, sgc, (0x0001<<i));
      }
      ChangeWindowColor(t->corners[i]);
      if((flush_expose(t->corners[i]))||(expose_win==t->corners[i])||
         (expose_win == None))
      {
        GC rgc,sgc;
	      
        rgc = ReliefGC;
        sgc = ShadowGC;
        if(!(t->flags & MWMButtons)&&(PressedW == t->corners[i]))
        {
          sgc = ReliefGC;
          rgc = ShadowGC;
        }
        RelieveWindow(t,t->corners[i],0,0,t->corner_width,
                      ((i/2)?t->corner_width+t->bw:t->corner_width),
                      rgc,sgc, corners[i]);
        if(t->boundary_width > 1)
          RelieveParts(t,i,((i/2)?rgc:sgc),((i%2)?rgc:sgc));
        else
          RelieveParts(t,i,((i/2)?sgc:sgc),((i%2)?sgc:sgc));
      }
    }
  }
  else      /* no decorative border */
  {
    /* for mono - put a black border on 
     * for color, make it the color of the decoration background */
    if(t->boundary_width < 2)
    {
      flush_expose (t->frame);
      if(Scr.d_depth <2)
      {
        XSetWindowBorder(dpy,t->frame,TextColor);
        XSetWindowBorder(dpy,t->Parent,TextColor);
        XSetWindowBackgroundPixmap(dpy,t->frame,BackPixmap);
        XClearWindow(dpy,t->frame);
        XSetWindowBackgroundPixmap(dpy,t->Parent,BackPixmap);
        XClearWindow(dpy,t->Parent);
      }
      else
      {
        XSetWindowBackground(dpy,t->frame,BorderColor);
        XSetWindowBorder(dpy,t->frame,BorderColor);
        XClearWindow(dpy,t->frame);
        XSetWindowBackground(dpy,t->Parent,BorderColor);
        XSetWindowBorder(dpy,t->Parent,BorderColor);
        XClearWindow(dpy,t->Parent);
        XSetWindowBorder(dpy,t->w,BorderColor);	      
      }
    }
    else
    {
      GC rgc,sgc;

      XSetWindowBorder(dpy,t->Parent,BorderColor);
      XSetWindowBorder(dpy,t->frame,BorderColor);	  

      rgc=ReliefGC;
      sgc=ShadowGC;
      if(!(t->flags & MWMButtons)&&(PressedW == t->frame))
      {
        sgc=ReliefGC;
        rgc=ShadowGC;
      }
      ChangeWindowColor(t->frame);
      if((flush_expose(t->frame))||(expose_win == t->frame)||
         (expose_win == None))
      {
        if(t->boundary_width > 2)
        {
          RelieveWindow(t,t->frame,t->boundary_width-1 - t->bw,
                        t->boundary_width-1-t->bw,
                        t->frame_width-
                        (t->boundary_width<<1)+2+3*t->bw,
                        t->frame_height-
                        (t->boundary_width<<1)+2+3*t->bw,
                        sgc,rgc,
                        TOP_HILITE|LEFT_HILITE|RIGHT_HILITE|
                        BOTTOM_HILITE);
          RelieveWindow(t,t->frame,0,0,t->frame_width+t->bw,
                        t->frame_height+t->bw,rgc,sgc,
                        TOP_HILITE|LEFT_HILITE|RIGHT_HILITE|
                        BOTTOM_HILITE);
        }
        else
        {
          RelieveWindow(t,t->frame,0,0,t->frame_width+t->bw,
                        t->frame_height+t->bw,rgc,rgc,
                        TOP_HILITE|LEFT_HILITE|RIGHT_HILITE|
                        BOTTOM_HILITE);	      
        }
      }
      else
      {
        XSetWindowBackground(dpy,t->Parent,BorderColor);	      
      }
    }
  }
  /* Sync to make the border-color change look fast! */
  XSync(dpy,0);
    
}


/****************************************************************************
 *
 *  Redraws just the title bar
 *
 ****************************************************************************/
void SetTitleBar (FVWMWIN *t,Bool onoroff, Bool NewTitle)
{
  int hor_off, w,i;
  GC ReliefGC,ShadowGC,tGC;
  Pixel Forecolor, BackColor;

  if(!t)
    return;
  if(!(t->flags & TITLE))
    return;

  if (onoroff) 
  {
    Forecolor = Scr.HiColors.fore;
    BackColor = Scr.HiColors.back;
    ReliefGC = Scr.HiReliefGC;
    ShadowGC = Scr.HiShadowGC;
  }
  else
  {
    Forecolor =t->TextPixel;
    BackColor = t->BackPixel;
    Globalgcv.foreground = t->ReliefPixel;
    Globalgcm = GCForeground;
    XChangeGC(dpy,Scr.ScratchGC1,Globalgcm,&Globalgcv); 
    ReliefGC = Scr.ScratchGC1;
      
    Globalgcv.foreground = t->ShadowPixel;
    XChangeGC(dpy,Scr.ScratchGC2,Globalgcm,&Globalgcv); 
    ShadowGC = Scr.ScratchGC2;
  }
  if(PressedW==t->title_w)
  {
    tGC = ShadowGC;
    ShadowGC = ReliefGC;
    ReliefGC = tGC;
  }
  flush_expose(t->title_w);
  
  if(t->name != (char *)NULL)
  {
    w=XTextWidth(Scr.WindowFont.font,t->name,strlen(t->name));
    if(w > t->title_width-12)
      w = t->title_width-4;
    if(w < 0)
      w = 0;
  }
  else
    w = 0;


  switch (Scr.TitleStyle.justify)
  {
    case JUSTIFY_LEFT:
      hor_off = 10;
      break;
    case JUSTIFY_RIGHT:
      hor_off = t->title_width - w - 10;
      break;
    case JUSTIFY_CENTER:
    default:
      hor_off = (t->title_width - w)/2;
      break;
  }
  
  NewFontAndColor(Scr.WindowFont.font->fid,Forecolor, BackColor);
  
  if(NewTitle)
    XClearWindow(dpy,t->title_w);
  
  /* for mono, we clear an area in the title bar where the window
   * title goes, so that its more legible. For color, no need */
  if(Scr.d_depth<2)
  {
    RelieveWindow(t,t->title_w,0,0,hor_off-2,t->title_height,
                  ReliefGC, ShadowGC, BOTTOM_HILITE);
    RelieveWindow(t,t->title_w,hor_off+w+2,0,
                  t->title_width - w - hor_off-2,t->title_height,
                  ReliefGC, ShadowGC, BOTTOM_HILITE);
    XFillRectangle(dpy,t->title_w,
                   (PressedW==t->title_w?ShadowGC:ReliefGC),
                   hor_off - 2, 0, w+4,t->title_height);
      
    XDrawLine(dpy,t->title_w,ShadowGC,hor_off+w+1,0,hor_off+w+1,
              t->title_height);
    if(t->name != (char *)NULL)
      XDrawString (dpy, t->title_w,Scr.ScratchGC3,hor_off, Scr.WindowFont.y+1, 
                   t->name, strlen(t->name));
  }
  else
  { 
    if(t->name != (char *)NULL)
      XDrawString (dpy, t->title_w,Scr.ScratchGC3,hor_off, Scr.WindowFont.y+1, 
                   t->name, strlen(t->name));
    switch (Scr.TitleStyle.appearance)
    {
      case TITLE_FLAT:
        break;
      case TITLE_SUNK:
        RelieveWindow(t,t->title_w,0,0,t->title_width,t->title_height,
                      ShadowGC, ReliefGC, BOTTOM_HILITE);
        break;
      case TITLE_RAISED:
      default:
        RelieveWindow(t,t->title_w,0,0,t->title_width,t->title_height,
                      ReliefGC, ShadowGC, BOTTOM_HILITE);
        break;
    }
  }
#if (0)	/****************************************************/
  /* now, draw lines in title bar if it's a sticky window */
  if(t->flags & STICKY)
#endif	/****************************************************/
  {
    for(i=0 ;i< t->title_height/2-3;i+=4)
    {
      XDrawLine(dpy,t->title_w,ShadowGC,4,t->title_height/2 - i-1,
                hor_off-6,t->title_height/2-i-1);
      XDrawLine(dpy,t->title_w,ShadowGC,6+hor_off+w,t->title_height/2 -i-1,
                t->title_width-5,t->title_height/2- i-1);
      XDrawLine(dpy,t->title_w,ReliefGC,4,t->title_height/2 - i,
                hor_off-6,t->title_height/2 - i);
      XDrawLine(dpy,t->title_w,ReliefGC,6+hor_off+w,t->title_height/2-i,
                t->title_width-5,t->title_height/2 - i);

      XDrawLine(dpy,t->title_w,ShadowGC,4,t->title_height/2 + i-1,
                hor_off-6,t->title_height/2+i-1);
      XDrawLine(dpy,t->title_w,ShadowGC,6+hor_off+w,t->title_height/2+i-1,
                t->title_width-5,t->title_height/2 + i-1);
      XDrawLine(dpy,t->title_w,ReliefGC,4,t->title_height/2 + i,
                hor_off-6,t->title_height/2 + i);
      XDrawLine(dpy,t->title_w,ReliefGC,6+hor_off+w,t->title_height/2+i,
                t->title_width-5,t->title_height/2 + i);
    }
  }
  

  XFlush(dpy);
}




/****************************************************************************
 *
 *  Draws the relief pattern around a window
 *
 ****************************************************************************/
FVWM_INLINE void RelieveWindow(FVWMWIN *t,Window win,
			       int x,int y,int w,int h,
			       GC ReliefGC,GC ShadowGC, int hilite)
{
  XSegment seg[4];
  int i;
  int edge;

  edge = 0; 
  if((win == t->sides[0])||(win == t->sides[1])||
     (win == t->sides[2])||(win == t->sides[3]))
    edge = -1;
  if(win == t->corners[0])
    edge = 1;
  if(win == t->corners[1])
    edge = 2;
  if(win == t->corners[2])
    edge = 3;
  if(win == t->corners[3])
    edge = 4;

  i=0;
  seg[i].x1 = x;        seg[i].y1   = y;
  seg[i].x2 = w+x-1;    seg[i++].y2 = y;

  seg[i].x1 = x;        seg[i].y1   = y;
  seg[i].x2 = x;        seg[i++].y2 = h+y-1;

  if(((t->boundary_width > 2)||(edge == 0))&&
     ((t->boundary_width > 3)||(edge < 1))&&
     (!(t->flags & MWMBorders)||
      (((edge==0)||(t->boundary_width > 3))&&(hilite & TOP_HILITE))))
  {
    seg[i].x1 = x+1;      seg[i].y1   = y+1;
    seg[i].x2 = x+w-2;    seg[i++].y2 = y+1;
  }
  if(((t->boundary_width > 2)||(edge == 0))&&
     ((t->boundary_width > 3)||(edge < 1))&&
     (!(t->flags & MWMBorders)||
      (((edge==0)||(t->boundary_width > 3))&&(hilite & LEFT_HILITE))))
  {
    seg[i].x1 = x+1;      seg[i].y1   = y+1;
    seg[i].x2 = x+1;      seg[i++].y2 = y+h-2;
  }
  XDrawSegments(dpy, win, ReliefGC, seg, i);

  i=0;
  seg[i].x1 = x;        seg[i].y1   = y+h-1;
  seg[i].x2 = w+x-1;    seg[i++].y2 = y+h-1;

  if(((t->boundary_width > 2)||(edge == 0))&&
     (!(t->flags & MWMBorders)||
      (((edge==0)||(t->boundary_width > 3))&&(hilite & BOTTOM_HILITE))))
  {
    seg[i].x1 = x+1;      seg[i].y1   = y+h-2;
    seg[i].x2 = x+w-2;    seg[i++].y2 = y+h-2;
  }

  seg[i].x1 = x+w-1;    seg[i].y1   = y;
  seg[i].x2 = x+w-1;    seg[i++].y2 = y+h-1;

  if(((t->boundary_width > 2)||(edge == 0))&&
     (!(t->flags & MWMBorders)||
      (((edge==0)||(t->boundary_width > 3))&&(hilite & RIGHT_HILITE))))
  {
    seg[i].x1 = x+w-2;    seg[i].y1   = y+1;
    seg[i].x2 = x+w-2;    seg[i++].y2 = y+h-2;
  }
  XDrawSegments(dpy, win, ShadowGC, seg, i);
}

void RelieveParts(FVWMWIN *t,int i,GC hor, GC vert)
{
  XSegment seg[2];
  int n = 0;

  if((t->flags & MWMBorders)||(t->boundary_width < 3))
  {
    switch(i)
    {
      case 0:
        seg[0].x1 = t->boundary_width-1;
        seg[0].x2 = t->corner_width;
        seg[0].y1 = t->boundary_width-1;
        seg[0].y2 = t->boundary_width-1;
        n=1;
        break;
      case 1:
        seg[0].x1 = 0;
        seg[0].x2 = t->corner_width - t->boundary_width /* -1*/ ;
        seg[0].y1 = t->boundary_width-1;
        seg[0].y2 = t->boundary_width-1;
        n=1;
        break;
      case 2:
        seg[0].x1 = t->boundary_width-1;
        seg[0].x2 = t->corner_width-2;
        seg[0].y1 = t->corner_width - t->boundary_width+t->bw;
        seg[0].y2 = t->corner_width - t->boundary_width+t->bw;
        n=1;
        break;
      case 3:
        seg[0].x1 = 0;
        seg[0].x2 = t->corner_width - t->boundary_width;
        seg[0].y1 = t->corner_width - t->boundary_width+t->bw;
        seg[0].y2 = t->corner_width - t->boundary_width+t->bw;
        n=1;
        break;
    }
    XDrawSegments(dpy, t->corners[i], hor, seg, n);
    switch(i)
    {
      case 0:
        seg[0].y1 = t->boundary_width-1;
        seg[0].y2 = t->corner_width;
        seg[0].x1 = t->boundary_width-1;
        seg[0].x2 = t->boundary_width-1;
        n=1;
        break;
      case 1:
        seg[0].y1 = t->boundary_width -1;
        seg[0].y2 = t->corner_width-2;
        seg[0].x1 = t->corner_width - t->boundary_width;
        seg[0].x2 = t->corner_width - t->boundary_width;
        n=1;
        break;
      case 2:
        seg[0].y1 = 0;
        seg[0].y2 = t->corner_width - t->boundary_width;
        seg[0].x1 = t->boundary_width-1;
        seg[0].x2 = t->boundary_width-1;
        n=1;
        break;
      case 3:
        seg[0].y1 = 0;
        seg[0].y2 = t->corner_width - t->boundary_width + t->bw;
        seg[0].x1 = t->corner_width - t->boundary_width;
        seg[0].x2 = t->corner_width - t->boundary_width;
        n=1;
        break;
    }
    XDrawSegments(dpy, t->corners[i], vert, seg, 1);
  }
  else
  {
    switch(i)
    {
      case 0:
        seg[0].x1 = t->boundary_width-2;
        seg[0].x2 = t->corner_width;
        seg[0].y1 = t->boundary_width-2;
        seg[0].y2 = t->boundary_width-2;
	  
        seg[1].x1 = t->boundary_width-2;
        seg[1].x2 = t->corner_width;
        seg[1].y1 = t->boundary_width-1;
        seg[1].y2 = t->boundary_width-1;
        n=2;
        break;
      case 1:
        seg[0].x1 = 1;
        seg[0].x2 = t->corner_width - t->boundary_width;
        seg[0].y1 = t->boundary_width-2;
        seg[0].y2 = t->boundary_width-2;
	
        seg[1].x1 = 0;
        seg[1].x2 = t->corner_width - t->boundary_width-1;
        seg[1].y1 = t->boundary_width-1;
        seg[1].y2 = t->boundary_width-1;
        n=2;
        break;
      case 2:
        seg[0].x1 = t->boundary_width-1;
        seg[0].x2 = t->corner_width-2;
        seg[0].y1 = t->corner_width - t->boundary_width+1;
        seg[0].y2 = t->corner_width - t->boundary_width+1;
        n=1;
        if(t->boundary_width > 3)
        {
          seg[1].x1 = t->boundary_width-2;
          seg[1].x2 = t->corner_width-3;
          seg[1].y1 = t->corner_width - t->boundary_width + 2;
          seg[1].y2 = t->corner_width - t->boundary_width + 2;
          n=2;
        }
        break;
      case 3:
        seg[0].x1 = 0;
        seg[0].x2 = t->corner_width - t->boundary_width;
        seg[0].y1 = t->corner_width - t->boundary_width+1;
        seg[0].y2 = t->corner_width - t->boundary_width+1;
        n=1;
        if(t->boundary_width > 3)
        {
          seg[0].x2 = t->corner_width - t->boundary_width + 1;

          seg[1].x1 = 0;
          seg[1].x2 = t->corner_width - t->boundary_width + 1;
          seg[1].y1 = t->corner_width - t->boundary_width + 2;
          seg[1].y2 = t->corner_width - t->boundary_width + 2;
          n=2;
        }
        break;
    }
    XDrawSegments(dpy, t->corners[i], hor, seg, n);
    switch(i)
    {
      case 0:
        seg[0].y1 = t->boundary_width-2;
        seg[0].y2 = t->corner_width;
        seg[0].x1 = t->boundary_width-2;
        seg[0].x2 = t->boundary_width-2;
	  
        seg[1].y1 = t->boundary_width-2;
        seg[1].y2 = t->corner_width;
        seg[1].x1 = t->boundary_width-1;
        seg[1].x2 = t->boundary_width-1;
        n=2;
        break;
      case 1:
        seg[0].y1 = t->boundary_width-1;
        seg[0].y2 = t->corner_width-2;
        seg[0].x1 = t->corner_width - t->boundary_width;
        seg[0].x2 = t->corner_width - t->boundary_width;
        n=1;
        if(t->boundary_width > 3)
        {
          seg[1].y1 = t->boundary_width-2;
          seg[1].y2 = t->corner_width-3;
          seg[1].x1 = t->corner_width - t->boundary_width+1;
          seg[1].x2 = t->corner_width - t->boundary_width+1;
          n=2;
        }
        break;
      case 2:
        seg[0].y1 = 1;
        seg[0].y2 = t->corner_width - t->boundary_width+1;
        seg[0].x1 = t->boundary_width-2;
        seg[0].x2 = t->boundary_width-2;
        n=1;

        if(t->boundary_width > 3)
        {
          seg[1].y1 = 0;
          seg[1].y2 = t->corner_width - t->boundary_width;
          seg[1].x1 = t->boundary_width-1;
          seg[1].x2 = t->boundary_width-1;
        }
        break;
      case 3:
        seg[0].y1 = 0;
        seg[0].y2 = t->corner_width - t->boundary_width + 1;
        seg[0].x1 = t->corner_width - t->boundary_width;
        seg[0].x2 = t->corner_width - t->boundary_width;
        n=1;

        if(t->boundary_width > 3)
        {
          seg[0].y2 = t->corner_width - t->boundary_width + 2;
          seg[1].y1 = 0;
          seg[1].y2 = t->corner_width - t->boundary_width + 2;
          seg[1].x1 = t->corner_width - t->boundary_width + 1;
          seg[1].x2 = t->corner_width - t->boundary_width + 1;
          n=2;
        }
        break;
    }
    XDrawSegments(dpy, t->corners[i], vert, seg, n);
  }
}
/****************************************************************************
 *
 *  Draws a little pattern within a window (more complex)
 *
 ****************************************************************************/
void DrawLinePattern(Window win,
                     GC ReliefGC,
                     GC ShadowGC,
                     int num_coords,
                     int *x_coord, 
                     int *y_coord,
                     int *line_style,
                     int th)
{
  int i;

  for (i=1;i<num_coords;i++)
  {
    XDrawLine(dpy,win,line_style[i]?ReliefGC:ShadowGC,
              th*x_coord[i-1]/100,
              th*y_coord[i-1]/100,
              th*x_coord[i]/100,
              th*y_coord[i]/100);
  }
}



/***********************************************************************
 *
 *  Procedure:
 *      Setupframe - set window sizes, this was called from either
 *              AddWindow, EndResize, or HandleConfigureNotify.
 *
 *  Inputs:
 *      tmp_win - the FVWMWIN pointer
 *      x       - the x coordinate of the upper-left outer corner of the frame
 *      y       - the y coordinate of the upper-left outer corner of the frame
 *      w       - the width of the frame window w/o border
 *      h       - the height of the frame window w/o border
 *
 *  Special Considerations:
 *      This routine will check to make sure the window is not completely
 *      off the display, if it is, it'll bring some of it back on.
 *
 *      The tmp_win->frame_XXX variables should NOT be updated with the
 *      values of x,y,w,h prior to calling this routine, since the new
 *      values are compared against the old to see whether a synthetic
 *      ConfigureNotify event should be sent.  (It should be sent if the
 *      window was moved but not resized.)
 *
 ************************************************************************/

void SetupFrame(FVWMWIN *tmp_win,int x,int y,int w,int h,Bool sendEvent)
{
  XEvent client_event;
  XWindowChanges frame_wc, xwc;
  ulong frame_mask, xwcm;
  int cx,cy,i;
  Bool Resized = False;
  int xwidth,ywidth,left,right;
  
  /* if windows is not being maximized, save size in case of maximization */
  if (!(tmp_win->flags & MAXIMIZED))
  {
    tmp_win->orig_x = x;
    tmp_win->orig_y = y;
    tmp_win->orig_wd = w;
    tmp_win->orig_ht = h;
  }
  if (x >= Scr.MyDisplayWidth + Scr.VxMax - Scr.Vx-16)
    x = Scr.MyDisplayWidth + Scr.VxMax -Scr.Vx - 16;
  if (y >= Scr.MyDisplayHeight+Scr.VyMax - Scr.Vy -16)
    y = Scr.MyDisplayHeight + Scr.VyMax - Scr.Vy - 16;

  /*
   * According to the July 27, 1988 ICCCM draft, we should send a
   * "synthetic" ConfigureNotify event to the client if the window
   * was moved but not resized.
   */
  if ((x != tmp_win->frame_x || y != tmp_win->frame_y) &&
      (w == tmp_win->frame_width && h == tmp_win->frame_height))
    sendEvent = TRUE;

  if((w != tmp_win->frame_width) || (h != tmp_win->frame_height))
    Resized = True;

  if(Resized)
  {
    left = tmp_win->nr_left_buttons;
    right = tmp_win->nr_right_buttons;
      
    if (tmp_win->flags & TITLE) 
      tmp_win->title_height = Scr.TitleHeight + tmp_win->bw;

    tmp_win->title_width= w- 
      (left+right)*tmp_win->title_height 
      -2*tmp_win->boundary_width+tmp_win->bw;


    if(tmp_win->title_width < 1) 
      tmp_win->title_width = 1;

    if (tmp_win->flags & TITLE) 
    {
      xwcm = CWWidth | CWX | CWY | CWHeight;
      tmp_win->title_x = tmp_win->boundary_width+
        (left)*tmp_win->title_height;
      if(tmp_win->title_x >=  w - tmp_win->boundary_width)
        tmp_win->title_x = -10;
      tmp_win->title_y = tmp_win->boundary_width;
	  
      xwc.width = tmp_win->title_width;

      xwc.height = tmp_win->title_height;
      xwc.x = tmp_win->title_x;
      xwc.y = tmp_win->title_y;
      XConfigureWindow(dpy, tmp_win->title_w, xwcm, &xwc);


      xwcm = CWX | CWY | CWHeight | CWWidth;
      xwc.height = tmp_win->title_height;
      xwc.width = tmp_win->title_height;

      xwc.y = tmp_win->boundary_width;
      xwc.x = tmp_win->boundary_width;
      for(i=0;i<Scr.nr_left_buttons;i++)
      {
        if(tmp_win->left_w[i] != None)
        {
          if(xwc.x + tmp_win->title_height < w-tmp_win->boundary_width)
            XConfigureWindow(dpy, tmp_win->left_w[i], xwcm, &xwc);
          else
          {
            xwc.x = -tmp_win->title_height;
            XConfigureWindow(dpy, tmp_win->left_w[i], xwcm, &xwc);
          }
          xwc.x += tmp_win->title_height;
        }
      }
	  
      xwc.x=w-tmp_win->boundary_width+tmp_win->bw;
      for(i=0;i<Scr.nr_right_buttons;i++)
      {
        if(tmp_win->right_w[i] != None)
        {
          xwc.x -=tmp_win->title_height;
          if(xwc.x > tmp_win->boundary_width)
            XConfigureWindow(dpy, tmp_win->right_w[i], xwcm, &xwc);
          else
          {
            xwc.x = -tmp_win->title_height;
            XConfigureWindow(dpy, tmp_win->right_w[i], xwcm, &xwc);
          }
        }
      }
    }

    if(tmp_win->flags & BORDER)
    {
      tmp_win->corner_width = Scr.TitleHeight + tmp_win->bw + 
        tmp_win->boundary_width ;

      if(w < 2*tmp_win->corner_width)
        tmp_win->corner_width = w/3;
      if(h < 2*tmp_win->corner_width)
        tmp_win->corner_width = h/3;
      xwidth = w - 2*tmp_win->corner_width+tmp_win->bw;
      ywidth = h - 2*tmp_win->corner_width;
      xwcm = CWWidth | CWHeight | CWX | CWY;
      if(xwidth<2)
        xwidth = 2;
      if(ywidth<2)
        ywidth = 2;

      for(i=0;i<4;i++)
      {
        if(i==0)
        {
          xwc.x = tmp_win->corner_width;
          xwc.y = 0;
          xwc.height = tmp_win->boundary_width;
          xwc.width = xwidth;
        }
        else if (i==1)
        {
          xwc.x = w - tmp_win->boundary_width+tmp_win->bw;	
          xwc.y = tmp_win->corner_width;
          xwc.width = tmp_win->boundary_width;
          xwc.height = ywidth;

        }
        else if(i==2)
        {
          xwc.x = tmp_win->corner_width;
          xwc.y = h - tmp_win->boundary_width+tmp_win->bw;
          xwc.height = tmp_win->boundary_width+tmp_win->bw;
          xwc.width = xwidth;
        }
        else
        {
          xwc.x = 0;
          xwc.y = tmp_win->corner_width;
          xwc.width = tmp_win->boundary_width;
          xwc.height = ywidth;
        }
        XConfigureWindow(dpy, tmp_win->sides[i], xwcm, &xwc);
      }

      xwcm = CWX|CWY|CWWidth|CWHeight;
      xwc.width = tmp_win->corner_width;
      xwc.height = tmp_win->corner_width;
      for(i=0;i<4;i++)
      {
        if(i%2)
          xwc.x = w - tmp_win->corner_width+tmp_win->bw;
        else
          xwc.x = 0;
	      
        if(i/2)
          xwc.y = h - tmp_win->corner_width;
        else
          xwc.y = 0;

        XConfigureWindow(dpy, tmp_win->corners[i], xwcm, &xwc);
      }

    }
  }
  tmp_win->attr.width = w - 2*tmp_win->boundary_width;
  tmp_win->attr.height = h - tmp_win->title_height 
    - 2*tmp_win->boundary_width;
  /* may need to omit the -1 for shaped windows, next two lines*/
  cx = tmp_win->boundary_width-tmp_win->bw;
  cy = tmp_win->title_height + tmp_win->boundary_width-tmp_win->bw;

  XResizeWindow(dpy, tmp_win->w, tmp_win->attr.width,
		tmp_win->attr.height);
  XMoveResizeWindow(dpy, tmp_win->Parent, cx,cy,
		    tmp_win->attr.width, tmp_win->attr.height);

  /* 
   * fix up frame and assign size/location values in tmp_win
   */
  frame_wc.x = tmp_win->frame_x = x;
  frame_wc.y = tmp_win->frame_y = y;
  frame_wc.width = tmp_win->frame_width = w;
  frame_wc.height = tmp_win->frame_height = h;
  frame_mask = (CWX | CWY | CWWidth | CWHeight);
  XConfigureWindow (dpy, tmp_win->frame, frame_mask, &frame_wc);


#ifdef SHAPE
  if (ShapesSupported)
  {
    if ((Resized)&&(tmp_win->wShaped))
    {
      SetShape(tmp_win,w);
    }
  }
#endif /* SHAPE */
  XSync(dpy,0);
  if (sendEvent)
  {
    client_event.type = ConfigureNotify;
    client_event.xconfigure.display = dpy;
    client_event.xconfigure.event = tmp_win->w;
    client_event.xconfigure.window = tmp_win->w;
      
    client_event.xconfigure.x = x + tmp_win->boundary_width;
    client_event.xconfigure.y = y + tmp_win->title_height+
      tmp_win->boundary_width;
    client_event.xconfigure.width = w-2*tmp_win->boundary_width;
    client_event.xconfigure.height =h-2*tmp_win->boundary_width -
      tmp_win->title_height;

    client_event.xconfigure.border_width =tmp_win->bw;
    /* Real ConfigureNotify events say we're above title window, so ... */
    /* what if we don't have a title ????? */
    client_event.xconfigure.above = tmp_win->frame;
    client_event.xconfigure.override_redirect = False;
    XSendEvent(dpy, tmp_win->w, False, StructureNotifyMask, &client_event);
  }

  BroadcastConfig(M_CONFIGURE_WINDOW,tmp_win);
}


#ifdef SHAPE
/****************************************************************************
 *
 * Sets up the shaped window borders 
 * 
 ****************************************************************************/
void SetShape(FVWMWIN *tmp_win, int w)
{
  if (ShapesSupported)
  {
    XRectangle rect;

    XShapeCombineShape (dpy, tmp_win->frame, ShapeBounding,
                        tmp_win->boundary_width,
                        tmp_win->title_height+tmp_win->boundary_width,
                        tmp_win->w,
                        ShapeBounding, ShapeSet);
    if (tmp_win->title_w) 
    {
      /* windows w/ titles */
      rect.x = tmp_win->boundary_width;
      rect.y = tmp_win->title_y;
      rect.width = w - 2*tmp_win->boundary_width+tmp_win->bw;
      rect.height = tmp_win->title_height;
      
      
      XShapeCombineRectangles(dpy,tmp_win->frame,ShapeBounding,
                              0,0,&rect,1,ShapeUnion,Unsorted);
    }
  }
}
#endif
