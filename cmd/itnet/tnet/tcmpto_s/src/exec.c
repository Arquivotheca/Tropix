/*
 ****************************************************************
 *								*
 *			exec.c					*
 *								*
 *	Comparação remota de árvores (servidor) 		*
 *								*
 *	Versão	2.3.0, de 28.10.91				*
 *		4.2.0, de 02.05.02				*
 *								*
 *	Módulo: TCMPTO_S/TNET					*
 *		Utilitários de comunicações			*
 *		Categoria D					*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 2002 NCE/UFRJ - tecle "man licença"	*
 * 								*
 ****************************************************************
 */

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/syscall.h>

#include <stdio.h>
#include <string.h>
#include <ftw.h>
#include <errno.h>

#include "../../tnet.h"
#include "../h/tcmpto_s.h"

/*
 ****************************************************************
 *	Processa as operações de atualizaçao			*
 ****************************************************************
 */
void
execute_op_list (void)
{
	char	*file_nm;
	char		*rmsg, *link_nm;
	int		op;
	char		file_nm_buf[BLSZ], link_nm_buf[BLSZ];

	for (EVER)
	{
		if ((op = t_rcv_msg (&rmsg)) < 0)	/* EOF */
			return;

		file_nm = file_nm_conversion (file_nm_buf, rmsg);

		/*
		 *	Processa as atualizações
		 */
		switch (op)
		{
		    case CPTO:
			copy_from (file_nm);
			break;
	
		    case CPFROM:
			copy_to (file_nm);
			break;
	
		    case R_RM:
			delete_remote_file (file_nm);
			break;
	
		    case R_LINK:
			t_rcv_msg (&rmsg);
			link_nm = file_nm_conversion (link_nm_buf, rmsg);
			link_remote_file (file_nm, link_nm);
			break;
	
		    default:
			error ("$Operação inválida: %d", op);
		
		}	/* end switch */

	}	/* end for (EVER) */

}	/* end execute_op_list */

/*
 ****************************************************************
 *	Remoção remota						*
 ****************************************************************
 */
void
delete_remote_file (const char *file_nm)
{
	STAT		s;

	if (lstat (file_nm, &s) < 0)
	{
		t_snd_msg (-1, strerror (errno));
		return;
	}

	if (S_ISDIR (s.st_mode))
	{
		if (rmdir (file_nm) < 0)
		{
			t_snd_msg (-1, strerror (errno));
			return;
		}
	}
	else
	{
		if (unlink (file_nm) < 0)
		{
			t_snd_msg (-1, strerror (errno));
			return;
		}
	}

	t_snd_msg (0, NOSTR);

}	/* end delete_remote_file */

/*
 ****************************************************************
 *	Link remoto						*
 ****************************************************************
 */
void
link_remote_file (const char *file_nm, const char *link_nm)
{
	if (verdir (file_nm) < 0)
	{
		t_snd_msg (-1, "Não consegui criar os diretórios intermediários");
		return;
	}

	if (link (link_nm, file_nm) < 0)
	{
		t_snd_msg (-1, strerror (errno));
		return;
	}

	t_snd_msg (0, NOSTR);

}	/* end link_remote_file */

/*
 ****************************************************************
 *	Processa as operações de comparação			*
 ****************************************************************
 */
void
execute_cmp_list (void)
{
	char	*cp, *file_nm;
	char		*rmsg;
	int		deleted = 0;

	qflag = 0;

	for (EVER)
	{
		if (t_rcv_msg (&rmsg) < 0)	/* EOF da lista */
			return;

		if (!deleted)
			{ delete_remote_link_list (); deleted++; }

		cp = file_prefix_ptr;

		strcpy (cp + 1, rmsg);	/* Deixa espaço para '/' */

		if   (cp[1] == '/')			/* "/alpha" */
		{
			file_nm = cp + 1;
		}
		elif (cp[1] == '.' && cp[2] == '\0')	/* "." */
		{
			file_nm = file_prefix_nm; cp[0] = '\0';
		}
		else					/* "alpha" */
		{
			file_nm = file_prefix_nm; cp[0] = '/';
		}

		/*
		 *	Processa as atualizações
		 */
		file_cmp (file_nm, (STAT *)NULL, FTW_F);

	}	/* end for (EVER) */

}	/* end execute_cmp_list */

/*
 ****************************************************************
 *	Converte o nome para o servidor				*
 ****************************************************************
 */
char *
file_nm_conversion (char *conv_area, const char *file_nm)
{
	char	*cp;

	cp = file_prefix_ptr;

	strcpy (cp + 1, file_nm);	/* Deixa espaço para '/' */

	if   (cp[1] == '/')			/* "/alpha" */
	{
		file_nm = cp + 1;
	}
	elif (cp[1] == '.' && cp[2] == '\0')	/* "." */
	{
		file_nm = file_prefix_nm; cp[0] = '\0';
	}
	else					/* "alpha" */
	{
		file_nm = file_prefix_nm; cp[0] = '/';
	}

	strcpy (conv_area, file_nm);

	return (conv_area);

}	/* end file_nm_conversion */
