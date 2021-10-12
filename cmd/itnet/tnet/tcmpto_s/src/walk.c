/*
 ****************************************************************
 *								*
 *			walk.c					*
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
#include <stdlib.h>
#include <string.h>
#include <ftw.h>
#include <xti.h>

#include "../../tnet.h"
#include "../h/tcmpto_s.h"

/*
 ****************************************************************
 *	Variáveis e Definições globais				*
 ****************************************************************
 */
const char	garfield[] = "garfield";

entry int	file_nm_offset;		/* Para obter o nome da BASE */

entry int	ftw_file_cnt = 0;

/*
 ******	Protótipos de funções ***********************************
 */
void	file_cp (const char *, STAT *);

/*
 ****************************************************************
 *	Processa os nomes das árvores (servidor)		*
 ****************************************************************
 */
void
walk (void)
{
	char		*cp, *tree_nm, *rmsg;

	for (EVER)
	{
		if (t_rcv_msg (&rmsg) < 0)
			break;

		cp = file_prefix_ptr;

		strcpy (cp + 1, rmsg);	/* Deixa espaço para '/' */

		if   (cp[1] == '/')			/* "/alpha" */
		{
			tree_nm = cp + 1;
			file_nm_offset = 0;
		}
		elif (cp[1] == '.' && cp[2] == '\0')	/* "." */
		{
			tree_nm = file_prefix_nm; cp[0] = '\0';
			file_nm_offset = cp + 1 - tree_nm;
		}
		else					/* "alpha" */
		{
			tree_nm = file_prefix_nm; cp[0] = '/';
			file_nm_offset = cp + 1 - tree_nm;
		}

		if (fromflag)
		{
			ftw_file_cnt = 0;

			ftw (tree_nm, name_analysis);

#if (0)	/*******************************************************/
	STAT		ls, s;

			if
			(	lstat (tree_nm, &ls) >= 0 && S_ISLNK (ls.st_mode) &&
				stat (tree_nm, &s) >= 0   && !S_ISDIR (s.st_mode)
			)
			{
				name_analysis (tree_nm, &ls, FTW_SL);
			}
			else
			{
				ftw (tree_nm, name_analysis);
			}
#endif	/*******************************************************/

			t_snd_msg (-1, NOSTR);
		}
		else
		{
			if (cpflag)
				file_cp (tree_nm, (STAT *)NULL);
			else
				file_cmp (tree_nm, (STAT *)NULL, FTW_F);
		}
	}

}	/* end walk */

/*
 ****************************************************************
 *	Analisa o nome de um arquivo				*
 ****************************************************************
 */
int
name_analysis (const char *file_nm, STAT *sp, int status)
{
	int		code = 0;

	ftw_file_cnt++;

	if (file_nm[0] == '.' && file_nm[1] == '/' && file_nm[2] != '\0')
		file_nm += 2;

	if (status == FTW_DNR)
		goto cmp;

	/*
	 *	Critérios para aceitar (ou não) o arquivo
	 */
	if (status == FTW_D)
	{
		if (ftw_file_cnt > 1)
		{
			if (!dotflag)
			{
				if (patmatch (file_nm, ".!.*") || patmatch (file_nm, "*/.!.*"))
					return (1);
			}

			if (Lflag)
				return (1);
		}
	}
	else
	{
		if (gflag && streq (file_nm, garfield))
			return (0);

		if (!pattern_accept (file_nm))
			return (0);
	}

	/*
	 *	Inicia a comparação
	 */
    cmp:
	t_snd_msg_no_push (0, file_nm + file_nm_offset);

	if (cpflag)
	{
		file_cp (file_nm, sp);
	}
	else
	{
		/* Para não caminhar em uma árvore */

		if (file_cmp (file_nm, sp, status) == -2)
			code = 1;
	}

	return (code);

}	/* end name_analysis */

/*
 ****************************************************************
 *	Copia o arquivo agora					*
 ****************************************************************
 */
void
file_cp (const char *file_nm, STAT *rsp)
{
	char		*rmsg, *link_nm, *remote_link_nm;
	STAT		remote_s;
	char		file_nm_buf[BLSZ], link_nm_buf[BLSZ];

	if (fromflag)
	{
		/*
		 *	Verifica se foi interativo e foi aceito
		 */
		t_push (tcp_fd);

		switch (t_rcv_msg ((char **)NULL))
		{
		    case F_PROCESS:
			break;

		    case F_PASS:
			goto pass;

		    default:
			error ("$Esperava um F_PROCESS ou F_PASS");
		}

		/*
		 *	Obtem o estado remoto
		 */
		if (rsp == (STAT *)NULL  &&  lstat (file_nm, &remote_s) >= 0)
			rsp = &remote_s;
	
		/*
		 *	Obtém o nome do LINK remoto (se houver)
		 */
		if (rsp != (STAT *)NULL)
			remote_link_nm = remote_link_proc (file_nm, rsp);
		else
			remote_link_nm = NOSTR;

		if (remote_link_nm == NOSTR)
		{
			t_snd_msg (CPFROM, NOSTR);

			if (t_rcv_msg (&rmsg) != CPFROM)
				error ("$Esperava um CPFROM");

			copy_to (file_nm);
		}
		else
		{
			t_snd_msg (L_LINK, remote_link_nm);
		}
	}
	else
	{
		t_push (tcp_fd);

		switch (t_rcv_msg (&rmsg))
		{
		    case CPTO:
			copy_from (file_nm);
			break;

		    case R_LINK:
			file_nm = strcpy (file_nm_buf, file_nm);
			t_rcv_msg (&rmsg);
			link_nm = file_nm_conversion (link_nm_buf, rmsg);
			link_remote_file (file_nm, link_nm);
			break;

		    case F_PASS:
			break;
			
		    default:
			error ("$Esperava um CPTO ou RLINK ou F_PASS");

		}	/* end switch */
	}

    pass:
	if (t_rcv_msg (&rmsg) >= 0)
		error ("$Esperava um -1");

}	/* end file_cp */
