/*
 ****************************************************************
 *								*
 *			fstab.c					*
 *								*
 *	Gerenciador de arquivos					*
 *								*
 *	Versão	4.5.0, de 08.10.03				*
 *		4.8.0, de 16.07.05				*
 *								*
 *	Módulo: xfm						*
 *		Utilitários Básicos				*
 *		Categoria B					*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 2005 NCE/UFRJ - tecle "man licença"	*
 * 								*
 ****************************************************************
 */

#include <sys/common.h>
#include <sys/scb.h>
#include <sys/sync.h>
#include <sys/mntent.h>
#include <sys/sb.h>
#include <sys/kcntl.h>
#include <sys/syscall.h>

#include <stdio.h>
#include <stdlib.h>
#include <stat.h>
#include <a.out.h>
#include <fcntl.h>

#include <X11/Intrinsic.h>

#include "../h/xfm.h"

/*
 ****************************************************************
 *	Variáveis e Definições globais				*
 ****************************************************************
 */
entry const char	fstab_nm[] = "/etc/fstab";

entry FSTAB		*fstab_list;

entry SB		*sb_list;	/* Lista de SBs (ordem normal) */

/*
 ****************************************************************
 *	Lê a tabela "fstab"					*
 ****************************************************************
 */
int
read_fstab (void)
{
	FILE		*fp;
	FSTAB		*tp, *next_tp;
	int		code;
	MNTENT		mntent;

	/*
	 *	Em primeiro lugar, remove a lista antiga
	 */
	for (tp = fstab_list; tp != NOFSTAB; tp = next_tp)
		{ next_tp = tp->fs_next; free (tp); }

	fstab_list = NOFSTAB;

	/*
	 *	Lê toda a FSTAB
	 */
	if ((fp = fopen (fstab_nm, "r")) == NOFILE)
		{ draw_error_msg_text ("*Não consegui abrir \"%s\"", fstab_nm); return (-1); }

	for (EVER)
	{
		if ((tp = malloc (sizeof (FSTAB))) == NOFSTAB)
			error (NOSTR);
		do
		{
			code = getmntent (fp, tp->fs_dev_nm, tp->fs_dir_nm, &mntent);
			tp->fs_flags = mntent.me_flags;
		}
		while (code == 0);

		if (code == EOF)
			{ free (tp); break; }

		tp->fs_next = fstab_list; fstab_list = tp;
	}

	fclose (fp);

	return (0);

}	/* end read_fstab */

/*
 ****************************************************************
 *	Procura um diretório em "fstab"				*
 ****************************************************************
 */
const FSTAB *
search_dir_in_fstab (const char *dir_nm)
{
	const FSTAB	*tp;

	/*
	 *	Procura na lista FSTAB
	 */
	for (tp = fstab_list; /* abaixo */; tp = tp->fs_next)
	{
		if (tp == NOFSTAB)
			return (NOFSTAB);

		if (streq (tp->fs_dir_nm, dir_nm))
			return (tp);
	}

}	/* end search_dir_in_fstab */

/*
 ****************************************************************
 *	Prepara a lista de SBs (na ordem normal)		*
 ****************************************************************
 */
void
create_SB_list (void)
{
	int		len;
	SYM		*sym_p;
	const SB	*phys_sp, *next_phys_sp;
	SB		*sp, *forw_sp, *last_sp = NOSB;
	SCB		scb;

	/*
	 *	Em primeiro lugar, remove a lista antiga
	 */
	for (sp = sb_list; sp != NOSB; sp = forw_sp)
		{ forw_sp = sp->sb_forw; free (sp); }

	sb_list = NOSB;

	/*
	 *	Obtem o "system control block"
	 */
	if ((int)getscb (&scb) == -1)
		error ("$*Não consegui obter o SCB");

	/*
	 *	Obtém o endereço da estrutura "sb_head"
	 */
	len = strlen ("_sb_head"); sym_p = alloca (SYM_SIZEOF (len));

	sym_p->s_nm_len = len; strcpy (sym_p->s_name, "_sb_head");

	if (kcntl (K_GETSYMGN, len, sym_p) < 0)
		error ("$*Não consegui obter o endereço de \"_sb_head\"");

	/*
	 *	Mapeia a cabeça da fila
	 */
	phys_sp = phys ((void *)sym_p->s_value, sizeof (SB), O_KERNEL|O_RDONLY);

	if ((int)phys_sp == -1)
		error ("$*Não consegui mapear SBHEAD");

	next_phys_sp = phys_sp->sb_forw;

	phys (phys_sp, 0, 0);

	/*
	 *	Percorre a Lista de SBs e cria uma cópia
	 */
	while (next_phys_sp != (SB *)sym_p->s_value)
	{
		phys_sp = phys (next_phys_sp, sizeof (SB), O_KERNEL|O_RDONLY);

		if ((int)phys_sp == -1)
			error ("$*Não consegui mapear um SB");

		if ((sp = malloc (sizeof (SB))) == NOSB)
			error (NOSTR);

		memmove (sp, phys_sp, sizeof (SB));

		if (sb_list == NOSB)
			sb_list = sp;
		else
			last_sp->sb_forw = sp;

		last_sp = sp; sp->sb_forw = NOSB;
			
		next_phys_sp = phys_sp->sb_forw;

		if ((int)phys (phys_sp, 0, 0) == -1)
			error ("*Não consegui desmapear um SB");
	}

#ifdef	DEBUG
	/*
	 *	Imprime a lista
	 */
	for (sp = sb_list; sp != NOSB; sp = sp->sb_forw)
		printf ("\"%s\" => \"%s\"\n", sp->sb_dev_nm, sp->sb_dir_nm);
#endif	DEBUG

}	/* end create_SB_list */

/*
 ****************************************************************
 * Procura um dispositivo e devolve o indicadores de montagem	*
 ****************************************************************
 */
int
get_mount_flags (dev_t dev)
{
	const SB	*sp;

	for (sp = sb_list; sp != NOSB; sp = sp->sb_forw)
	{
		if (sp->sb_dev == dev)
			return (sp->sb_mntent.me_flags);
	}

printf ("get_mount_flags: NÃO achei o SB_FLAGS\n");
	return (0);

}	/* end get_mount_flags */

/*
 ****************************************************************
 *	Procura um dispositivo e devolve o SB			*
 ****************************************************************
 */
const SB *
get_super_block (dev_t dev)
{
	const SB	*sp;

	for (sp = sb_list; sp != NOSB; sp = sp->sb_forw)
	{
		if (sp->sb_dev == dev)
			return (sp);
	}

	return (NOSB);

}	/* end get_super_block */
