/*
 ****************************************************************
 *								*
 *			src/module.c				*
 *								*
 *	Interface com os módulos (filhotes)			*
 *								*
 *	Versão	3.0.0, de 20.05.97				*
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

#include <sys/types.h>

#include "../global.h"

#include <stdio.h>
#include <ctype.h>
#include <stdarg.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <fcntl.h>
#include <xti.h>
#include <errno.h>

#include <X11/keysym.h>

#include "fvwm.h"
#include "menus.h"
#include "misc.h"
#include "parse.h"
#include "screen.h"
#include "module.h"


int npipes;
int *readPipes;
int *writePipes;
int *pipeOn;
char **pipeName;

ulong *PipeMask;
QUEUE **pipeQueue;

FVWM_INLINE int PositiveWrite(int module, ulong *ptr, int size);
void DeleteQueueBuff(int module);
void AddToQueue(int module, ulong *ptr, int size, int done);

void
initModules (void)
{
  int i;

  npipes = GetFdWidth();

  writePipes = (int *)safemalloc(sizeof(int)*npipes);
  readPipes = (int *)safemalloc(sizeof(int)*npipes);
  pipeOn = (int *)safemalloc(sizeof(int)*npipes);
  PipeMask = (ulong *)safemalloc(sizeof(ulong)*npipes);
  pipeName = (char **)safemalloc(sizeof(char *)*npipes);
  pipeQueue=(QUEUE **)
    safemalloc(sizeof(QUEUE *)*npipes);

  for(i=0;i<npipes;i++)
    {
      writePipes[i]= -1;
      readPipes[i]= -1;
      pipeOn[i] = -1;
      PipeMask[i] = MAX_MASK;
      pipeQueue[i] = (QUEUE *)NULL;
      pipeName[i] = NULL;
    }
}

/*
 ****************************************************************
 *	Fecha todos os canais de comunicação com os módulos	*
 ****************************************************************
 */
void
ClosePipes (void)
{
	int i;

	Broadcast (M_FVWM_IS_DYING, 0);
	FlushAllQueues ();

	for (i = 0; i < npipes; i++)   
	{
		if (writePipes[i] > 0)
		{
			close (writePipes[i]);
			close (readPipes[i]);
		}

		if (pipeName[i] != NULL)
		{
			free (pipeName[i]);
			pipeName[i] = 0;
		}

		while (pipeQueue[i] != NULL)
			DeleteQueueBuff(i);
	}
}


void
executeModule (XEvent *eventp,Window w,FVWMWIN *tmp_win,
		     ulong context, char *action,int* Module)
{
  int fvwm_to_app[2],app_to_fvwm[2];
  int i,val,nargs = 0;
  char *cptr;
  char *args[20];
  char *arg1 = NULL;
  char arg2[20];
  char arg3[20];
  char arg5[20];
  char arg6[20];
  extern char *ModulePath;
  extern char *fvwm_file;
  Window win;

  if(eventp->type != KeyPress)
    UngrabEm();

  if(action == NULL)
    return;

  if(tmp_win)
    win = tmp_win->w;
  else
    win = None;

  /* If we execute a module, don't wait for buttons to come up,
   * that way, a pop-up menu could be implemented */
  *Module = 0;

  action = GetNextToken(action, &cptr);

  arg1 = findIconFile(cptr,ModulePath,X_OK);
  if(arg1 == NULL)
    {
      if(cptr != NULL)
	{
	  fvwm_msg(ERR,"executeModule",
                   "No such module %s %s",ModulePath,cptr);
	  free (cptr);
	}
      return;
    }

  /* Look for an available pipe slot */
  i=0;
  while((i<npipes) && (writePipes[i] >=0))
    i++;
  if(i>=npipes)
    {
      fvwm_msg(ERR,"executeModule","Too many Accessories!");
      if(cptr != NULL)
	free (cptr);
      return;
    }
  
  /* I want one-ended pipes, so I open two two-ended pipes,
   * and close one end of each. I need one ended pipes so that
   * I can detect when the module crashes/malfunctions */
  if(pipe(fvwm_to_app)!=0)
    {
      fvwm_msg(ERR,"executeModule","Failed to open pipe");
      return;
    }
  if(pipe(app_to_fvwm)!=0)
    {
      fvwm_msg(ERR,"executeModule","Failed to open pipe2");
      if(cptr != NULL)
	free (cptr);
      close(fvwm_to_app[0]);
      close(fvwm_to_app[1]);
      return;
    }

  pipeName[i] = stripcpy(cptr);
  sprintf(arg2,"%d",app_to_fvwm[1]);
  sprintf(arg3,"%d",fvwm_to_app[0]);
  sprintf(arg5,"%lx",(ulong)win);
  sprintf(arg6,"%lx",(ulong)context);
  args[0] = (char *)basename (arg1);
  args[1]=arg2;
  args[2]=arg3;
  if(fvwm_file != NULL)
    args[3]=fvwm_file;
  else
    args[3]="none";
  args[4]=arg5;
  args[5]=arg6;
  nargs = 6;
  while((action != NULL)&&(nargs < 20)&&(strlen(args[nargs-1]) > 0))
    {
      args[nargs] = 0;
      action = GetNextToken(action,&args[nargs]);
      nargs++;
    }
  if(strlen(args[nargs-1]) <= 0)
    {
      nargs--;
      if(args[nargs] != NULL)
	free (args[nargs]);
    }
  args[nargs] = 0;

  /* Try vfork instead of fork. The man page says that vfork is better! */
  /* Also, had to change exit to _exit() */
  /* Not everyone has vfork! */
  val = fork();
  if(val > 0)
    {
      /* This fork remains running fvwm */
      /* close appropriate descriptors from each pipe so
       * that fvwm will be able to tell when the app dies */
      close(app_to_fvwm[1]);
      close(fvwm_to_app[0]);

      /* add these pipes to fvwm's active pipe list */
      writePipes[i] = fvwm_to_app[1];
      readPipes[i] = app_to_fvwm[0];
      pipeOn[i] = -1;
      PipeMask[i] = MAX_MASK;
      free (arg1);
      pipeQueue[i] = NULL;

#if (0)	/****************************************************/
      /* make the PositiveWrite pipe non-blocking. Don't want to jam up
	 fvwm because of an uncooperative module */
      fcntl(writePipes[i],F_SETFL,O_NDELAY);
#endif	/****************************************************/

      /* Mark the pipes close-on exec so other programs
       * won`t inherit them */
      if (fcntl(readPipes[i], F_SETFD, 1) == -1) 
	fvwm_msg(ERR,"executeModule","module close-on-exec failed");
      if (fcntl(writePipes[i], F_SETFD, 1) == -1) 
	fvwm_msg(ERR,"executeModule","module close-on-exec failed");

      for(i=6;i<nargs;i++)
	{
	  if(args[i] != 0)
	    free (args[i]);
	}
	
      if(cptr != NULL)
	free (cptr);
    }
  else if (val ==0)
    {
      /* this is  the child */
      /* this fork execs the module */
      close(fvwm_to_app[1]);
      close(app_to_fvwm[0]);
msg ("Executando módulo <%s>", arg1);
      execvp(arg1,args);
      fvwm_msg(ERR,"executeModule","Execution of module failed: %s",arg1);
      perror("");
      close(app_to_fvwm[1]);
      close(fvwm_to_app[0]);
      exit(1);
    }
  else
    {
      fvwm_msg(ERR,"executeModule","Fork failed");
      free (arg1);
    }
  return;
}

void
HandleModuleInput (Window w, int channel)
{
  char text[256];
  int size;
  int cont,n;

  /* Already read a (possibly NULL) window id from the pipe,
   * Now read an fvwm bultin command line */
  n = read(readPipes[channel], &size, sizeof(size));
  if(n < sizeof(size))
    {
      KillModule(channel,1);
      return;
    }

  if(size >255)
    {
      fvwm_msg(ERR,"HandleModuleInput",
               "Module command is too big (%d)",(void *)size);
      size=255;
    }

  pipeOn[channel] = 1;

  n = read(readPipes[channel],text, size);
  if(n < size)
    {
      KillModule(channel,2);
      return;
    }
  
  text[n]=0;
  n = read(readPipes[channel],&cont, sizeof(cont));
  if(n < sizeof(cont))
    {
      KillModule(channel,3);
      return;
    }
  if(cont == 0)
    {
      KillModule(channel,4);
    }
  if(strlen(text)>0)
    {
      extern int Context;
      FVWMWIN *tmp_win;

      /* first, check to see if module config line */
      if (text[0] == '*')
      {
        AddToModList(text);
        return;
      }

      /* perhaps the module would like us to kill it? */
      if(strncasecmp(text,"KillMe",6)==0) 
      {
        KillModule(channel,12);
        return;
      }

      /* If a module does XUngrabPointer(), it can now get proper Popups */
      if(strncasecmp(text,"popup",5)==0)
	  Event.xany.type = ButtonPress;
      else
	  Event.xany.type = ButtonRelease;
      Event.xany.window = w;

      if (XFindContext (dpy, w, FvwmContext, (caddr_t *) &tmp_win) == XCNOENT)
	{
	  tmp_win = NULL;
	  w = None;
	}
      if(tmp_win)
	{
	  Event.xbutton.button = 1;
	  Event.xbutton.x_root = tmp_win->frame_x;
	  Event.xbutton.y_root = tmp_win->frame_y;
	  Event.xbutton.x = 0;
	  Event.xbutton.y = 0;
	  Event.xbutton.subwindow = None;
	}
      else
	{
	  Event.xbutton.button = 1;
	  Event.xbutton.x_root = 0;
	  Event.xbutton.y_root = 0;
	  Event.xbutton.x = 0;
	  Event.xbutton.y = 0;
	  Event.xbutton.subwindow = None;
	}
      Context = GetContext(tmp_win,&Event,&w);
      ExecuteFunction(text,tmp_win,&Event,Context ,channel); 
    }
  return;
}

void
DeadPipe (int nonsense, ...)
{
  signal(SIGPIPE, DeadPipe);
}


void
KillModule (int channel, int place)
{
  close(readPipes[channel]);
  close(writePipes[channel]);
  
  readPipes[channel] = -1;
  writePipes[channel] = -1;
  pipeOn[channel] = -1;
  while(pipeQueue[channel] != NULL)
    {
      DeleteQueueBuff(channel);
    }
  if(pipeName[channel] != NULL)
    {
      free (pipeName[channel]);
      pipeName[channel] = NULL;
    }
	
  return;
}

void
KillModuleByName (char *name)
{
  int i = 0;

  if(name == NULL)
    return;
  
  while(i<npipes)
    {
      if((pipeName[i] != NULL)&&(matchWildcards(name,pipeName[i])))
	{
	  KillModule(i,10);	
	}
      i++;
    }
  return;
}

/*
 ****************************************************************
 *	Envia uma mensagem a todos os módulos			*
 ****************************************************************
 */
void
Broadcast (ulong event_type, ulong num_datum, ...)
{
	va_list		ap;
	int		i;
	ulong		body[11];
	extern Time	lastTimestamp;

	body[0] = START_FLAG;
	body[1] = event_type;
	body[2] = num_datum + HEADER_SIZE;
	body[3] = lastTimestamp;

	va_start (ap, num_datum);
	for (i = 0; i < num_datum; i++)
		body[HEADER_SIZE + i] = va_arg (ap, ulong);

	va_end (ap);

	for (i = 0; i < npipes; i++)   
		PositiveWrite (i, body, (num_datum + 4) * sizeof (body[0]));
}


/*
 ****************************************************************
 *	Envia um pacote a um módulo				*
 ****************************************************************
 */
void
SendPacket (int module, ulong event_type, ulong num_datum, ...)
{
	va_list		ap;
	int		i;
	ulong		body[11];
	extern Time	lastTimestamp;

	body[0] = START_FLAG;
	body[1] = event_type;
	body[2] = num_datum + HEADER_SIZE;
	body[3] = lastTimestamp;    

	va_start (ap, num_datum);
	for (i = 0; i < num_datum; i++)
		body[HEADER_SIZE + i] = va_arg (ap, ulong);

	va_end (ap);

	PositiveWrite (module, body, (num_datum + 4) * sizeof (body[0]));

}	/* end SendPacket */

/*
 ****************************************************************
 *	Envia não sei o que					*
 ****************************************************************
 */
void
SendConfig (int module, ulong event_type, FVWMWIN *t)
{
	ulong		body[MAX_BODY_SIZE+HEADER_SIZE];
	extern Time	lastTimestamp;

	body[0] = START_FLAG;
	body[1] = event_type;
	body[2] = HEADER_SIZE+24;
	body[3] = lastTimestamp;
	body[HEADER_SIZE] = t->w;
	body[HEADER_SIZE+1] = t->frame;
	body[HEADER_SIZE+2] = (ulong)t;
	body[HEADER_SIZE+3] = t->frame_x;
	body[HEADER_SIZE+4] = t->frame_y;
	body[HEADER_SIZE+5] = t->frame_width;
	body[HEADER_SIZE+6] = t->frame_height;
	body[HEADER_SIZE+7] = t->Desk;
	body[HEADER_SIZE+8] = t->flags;
	body[HEADER_SIZE+9] = t->title_height;
	body[HEADER_SIZE+10] = t->boundary_width;
	body[HEADER_SIZE+11] = t->hints.base_width;
	body[HEADER_SIZE+12] = t->hints.base_height;
	body[HEADER_SIZE+13] = t->hints.width_inc;
	body[HEADER_SIZE+14] = t->hints.height_inc;
	body[HEADER_SIZE+15] = t->hints.min_width;
	body[HEADER_SIZE+16] = t->hints.min_height;
	body[HEADER_SIZE+17] = t->hints.max_width;
	body[HEADER_SIZE+18] = t->hints.max_height;
	body[HEADER_SIZE+19] = t->icon_w;
	body[HEADER_SIZE+20] = t->icon_pixmap_w;
	body[HEADER_SIZE+21] = t->hints.win_gravity;
	body[HEADER_SIZE+22] = t->TextPixel;
	body[HEADER_SIZE+23] = t->BackPixel;

	PositiveWrite (module, body, (HEADER_SIZE + 24) * sizeof (body[0]));

}	/* end SendConfig */


void BroadcastConfig(ulong event_type, FVWMWIN *t)
{
  ulong body[MAX_BODY_SIZE+HEADER_SIZE];
  int i;
  extern Time lastTimestamp;

  body[0] = START_FLAG;
  body[1] = event_type;
  body[2] = HEADER_SIZE+24;
  body[3] = lastTimestamp;
  body[HEADER_SIZE] = t->w;
  body[HEADER_SIZE+1] = t->frame;
  body[HEADER_SIZE+2] = (ulong)t;
  body[HEADER_SIZE+3] = t->frame_x;
  body[HEADER_SIZE+4] = t->frame_y;
  body[HEADER_SIZE+5] = t->frame_width;
  body[HEADER_SIZE+6] = t->frame_height;
  body[HEADER_SIZE+7] = t->Desk;
  body[HEADER_SIZE+8] = t->flags;
  body[HEADER_SIZE+9] = t->title_height;
  body[HEADER_SIZE+10] = t->boundary_width;
  body[HEADER_SIZE+11] = t->hints.base_width;
  body[HEADER_SIZE+12] = t->hints.base_height;
  body[HEADER_SIZE+13] = t->hints.width_inc;
  body[HEADER_SIZE+14] = t->hints.height_inc;
  body[HEADER_SIZE+15] = t->hints.min_width;
  body[HEADER_SIZE+16] = t->hints.min_height;
  body[HEADER_SIZE+17] = t->hints.max_width;
  body[HEADER_SIZE+18] = t->hints.max_height;
  body[HEADER_SIZE+19] = t->icon_w;
  body[HEADER_SIZE+20] = t->icon_pixmap_w;
  body[HEADER_SIZE+21] = t->hints.win_gravity;
  body[HEADER_SIZE+22] = t->TextPixel;
  body[HEADER_SIZE+23] = t->BackPixel;

  for(i=0;i<npipes;i++)   
    {  
      PositiveWrite(i,body,(HEADER_SIZE+24)*sizeof(body[0]));
    }
}

void BroadcastName(ulong event_type, ulong data1,
		   ulong data2, ulong data3, char *name)
{
  int l,i;
  ulong *body;
  extern Time lastTimestamp;


  if(name==NULL)
    return;
  l=strlen(name)/(sizeof(*body))+HEADER_SIZE+4;
  body = (ulong *)safemalloc(l*sizeof(*body));

  body[0] = START_FLAG;
  body[1] = event_type;
  body[2] = l;
  body[3] = lastTimestamp;

  body[HEADER_SIZE] = data1;
  body[HEADER_SIZE+1] = data2;
  body[HEADER_SIZE+2] = data3; 
  strcpy((char *)&body[HEADER_SIZE+3],name);


  for(i=0;i<npipes;i++)   
    PositiveWrite(i,(ulong *)body, l*sizeof(*body));
      
  free (body);

}


void SendName(int module, ulong event_type,
	      ulong data1,ulong data2, 
	      ulong data3, char *name)
{
  int l;
  ulong *body;
  extern Time lastTimestamp;

  if(name == NULL)
    return;
  l=strlen(name)/(sizeof(ulong))+HEADER_SIZE+4;
  body = (ulong *)safemalloc(l*sizeof(ulong));

  body[0] = START_FLAG;
  body[1] = event_type;
  body[2] = l;
  body[3] = lastTimestamp;

  body[HEADER_SIZE] = data1;
  body[HEADER_SIZE+1] = data2;
  body[HEADER_SIZE+2] = data3; 
  strcpy((char *)&body[HEADER_SIZE+3],name);

  PositiveWrite(module,(ulong *)body, l*sizeof(ulong));

  free (body);
}

/*
** send an arbitrary string to all instances of a module
*/
void SendStrToModule(XEvent *eventp,Window junk,FVWMWIN *tmp_win,
                     ulong context, char *action,int* Module)
{
  char *module,*str;
  int i;

  GetNextToken(action,&module);
  str = strdup(action + strlen(module) + 1);

  for (i=0;i<npipes;i++)
  {
    if((pipeName[i] != NULL)&&(matchWildcards(module,pipeName[i])))
    {
      SendName(i,M_STRING,0,0,0,str);
    }
  }
  
  free (module);
  free (str);
}



#include <errno.h>
FVWM_INLINE int PositiveWrite(int module, ulong *ptr, int size)
{
  if((pipeOn[module]<0)||(!((PipeMask[module]) & ptr[1])))
    return -1;

  AddToQueue(module,ptr,size,0);
  return size;
}


void AddToQueue(int module, ulong *ptr, int size, int done)
{
  QUEUE *c,*e;
  ulong *d;

  c = (QUEUE *)safemalloc(sizeof(QUEUE));
  c->next = NULL;
  c->size = size;
  c->done = done;
  d = (ulong *)safemalloc(size);
  c->data = d;
  memcpy(d,ptr,size);

  e = pipeQueue[module];
  if(e == NULL)
    {
      pipeQueue[module] = c;
      return;
    }
  while(e->next != NULL)
    e = e->next;
  e->next = c;
}

void
DeleteQueueBuff (int module)
{
	QUEUE	*q;

	if ((q = pipeQueue[module]) == NULL)
		return;

	pipeQueue[module] = q->next;
	free (q->data);
	free (q);

}	/* end DeleteQueueBuff */

void
FlushAllQueues (void)
{
	int	i;

	for (i = 0; i < npipes; i++)
		FlushQueue (i);

}	/* end FlushAllQueues */

void
FlushQueue (int module)
{
	char	*dptr;
	QUEUE	*d;
	int	n;

	if (pipeOn[module] <= 0 || pipeQueue[module] == NULL)
		return;

	while (pipeQueue[module] != NULL)
	{
		d = pipeQueue[module];
		dptr = (char *)d->data;

		while (d->done < d->size)
		{
			n =	write
				(	writePipes[module],
					&dptr[d->done],
					d->size - d->done
				);

			if (n >= 0)
			{
				d->done += n;
				continue;
			}

			if (errno == EPFULL || errno == EINTR)
				return;

			KillModule (module, 123);
			return;
		}

		DeleteQueueBuff (module);
	}
}


void send_list_func(XEvent *eventp,Window w,FVWMWIN *tmp_win,
			ulong context, char *action,
			int *Module)
{
  FVWMWIN *t;

  if(*Module >= 0)
    {
      SendPacket(*Module,M_NEW_DESK,1,Scr.CurrentDesk);
      SendPacket(*Module,M_NEW_PAGE,5,Scr.Vx,Scr.Vy,Scr.CurrentDesk,
		 Scr.VxMax,Scr.VyMax);
      if(Scr.Hilite != NULL)
	SendPacket(*Module,M_FOCUS_CHANGE,5,Scr.Hilite->w,Scr.Hilite->frame,
		   (ulong)Scr.Hilite,Scr.HiColors.fore,
		   Scr.HiColors.back);
      else
	SendPacket(*Module,M_FOCUS_CHANGE,5,0,0,0,Scr.HiColors.fore,
		   Scr.HiColors.back);
      if (Scr.DefaultIcon != NULL)
	SendName(*Module,M_DEFAULTICON,0,0,0,Scr.DefaultIcon);
      for (t = Scr.FvwmRoot.next; t != NULL; t = t->next)
	{
	  SendConfig(*Module,M_CONFIGURE_WINDOW,t);
	  SendName(*Module,M_WINDOW_NAME,t->w,t->frame,
		   (ulong)t,t->name);
	  SendName(*Module,M_ICON_NAME,t->w,t->frame,
		   (ulong)t,t->icon_name);
         if (t->icon_bitmap_file != NULL &&
	     t->icon_bitmap_file != Scr.DefaultIcon) 
           SendName(*Module,M_ICON_FILE,t->w,t->frame,
                    (ulong)t,t->icon_bitmap_file);
	  SendName(*Module,M_RES_CLASS,t->w,t->frame,
		   (ulong)t,t->class.res_class);
	  SendName(*Module,M_RES_NAME,t->w,t->frame,
		   (ulong)t,t->class.res_name);
	  
	  if((t->flags & ICONIFIED)&&(!(t->flags & ICON_UNMAPPED)))
	    SendPacket(*Module,M_ICONIFY,7,t->w,t->frame,
		       (ulong)t,
		       t->icon_x_loc,t->icon_y_loc,
		       t->icon_w_width, 
		       t->icon_w_height+t->icon_p_height);
	  if((t->flags & ICONIFIED) && (t->flags & ICON_UNMAPPED))
	    SendPacket(*Module,M_ICONIFY,7,t->w,t->frame,
		       (ulong)t,0,0,0,0);
	}
      if(Scr.Hilite == NULL)
	{
	  Broadcast(M_FOCUS_CHANGE,5,0,0,0,Scr.HiColors.fore,
		    Scr.HiColors.back);
	}
      else 
	{
	  Broadcast(M_FOCUS_CHANGE,5,Scr.Hilite->w,
		    Scr.Hilite->frame,(ulong)Scr.Hilite,
		    Scr.HiColors.fore,Scr.HiColors.back);
	}
      SendPacket(*Module,M_END_WINDOWLIST,0);
    }
}
void set_mask_function(XEvent *eventp,Window w,FVWMWIN *tmp_win,
		     ulong context, char *action,int* Module)
{
  int n, val1_unit;
  long val1;
  n = GetOneArgument(action, &val1, &val1_unit);
  PipeMask[*Module] = (ulong)val1;
}
