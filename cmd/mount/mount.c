/*
 ****************************************************************
 *								*
 *			mount.c					*
 *								*
 *	Monta um sistema de arquivos				*
 *								*
 *	Versão	1.1.0, de 15.05.87				*
 *		4.8.0, de 30.09.05				*
 *								*
 *	Módulo: mount						*
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

#include <sys/stat.h>
#include <sys/kcntl.h>
#include <sys/itnet.h>
#include <sys/mntent.h>
#include <sys/sb.h>
#include <sys/syscall.h>
#include <sys/xti.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <a.out.h>
#include <pwd.h>
#include <fcntl.h>
#include <errno.h>

/*
 ****************************************************************
 *	Variáveis e Definições globais				*
 ****************************************************************
 */
const char		pgversion[] =  "Versão:	4.8.0, de 30.09.05";

#undef	DEBUG

entry int		aflag;		/* Auto */
entry int		vflag;		/* Verbose */

entry SB		*sb_list;	/* Lista de SBs (ordem normal) */

entry const EXPORT	*nfs_mount;	/* Sistemas de arquivos montados remotamente */
entry int		nfs_mount_sz;

const char		fstab_nm[] = "/etc/fstab";

/*
 ******	Tabela dos códigos dos Sistemas de Arquivos *************
 */
const char		*fscode_to_str[] = FS_NAMES;

/*
 ****** Protótipos de funções ***********************************
 */
void			read_fstab_and_mount_auto (const char *, struct mntent *);
void			search_in_fstab_and_mount (const char *nm, const char *opt_str_nm, struct mntent *);
void			do_mount (const char *dev_nm, const char *dir_nm, struct mntent *);
int			decode_nfs_details (const char *, const char *, struct mntent *);
void			print_mount_table (struct mntent *);
void			create_SB_list (void);
const SB		*search_SB_list (const char *dev_nm);
void			read_nfs_mount_table (void);
void			help (void);

/*
 ****************************************************************
 *	Monta um sistema de arquivos				*
 ****************************************************************
 */
int
main (int argc, const char *argv[])
{
	int		opt;
	const char	*opt_str_nm = "";
	struct mntent	mntent;

	/*
	 *	Analisa as opções
	 */
	while ((opt = getopt (argc, argv, "rwo:avH")) != EOF)
	{
		switch (opt)
		{
		    case 'r':			/* Read-only */
			if (opt_str_nm[0])
				help ();

			opt_str_nm = "ro";
			break;

		    case 'w':			/* Read/write */
			if (opt_str_nm[0])
				help ();

			opt_str_nm = "rw";
			break;

		    case 'o':			/* Lista de opções */
			if (opt_str_nm[0])
				help ();

			opt_str_nm = optarg;
			break;

		    case 'a':			/* Auto */
			aflag++;
			break;

		    case 'v':			/* Verbose */
			vflag++;
			break;

		    case 'H':			/* Help */
			help ();

		    default:			/* Erro */
			fprintf (stderr, "\n");
			help ();

		}	/* end switch */

	}	/* end while */

	argv += optind;
	argc -= optind;

	/*
	 *	Verifica o Usuário Efetivo
	 */
	if (geteuid () != 0)
		error ("$O usuário efetivo NÃO é o SUPERUSUÁRIO");

	/*
	 *	Atribui os valores "default"
	 */
	defmntent (&mntent);

	/*
	 *	Cria a lista dos dispositivos já montados
	 */
	create_SB_list ();

	/*
	 *	Lê a tabela de sistemas de arquivos montados remotamente 
	 */
	read_nfs_mount_table ();

	/*
	 *	Verifica se deve imprimir a tabela
	 */
	if   (aflag)				/* Monta todos com AUTO */
	{
		if (argc != 0)
			help ();

		if (getuid () != 0)
			error ("$Opção \"-a\" permitida apenas ao superusuário");

		read_fstab_and_mount_auto (opt_str_nm, &mntent);
	}
	elif (argc == 0)			/* Imprime a tabela */
	{
		print_mount_table (&mntent);
	}
	elif (argc == 1)			/* Procura os dados em "/etc/fstab" */
	{
		search_in_fstab_and_mount (argv[0], opt_str_nm, &mntent);
	}
	elif (argc == 2)			/* Tudo dado para a montagem */
	{
		if (getuid () != 0)
			error ("$Forma de montagem permitida apenas ao superusuário");

		if (getmntopt (opt_str_nm, &mntent) == 0)
			exit (1);
#ifdef	DEBUG
		{
			char		area[256];

			printf
			(	"Montando \"%s\" em \"%s\", %P (%s)\n",
				argv[0], argv[1], mntent.me_flags,
				editmntopt (area, &mntent, vflag)
			);
		}
#else
		do_mount (argv[0], argv[1], &mntent);
#endif	DEBUG
	}
	else					/* Erro */
	{
		help ();
	}

	return (0);

}	/* end mount */

/*
 ****************************************************************
 *	Monta todos com AUTO					*
 ****************************************************************
 */
void
read_fstab_and_mount_auto (const char *opt_str_nm, struct mntent *mntent_ptr)
{
	FILE		*fp;
	const SB	*sp;
	int		code;
	char		dev_nm[32];
	char		dir_nm[32];

	/*
	 *	Procura no arquivo FSTAB
	 */
	if ((fp = fopen (fstab_nm, "r")) == NOFILE)
		error ("$*Não consegui abrir \"%s\"", fstab_nm);

	for (EVER)
	{
		if ((code = getmntent (fp, dev_nm, dir_nm, mntent_ptr)) == EOF)
			break;

		if (code == 0)
			continue;

		if ((mntent_ptr->me_flags & SB_AUTO) == 0)
			continue;

		if (getmntopt (opt_str_nm, mntent_ptr) == 0)
			continue;
#ifdef	DEBUG
		{
			char		area[256];

			printf
			(	"Montando \"%s\"\tem \"%s\"\t%P (%s)\n",
				dev_nm, dir_nm, mntent_ptr->me_flags,
				editmntopt (area, mntent_ptr, vflag)
			);
		}
#else
		/* Verifica se já está montado */

		if ((sp = search_SB_list (dev_nm)) != NOSB)
		{
			if (!streq (sp->sb_dir_nm, dir_nm))
			{
				error
				(	"Dispositivo \"%s\" montado em \"%s\" ao invés de \"%s\"",
					dev_nm, sp->sb_dir_nm, dir_nm
				);
			}

			continue;
		}

		do_mount (dev_nm, dir_nm, mntent_ptr);
#endif	DEBUG
	}

	fclose (fp);

}	/* end read_fstab_and_mount_auto */

/*
 ****************************************************************
 *	Procura um nome em FSTAB e monta			*
 ****************************************************************
 */
void
search_in_fstab_and_mount (const char *nm, const char *opt_str_nm, struct mntent *mntent_ptr)
{
	FILE		*fp;
	int		fstab_flags, code;
	char		dev_nm[64];
	char		dir_nm[64];

	/*
	 *	Procura no arquivo FSTAB
	 */
	if ((fp = fopen (fstab_nm, "r")) == NOFILE)
		error ("$*Não consegui abrir \"%s\"", fstab_nm);

	for (EVER)
	{
		if ((code = getmntent (fp, dev_nm, dir_nm, mntent_ptr)) == EOF)
			error ("$Não achei \"%s\" em \"%s\"", nm, fstab_nm);

		if (code == 0)
			continue;

		if (streq (dev_nm, nm) || streq (dir_nm, nm))
			break;
	}

	fclose (fp);

	/*
	 *	Completa as opções e monta
	 */
	fstab_flags = mntent_ptr->me_flags;

	if (getmntopt (opt_str_nm, mntent_ptr) == 0)
		exit (1);

	if (getuid () != 0)
	{
		if ((fstab_flags & SB_USER) == 0)
			error ("$Somente o superusuário pode montar o dispositivo \"%s\"", dev_nm);

		/* Usuário normal pode mudar apenas RONLY e EJECT */

		mntent_ptr->me_flags =  fstab_flags	     & ~(SB_RONLY|SB_EJECT)	|
					mntent_ptr->me_flags &  (SB_RONLY|SB_EJECT);
	}

#ifdef	DEBUG
	{
		char		area[256];

		printf
		(	"Montando \"%s\" em \"%s\", %P (%s)\n",
			dev_nm, dir_nm, mntent_ptr->me_flags,
			editmntopt (area, mntent_ptr, vflag)
		);
	}
#else
	do_mount (dev_nm, dir_nm, mntent_ptr);
#endif	DEBUG

}	/* end search_in_fstab_and_mount */

/*
 ****************************************************************
 *	Executa a montagem propriamente dita			*
 ****************************************************************
 */
void
do_mount (const char *dev_nm, const char *dir_nm, struct mntent *mntent_ptr)
{
	const SB	*sp;
	int		len;
	char		*path, *remote_dir_nm;
	static char	*cwd;

	/*
	 *	Verifica se o dispositivo é da forma "servidor:dir" (NFS)
	 */
	if   ((remote_dir_nm = strchr (dev_nm, ':')) != NOSTR)
	{
		*remote_dir_nm++ = '\0';

		if (decode_nfs_details (dev_nm, remote_dir_nm, mntent_ptr) < 0)
			return;

		*--remote_dir_nm = ':';
	}
	elif (dev_nm[0] != '/')
	{
		/* Se não for um caminho absoluto, expande */

		if (cwd == NOSTR && (cwd = getcwd (NOSTR, 128)) == NOSTR)
			{ error ("*NÃO consegui obter o nome do diretório corrente"); return; }

		len = strlen (cwd) + 1 + strlen (dev_nm) + 1; path = alloca (len);

		sprintf (path, "%s/%s", cwd, dev_nm); dev_nm = path;
	}

	/*
	 *	Verifica se o tipo é NFS
	 */
	if ((remote_dir_nm = strchr (dev_nm, ':')) != NOSTR)
	{
		*remote_dir_nm++ = '\0';

		if (decode_nfs_details (dev_nm, remote_dir_nm, mntent_ptr) < 0)
			return;

		*--remote_dir_nm = ':';
	}

	/*
	 *	Verifica se por acaso já não está montado
	 */
	for (sp = sb_list; sp != NOSB; sp = sp->sb_forw)
	{
		if (streq (sp->sb_dev_nm, dev_nm) && streq (sp->sb_dir_nm, dir_nm))
		{
			printf
			(	"O dispositivo \"%s\" já está montado em \"%s\"\n",
				dev_nm, dir_nm
			);

			return;
		}
	}

	/*
	 *	Finalmente, monta
	 */
	if (mount (dev_nm, dir_nm, mntent_ptr) >= 0)
		return;

	if (errno == EROFS)
	{
		mntent_ptr->me_flags |= SB_RONLY;

		if (mount (dev_nm, dir_nm, mntent_ptr) >= 0)
		{
			error
			(	"Dispositivo \"%s\" protegido para escritas, "
				"montado somente para leituras",
				dev_nm
			);

			return;
		}
	}

	error ("*Não consegui montar \"%s\" em \"%s\"", dev_nm, dir_nm);

}	/* end do_mount */

/*
 ****************************************************************
 *	Processa os detalhes do NFS				*
 ****************************************************************
 */
int
decode_nfs_details (const char *server_nm, const char *remote_dir_nm, struct mntent *mntent_ptr)
{
	int		fd;

	if ((fd = open ("/dev/itnudp", O_RDWR)) < 0)
		{ error ("*Não consegui abrir \"/dev/itnudp\""); return (-1); }

	if ((mntent_ptr->me_server_addr = t_node_to_addr (fd, server_nm, NULL)) == -1)
		{ close (fd); error ("*Não consegui obter o endereço de \"%s\"", server_nm); return (-1); }

	close (fd); return (0);

}	/* end decode_nfs_details */

/*
 ****************************************************************
 *	Imprime a Tabela de dispositivos montados		*
 ****************************************************************
 */
void
print_mount_table (struct mntent *mntent_ptr)
{
	const SB	*sp;
	const EXPORT	*ep, *end_ep;
	int		len, dev_max = 0, dir_max = 0, fname_max = 0;
	int		one_skip = 0;
	char		dev_nm[16];
	char		area[256];

	/*
	 *	Acha os tamanhos máximos
	 */
	for (sp = sb_list; sp != NOSB; sp = sp->sb_forw)
	{
		if ((len = strlen (sp->sb_dev_nm)) > dev_max)
			dev_max = len;

		if ((len = strlen (sp->sb_dir_nm)) > dir_max)
			dir_max = len;

		if ((len = strlen ((char *)sp->sb_fname)) > fname_max)
			fname_max = len;
	}

	/*
	 *	Imprime a tabela
	 */
	for (sp = sb_list; sp != NOSB; sp = sp->sb_forw)
	{
		if   (sp->sb_code == FS_FAT)
			printf ("FAT%2d  ", sp->sb_sub_code);
		elif (sp->sb_code == FS_EXT2)
			printf ("EXT2 %d ", 1 << sp->sb_sub_code);
		elif (sp->sb_code == FS_NT)
			printf ("NTFS %c ", sp->sb_sub_code ? 'C' : ' ');
		else
			printf ("%-6.6s ", fscode_to_str[sp->sb_code]);

		dev_nm[0] = 0; kcntl (K_GETDEVNM, sp->sb_dev, dev_nm);

		printf
		(	" %-*.*s  %-*.*s  %-*.*s  %-8.8s",
			dev_max, dev_max, sp->sb_dev_nm,
			dir_max, dir_max, sp->sb_dir_nm,
			fname_max, fname_max, sp->sb_fname,
			dev_nm[0] != 0 ? dev_nm : sp->sb_fpack
		);

		printf ("%s\n", editmntopt (area, &sp->sb_mntent, vflag));
	}

	/*
	 *	Imprime os sistemas de arquivos montados remotamente
	 */
	for (ep = nfs_mount, end_ep = ep + nfs_mount_sz; ep < end_ep; ep++)
	{
		if (!ep->e_mounted)
			continue;

		if (one_skip == 0)
			{ printf ("\n"); one_skip++; }

		printf ("NFS     ");

		printf ("%s:%s(%s", ep->e_client_nm, ep->e_dir_path, ep->e_ronly ? "ro" : "rw");

		if (ep->e_no_root_squash)
			printf (",no_root_squash");

		if (ep->e_all_squash)
			printf (",all_squash");

		if (ep->e_use_anon_ids)
			printf (",anonuid=%d,anongid=%d", ep->e_anon_uid, ep->e_anon_gid);

		printf (")\n");
	}

}	/* end print_mount_table */

/*
 ****************************************************************
 *	Prepara a lista de SBs (na ordem normal)		*
 ****************************************************************
 */
void
create_SB_list (void)
{
	int		len;
	SYM		*sym;
	const SB	*phys_sp, *next_phys_sp;
	SB		*sp, *last_sp = NOSB;

	/*
	 *	Obtém o endereço da estrutura "sb_head"
	 */
	len = strlen ("_sb_head"); sym = alloca (SYM_SIZEOF (len));

	sym->s_nm_len = len; strcpy (sym->s_name, "_sb_head");

	if (kcntl (K_GETSYMGN, len, sym) < 0)
		error ("$*Não consegui obter o endereço de \"_sb_head\"");

	/*
	 *	Mapeia a cabeça da fila
	 */
	phys_sp = phys ((void *)sym->s_value, sizeof (SB), O_KERNEL|O_RDONLY);

	if ((int)phys_sp == -1)
		error ("$*Não consegui mapear SBHEAD");

	next_phys_sp = phys_sp->sb_forw;

	phys (phys_sp, 0, 0);

	/*
	 *	Percorre a Lista de SBs e cria uma cópia
	 */
	while (next_phys_sp != (SB *)sym->s_value)
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
 *	Procura na lista de SBs					*
 ****************************************************************
 */
const SB *
search_SB_list (const char *dev_nm)
{
	const SB	*sp;

	for (sp = sb_list; sp != NOSB; sp = sp->sb_forw)
	{
		if (streq (sp->sb_dev_nm, dev_nm))
			return (sp);
	}

	return (NOSB);

}	/* end search_SB_list */

/*
 ****************************************************************
 *	Lê a tabela NFS_MOUNT					*
 ****************************************************************
 */
void
read_nfs_mount_table (void)
{
	int		fd;

	if ((fd = open ("/dev/itnudp", O_RDWR)) < 0)
	{
		if (errno == TBADNET)
			return;
		else
			error ("$*Não consegui abrir a INTERNET");
	}

	if ((nfs_mount_sz = ioctl (fd, I_GET_NFS_MOUNT, NOEXPORT)) < 0)
		error ("$*Não consegui obter o tamanho da tabela NFS_MOUNT");

	if ((nfs_mount = malloc (nfs_mount_sz * sizeof (EXPORT))) == NOEXPORT)
		error (NOSTR);

	if (ioctl (fd, I_GET_NFS_MOUNT, nfs_mount) < 0)
		error ("$*Não consegui obter a tabela NFS_MOUNT");

	close (fd);

}	/* end read_nfs_mount_table */

#if (0)	/*******************************************************/
const char		*edit_option (int flags, const SB *);
/*
 ****************************************************************
 *	Edita uma lista de opções				*
 ****************************************************************
 */
const char *
edit_option (int flags, const SB *sp)
{
	const FSOPT	*op;
	char		first = 1;
	const char	*str;
	static char	*option_str;

	if (!vflag && ((flags ^ SB_DEFAULT_ON) & ~SB_NOT_KERNEL) == 0)
		return ("default");

	if (option_str == NOSTR && (option_str = malloc (128)) == NOSTR)
		error (NOSTR);

	for (option_str[0] = '\0', op = value_tb; op->opt_on_name[0] != '\0'; op++)
	{
		if (!vflag && ((flags ^ SB_DEFAULT_ON) & op->opt_value) == 0)
			continue;

		if (vflag < 2 && (op->opt_value == SB_ROCK || op->opt_value == SB_JOLIET))
			continue;

		if (op->opt_value & flags)
			str = op->opt_on_name;
		else
			str = op->opt_off_name;

		if (first)
			first = 0;
		else
			strcat (option_str, ",");

		strcat (option_str, str);

		if (streq (str, "user"))
		{
			char	digits[16];

			strcat (option_str, "=");

			if ((str = pwcache (sp->sb_uid)) != NOSTR)
				strcat (option_str, str);
			else
				{ sprintf (digits, "%d", sp->sb_uid); strcat (option_str, digits); }
		}
	}

	return (option_str);

}	/* end edit_option */
#endif	/*******************************************************/

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
		"%s - monta sistemas de arquivos\n"
		"\n%s\n"
		"\nSintaxe:\n"
		"\t%s [-rw] [-o <opções>] <dispositivo> <diretório>\n"
		"\t%s [-rw] [-o <opções>] <dispositivo ou diretório>\n"
		"\t%s [-rw] [-o <opções>] -a\n"
		"\t%s\n",
		pgname, pgversion, pgname, pgname, pgname, pgname
	);
	fprintf
	(	stderr,
		"\nOpções:"
		"\t-r: Monta somente para leituras\n"
		"\t-w: Monta para leituras e escritas\n"
		"\t-o: Dá uma lista de <opções> de montagem\n"
		"\t-a: Monta os dispositivos com a opção \"auto\" em \"/etc/fstab\"\n"
		"\nObs.:\tSem argumentos, imprime uma tabela dos dispositivos montados\n"
	);

	exit (2);

}	/* end help */
