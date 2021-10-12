/*
 ****************************************************************
 *								*
 *			src/style.c				*
 *								*
 *	Processamento do comando "Style" do ".fvwmrc"		*
 *								*
 *	Versão	3.0.0, de 11.09.97				*
 *		4.4.0, de 20.03.03				*
 *								*
 *	Módulo: cmd/fvwm					*
 *		X Windows - Clientes				*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright (c) 2003 TROPIX Technologies Ltd.	*
 *								*
 ****************************************************************
 */

/****************************************************************************
 * This module is all original code 
 * by Rob Nation 
 * Copyright 1993, Robert Nation
 *     You may use this code for any purpose, as long as the original
 *     copyright remains in the source code and all documentation
 ****************************************************************************/

#include "../global.h"

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <unistd.h>

#include "fvwm.h"
#include "menus.h"
#include "misc.h"
#include "parse.h"
#include "screen.h"

void
ProcessNewStyle (XEvent *eventp, Window w, FVWMWIN *tmp_win, ulong context,
		 char *text, int *Module)
{
  char *name, *line;
  char *restofline,*tmp;
  char *icon_name = NULL;
  char *forecolor = NULL;
  char *backcolor = NULL;
  ulong off_buttons=0;
  ulong on_buttons=0;
  name_list *nptr;
  int butt;
  int BoxFillMethod = 0;
  int IconBox[4];
  int num,i;

  int len,desknumber = 0,bw=0, nobw = 0;
  ulong off_flags = 0;
  ulong on_flags = 0;
  
  IconBox[0] = -1;
  IconBox[1] = -1;
  IconBox[2] = Scr.MyDisplayWidth;
  IconBox[3] = Scr.MyDisplayHeight;

  restofline = GetNextToken(text,&name);
  /* in case there was no argument! */
  if((name == NULL)||(restofline == NULL))
    return;

  while(isspace(*restofline)&&(*restofline != 0))restofline++;
  line = restofline;

  if(restofline == NULL)return;
  while((*restofline != 0)&&(*restofline != '\n'))
  {
    while(isspace(*restofline)) restofline++;
    switch (tolower(restofline[0]))
    {
      case 'a':
        if(strncasecmp(restofline,"ACTIVEPLACEMENT",15)==0)
        {
          restofline +=15;
          on_flags |= RANDOM_PLACE_FLAG;
        }
        break;
      case 'b':
        if(strncasecmp(restofline,"BACKCOLOR",9)==0)
        {
          restofline +=9;
          while(isspace(*restofline))restofline++;
          tmp = restofline;
          len = 0;
          while((tmp != NULL)&&(*tmp != 0)&&(*tmp != ',')&&
                (*tmp != '\n')&&(!isspace(*tmp)))
          {
            tmp++;
            len++;
          }
          if(len > 0)
          {
            backcolor = safemalloc(len+1);
            strncpy(backcolor,restofline,len);
            backcolor[len] = 0;
            off_flags |= BACK_COLOR_FLAG;
          }
          restofline = tmp;
        }
        else if (strncasecmp(restofline,"BUTTON",6)==0)
        {
          restofline +=6;
	  
          sscanf(restofline,"%d",&butt);
          while(isspace(*restofline))restofline++;
          while((!isspace(*restofline))&&(*restofline!= 0)&&
                (*restofline != ',')&&(*restofline != '\n'))
            restofline++;
          while(isspace(*restofline))restofline++;
	  
          on_buttons |= (1<<(butt-1));        
        }
        else if(strncasecmp(restofline,"BorderWidth",11)==0)
        {
          restofline +=11;
          off_flags |= BW_FLAG;
          sscanf(restofline,"%d",&bw);
          while(isspace(*restofline))restofline++;
          while((!isspace(*restofline))&&(*restofline!= 0)&&
                (*restofline != ',')&&(*restofline != '\n'))
            restofline++;
          while(isspace(*restofline))restofline++;
        }
        break;
      case 'c':
        if(strncasecmp(restofline,"COLOR",5)==0)
        {
          restofline +=5;
          while(isspace(*restofline))restofline++;
          tmp = restofline;
          len = 0;
          while((tmp != NULL)&&(*tmp != 0)&&(*tmp != ',')&&
                (*tmp != '\n')&&(*tmp != '/')&&(!isspace(*tmp)))
          {
            tmp++;
            len++;
          }
          if(len > 0)
          {
            forecolor = safemalloc(len+1);
            strncpy(forecolor,restofline,len);
            forecolor[len] = 0;
            off_flags |= FORE_COLOR_FLAG;
          }
          
          while(isspace(*tmp))tmp++;
          if(*tmp == '/')
          {
            tmp++;
            while(isspace(*tmp))tmp++;
            restofline = tmp;
            len = 0;
            while((tmp != NULL)&&(*tmp != 0)&&(*tmp != ',')&&
                  (*tmp != '\n')&&(*tmp != '/')&&(!isspace(*tmp)))
            {
              tmp++;
              len++;
            }
            if(len > 0)
            {
              backcolor = safemalloc(len+1);
              strncpy(backcolor,restofline,len);
              backcolor[len] = 0;
              off_flags |= BACK_COLOR_FLAG;
            }
          }
          restofline = tmp;
        }
        else if(strncasecmp(restofline,"CirculateSkipIcon",17)==0)
        {
          restofline +=17;
          off_flags |= CIRCULATE_SKIP_ICON_FLAG;
        }
        else if(strncasecmp(restofline,"CirculateHitIcon",16)==0)
        {
          restofline +=16;
          on_flags |= CIRCULATE_SKIP_ICON_FLAG;
        }
        else if(strncasecmp(restofline,"CLICKTOFOCUS",12)==0)
        {
          restofline +=12;
          off_flags |= CLICK_FOCUS_FLAG;
          on_flags |= SLOPPY_FOCUS_FLAG;
        }
        else if(strncasecmp(restofline,"CirculateSkip",13)==0)
        {
          restofline +=13;
          off_flags |= CIRCULATESKIP_FLAG;
        }
        else if(strncasecmp(restofline,"CirculateHit",12)==0)
        {
          restofline +=12;
          on_flags |= CIRCULATESKIP_FLAG;
        }
        break;
      case 'd':
        if(strncasecmp(restofline,"DecorateTransient",17)==0)
        {
          restofline +=17;
          off_flags |= DECORATE_TRANSIENT_FLAG;
        }
        else if(strncasecmp(restofline,"DUMBPLACEMENT",13)==0)
        {
          restofline +=13;
          on_flags |= SMART_PLACE_FLAG;
        }
        break;
      case 'e':
        break;
      case 'f':
        if(strncasecmp(restofline,"FORECOLOR",9)==0)
        {
          restofline +=9;
          while(isspace(*restofline))restofline++;
          tmp = restofline;
          len = 0;
          while((tmp != NULL)&&(*tmp != 0)&&(*tmp != ',')&&
                (*tmp != '\n')&&(!isspace(*tmp)))
          {
            tmp++;
            len++;
          }
          if(len > 0)
          {
            forecolor = safemalloc(len+1);
            strncpy(forecolor,restofline,len);
            forecolor[len] = 0;
            off_flags |= FORE_COLOR_FLAG;
          }
          restofline = tmp;
        }
        else if(strncasecmp(restofline,"FVWMBUTTONS",11)==0)
        {
          restofline +=11;
          on_flags |= MWM_BUTTON_FLAG;
        }
        else if(strncasecmp(restofline,"FVWMBORDER",10)==0)
        {
          restofline +=10;
          on_flags |= MWM_BORDER_FLAG;
        }
        else if(strncasecmp(restofline,"FocusFollowsMouse",17)==0)
        {
          restofline +=17;
          on_flags |= CLICK_FOCUS_FLAG;
          on_flags |= SLOPPY_FOCUS_FLAG;
        }
        break;
      case 'g':
        break;
      case 'h':
        if(strncasecmp(restofline,"HINTOVERRIDE",12)==0)
        {
          restofline +=12;
          off_flags |= MWM_OVERRIDE_FLAG;
        }
        else if(strncasecmp(restofline,"HANDLES",7)==0)
        {
          restofline +=7;
          on_flags |= NOBORDER_FLAG;
        }
        else if(strncasecmp(restofline,"HandleWidth",11)==0)
        {
          restofline +=11;
          off_flags |= NOBW_FLAG;
          sscanf(restofline,"%d",&nobw);
          while(isspace(*restofline))restofline++;
          while((!isspace(*restofline))&&(*restofline!= 0)&&
                (*restofline != ',')&&(*restofline != '\n'))
            restofline++;
          while(isspace(*restofline))restofline++;
        }
        break;
      case 'i':
        if(strncasecmp(restofline,"IconTitle",9)==0)
        {
          on_flags |= NOICON_TITLE_FLAG;
          restofline +=9;
        }
        else if(strncasecmp(restofline,"IconBox",7) == 0)
        {
          restofline +=7;
          /* Standard X11 geometry string */
          num = sscanf(restofline,"%d%d%d%d",&IconBox[0], &IconBox[1],
                       &IconBox[2],&IconBox[3]);
          for(i=0;i<num;i++)
          {
            while(isspace(*restofline))restofline++;
            while((!isspace(*restofline))&&(*restofline!= 0)&&
                  (*restofline != ',')&&(*restofline != '\n'))
              restofline++;
          }
          if(num !=4)
            fvwm_msg(ERR,"ProcessNewStyle",
                     "IconBox style requires 4 arguments!");
          else
          {
            /* check for negative locations */
            if(IconBox[0] < 0)
              IconBox[0] += Scr.MyDisplayWidth;
            if(IconBox[1] < 0)
              IconBox[1] += Scr.MyDisplayHeight;
            if(IconBox[2] < 0)
              IconBox[2] += Scr.MyDisplayWidth;
            if(IconBox[3] < 0)
              IconBox[3] += Scr.MyDisplayHeight;
          }
        }
        else if(strncasecmp(restofline,"ICON",4)==0)
        {
          restofline +=4;
          while(isspace(*restofline))restofline++;
          tmp = restofline;
          len = 0;
          while((tmp != NULL)&&(*tmp != 0)&&(*tmp != ',')&&(*tmp != '\n'))
          {
            tmp++;
            len++;
          }
          if(len > 0)
          {
            icon_name = safemalloc(len+1);
            strncpy(icon_name,restofline,len);
            icon_name[len] = 0;
            off_flags |= ICON_FLAG;
            on_flags |= SUPPRESSICON_FLAG;
          }
          else
            on_flags |= SUPPRESSICON_FLAG;	    
          restofline = tmp;
        }
        break;
      case 'j':
        break;
      case 'k':
        break;
      case 'l':
        if(strncasecmp(restofline,"LENIENCE",8)==0)
        {
          restofline += 8;
          off_flags |= LENIENCE_FLAG;
        }
        break;
      case 'm':
        if(strncasecmp(restofline,"MWMBUTTONS",10)==0)
        {
          restofline +=10;
          off_flags |= MWM_BUTTON_FLAG;
        }
        else if(strncasecmp(restofline,"MWMBORDER",9)==0)
        {
          restofline +=9;
          off_flags |= MWM_BORDER_FLAG;
        }
        else if(strncasecmp(restofline,"MWMDECOR",8)==0)
        {
          restofline +=8;
          off_flags |= MWM_DECOR_FLAG;
        }
        else if(strncasecmp(restofline,"MWMFUNCTIONS",12)==0)
        {
          restofline +=12;
          off_flags |= MWM_FUNCTIONS_FLAG;
        }
        else if(strncasecmp(restofline,"MOUSEFOCUS",10)==0)
        {
          restofline +=10;
          on_flags |= CLICK_FOCUS_FLAG;
          on_flags |= SLOPPY_FOCUS_FLAG;
        }
        break;
      case 'n':
        if(strncasecmp(restofline,"NoIconTitle",11)==0)
        {
          off_flags |= NOICON_TITLE_FLAG;
          restofline +=11;
        }
        else if(strncasecmp(restofline,"NOICON",6)==0)
        {
          restofline +=6;
          off_flags |= SUPPRESSICON_FLAG;
        }
        else if(strncasecmp(restofline,"NOTITLE",7)==0)
        {
          restofline +=7;
          off_flags |= NOTITLE_FLAG;
        }
        else if(strncasecmp(restofline,"NoPPosition",11)==0)
        {
          restofline +=11;
          off_flags |= NO_PPOSITION_FLAG;
        }
        else if(strncasecmp(restofline,"NakedTransient",14)==0)
        {
          restofline +=14;
          on_flags |= DECORATE_TRANSIENT_FLAG;
        }
        else if(strncasecmp(restofline,"NODECORHINT",11)==0)
        {
          restofline +=11;
          on_flags |= MWM_DECOR_FLAG;
        }
        else if(strncasecmp(restofline,"NOFUNCHINT",10)==0)
        {
          restofline +=10;
          on_flags |= MWM_FUNCTIONS_FLAG;
        }
        else if(strncasecmp(restofline,"NOOVERRIDE",10)==0)
        {
          restofline +=10;
          on_flags |= MWM_OVERRIDE_FLAG;
        }
        else if(strncasecmp(restofline,"NOHANDLES",9)==0)
        {
          restofline +=9;
          off_flags |= NOBORDER_FLAG;
        }
        else if(strncasecmp(restofline,"NOLENIENCE",10)==0)
        {
          restofline += 10;
          on_flags |= LENIENCE_FLAG;
        }
        else if (strncasecmp(restofline,"NOBUTTON",8)==0)
        {
          restofline +=8;
	  
          sscanf(restofline,"%d",&butt);
          while(isspace(*restofline))restofline++;
          while((!isspace(*restofline))&&(*restofline!= 0)&&
                (*restofline != ',')&&(*restofline != '\n'))
            restofline++;
          while(isspace(*restofline))restofline++;
	  
          off_buttons |= (1<<(butt-1));
        }
        else if(strncasecmp(restofline,"NOOLDECOR",9)==0)
        {
          restofline += 9;
          on_flags |= OL_DECOR_FLAG;
        }
        break;
      case 'o':
        if(strncasecmp(restofline,"OLDECOR",7)==0)
        {
          restofline += 7;
          off_flags |= OL_DECOR_FLAG;
        }
        break;
      case 'p':
        break;
      case 'q':
        break;
      case 'r':
        if(strncasecmp(restofline,"RANDOMPLACEMENT",15)==0)
        {
          restofline +=15;
          off_flags |= RANDOM_PLACE_FLAG;
        }
        break;
      case 's':
        if(strncasecmp(restofline,"SMARTPLACEMENT",14)==0)
        {
          restofline +=14;
          off_flags |= SMART_PLACE_FLAG;
        }
        else if(strncasecmp(restofline,"SkipMapping",11)==0)
        {
          restofline +=11;
          off_flags |= SHOW_MAPPING;
        }
        else if(strncasecmp(restofline,"ShowMapping",11)==0)
        {
          restofline +=12;
          on_flags |= SHOW_MAPPING;
        }
        else if(strncasecmp(restofline,"StickyIcon",10)==0)
        {
          restofline +=10;
          off_flags |= STICKY_ICON_FLAG;
        }
        else if(strncasecmp(restofline,"SlipperyIcon",12)==0)
        {
          restofline +=12;
          on_flags |= STICKY_ICON_FLAG;
        }
        else if(strncasecmp(restofline,"SLOPPYFOCUS",11)==0)
        {
          restofline +=11;
          on_flags |= CLICK_FOCUS_FLAG;
          off_flags |= SLOPPY_FOCUS_FLAG;
        }
        else if(strncasecmp(restofline,"StartIconic",11)==0)
        {
          restofline +=11;
          off_flags |= START_ICONIC_FLAG;
        }
        else if(strncasecmp(restofline,"StartNormal",11)==0)
        {
          restofline +=11;
          on_flags |= START_ICONIC_FLAG;
        }
        else if(strncasecmp(restofline,"StaysOnTop",10)==0)
        {
          restofline +=10;
          off_flags |= STAYSONTOP_FLAG;	  
        }
        else if(strncasecmp(restofline,"StaysPut",8)==0)
        {
          restofline +=8;
          on_flags |= STAYSONTOP_FLAG;	  
        }
        else if(strncasecmp(restofline,"Sticky",6)==0)
        {
          off_flags |= STICKY_FLAG;	  
          restofline +=6;
        }
        else if(strncasecmp(restofline,"Slippery",8)==0)
        {
          on_flags |= STICKY_FLAG;	  
          restofline +=8;
        }
        else if(strncasecmp(restofline,"STARTSONDESK",12)==0)
        {
          restofline +=12;
          off_flags |= STARTSONDESK_FLAG;
          sscanf(restofline,"%d",&desknumber);
          while(isspace(*restofline))restofline++;
          while((!isspace(*restofline))&&(*restofline!= 0)&&
                (*restofline != ',')&&(*restofline != '\n'))
            restofline++;
          while(isspace(*restofline))restofline++;
        }
        else if(strncasecmp(restofline,"STARTSANYWHERE",14)==0)
        {
          restofline +=14;
          on_flags |= STARTSONDESK_FLAG;
        }
        break;
      case 't':
        if(strncasecmp(restofline,"TITLE",5)==0)
        {
          restofline +=5;
          on_flags |= NOTITLE_FLAG;
        }
        break;
      case 'u':
        if(strncasecmp(restofline,"UsePPosition",12)==0)
        {
          restofline +=12;
          on_flags |= NO_PPOSITION_FLAG;
        }
        else if(strncasecmp(restofline,"UseStyle",8)==0)
        {
          restofline +=8;
          while(isspace(*restofline))restofline++;
          tmp = restofline;
          while( tmp && *tmp &&(*tmp != ',')&&
                 (*tmp != '\n')&&(!isspace(*tmp)))
            tmp++;
          if((len = tmp - restofline) > 0)
          {
            nptr = Scr.TheList;
            while(nptr && strncasecmp(restofline,nptr->name,len) )
              nptr = nptr->next;
            if (nptr&& !strncasecmp(restofline,nptr->name,len))
            {
              on_flags      = nptr->on_flags;
              off_flags     = nptr->off_flags;
              icon_name     = nptr->value;
              desknumber    = nptr->Desk;
              bw            = nptr->border_width;
              nobw          = nptr->resize_width;
              forecolor     = nptr->ForeColor;
              backcolor     = nptr->BackColor;
              BoxFillMethod = nptr->BoxFillMethod;
              IconBox[0]    = nptr->IconBox[0];
              IconBox[1]    = nptr->IconBox[1];
              IconBox[2]    = nptr->IconBox[2];
              IconBox[3]    = nptr->IconBox[3];
              off_buttons   = nptr->off_buttons;
              on_buttons    = nptr->on_buttons;
              restofline = tmp;
            }
            else 
            {
              restofline = tmp;
              tmp=safemalloc(500);
              strcat(tmp,"UseStyle: ");
              strncat(tmp,restofline-len,len);
              strcat(tmp," style not found!");
              fvwm_msg(ERR,"ProcessNewStyle",tmp);
              free (tmp);
            }
          }
          while(isspace(*restofline)) restofline++;
        }
        break;
      case 'v':
        break;
      case 'w':
        if(strncasecmp(restofline,"WindowListSkip",14)==0)
        {
          restofline +=14;
          off_flags |= LISTSKIP_FLAG;
        }
        else if(strncasecmp(restofline,"WindowListHit",13)==0)
        {
          restofline +=13;
          on_flags |= LISTSKIP_FLAG;
        }
        break;
      case 'x':
        break;
      case 'y':
        break;
      case 'z':
        break;
      default:
        break;
    }

    while(isspace(*restofline))restofline++;
    if(*restofline == ',')
      restofline++;
    else if((*restofline != 0)&&(*restofline != '\n'))
    {
      fvwm_msg(ERR,"ProcessNewStyle",
               "bad style command: %s", restofline);
      return;
    }
  }

  /* capture default icons */
  if(strcmp(name,"*") == 0)
  {
    if(off_flags & ICON_FLAG)
      Scr.DefaultIcon = icon_name;
    off_flags &= ~ICON_FLAG;
    icon_name = NULL;
  }

  AddToList(name,icon_name,off_flags,on_flags,desknumber,bw,nobw,
	    forecolor,backcolor,off_buttons,on_buttons,IconBox,BoxFillMethod);
}


void
AddToList (char *name, char *icon_name, ulong off_flags,  ulong on_flags, int desk,
	   int bw, int nobw, char *forecolor, char *backcolor, ulong off_buttons,
	   ulong on_buttons, int *IconBox, int BoxFillMethod)
{
  name_list *nptr,*lastptr = NULL;

  if((name == NULL)||((off_flags == 0)&&(on_flags == 0)&&(on_buttons == 0)&&
		      (off_buttons == 0)&(IconBox[0] < 0)))
  {
    if(name)
      free (name);
    if(icon_name)
      free (icon_name);
    return;
  }

  /* used to merge duplicate entries, but that is no longer
   * appropriate since conficting styles are possible, and the
   * last match should win! */
  for (nptr = Scr.TheList; nptr != NULL; nptr = nptr->next)
  {
    lastptr=nptr;
  }

  nptr = (name_list *)safemalloc(sizeof(name_list));
  nptr->next = NULL;
  nptr->name = name;
  nptr->on_flags = on_flags;
  nptr->off_flags = off_flags;
  nptr->value = icon_name;
  nptr->Desk = desk;
  nptr->border_width = bw;
  nptr->resize_width = nobw;
  nptr->ForeColor = forecolor;
  nptr->BackColor = backcolor;
  nptr->BoxFillMethod = BoxFillMethod;
  nptr->IconBox[0] = IconBox[0];
  nptr->IconBox[1] = IconBox[1];
  nptr->IconBox[2] = IconBox[2];
  nptr->IconBox[3] = IconBox[3];
  nptr->off_buttons = off_buttons;
  nptr->on_buttons = on_buttons;

  if(lastptr != NULL)
    lastptr->next = nptr;
  else
    Scr.TheList = nptr;
}

