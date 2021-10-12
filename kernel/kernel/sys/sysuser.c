/*
 ****************************************************************
 *								*
 *			sysuser.c				*
 *								*
 *	Chamadas ao sistema: estado do processo do usuário	*
 *								*
 *	Versão	3.0.0, de 22.12.94				*
 *		3.0.3, de 05.08.96				*
 *								*
 *	Módulo: Núcleo						*
 *		NÚCLEO do TROPIX para PC			*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 1999 NCE/UFRJ - tecle "man licença"	*
 * 								*
 ****************************************************************
 */

#include "../h/common.h"
#include "../h/sync.h"
#include "../h/scb.h"
#include "../h/region.h"

#include "../h/frame.h"
#include "../h/utsname.h"
#include "../h/uerror.h"
#include "../h/signal.h"
#include "../h/uproc.h"

#include "../h/extern.h"
#include "../h/proto.h"

/*
 ****************************************************************
 *	Chamada ao sistema "setreuid"				*
 ****************************************************************
 */
int
setreuid (register int ruid, register int euid)
{
	if   (ruid == -1)
		ruid = u.u_ruid;
	elif ((unsigned)ruid > 65535)
		{ u.u_error = EINVAL; return (-1); }

	if   (euid == -1)
		euid = u.u_euid;
	elif ((unsigned)euid > 65535)
		{ u.u_error = EINVAL; return (-1); }

	if (u.u_euid != 0)
	{
		 if (ruid != euid  ||  ruid != u.u_euid && euid != u.u_ruid)
			{ u.u_error = EPERM; return (-1); }
	}

	u.u_euid = euid;
	u.u_ruid = ruid;

	return (0);

}	/* end setreuid */

/*
 ****************************************************************
 *	Chamada ao sistema "setuid"				*
 ****************************************************************
 */
int
setuid (register int uid)
{
	if   ((unsigned)uid > 65535)
	{
		u.u_error = EINVAL;
	}
	elif (u.u_euid == 0)
	{
		u.u_euid = uid;
		u.u_ruid = uid;
	}
	elif (u.u_ruid == uid)
	{
		u.u_euid = uid;
	}
	else
	{
		u.u_error = EPERM;
	}

	return (UNDEF);

}	/* end setuid */

/*
 ****************************************************************
 *	Chamada ao sistema "getuid"				*
 ****************************************************************
 */
int
getuid (void)
{
	u.u_frame->s.sf_r1 = u.u_euid;

	return (u.u_ruid);

}	/* end getuid */

/*
 ****************************************************************
 *	Chamada ao sistema "setregid"				*
 ****************************************************************
 */
int
setregid (register int rgid, register int egid)
{
	if   (rgid == -1)
		rgid = u.u_rgid;
	elif ((unsigned)rgid > 65535)
		{ u.u_error = EINVAL; return (-1); }

	if   (egid == -1)
		egid = u.u_egid;
	elif ((unsigned)egid > 65535)
		{ u.u_error = EINVAL; return (-1); }

	if (u.u_euid != 0)
	{
		 if (rgid != egid  ||  rgid != u.u_egid && egid != u.u_rgid)
			{ u.u_error = EPERM; return (-1); }
	}

	u.u_egid = egid;
	u.u_rgid = rgid;

	return (0);

}	/* end setregid */

/*
 ****************************************************************
 *	Chamada ao sistema "setgid"				*
 ****************************************************************
 */
int
setgid (register int gid)
{
	if   ((unsigned)gid > 65535)
	{
		u.u_error = EINVAL;
	}
	elif (u.u_euid == 0)
	{
		u.u_egid = gid;
		u.u_rgid = gid;
	}
	elif (u.u_rgid == gid)
	{
		u.u_egid = gid;
	}
	else
	{
		u.u_error = EPERM;
	}

	return (UNDEF);

}	/* end setgid */

/*
 ****************************************************************
 *	Chamada ao sistema "getgid"				*
 ****************************************************************
 */
int
getgid (void)
{

	u.u_frame->s.sf_r1 = u.u_egid;

	return (u.u_rgid);

}	/* end getgid */

#if (0)	/*************************************/
/*
 ****************************************************************
 *	Chamada ao sistema "ulimit"				*
 ****************************************************************
 */
int
ulimit (void)
{
	u.u_error = EINVAL;

	return (-1);

}	/* end ulimit */
#endif	/*************************************/

/*
 ****************************************************************
 *	Chamada ao sistema "uname"				*
 ****************************************************************
 */
int
uname (UTSNAME *area)
{
	if (unimove (area, &scb.y_uts, sizeof (UTSNAME), SU) < 0)
		u.u_error = EINVAL;

	return (UNDEF);

}	/* end uname */
