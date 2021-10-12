/*
 ****************************************************************
 *								*
 *			mkdev.c					*
 *								*
 *	Cria/atualiza os dispositivos de "/dev"			*
 *								*
 *	Versão	3.0.0, de 02.02.95				*
 *		4.9.0, de 26.09.06				*
 *								*
 *	Módulo: mkdev						*
 *		Utilitários Básicos				*
 *		Categoria B					*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 2006 NCE/UFRJ - tecle "man licença"	*
 * 								*
 ****************************************************************
 */

#include <sys/common.h>
#include <sys/scb.h>
#include <sys/disktb.h>
#include <sys/video.h>
#include <sys/kcntl.h>
#include <sys/devmajor.h>
#include <sys/stat.h>
#include <sys/syscall.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <errno.h>

/*
 ****************************************************************
 *	Variáveis e Definições globais				*
 ****************************************************************
 */
const char	pgversion[] =  "Versão:	4.9.0, de 26.09.06";

#undef	DEBUG

#define	elif	else if
#define	EVER	;;

const char	*dev_path = "/dev";

entry int	exit_code = 0;	/* Código de retorno */

entry int	wflag;		/* Atualiza os dispositivos em "/dev" */
entry int	fflag;		/* Não consulta para uma atualização */
entry int	sflag;		/* Silencioso */
entry int	vflag;		/* Verbose */

/*
 ******	Definições globais **************************************
 */
typedef struct	devtb	DEVTB;

struct	devtb
{
	long	v_mode;		/* Modo e Tipo do Arquivo */
	ushort	v_nlink;	/* No. de Ponteiros para este Arquivo */
	ushort 	v_uid;		/* Dono  do Arquivo */
	ushort 	v_gid;		/* Grupo do Arquivo */
	ino_t	v_ino;		/* No. do Inode */
	dev_t	v_rdev;		/* Dispositivo (BLK e CHR) */
	char	*v_name;	/* Nome do dispositivo */
	DEVTB	*v_next;	/* Ponteiro para o próximo */

};

#define	NODEVTB	(DEVTB *)NULL

/*
 ******	Estrutura de definição dos vários dispositivos **********
 */
typedef struct rawdevtb	RAWDEVTB;

struct rawdevtb
{
	const char	*r_format;
	int		r_first;
   /***	int		r_last;	***/
	int		r_major;
	int		r_minor;
	int		r_mode;
};

/* Na tabela abaixo, o formato do número deve ser "%01d", "%02d" ou "%03d" */

const RAWDEVTB	rawdevtb[] =
{
     /*  format	      first      *** last	***	  major	      minor	  mode	*/

	"console",	0,	/*** 0,		***/	CON_MAJOR,	0,	S_IFCHR|0666,
	"video%01d",	1,	/*** NCON,	***/	CON_MAJOR,	0,	S_IFCHR|0666,
	"tty",		0,	/*** 0,		***/	TTY_MAJOR,	0,	S_IFCHR|0666,
	"mem",		0,	/*** 0,		***/	MEM_MAJOR,	0,	S_IFCHR|0666,
	"kmem",		0,	/*** 0,		***/	MEM_MAJOR,	1,	S_IFCHR|0666,
	"null",		0,	/*** 0,		***/	MEM_MAJOR,	2,	S_IFCHR|0666,
	"grave",	0,	/*** 0,		***/	MEM_MAJOR,	3,	S_IFCHR|0666,
	"tty%03d",	0,	/*** NSIO - 1,	***/	SIO_MAJOR,	0,	S_IFCHR|0666,
	"itndaemon",	0,	/*** 0,		***/	ITN_MAJOR,	0,	S_IFCHR|0666,
	"itnraw",	0,	/*** 0,		***/	ITN_MAJOR,	1,	S_IFCHR|0666,
	"itnudp",	0,	/*** 0,		***/	ITN_MAJOR,	2,	S_IFCHR|0666,
	"itntcp",	0,	/*** 0,		***/	ITN_MAJOR,	3,	S_IFCHR|0666,
	"ptyc%02d",	0,	/*** NPTY - 1,	***/	PTYC_MAJOR,	0,	S_IFCHR|0666,
	"ptys%02d",	0,	/*** NPTY - 1,	***/	PTYS_MAJOR,	0,	S_IFCHR|0666,
	"slip%03d",	0,	/*** NSIO - 1,	***/	SLIP_MAJOR,	0,	S_IFCHR|0666,
	"lp",		0,	/*** 0,		***/	LP_MAJOR,	0,	S_IFCHR|0666,
	"lp%01d",	0,	/*** 1,		***/	LP_MAJOR,	0,	S_IFCHR|0666,
	"ed%01d",	0,	/*** 1,		***/	ED_MAJOR,	0,	S_IFCHR|0600,
	"ppp%03d",	0,	/*** NSIO - 1,	***/	PPP_MAJOR,	0,	S_IFCHR|0666,
	"xconsole",	0,	/*** 0,		***/	XCON_MAJOR,	0,	S_IFCHR|0644,
	"sb",		0,	/*** 0,		***/	SB_MAJOR,	0,	S_IFCHR|0666,
	"rtl%01d",	0,	/*** 1,		***/	RTL_MAJOR,	0,	S_IFCHR|0600,
	"ps2m",		0,	/*** 0,		***/	PS2_MAJOR,	0,	S_IFCHR|0666,
	"ums0",		0,	/*** 0,		***/	UMS_MAJOR,	0,	S_IFCHR|0666,
	"ulp%01d",	0,	/*** 1,		***/	ULP_MAJOR,	0,	S_IFCHR|0666,
	NOSTR
};

/*
 ****** Protótipos de funções ***********************************
 */
void		raw_devices_check (const SCB *, const RAWDEVTB *, const DEVTB *vp_first);
void		mk_new_dev (const DISKTB *, const char *, long);
int		mk_old_dev (const DISKTB *, const DEVTB *, long);
void		mk_link_dev (const DISKTB *, const char *, const DEVTB *);
void		link_to (const char *, const char *);
int		is_dev_name (const char *);
void		do_unlink (const DEVTB *, const DEVTB *);
int		fmode (long);
void		help (void);

/*
 ****************************************************************
 *	Cria/atualiza os dispositivos de "/dev"			*
 ****************************************************************
 */
int
main (int argc, const char *argv[])
{
	int		opt, n;
	DEVTB		*vp_first = NODEVTB, *vp_last = NODEVTB, *vp;
	const DISKTB	*disktb, *dp;
	const char	*nm;
	const DISKTB	*root_dp = NODISKTB, *swap_dp = NODISKTB;
	dev_t		dev_dev;
	ino_t		old_ino, new_ino;
	DIR		*dir_fp;
	const DIRENT	*dir_p;
	const RAWDEVTB	*rp;
	STAT		s;
	char		raw_nm[MAXNAMLEN + 1];
	SCB		scb;

	/*
	 *	Analisa as opções
	 */
	while ((opt = getopt (argc, argv, "wufasvH")) != EOF)
	{
		switch (opt)
		{
		    case 'w':			/* Atualiza */
		    case 'u':			/* Atualiza */
			wflag++;
			break;

		    case 'f':			/* Sem consultar */
			fflag++;
			break;

		    case 'a':			/* "/aroot/..." */
			dev_path = "/aroot/dev";
			break;

		    case 's':			/* Muito silêncio */
			sflag++;
			break;

		    case 'v':			/* Verbose */
			vflag++;
			break;

		    case 'H':			/* Help */
			help ();

		    default:			/* Erro */
			putc ('\n', stderr);
			help ();

		}	/* end switch */

	}	/* end while */

	if (sflag)
		{ vflag = 0; fflag++; }

	argv += optind;
	argc -= optind;

	if (argc != 0)
		help ();

	askyesno_stderr = stdout;

	/*
	 *	Para obedecer às proteções
	 */
	umask (0);

	/*
	 *	Verifica se é SUPERUSUÁRIO
	 */
	if (wflag && geteuid () != 0)
		error ("$NÃO sou SUPERUSUÁRIO!");

	/*
	 *	Obtém o SCB
	 */
	if ((int)getscb (&scb) == -1)
		error ("$*Não consegui obter o SCB");

	/*
	 *	Obtém a tabela de disco
	 */
	if ((n = kcntl (K_GET_DEV_TB, NODEV, NODISKTB)) < 0)
		error ("$*Não consegui obter o tamanho da tabela de discos");

	if ((disktb = malloc (n * sizeof (DISKTB))) == NODISKTB)
		error (NOSTR);

	if (kcntl (K_GET_DEV_TB, NODEV, disktb) < 0)
		error ("$*Não consegui obter a tabela de discos");

	/*
	 *	Vai para "/dev" para facilitar
	 */
	if (chdir (dev_path) < 0)
		error ("$*Não consegui trocar o diretório corrente para \"%s\"", dev_path);

	/*
	 *	Abre o diretório "/dev"
	 */
	if (stat (".", &s) < 0 || (dir_fp = inopendir (s.st_dev, s.st_ino)) == NODIR)
		error ("$*Não consegui abrir \"%s\"", dev_path);

	dev_dev = s.st_dev;

	/*
	 *	Gera uma tabela dos dispositivos de "/dev"
	 */
	if ((dir_fp = inopendir (s.st_dev, s.st_ino)) == NODIR)
		error ("$*Não consegui abrir \"%s\"", dev_path);

	/*
	 *	Percorre o diretório
	 */
	while ((dir_p = readdir (dir_fp)) != NODIRENT)
	{
		if (dir_p->d_name[0] == '.')
		{
			if   (dir_p->d_name[1] == '\0')
				continue;
			elif (dir_p->d_name[1] == '.' && dir_p->d_name[2] == '\0')
				continue;
		}

		if (instat (dev_dev, dir_p->d_ino, &s) < 0)
		{
			error ("*Não consegui obter o estado de \"%s/%s\"", dev_path, dir_p->d_name);
			continue;
		}

		if ((vp = malloc (sizeof (DEVTB))) == NODEVTB)
			error (NOSTR);

		if ((vp->v_name = malloc (dir_p->d_namlen + 1)) == NOSTR)
			error (NOSTR);

		vp->v_mode	= s.st_mode;
		vp->v_nlink	= s.st_nlink;
		vp->v_uid	= s.st_uid;
		vp->v_gid	= s.st_gid;
		vp->v_ino	= s.st_ino;
		vp->v_rdev	= s.st_rdev;
	   	strcpy (vp->v_name, dir_p->d_name);

		if (vp_first == NODEVTB)
			vp_first = vp;
		else
			vp_last->v_next = vp;

		vp_last = vp; vp->v_next = NODEVTB;

#ifdef	DEBUG
		if (vflag) printf
		(	"nlink = %2d, ino = %5d, major = %2d, minor = %2d, name = \"%s\"\n",
			vp->v_nlink, vp->v_ino, MAJOR (vp->v_rdev),
			MINOR (vp->v_rdev), vp->v_name
		);
#endif	DEBUG

	}	/* end lendo o diretório "/dev" */

	closedir (dir_fp);

	/*
	 *	Examina cada dispositivo BLK da tabela de partições
	 */
	for (dp = disktb; dp->p_name[0]; dp++)
	{
		if (dp->p_name[0] == '*')
			continue;

		nm = dp->p_name;

		/*
		 *	Guarda as entradas de "root" e "swap"
		 */
		if (dp->p_dev == scb.y_rootdev)
			root_dp = dp;

		if (dp->p_dev == scb.y_swapdev)
			swap_dp = dp;

		/*
		 *	Pula os pseudo-discos
		 */
		if (MAJOR (dp->p_dev) == PSEUDO_MAJOR)
			continue;

		/*
		 *	Procura o dispositivo no diretório "/dev"
		 */
		for (vp = vp_first; /* abaixo */; vp = vp->v_next)
		{
			/*
			 *	Não encontrou
			 */
			if (vp == NODEVTB)
				{ mk_new_dev (dp, nm, S_IFBLK|0740); break; }

			if (!streq (vp->v_name, nm))
				continue;

			/*
			 *	Encontrou o nome original
			 */
			if ((new_ino = mk_old_dev (dp, vp, S_IFBLK)) < 0)
				break;

			if ((old_ino = vp->v_ino) == new_ino)
				break;

			/*
			 *	Procura outros nomes encadeados ao nome original
			 */
			for (vp = vp_first; vp != NODEVTB; vp = vp->v_next)
			{
				if (streq (vp->v_name, nm))
					continue;

				if (vp->v_ino == old_ino)
					mk_link_dev (dp, nm, vp);
			}

			break;

		}	/* end (malha pela tabela de "/dev") */

	}	/* end (malha pela tabela de partições) */

	/*
	 *	Examina cada dispositivo CHR da tabela de partições
	 */
	for (dp = disktb; dp->p_name[0]; dp++)
	{
		if (dp->p_name[0] == '*')
			continue;

		raw_nm[0] = 'r'; strcpy (raw_nm + 1, dp->p_name); nm = raw_nm;

		/*
		 *	Pula os pseudo-discos
		 */
		if (MAJOR (dp->p_dev) == PSEUDO_MAJOR)
			continue;

		/*
		 *	Procura o dispositivo no diretório "/dev"
		 */
		for (vp = vp_first; /* abaixo */; vp = vp->v_next)
		{
			/*
			 *	Não encontrou
			 */
			if (vp == NODEVTB)
				{ mk_new_dev (dp, nm, S_IFCHR|0640); break; }

			if (!streq (vp->v_name, nm))
				continue;

			/*
			 *	Encontrou o nome original
			 */
			if ((new_ino = mk_old_dev (dp, vp, S_IFCHR)) < 0)
				break;

			if ((old_ino = vp->v_ino) == new_ino)
				break;

			/*
			 *	Procura outros nomes encadeados ao nome original
			 */
			for (vp = vp_first; vp != NODEVTB; vp = vp->v_next)
			{
				if (streq (vp->v_name, nm))
					continue;

				if (vp->v_ino == old_ino)
					mk_link_dev (dp, nm, vp);
			}

			break;

		}	/* end (malha pela tabela de "/dev") */

	}	/* end (malha pela tabela de partições) */

	/*
	 *	Verifica se há dispositivos espúrios
	 */
	for (vp = vp_first; vp != NODEVTB; vp = vp->v_next)
	{
		nm = vp->v_name;

		if (*nm == 'r')
			nm++;

		if (!is_dev_name (nm))
			continue;

		for (dp = disktb; /* abaixo */; dp++)
		{
			if (dp->p_name[0] == '*')
				continue;

			if (dp->p_name[0] == 0)
				{ do_unlink (vp, vp_first); break; }

			if (streq (dp->p_name, nm))
				break;
		}

	}	/* end percorrendo a tabela gerada a partir de "/dev" */

	/*
	 *	Confere o dispositivo "root"
	 */
	if (root_dp == NODISKTB)
		error ("$ERRO FATAL - Não encontrei o dispositivo raiz na tabela de partições");

	link_to ("root", root_dp->p_name);

	strcpy (raw_nm + 1, root_dp->p_name);
	link_to ("rroot", raw_nm);

	/*
	 *	Confere o dispositivo "swap"
	 */
	if   (swap_dp != NODISKTB)
	{
		strcpy (raw_nm + 1, swap_dp->p_name);
		link_to ("swap", raw_nm);
	}
	elif (unlink ("swap") < 0 && errno != ENOENT)
	{
		error ("*Não consegui remover \"%s/swap\"", dev_path);
	}

	/*
	 *	Analisa os dispositivos NÃO estruturados
	 */
	for (rp = rawdevtb; rp->r_format != NOSTR; rp++)
		raw_devices_check (&scb, rp, vp_first);

	return (exit_code);

}	/* end mkdev */

/*
 ****************************************************************
 *	Analisa os dispositivos NÃO estruturados		*
 ****************************************************************
 */
void
raw_devices_check (const SCB *sp, const RAWDEVTB *rp, const DEVTB *vp_first)
{
	const DEVTB	*vp;
	int		i, r_last, last;
	const char	*str;
	DISKTB		d;

	/*
	 *	Obtém o último elemento
	 */
	switch (rp->r_major)
	{
	    case CON_MAJOR:
		if (streq (rp->r_format, "console"))
			r_last = 0;
		else
			r_last = NCON_MAX;  /*** sp->y_NCON; ***/

		break;

	    case SIO_MAJOR:
	    case SLIP_MAJOR:
	    case PPP_MAJOR:
		r_last = sp->y_NSIO - 1;
		break;

	    case PTYC_MAJOR:
	    case PTYS_MAJOR:
		r_last = sp->y_npty - 1;
		break;

	    case LP_MAJOR:
		if (streq (rp->r_format, "lp"))
			r_last = 0;
		else
			r_last = 1;

		break;

	    case ED_MAJOR:
	    case RTL_MAJOR:
	    case ULP_MAJOR:
		r_last = 1;
		break;

	    default:
		r_last = 0;
		break;
	}

	/*
	 *	Percorre a tabela
	 */
	for (i = rp->r_first; i <= r_last; i++)
	{
		sprintf (d.p_name, rp->r_format, i);
		d.p_dev = MAKEDEV (rp->r_major, rp->r_minor + i - rp->r_first);

		for (vp = vp_first; /* abaixo */; vp = vp->v_next)
		{
			if   (vp == NODEVTB)
				{ mk_new_dev (&d, d.p_name, rp->r_mode); break; }
			elif (streq (vp->v_name, d.p_name))
				{ mk_old_dev (&d, vp, S_IFCHR); break; }
		}
	}

	/*
	 *	Se houver um "%d" no formato, procura espúrios
	 */
	if ((str = strchr (rp->r_format, '%')) == NOSTR)
		return;

	switch (str[2] - '0')
	{
	    case 1:
		last = 9;
		break;

	    case 2:
	    case 3:
	    default:
		last = 99;
		break;
	}

	for (i = r_last + 1; i <= last; i++)
	{
		sprintf (d.p_name, rp->r_format, i);
#ifdef	DEBUG
		if (vflag)
			printf ("procurando \"%s\"\n", d.p_name);
#endif	DEBUG
		for (vp = vp_first; vp != NODEVTB; vp = vp->v_next)
		{
			if (streq (vp->v_name, d.p_name))
				{ do_unlink (vp, vp_first); break; }
		}
	}

}	/* end raw_devices_check */

/*
 ****************************************************************
 *	Cria uma entrada nova em "/dev"				*
 ****************************************************************
 */
void
mk_new_dev (const DISKTB *dp, const char *nm, long mode)
{
#ifdef	DEBUG
	if (vflag)
		printf ("mk_new_dev: \"%s\"\n", nm);
#endif	DEBUG

	if (!sflag)
		printf ("Faltando o dispositivo \"%s/%s\"", dev_path, nm);

	if (!wflag)
	{
		if (!sflag)
			printf ("\n");

		return;
	}

	if (!sflag)
		printf (". Atualiza? (n): ");

	if (!fflag)
	{
		if (askyesno () <= 0)
			return;
	}
	else
	{
		if (!sflag)
			printf ("SIM\n");
	}

	if (mknod (nm, mode, dp->p_dev) < 0)
		error ("*Não consegui criar \"%s/%s\"", dev_path, nm);

}	/* end mk_new_dev */

/*
 ****************************************************************
 *	Confere uma entrada antiga de "/dev"			*
 ****************************************************************
 */
int
mk_old_dev (const DISKTB *dp, const DEVTB *vp, long mode)
{
	int		ino = vp->v_ino;
	STAT		s;

	/*
	 *	Retorna:
	 *		>= 0: No. do INODE atual do dispositivo em "/dev"
	 *		<  0: Erro que impede a continuação
	 */

#ifdef	DEBUG
	if (vflag)
		printf ("mk_old_dev: \"%s\"\n", vp->v_name);
#endif	DEBUG

	if ((vp->v_mode & S_IFMT) != mode)
	{
		if (!sflag) printf
		(	"Os tipos do dispositivo \"%s/%s\" NÃO conferem: (%c :: %c)",
			dev_path, vp->v_name, fmode (vp->v_mode), fmode (mode)
		);
	}
	elif (vp->v_rdev != dp->p_dev)
	{
		if (!sflag) printf
		(	"O MAJOR e/ou MINOR do dispositivo \"%s/%s\" NÃO confere(m): (%d/%d :: %d/%d)",
			dev_path, vp->v_name,
			MAJOR (vp->v_rdev), MINOR (vp->v_rdev),
			MAJOR (dp->p_dev),  MINOR (dp->p_dev)
		);
	}
	else
	{
		return (ino);	/* Confere */
	}

	if (!wflag)
	{
		if (!sflag)
			printf ("\n");

		return (ino);
	}


	if (!sflag)
		printf (". Atualiza? (n): ");

	if (!fflag)
	{
		if (askyesno () <= 0)
			return (ino);
	}
	else
	{
		if (!sflag)
			printf ("SIM\n");
	}

	if (unlink (vp->v_name) < 0)
	{
		error ("*Não consegui remover \"%s/%s\"", dev_path, vp->v_name);
		return (ino);
	}

	if (mknod (vp->v_name, (vp->v_mode & ~S_IFMT) | mode, dp->p_dev) < 0)
	{
		error ("*Não consegui criar \"%s/%s\"", dev_path, vp->v_name);
		return (-1);
	}

	if (chown (vp->v_name, vp->v_uid, vp->v_gid) < 0)
		error ("*Não consegui atualizar o dono/grupo de \"%s/%s\"", dev_path, vp->v_name);

	if (stat (vp->v_name, &s) < 0)
	{
		error ("*Não consegui obter o estado de \"%s/%s\"", dev_path, vp->v_name);
		return (-1);
	}

	return (s.st_ino);

}	/* end mk_old_dev */

/*
 ****************************************************************
 *	Confere um elo a uma entrada antiga de "/dev"		*
 ****************************************************************
 */
void
mk_link_dev (const DISKTB *dp, const char *old_nm, const DEVTB *vp)
{
#ifdef	DEBUG
	if (vflag)
		printf ("mk_link_dev: \"%s\", \"%s\"\n", vp->v_name, old_nm);
#endif	DEBUG

	if (!sflag) printf
	(	"O dispositivo \"%s/%s\" deve ser um elo físico de \"%s/%s\"",
		dev_path, vp->v_name, dev_path, old_nm
	);

	if (!wflag)
	{
		if (!sflag)
			printf ("\n");

		return;
	}

	if (!sflag)
		printf (". Atualiza? (n): SIM\n");

	if (unlink (vp->v_name) < 0)
	{
		error ("*Não consegui remover \"%s/%s\"", dev_path, vp->v_name);
		return;
	}

	if (link (old_nm, vp->v_name) >= 0)
		return;

	error
	(	"*Não consegui criar um elo físico de \"%s/%s\" para \"%s/%s\"",
		dev_path, old_nm, dev_path, vp->v_name);

}	/* end mk_link_dev */

/*
 ****************************************************************
 *	Conecta um nome a outro					*
 ****************************************************************
 */
void
link_to (const char *root_nm, const char *dev_nm)
{
	STAT		root_s, dev_s;

#ifdef	DEBUG
	if (vflag)
		printf ("link_to: \"%s\", \"%s\"\n", root_nm, dev_nm);
#endif	DEBUG

	if (stat (dev_nm, &dev_s) < 0)
	{
		error
		(	"Não consegui atualizar \"%s/%s\" pois não encontrei \"%s/%s\"\n",
			dev_path, root_nm, dev_path, dev_nm
		);
		return;
	}

	root_s.st_mode = 0;

	if (stat (root_nm, &root_s) >= 0 && root_s.st_ino == dev_s.st_ino)
		return;

	if (!sflag) printf
	(	"O dispositivo \"%s/%s\" deve ser um elo físico de \"%s/%s\"",
		dev_path, root_nm, dev_path, dev_nm
	);

	if (!wflag)
	{
		if (!sflag)
			printf ("\n");

		return;
	}

	if (!sflag)
		printf (". Atualiza? (n): ");

	if (!fflag)
	{
		if (askyesno () <= 0)
			return;
	}
	else
	{
		if (!sflag)
			printf ("SIM\n");
	}

	if (root_s.st_mode != 0 && unlink (root_nm) < 0)
	{
		error ("*Não consegui remover \"%s/%s\"", dev_path, root_nm);
		return;
	}

	if (link (dev_nm, root_nm) >= 0)
		return;

	error
	(	"*Não consegui criar um elo físico de \"%s/%s\" para \"%s/%s\"",
		dev_path, dev_nm, dev_path, root_nm
	);

}	/* end link_to */

/*
 ****************************************************************
 *	Verifica se um nome é de um disco/partição		*
 ****************************************************************
 */
int
is_dev_name (const char *nm)
{
	/*
	 *	Retorna 1 se é um nome de disco e 0 em caso contrário
	 */

	/*
	 *	Primeiro caractere deve ser "h", "s" ou "u"
	 */
	if (*nm != 's' && *nm != 'h' && *nm != 'u')
		return (0);

	nm++;

	/*
	 *	Segundo caractere deve ser "d"
	 */
	if (*nm != 'd')
		return (0);

	nm++;

	/*
	 *	Terceiro caractere deve ser "a".."z"
	 */
	if (*nm < 'a' || *nm > 'z')
		return (0);

	nm++;

	/*
	 *	Estilo "hda"
	 */
	if (*nm == '\0')
		return (1);

	/*
	 *	Quarto caractere deve ser "1".."4"
	 */
	if (*nm < '1' || *nm > '4')
		return (0);

	nm++;

	/*
	 *	Estilo "hda1"
	 */
	if (*nm == '\0')
		return (1);

	/*
	 *	Quinto caractere deve ser "a".."z"
	 */
	if (*nm < 'a' || *nm > 'z')
		return (0);

	nm++;

	/*
	 *	Estilo "hda1a"
	 */
	if (*nm == '\0')
		return (1);

	/*
	 *	Em caso contrário, ...
	 */
	return (1);

}	/* end is_dev_name */

/*
 ****************************************************************
 *	Remove uma entrada espúria em "/dev"			*
 ****************************************************************
 */
void
do_unlink (const DEVTB *vp, const DEVTB *vp_first)
{
	const char	*nm = vp->v_name;
	int		ino = vp->v_ino;

#ifdef	DEBUG
	if (vflag)
		printf ("do_unlink: \"%s\"\n", nm);
#endif	DEBUG

	if (!sflag)
		printf ("Dispositivo ESPÚRIO \"%s/%s\"", dev_path, nm);

	for (EVER)
	{
		if (!wflag)
		{
			if (!sflag)
				printf ("\n");

			break;
		}

		if (!sflag)
			printf (". Remove? (n): ");

		if (!fflag)
		{
			if (askyesno () <= 0)
				break;
		}
		else
		{
			if (!sflag)
				printf ("SIM\n");
		}

		if (unlink (nm) < 0)
		{
			error ("*Não consegui remover \"%s/%s\"", dev_path, nm);
		}

		break;
	}

	/*
	 *	Procura outros nomes encadeados ao nome original
	 */
	for (vp = vp_first; vp != NODEVTB; vp = vp->v_next)
	{
		if (streq (vp->v_name, nm))
			continue;

		if (vp->v_ino == ino)
		{
			if (!sflag) printf
			(	"Dispositivo ESPÚRIO \"%s/%s\" é um elo físico de \"%s\"",
				dev_path, vp->v_name, nm
			);

			if (!wflag)
			{
				if (!sflag)
					printf ("\n");

				continue;
			}

			if (!sflag)
				printf (". Remove? (n): ");

			if (!fflag)
			{
				if (askyesno () <= 0)
					continue;
			}
			else
			{
				if (!sflag)
					printf ("SIM\n");
			}

			if (unlink (vp->v_name) < 0)
				error ("*Não consegui remover \"%s/%s\"", dev_path, vp->v_name);
		}
	}

}	/* end do_unlink */

/*
 ****************************************************************
 *	Obtem o código do tipo do arquivo			*
 ****************************************************************
 */
int
fmode (long mode)
{
	switch (mode & S_IFMT)
	{
	    case S_IFREG:
		return ('R');

	    case S_IFDIR:
		return ('D');

	    case S_IFBLK:
		return ('B');

	    case S_IFCHR:
		return ('C');

	    case S_IFIFO:
		return ('F');

	    default:
		return ('?');
	}

}	/* end fmode */

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
		"%s - cria/atualiza os dispositivos de \"/dev\"\n"
		"\n%s\n"
		"\nSintaxe:\n"
		"\t%s [-wufasv]\n",
		pgname, pgversion, pgname
	);

	fprintf
	(	stderr,
		"\nOpções:"
		"\t-w: Cria/atualiza os dispositivos em \"/dev\"\n"
		"\t-u: Como \"-w\"\"\n"
		"\t-f: Não consulta para atualizar\n"
		"\t-a: Analisa o diretório da raiz alternativa \"/aroot/dev\"\n"
		"\t-s: Modo silencioso (implica \"-f\")\n"
		"\t-v: Verboso\n"
	);

	exit (2);

}	/* end help */
