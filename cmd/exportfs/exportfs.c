/*
 ****************************************************************
 *								*
 *			exportfs.c				*
 *								*
 *	Ativa a tabela EXPORTS do NFS				*
 *								*
 *	Versão	4.8.0, de 21.09.05				*
 *		4.8.0, de 03.12.05				*
 *								*
 *	Módulo: exportfs					*
 *		Utilitários Básicos				*
 *		Categoria B					*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 2005 NCE/UFRJ - tecle "man licença"	*
 *								*
 ****************************************************************
 */

#include <sys/common.h>
#include <sys/scb.h>
#include <sys/sync.h>
#include <sys/mntent.h>
#include <sys/sb.h>
#include <sys/stat.h>
#include <sys/kcntl.h>
#include <sys/itnet.h>
#include <sys/syscall.h>

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
const char		pgversion[] =  "Versão:	4.8.0, de 03.12.05";

#undef	DEBUG

entry int		eflag;			/* Imprime "/etc/exports" */
entry int		dflag;			/* Imprime as diferenças */
entry int		uflag;			/* Atualiza a tabela "exports" do núcleo */
entry int		fflag;			/* Força */
entry int		vflag = 1;		/* Verbose */

entry int		diff;			/* Diferenças */

entry EXPORT		*kernel_export;		/* Tabela EXPORT do núcleo */
entry int		kernel_export_sz;

const char		file_export_nm[] = "/etc/exports";

entry EXPORT		*file_export;		/* Tabela EXPORT do arquivo */
entry int		file_export_sz;

/*
 ****** Protótipos de funções ***********************************
 */
void			read_kernel_export_table (void);
void			read_file_export_table (void);
void			print_export_table (const EXPORT *export_table, int export_sz);
void			printf_export_table_line (const EXPORT *ep, int);
void			compare_tables (void);
int			clientnmeq (const char *nm1, const char *nm2);
void			insert_new_line (const EXPORT *fp);
void			compare_lines (const EXPORT *fp, EXPORT *kp);
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

	/*
	 *	Analisa as opções
	 */
	while ((opt = getopt (argc, argv, "edufvH")) != EOF)
	{
		switch (opt)
		{
		    case 'e':			/* Imprime "/etc/exports" */
			eflag++;
			break;

		    case 'd':			/* Imprime as diferenças */
			dflag++;
			break;

		    case 'u':			/* Atualiza a tabela "exports" do núcleo */
			uflag++;
			break;

		    case 'f':			/* Força */
			fflag++;
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

	if (eflag + dflag + uflag > 1)
		help ();

	/*
	 *	Lê as duas tabelas de "export"
	 */
	read_kernel_export_table ();

	read_file_export_table ();

	/*
	 *	Examina a função
	 */
	if (eflag)
		{ print_export_table (file_export, file_export_sz); return (0); }

	if (dflag || uflag)
		{ compare_tables (); return (0); }

	/*
	 *	Imprime a tabela do núcleo
	 */
	print_export_table (kernel_export, kernel_export_sz);

	return (0);

}	/* end mount */

/*
 ****************************************************************
 *	Imprime uma das tabelas "exports"			*
 ****************************************************************
 */
void
print_export_table (const EXPORT *export_table, int export_sz)
{
	const EXPORT	*ep, *end_ep;
	int		len, max_len = -1;

	/*
	 *	Encontra a coluna mais larga
	 */
	for (ep = export_table, end_ep = ep + export_sz; ep < end_ep; ep++)
	{
		if (!ep->e_active)
			continue;

		if ((len = strlen (ep->e_dir_path)) > max_len)
			max_len = len;
	}

	/*
	 *	Imprime a tabela
	 */
	for (ep = export_table, end_ep = ep + export_sz; ep < end_ep; ep++)
	{
		if (!ep->e_active)
			continue;

		printf_export_table_line (ep, max_len);

#if (0)	/*******************************************************/
		printf ("%-*s\t%s", max_len, ep->e_dir_path, ep->e_client_nm);

		printf ("(%s", ep->e_ronly ? "ro" : "rw");

		if (ep->e_no_root_squash)
			printf (",no_root_squash");

		if (ep->e_all_squash)
			printf (",all_squash");

		if (ep->e_use_anon_ids)
			printf (",anonuid=%d,anongid=%d", ep->e_anon_uid, ep->e_anon_gid);

		printf (")\n");
#endif	/*******************************************************/
	}

}	/* end print_export_table */

/*
 ****************************************************************
 *	Imprime uma linha de tabela "exports"			*
 ****************************************************************
 */
void
printf_export_table_line (const EXPORT *ep, int max_len)
{
	if (max_len > 0)
		printf ("%-*s\t%s", max_len, ep->e_dir_path, ep->e_client_nm);
	else
		printf ("%s\t%s", ep->e_dir_path, ep->e_client_nm);

	printf ("(%s", ep->e_ronly ? "ro" : "rw");

	if (ep->e_no_root_squash)
		printf (",no_root_squash");

	if (ep->e_all_squash)
		printf (",all_squash");

	if (ep->e_use_anon_ids)
		printf (",anonuid=%d,anongid=%d", ep->e_anon_uid, ep->e_anon_gid);

	printf (")\n");

}	/* end printf_export_table_line */

/*
 ****************************************************************
 *	Lê a tabela EXPORT do núcleo				*
 ****************************************************************
 */
void
read_kernel_export_table (void)
{
	int		fd;

	if ((fd = open ("/dev/itnudp", O_RDWR)) < 0)
		error ("$*Não consegui abrir a INTERNET");

	if ((file_export_sz = ioctl (fd, I_GET_EXPORT, NOEXPORT)) < 0)
		error ("$*Não consegui obter o tamanho da tabela EXPORT");

	kernel_export_sz = file_export_sz;

	if ((kernel_export = malloc (kernel_export_sz * sizeof (EXPORT))) == NOEXPORT)
		error (NOSTR);

	if (ioctl (fd, I_GET_EXPORT, kernel_export) < 0)
		error ("$*Não consegui obter a tabela EXPORT");

	close (fd);

}	/* end read_kernel_export_table */

/*
 ****************************************************************
 *	Lê a tabela EXPORT de "/etc/exports"			*
 ****************************************************************
 */
void
read_file_export_table (void)
{
	FILE		*fp;
	const char	*str, *options = NOSTR;
	char		*abre;
	EXPORT		*ep;
	const EXPORT	*end_ep;
	int		lineno = 0;
	char		area[128];

	/*
	 *	Verifica se o arquivo existe
	 */
	if ((fp = fopen (file_export_nm, "r")) == NOFILE)
		error ("$*Não consegui abrir \"%s\"", file_export_nm);

	if ((file_export = calloc (file_export_sz, sizeof (EXPORT))) == NOEXPORT)
		error (NOSTR);

	ep = file_export - 1; end_ep = file_export + (file_export_sz - 1);

	/*
	 *	Le e processa o arquivo (por enquanto, um padrão por linha)
	 */
	while (fngets (area, sizeof (area), fp) == area)
	{
		lineno++;

		if (area[0] == '#' || area[0] == '\0')
			continue;

		if ((str = strtok (area, " \t")) == NOSTR)
			continue;

		if (++ep >= end_ep)
			{ error ("Tabela EXPORTs esgotada"); fclose (fp); return; }

		ep->e_ronly = 1;	/* Em principio */

		/* Insere o nome do diretório que será exportado */

		strscpy (ep->e_dir_path, str, sizeof (ep->e_dir_path));

		if ((str = strtok (NOSTR, " \t")) == NOSTR)
		{
			error
			(	"Arquivo \"%s\", linha %d: Faltando o nome do cliente",
				file_export_nm, lineno
			);
			ep--; continue;
		}

		if ((abre = strchr (str, '(')) != NOSTR)
			{ *abre++ = '\0'; options = strtok (abre, ",)"); }

		/* Insere o nome do cliente autorizado a montar o diretório */

		strscpy (ep->e_client_nm, str, sizeof (ep->e_client_nm));

		/* Examina as opções */

		for (/* acima */; options != NOSTR; options = strtok (NOSTR, ",)"))
		{
			if   (streq (options, "secure"))
			{
				error
				(	"Arquivo \"%s\", linha %d: \"%s\" ainda NÃO implementado",
					file_export_nm, lineno, "secure"
				);
				ep->e_secure = 1;
			}
			elif (streq (options, "insecure"))
			{
				ep->e_secure = 0;
			}
			elif (streq (options, "ro"))
			{
				ep->e_ronly = 1;
			}
			elif (streq (options, "rw"))
			{
				ep->e_ronly = 0;
			}
			elif (streq (options, "sync"))
			{
				error
				(	"Arquivo \"%s\", linha %d: \"%s\" ainda NÃO implementado",
					file_export_nm, lineno, "sync"
				);
				ep->e_sync = 1;
			}
			elif (streq (options, "async"))
			{
				ep->e_sync = 0;
			}
			elif (streq (options, "hide"))
			{
				error
				(	"Arquivo \"%s\", linha %d: \"%s\" ainda NÃO implementado",
					file_export_nm, lineno, "hide"
				);
				ep->e_hide = 1;
			}
			elif (streq (options, "nohide"))
			{
				ep->e_hide = 0;
			}
			elif (streq (options, "no_root_squash"))
			{
				ep->e_no_root_squash = 1;
			}
			elif (streq (options, "root_squash"))
			{
				ep->e_no_root_squash = 0;
			}
			elif (streq (options, "all_squash"))
			{
				ep->e_all_squash = 1;
			}
			elif (!strncmp (options, "anonuid=", 8))
			{
				if ((ep->e_anon_uid = strtol (options + 8, &str, 0)) < 0 || *str != '\0')
					error ("UID inválido: \"%s\"", options + 8);
				else
					ep->e_use_anon_ids = 1;
			}
			elif (!strncmp (options, "anongid=", 8))
			{
				if ((ep->e_anon_gid = strtol (options + 8, &str, 0)) < 0 || *str != '\0')
					error ("GID inválido: \"%s\"", options + 8);
				else
					ep->e_use_anon_ids = 1;
			}
			else
			{
				error
				(	"Arquivo \"%s\", linha %d: Palavra chave \"%s\" desconhecida",
					file_export_nm, lineno, options
				);
			}

		}	/* end while (options != NOSTR) */

		ep->e_active = 1;

	}	/* end for (lendo as linhas) */

	fclose (fp);

}	/* end read_file_export_table */

/*
 ****************************************************************
 *	Compara as duas tabelas					*
 ****************************************************************
 */
void
compare_tables (void)
{
	const EXPORT	*end_fp = file_export   + file_export_sz, *fp;
	const EXPORT	*end_kp = kernel_export + kernel_export_sz;
	EXPORT		*kp;

	/*
	 *	Para cada linha de "/etc/exports", procura a correspondente do núcleo
	 */
	for (fp = file_export; fp < end_fp; fp++)
	{
		if (fp->e_dir_path[0] == '\0')
			continue;

		if (!fp->e_active)
			continue;

		for (kp = kernel_export; /* abaixo */; kp++)
		{
			if (kp >= end_kp)		/* Não achou a linha correspondente */
			{
				insert_new_line (fp);
				break;
			}

			if (kp->e_dir_path[0] == '\0')
				continue;

			if (!kp->e_active)
				continue;

			if (!streq (kp->e_dir_path, fp->e_dir_path))
				continue;

			if (!clientnmeq (kp->e_client_nm, fp->e_client_nm))
				continue;

#if (0)	/*******************************************************/
			printf ("Achei: file %d, kernel %d\n", fp - file_export, kp - kernel_export);
#endif	/*******************************************************/

			compare_lines (fp, kp);

			break;	/* Achou */
		}
	}

	/*
	 *	Verifica se alguma linha foi removida em "/etc/exports"
	 */
	for (kp = kernel_export; kp < end_kp; kp++)
	{
		if (kp->e_dir_path[0] == '\0')
			continue;

		if (!kp->e_active)
			continue;

		for (fp = file_export; /* abaixo */; fp++)
		{
			if (fp >= end_fp)		/* Não achou a linha correspondente */
			{
				if (vflag)
				{
					printf ("-\t");
					printf_export_table_line (kp, 0);
				};

				kp->e_active = 0;
				diff++;
				break;
			}

			if (!streq (fp->e_dir_path, kp->e_dir_path))
				continue;

			if (!clientnmeq (fp->e_client_nm, kp->e_client_nm))
				continue;

			break;	/* Achou */
		}
	}

#if (0)	/*******************************************************/
	/*
	 *	Imprime a estatística
	 */
	if   (diff == 1)
		printf ("Foi encontrada 1 diferença\n");
	elif (diff > 1)
		printf ("Foram encontradas %d diferenças\n", diff);
#endif	/*******************************************************/

	/*
	 *	Atualiza, se for o caso
	 */
	if (diff && uflag)
	{
		int		fd;

		if (fflag == 0)
		{
			fprintf
			(	stderr,
				"\nÚltima chance antes de atualizar a tabela do núcleo. "
				"Continua? (n): "
			);

			if (askyesno () <= 0)
				exit (0);
		}

		if ((fd = open ("/dev/itnudp", O_RDWR)) < 0)
			error ("$*Não consegui abrir a INTERNET");

		if (ioctl (fd, I_PUT_EXPORT, kernel_export) < 0)
			error ("$*Não consegui atualiza a tabela \"export\" do núcleo");

		close (fd);
	}

}	/* end compare_tables 

/*
 ****************************************************************
 *	Compara nomes de clientes				*
 ****************************************************************
 */
int
clientnmeq (const char *nm1, const char *nm2)
{
	const char	*dot1, *dot2;
	int		len;

	/*
	 *	Tenta comparar, apesar dos sufixos
	 */
	dot1 = strchr (nm1, '.');
	dot2 = strchr (nm2, '.');

	if (dot1)
	{
		if   (dot2)
			return (streq (nm1, nm2));
		elif ((len = strlen (nm2)) != dot1 - nm1)
			return (0);
	}
	else /* !dot1 */
	{
		if (!dot2)
			return (streq (nm1, nm2));
		elif ((len = strlen (nm1)) != dot2 - nm2)
			return (0);
	}

	return (!strncmp (nm1, nm2, len));

}	/* end clientnmeq */

/*
 ****************************************************************
 *	Insere uma linha nova na tabela do núcleo		*
 ****************************************************************
 */
void
insert_new_line (const EXPORT *fp)
{
	const EXPORT	*end_kp = kernel_export + kernel_export_sz;
	EXPORT		*kp;

	for (kp = kernel_export; /* abaixo */; kp++)
	{
		if (kp >= end_kp)
		{
			error ("Tabela \"exports\" do núcleo esgotada");
			break;
		}

		if (kp->e_dir_path[0] == '\0')
		{
			if (vflag)
			{
				printf ("+\t");
				printf_export_table_line (fp, 0);
			};

			memmove (kp, fp, sizeof (EXPORT));
			kp->e_active = 1;
			diff++;
			break;
		}
	}

}	/* end insert_new_line */

/*
 ****************************************************************
 *	Compara/modifica a linha do núcleo			*
 ****************************************************************
 */
void
compare_lines (const EXPORT *fp, EXPORT *kp)
{
	/*
	 *	Compara
	 */
	if (kp->e_ronly != fp->e_ronly)
	{
		if (vflag) printf
		(
			"!\t%s\t%s:\t%s => %s\n",
			fp->e_dir_path, fp->e_client_nm,
			kp->e_ronly ? "ro" : "rw", fp->e_ronly ? "ro" : "rw"
		);

		diff++;
	}

	if (kp->e_no_root_squash != fp->e_no_root_squash)
	{
		if (vflag) printf
		(
			"!\t%s\t%s:\t%s => %s\n",
			fp->e_dir_path, fp->e_client_nm,
			kp->e_no_root_squash ? "no_root_squash" : "root_squash",
			fp->e_no_root_squash ? "no_root_squash" : "root_squash"
		);

		diff++;
	}

	if (kp->e_all_squash != fp->e_all_squash)
	{
		if (vflag) printf
		(
			"!\t%s\t%s:\t%s => %s\n",
			fp->e_dir_path, fp->e_client_nm,
			kp->e_all_squash ? "all_squash" : "not_all_squash",
			fp->e_all_squash ? "all_squash" : "not_all_squash"
		);

		diff++;
	}

	if   (kp->e_use_anon_ids != fp->e_use_anon_ids)
	{
		if (vflag)
		{
			printf
			(
				"!\t%s\t%s:\t",
				fp->e_dir_path, fp->e_client_nm
			);

			if (fp->e_use_anon_ids) printf
			(
				"no_anonuid/gid => (%d, %d)\n",
				fp->e_anon_uid, fp->e_anon_gid
			);
			else printf
			(
				"(%d, %d) => no_anonuid/gid\n",
				kp->e_anon_uid, kp->e_anon_gid
			);
		}

		diff++;
	}
	elif (kp->e_use_anon_ids)
	{
		if (kp->e_anon_uid != fp->e_anon_uid)
		{
			if (vflag) printf
			(
				"!\t%s\t%s:\tanonuid %d => %d\n",
				fp->e_dir_path, fp->e_client_nm,
				kp->e_anon_uid, fp->e_anon_uid
			);

			diff++;
		}

		if (kp->e_anon_gid != fp->e_anon_gid)
		{
			if (vflag) printf
			(
				"!\t%s\t%s:\tanongid %d => %d\n",
				fp->e_dir_path, fp->e_client_nm,
				kp->e_anon_gid, fp->e_anon_gid
			);

			diff++;
		}
	}

	/*
	 *	Copia
	 */
	kp->e_ronly	     = fp->e_ronly;
	kp->e_no_root_squash = fp->e_no_root_squash;
	kp->e_all_squash     = fp->e_all_squash;
	kp->e_use_anon_ids   = fp->e_use_anon_ids;
	kp->e_anon_uid	     = fp->e_anon_uid;
	kp->e_anon_gid	     = fp->e_anon_gid;

}	/* end compare_lines */

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
		"%s - lista/atualiza a tabela \"exports\" do NFS\n"
		"\n%s\n"
		"\nSintaxe:\n"
		"\t%s [-eduf]\n",
		pgname, pgversion, pgname
	);
	fprintf
	(	stderr,
		"\nOpções:"
		"\t--: Lista a tabela \"exports\" do núcleo\n"
		"\t-e: Lista a tabela \"/etc/exports\"\n"
		"\t-d: Indica as diferenças entre as duas tabelas\n"
		"\t-u: Atualiza a tabela \"exports\" do núcleo\n"
		"\t-f: Não pede confirmação para a atualização\n"
	);

	exit (2);

}	/* end help */
