/*
 ****************************************************************
 *								*
 *			umount.c				*
 *								*
 *	Desmonta sistemas de arquivos				*
 *								*
 *	Versão	1.1.0, de 23.07.87				*
 *		4.6.0, de 31.07.04				*
 *								*
 *	Módulo:	umount						*
 *		Utilitarios do Administrador			*
 *		Categoria B					*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 2004 NCE/UFRJ - tecle "man licença"	*
 * 								*
 ****************************************************************
 */

#include <sys/common.h>
#include <sys/scb.h>
#include <sys/sync.h>
#include <sys/mntent.h>
#include <sys/sb.h>
#include <sys/syscall.h>
#include <sys/kcntl.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stat.h>
#include <a.out.h>
#include <fcntl.h>
#include <errno.h>

/*
 ****************************************************************
 *	Definições e Variáveis Globais				*
 ****************************************************************
 */
const char		pgversion[] =  "Versão: 4.6.0, de 31.07.04";

const char		fstab_nm[] = "/etc/fstab";

entry int		aflag;		/* Desmonta todos os dispositivos */

entry int		ruid;		/* Usuário real */

entry SB		*sb_list;	/* Lista de SBs (ordem inversa) */

/*
 ******	Protótipos de funções ***********************************
 */
void			search_in_sblist_and_umount (const char *nm, int flags);
void			search_in_fstab_and_umount (const char *nm, long mode, int flags);
void			umount_all_devices (int flags);
void			create_SB_list (void);
void			help (void);

/*
 ****************************************************************
 *	Desmonta sistemas de arquivos				*
 ****************************************************************
 */
int
main (int argc, register const char *argv[])
{
	int		opt;
	const char	*nm;
	const char      *opt_str_nm = NOSTR;
	MNTENT		user_mntent;

	/*
	 *	Analisa as Opções
	 */
	while ((opt = getopt (argc, argv, "ao:H")) != EOF)
	{
		switch (opt)
		{
		    case 'a':		/* Todos os dispositivos */
			aflag++;
			break;

		    case 'o':		/* Lista de opções */
			opt_str_nm = optarg;
			break;

		    case 'H':		/* Help */
			help ();

		    default:		/* Oops */
			fprintf (stderr, "\n");
			help ();
		}
	}

	argv += optind;
	argc -= optind;

	/*
	 *	Verifica o Usuário Efetivo
	 */
	if (geteuid () != 0)
		error ("$O usuário efetivo NÃO é o SUPERUSUÁRIO");

	ruid = getuid ();

	/*
	 *	Cria a lista de SBs
	 */
	create_SB_list ();

	/*
	 *	Analisa as opções de montatem
	 */
	user_mntent.me_flags = 0;

	if (opt_str_nm != NOSTR)
	{
		if (getmntopt (opt_str_nm, &user_mntent) == 0)
			exit (1);
	}
	else
	{
		user_mntent.me_flags = -1;
	}

	/*
	 *	Analisa os Argumentos, se dados
	 */
	if (argc > 0)		/* Foram dados Argumentos */
	{
		if (aflag)
			help ();

		while ((nm = *argv++) != NOSTR)
			search_in_sblist_and_umount (nm, user_mntent.me_flags);
	}
	elif (aflag)		/* Desmonta todos os dispositivos */
	{
		if (ruid != 0)
			error ("$Opção \"-a\" permitida apenas ao superusuário");

		umount_all_devices (user_mntent.me_flags);
	}
	else
	{
		help ();
	}

	return (0);

}	/* end umount */

/*
 ****************************************************************
 *	Procura um nome na lista de SBs e desmonta		*
 ****************************************************************
 */
void
search_in_sblist_and_umount (const char *nm, int user_flags)
{
	const SB	*sp;
	static FILE	*fp = NOFILE;
	int		code, len;
	char		*path;
	static char	*cwd;
	char		dev_nm[sizeof (((SB *)0)->sb_dev_nm)];
	char		dir_nm[sizeof (((SB *)0)->sb_dev_nm)];
	MNTENT		fstab_mntent;

	/*
	 *	Se não for um caminho absoluto, expande
	 */
	if (nm[0] != '/')
	{
		if (cwd == NOSTR && (cwd = getcwd (NOSTR, 128)) == NOSTR)
			{ error ("*NÃO consegui obter o nome do diretório corrente"); return; }

		len = strlen (cwd) + 1 + strlen (nm) + 1; path = alloca (len);

		sprintf (path, "%s/%s", cwd, nm); nm = path;
	}

	/*
	 *	Percorre a lista
	 */
	for (sp = sb_list; /* abaixo */; sp = sp->sb_forw)
	{
		if (sp == NOSB)
			{ error ("Não achei \"%s\" na lista de dispositivos montados", nm); return; }

		if (streq (sp->sb_dev_nm, nm) || streq (sp->sb_dir_nm, nm))
			break;
	}

	/*
	 *	Agora que achamos o dispositivo, procura no arquivo FSTAB
	 */
	if   (fp != NOFILE)
		rewind (fp);
	elif ((fp = fopen (fstab_nm, "r")) == NOFILE)
		{ error ("*Não consegui abrir \"%s\"", fstab_nm); return; }

	for (EVER)
	{
		if ((code = getmntent (fp, dev_nm, dir_nm, &fstab_mntent)) == EOF)
			{ fstab_mntent.me_flags = SB_DEFAULT_ON & ~SB_USER; break; }

		if (code == 0)
			continue;

		if (streq (dev_nm, nm) || streq (dir_nm, nm))
			break;
	}

	/*
	 *	Completa as opções e desmonta
	 */
	if   (ruid != 0 && (fstab_mntent.me_flags & SB_USER) == 0)
		error ("Somente o superusuário pode desmontar o dispositivo \"%s\"", sp->sb_dev_nm);
	elif (umount (sp->sb_dev_nm, (user_flags != -1) ? user_flags : fstab_mntent.me_flags) < 0)
		error ("*Não consegui desmontar \"%s\"", sp->sb_dev_nm);


}	/* end search_in_sblist_and_umount */

/*
 ****************************************************************
 *	Desmonta todos os Sistemase de Arquivos			*
 ****************************************************************
 */
void
umount_all_devices (int user_flags)
{
	const SB	*sp;

	/*
	 *	Percorre a lista
	 */
	for (sp = sb_list; sp != NOSB; sp = sp->sb_forw)
	{
		if (umount (sp->sb_dev_nm, (user_flags != -1) ? user_flags : sp->sb_mntent.me_flags) < 0)
			error ("*Não consegui desmontar \"%s\"", sp->sb_dev_nm);
	}

}	/* end umount_all_devices */

/*
 ****************************************************************
 *	Prepara a lista de SBs (na ordem inversa)		*
 ****************************************************************
 */
void
create_SB_list (void)
{
	int		len;
	SYM		*sym_p;
	const SB	*phys_sp, *next_phys_sp;
	SB		*sp, *last_sp = NOSB;
	SCB		scb;

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

	next_phys_sp = phys_sp->sb_back;

	phys (phys_sp, 0, 0);

	/*
	 *	Percorre a Lista de SBs e cria uma cópia
	 */
	while (next_phys_sp != (SB *)sym_p->s_value)
	{
		phys_sp = phys (next_phys_sp, sizeof (SB), O_KERNEL|O_RDONLY);

		if ((int)phys_sp == -1)
			error ("$*Não consegui mapear um SB");

		if (phys_sp->sb_code == FS_PIPE)
			break;

		if ((sp = malloc (sizeof (SB))) == NOSB)
			error (NOSTR);

		memmove (sp, phys_sp, sizeof (SB));

		if (sb_list == NOSB)
			sb_list = sp;
		else
			last_sp->sb_forw = sp;

		last_sp = sp; sp->sb_forw = NOSB;
			
		next_phys_sp = phys_sp->sb_back;

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
 *	Resumo de utilização do programa			*
 ****************************************************************
 */
void
help (void)
{
	fprintf
	(	stderr,
		"%s - desmonta sistemas de arquivos\n"
		"\n%s\n"
		"\nSintaxe:\n"
		"\t%s [-o <opções>] <dispositivo ou diretório> ...\n"
		"\t%s [-o <opções>] -a\n",
		pgname, pgversion, pgname, pgname
	);
	fprintf
	(	stderr,
		"\nOpções:"
		"\t-o: Dá uma lista de <opções> de (des)montagem\n"
		"\t-a: Desmonta todos os dispositivos (fora de uso) montados\n"
	);

	exit (2);

}	/* end help */
