/*
 ****************************************************************
 *								*
 *			exec.c					*
 *								*
 *	Comparação remota de árvores (cliente) 			*
 *								*
 *	Versão	2.3.0, de 28.10.91				*
 *		4.2.0, de 02.05.02				*
 *								*
 *	Módulo: TCMPTO_C/TNET					*
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
#include <stdarg.h>
#include <string.h>
#include <termio.h>
#include <xti.h>

#include "../../tnet.h"
#include "../h/tcmpto_c.h"

/*
 ****************************************************************
 *	Variáveis e Definições globais				*
 ****************************************************************
 */
entry OP_BL	*op_list_first,		/* Lista de operações */
		*op_list_last;

extern TERMIO	old_video, video;	/* modos do terminal */

/*
 ******	Protótipos de funções ***********************************
 */
void		add_op_in_list (const char *file_nm, OP_NM op, const char *nm);

/*
 ****************************************************************
 *	Guarda informações sobre a operação a realizar		*
 ****************************************************************
 */
int
save_op_info (const char *file_nm, const STAT *lsp, const STAT *rsp, OP_NM op, ...)
{
	va_list		args;
	time_t		ltime = 0, rtime = 0;
	char		*local_link_nm = NOSTR, *remote_link_nm = NOSTR;
	int		local = 0, cuidado = 0;

	/*
	 *	Devolve:
	 *		 0 se não é necessária atualização ou ela vai ser feita
	 *		 1 se é necessária atualização de um diretório, mas foi recusada
	 */

	/*
	 *	Verifica se deve ser feita alguma operação
	 */
	if (tflag + fflag + dflag + gflag == 0)
		return (0);

	if (gflag)
		{ fprintf (gar_fp, "%s\n", file_nm); return (0); }

	/*
	 *	Inicialmente, obtém os argumentos
	 */
	va_start (args, op);

	switch (op)
	{
	    case CP:
		ltime = va_arg (args, time_t);
		rtime = va_arg (args, time_t);
		break;

	    case L_LINK:
	    case L_RM_LINK:
		remote_link_nm = va_arg (args, char *);
		break;

	    case R_LINK:
	    case R_RM_LINK:
		local_link_nm = va_arg (args, char *);
		break;

	    default:	/* Pode ocorrer? */
		break;

	}	/* end switch (op) */

	va_end (args);

	/*
	 *	Tenta decidir o sentido, se não foi dado
	 */
	if (op == CP)
	{
		if   (tflag)
		{
			op = CPTO;

			if (rtime > ltime)
			{
				msg ("CUIDADO: A versão mais recente pode ser DESTRUÍDA!");
				cuidado++;
			}
		}
		elif (fflag)
		{
			op = CPFROM;

			if (ltime > rtime)
			{
				msg ("CUIDADO: A versão mais recente pode ser DESTRUÍDA!");
				cuidado++;
			}
		}
		elif (ltime > rtime)
		{
			op = CPTO;
		}
		elif (ltime < rtime)
		{
			op = CPFROM;
		}
		else
		{
			msg ("Os arquivos têm a mesma data");
			msg ("Não foi possível determinar o sentido da cópia");
			return (0);
		}
	}

	/*
	 *	Descobre se é uma operação local ou remota
	 */
	switch (op)
	{
	    case CPTO:
	    case R_RM_CPTO:
	    case R_LINK:
	    case R_RM_LINK:
		local = 0;
		break;

	    case CPFROM:
	    case L_RM_CPFROM:
	    case L_LINK:
	    case L_RM_LINK:
		local = 1;
		break;

	    default:
		error ("$Operação inválida: %d", op);

	}	/* end switch (op) */

	/*
	 *	Verifica em que casos a operação nem deve ser feita
	 */
	if (local)
	{
		if (tflag)
			return (0);
	}
	else
	{
		if (fflag)
			return (0);
	}

	/*
	 *	Confirma a operação, no modo interativo
	 */
	if (iflag + cuidado)
	{
		if (cuidado)
			askyesno_set (1);

		if (local) 		/* Operação local */
		{
			fprintf (stderr, "\tAtualiza \"%s\" em \"%s\" <== ? (n): ", file_nm, local_node_nm);

			if (askyesno () <= 0)
			{
				if (lsp == (STAT *)NULL)
				{
					fprintf
					(	stderr,
						"\tRemove \"%s\" em \"%s\"? (n): ",
						file_nm, remote_node_nm
					);

					if (askyesno () > 0)
						add_op_in_list (file_nm, R_RM, NOSTR);
				}

				if (fromflag && S_ISDIR (rsp->st_mode))
					return (1);
				else
					return (0);
			}
		}
		else			/* Operação remota */
		{
			fprintf (stderr, "\tAtualiza \"%s\" em ==> \"%s\" ? (n): ", file_nm, remote_node_nm);

			if (askyesno () <= 0)
			{
				if (rsp == (STAT *)NULL)
				{
					fprintf
					(	stderr,
						"\tRemove \"%s\" em \"%s\"? (n): ",
						file_nm, local_node_nm
					);

					if (askyesno () > 0)
						add_op_in_list (file_nm, L_RM, NOSTR);
				}

				if (!fromflag && S_ISDIR (lsp->st_mode))
					return (1);
				else
					return (0);
			}
		}

	}	/* end if (interativo) */

#if (0)	/*******************************************************/
		fprintf
		(	stderr,
			"\tAtualiza \"%s\" em %s \"%s\" %s? (n): ",
			file_nm,
			local ? "" : "==>",
			local ? local_node_nm : remote_node_nm,
			local ? "<== " : ""
		);
			if (lsp == (STAT *)NULL || lsp->st_mode == 0)
			{
				fprintf
				(	stderr,
					"\tRemove \"%s\" em \"%s\"? (n): ",
					file_nm, local ? remote_node_nm : local_node_nm
				);

				if (askyesno () > 0)
					add_op_in_list (file_nm, local ? R_RM : L_RM, NOSTR);

			}
#endif	/*******************************************************/

	/*
	 *	Acrescenta a operação na lista
	 */
	switch (op)
	{
	    case CPTO:
	    case CPFROM:
		add_op_in_list (file_nm, op, NOSTR);
		break;

	    case R_RM_CPTO:
		add_op_in_list (file_nm, R_RM, NOSTR);
		add_op_in_list (file_nm, CPTO, NOSTR);
		break;

	    case L_RM_CPFROM:
		add_op_in_list (file_nm, L_RM, NOSTR);
		add_op_in_list (file_nm, CPFROM, NOSTR);
		break;

	    case R_LINK:
		add_op_in_list (file_nm, R_LINK, local_link_nm);
		break;

	    case R_RM_LINK:
		add_op_in_list (file_nm, R_RM, NOSTR);
		add_op_in_list (file_nm, R_LINK, local_link_nm);
		break;

	    case L_LINK:
		add_op_in_list (file_nm, L_LINK, remote_link_nm);
		break;

	    case L_RM_LINK:
		add_op_in_list (file_nm, L_RM, NOSTR);
		add_op_in_list (file_nm, L_LINK, remote_link_nm);
		break;

	    default:
		error ("$Operação inválida: %d", op);

	}	/* end switch (op) */

	return (0);

}	/* end save_op_info */

/*
 ****************************************************************
 *	Acrescenta uma operação na lista			*
 ****************************************************************
 */
void
add_op_in_list (const char *file_nm, OP_NM op, const char *link_nm)
{
	OP_BL		*otp;

	if ((otp = malloc (sizeof (OP_BL))) == (OP_BL *)NULL)
		error ("$Memória esgotada");

	otp->o_op = op;

	if ((otp->o_file_nm = strdup (file_nm)) == NOSTR)
		error ("$Memória esgotada");

	otp->o_link_nm = NOSTR;

	if (link_nm != NOSTR && (otp->o_link_nm = strdup (link_nm)) == NOSTR)
		error ("$Memória esgotada");

	/*
	 *	Põe na lista
	 */
	otp->o_next = (OP_BL *)NULL;

	if (op_list_first == (OP_BL *)NULL) 
		op_list_first = otp;
	else
		op_list_last->o_next = otp;

	op_list_last  = otp;

}	/* end add_op_in_list */

/*
 ****************************************************************
 *	Imprime a Mensagem Comparando os Tempos			*
 ****************************************************************
 */
void
time_cmp (time_t ltime, time_t rtime)
{
	if (ltime == rtime)
		return;
	else
		fprintf (stderr, "\t**** A Versão mais recente é de ");

	if (ltime > rtime)
		fprintf (stderr, "\"%s\" ", local_node_nm);
	else
		fprintf (stderr, "\"%s\" ", remote_node_nm);

	print_interval (ltime - rtime);

	fprintf (stderr, "\n");

}	/* end time_cmp */

/*
 ****************************************************************
 *	Emite uma noção de intervalo de tempo			*
 ****************************************************************
 */
void
print_interval (time_t inter)
{
	if   (inter == 0)
		return;
	elif (inter < 0)
		inter = -inter;

	fprintf (stderr, "(");

	if   (inter < 60)
		fprintf (stderr, "%d segundo%s", inter, plural (inter));
	elif ((inter /= 60) < 60)
		fprintf (stderr, "%d minuto%s", inter, plural (inter));
	elif ((inter /= 60) < 24)
		fprintf (stderr, "%d hora%s", inter, plural (inter));
	elif ((inter /= 24) < 30)
		fprintf (stderr, "%d dia%s", inter, plural (inter));
	elif (inter < 365)
		fprintf (stderr, "%d mes(es)", inter / 30);
	else
		fprintf (stderr, "%d ano%s", inter / 365, plural (inter / 365));

	fprintf (stderr, ")");

}	/* end print_interval */

#ifdef	DEBUG
const char	* const op_names[] = OP_STR;

/*
 ****************************************************************
 *	Imprime a lista de operações				*
 ****************************************************************
 */
void
print_op_list (void)
{
	OP_BL		*otp;

	for (otp = op_list_first; otp != (OP_BL *)NULL; otp = otp->o_next)
	{
		fprintf
		(	stderr,
			"%8s \"%s\" \"%s\"\n",
			otp->o_op >= 0 && otp->o_op <= R_RM ? op_names[otp->o_op] : "?",
			otp->o_file_nm == NOSTR ? "-" : otp->o_file_nm,
			otp->o_link_nm == NOSTR ? "-" : otp->o_link_nm
		);
	}

}	/* print_op_list */
#endif	DEBUG

/*
 ****************************************************************
 *	Processa o Comando					*
 ****************************************************************
 */
void
execute_op_list (void)
{
	OP_BL		*otp;

	if (op_list_first != (OP_BL *)NULL)
		fprintf (stderr, "ATUALIZANDO ...\n");

	for (otp = op_list_first; otp != (OP_BL *)NULL; otp = otp->o_next)
	{
		fprintf (stderr, "%s:\n", otp->o_file_nm);

		switch (otp->o_op)
		{
		    case CPTO:
			copy_to (otp->o_file_nm);
			break;

		    case CPFROM:
			copy_from (otp->o_file_nm);
			break;
	
		    case R_RM:
			delete_remote_file (otp->o_file_nm);
			break;
	
		    case L_RM:
			delete_local_file (otp->o_file_nm);
			break;
	
		    case R_LINK:
			link_remote_file (otp->o_file_nm, otp->o_link_nm);
			break;
	
		    case L_LINK:
			link_local_file (otp->o_file_nm, otp->o_link_nm);
			break;
	
		    default:
			error ("$Operação inválida: %d", otp->o_op);
	
		}	/* end switch (op) */

	}	/* end for (lista) */

	t_snd_msg (-1, NOSTR);		/* EOF */

}	/* end execute_op_list */

/*
 ****************************************************************
 *	Realiza as comparações após as cópias			*
 ****************************************************************
 */
void
execute_cmp_list (void)
{
	OP_BL		*otp;

	/*
	 *	Para não tentar outra cópia
	 */
	if (vflag && op_list_first != (OP_BL *)NULL)
	{
		tflag = 0; 	fflag = 0; 	dflag = 0;
	 	gflag = 0; 	qflag = 0;

#ifdef	REPORTs
		ioctl (fileno (tty_fp), TCSETNR, &old_video);
#endif	REPORTs

		fprintf (stderr, "COMPARANDO ...\n");

		delete_local_link_list ();

		/*
		 *	Percorre a lista comparando
		 */
		for (otp = op_list_first; otp != (OP_BL *)NULL; otp = otp->o_next)
		{
			if (otp->o_op == L_RM || otp->o_op == R_RM)
				continue;

			t_snd_msg (0, otp->o_file_nm);
	
			file_cmp (otp->o_file_nm, (STAT *)NULL);
	
			t_snd_msg (-1, NOSTR);		/* EOF do arquivo */
	
		}	/* end for (lista) */

	}	/* end if (lista não vazia) */

	t_snd_msg (-1, NOSTR);		/* EOF da lista */

}	/* end execute_cmp_list */

/*
 ****************************************************************
 *	Executa o pipe						*
 ****************************************************************
 */
void
execute_pipe (const char **argv)
{
	t_snd_msg (-1, NOSTR);		/* EOF correspondente a "walk" */

	for (/* vazio */; *argv != NOSTR; argv++) 
		copy_from (*argv);

	t_snd_msg (-1, NOSTR);		/* EOF correspondente a "op_list" */

	t_snd_msg (-1, NOSTR);		/* EOF correspondente a "cmp_list" */

}	/* end execute_pipe */
