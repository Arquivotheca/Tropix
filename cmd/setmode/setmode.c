/*
 ****************************************************************
 *								*
 *			setmode.c				*
 *								*
 *	Atribui as proteções corretas aos arquivos do sistema	*
 *								*
 *	Versão	3.0.0, de 04.11.98				*
 *		4.5.0, de 15.10.03				*
 *								*
 *	Módulo: setmode						*
 *		Utilitários Básicos				*
 *		Categoria B					*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 2003 NCE/UFRJ - tecle "man licença"	*
 * 								*
 ****************************************************************
 */

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/syscall.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <pwd.h>
#include <ftw.h>
#include <errno.h>

/*
 ****************************************************************
 *	Variáveis e Definições globais				*
 ****************************************************************
 */
const char	pgversion[] =  "Versão: 4.5.0, de 15.10.03";

#define	NOVOID	(void *)NULL
#define	NOSTAT	(STAT *)NULL
#define	elif	else if
#define	EVER	;;
#define STR(x)	# x
#define XSTR(x)	STR (x)

entry int	vflag;		/* Verbose */
entry int	uflag;		/* Atualiza */
entry int	eflag;		/* Existência */
entry int	fflag;		/* Força */

/*
 ******	Estruturas **********************************************
 */
typedef struct
{
	const char	*m_nm;		/* Nome do arquivo anômalo */
	int		m_mode;		/* Modo do arquivo anômalo */

}	ANOM;

#define	NOANOM	(ANOM *)NULL

typedef struct
{
	const char	*t_nm;		/* Nome da árvore */
	int		t_dir_mode;	/* Modo dos diretório da árvore */
	int		t_file_mode;	/* Modo dos arquivos da árvore */
	const ANOM	*t_anom;	/* Lista de anomalias */

}	TREE;

entry int		global_uid;		/* Dono */
entry int		global_gid;		/* Grupo */
entry int		global_dir_mode;	/* Modo para diretórios */
entry int		global_file_mode;	/* Modo para arquivos */
entry const ANOM	*global_anom_ptr; 	/* Verificando anômalos */

entry int		path_offset = 6;	/* Para pular ou não o "/aroot" */

/*
 ****** Pré-definição das anomalias *****************************
 */
const ANOM	bin_anom[];
const ANOM	lib_anom[];
const ANOM	usr_lib_anom[];
const ANOM	usr_bin_anom[];
const ANOM	usr_pub_bin_anom[];
const ANOM	usr_xwin_bin_anom[];
const ANOM	etc_anom[];
const ANOM	usr_etc_anom[];
const ANOM	dev_anom[];

/*
 ******	As árvores **********************************************
 */
const TREE	tree[] =
{
	"/aroot/",			0755,	0,	NOANOM,
	"/aroot/dev",			0755,	0,	NOANOM,
	"/aroot/tmp",			0777,	0,	NOANOM,
	"/aroot/home",			0755,	0,	NOANOM,
	"/aroot/usr",			0755,	0,	NOANOM,
	"/aroot/usr/lib",		0755,	0,	usr_lib_anom,
	"/aroot/usr/pub",		0755,	0,	NOANOM,
	"/aroot/usr/xwin",		0755,	0,	NOANOM,
	"/aroot/var",			0755,	0,	NOANOM,
	"/aroot/var/spool",		0755,	0,	NOANOM,
	"/aroot/var/mail",		0755,	0,	NOANOM,
	"/aroot/src",			0700,	0,	NOANOM,

	"/aroot/bin",			0755,	0755,	bin_anom,
	"/aroot/usr/bin",		0755,	0755,	usr_bin_anom,
	"/aroot/usr/pub/bin",		0755,	0755,	usr_pub_bin_anom,
	"/aroot/usr/xwin/bin",		0755,	0755,	usr_xwin_bin_anom,
	"/aroot/lib",			0755,	0644,	lib_anom,
	"/aroot/usr/include",		0755,	0644,	NOANOM,
	"/aroot/usr/man",		0755,	0644,	NOANOM,
	"/aroot/usr/lib/terminfo",	0755,	0644,	NOANOM,
	"/aroot/usr/pub/lib",		0755,	0644,	NOANOM,
	"/aroot/usr/xwin/bitmaps",	0755,	0644,	NOANOM,
	"/aroot/usr/xwin/conf",		0755,	0644,	NOANOM,
	"/aroot/usr/xwin/include",	0755,	0644,	NOANOM,
	"/aroot/usr/xwin/lib",		0755,	0644,	NOANOM,
	"/aroot/usr/xwin/log",		0755,	0644,	NOANOM,
	"/aroot/usr/xwin/pixmaps",	0755,	0644,	NOANOM,
	"/aroot/etc",			0755,	0700,	etc_anom,
	"/aroot/usr/etc",		0755,	0700,	usr_etc_anom,
	"/aroot/usr/lib/itnet",		0700,	0700,	NOANOM,
	"/aroot/var/adm",		0755,	0644,	NOANOM,
   /***	"/aroot/dev",			0755,	0666,	dev_anom, ***/
	NOSTR
};

const char	mail_box_path[] = "/aroot/var/mail";


/*
 ******	Anomalias da árvore "/bin" ******************************
 */
const ANOM	bin_anom[] =
{
	"/aroot/bin/as",			0755|S_IMETX,
	"/aroot/bin/cc",			0755|S_IMETX,
	"/aroot/bin/df",			0755|S_ISUID|S_ISGID,
	"/aroot/bin/lc",			0755|S_IMETX,
	"/aroot/bin/ld",			0755|S_IMETX,
	"/aroot/bin/ps",			0755|S_IMETX|S_ISUID|S_ISGID,
	"/aroot/bin/sh",			0755|S_IMETX,
	"/aroot/bin/man",			0755|S_IMETX,
	"/aroot/bin/modif",			0755|S_IMETX,
	"/aroot/bin/show",			0755|S_IMETX,
	"/aroot/bin/vi",			0755|S_IMETX,
	"/aroot/bin/ex",			0755|S_IMETX,
	NOSTR
};

/*
 ******	Anomalias da árvore "/usr/lib" **************************
 */
const ANOM	usr_lib_anom[] =
{
	"/aroot/usr/lib/send_mail",		0755|S_ISUID|S_ISGID,
	NOSTR
};

/*
 ******	Anomalias da árvore "/usr/bin" **************************
 */
const ANOM	usr_bin_anom[] =
{
	"/aroot/usr/bin/eject",			0755|S_ISUID|S_ISGID,
	"/aroot/usr/bin/make",			0755|S_IMETX,
	"/aroot/usr/bin/m",			0755|S_IMETX,
	"/aroot/usr/bin/mvtree",		0755|S_ISUID|S_ISGID,
	"/aroot/usr/bin/passwd",		0755|S_ISUID|S_ISGID,
	"/aroot/usr/bin/su",			0755|S_ISUID|S_ISGID,
	"/aroot/usr/bin/xfm",			0755|S_ISUID|S_ISGID,
	NOSTR
};

/*
 ******	Anomalias da árvore "/usr/pub/bin" **********************
 */
const ANOM	usr_pub_bin_anom[] =
{
   /***	"/aroot/usr/pub/bin/ccheck",		0700,		***/
	"/aroot/usr/pub/bin/putcvtb",		0700,
	"/aroot/usr/pub/bin/ziptool",		0755|S_ISUID|S_ISGID,
	NOSTR
};

/*
 ******	Anomalias da árvore "/etc" ******************************
 */
const ANOM	etc_anom[] =
{
	"/aroot/etc/.profile",			0644,
	"/aroot/etc/checklist",			0644,
	"/aroot/etc/fstab",			0644,
	"/aroot/etc/globalenv",			0644,
	"/aroot/etc/group",			0644,
	"/aroot/etc/initab",			0644,
	"/aroot/etc/itnetdev",			0644,
	"/aroot/etc/itnetserv",			0644,
	"/aroot/etc/mailrc",			0644,
	"/aroot/etc/mount",			0755|S_ISUID|S_ISGID,
	"/aroot/etc/passwd",			0644,
	"/aroot/etc/rc",			0644,
	"/aroot/etc/boot",			0755,
	"/aroot/etc/boot/d.boot0",		0644,
	"/aroot/etc/boot/h.boot0",		0644,
	"/aroot/etc/boot/t1.d.boot1",		0644,
	"/aroot/etc/boot/t1.g.boot1",		0644,
	"/aroot/etc/boot/t1.l.boot1",		0644,
	"/aroot/etc/boot/v7.d.boot1",		0644,
	"/aroot/etc/boot/v7.g.boot1",		0644,
	"/aroot/etc/boot/v7.l.boot1",		0644,
	"/aroot/etc/boot/mbr",			0644,
	"/aroot/etc/dmesg",			0755,
	"/aroot/etc/locale",			0755,
	"/aroot/etc/locale/time_E",		0644,
	"/aroot/etc/locale/time_P",		0644,
	"/aroot/etc/umount",			0755|S_ISUID|S_ISGID,
	NOSTR
};

/*
 ******	Anomalias da árvore "/usr/etc" **************************
 */
const ANOM	usr_etc_anom[] =
{
	"/aroot/usr/etc/coremap",		0755,
	"/aroot/usr/etc/fdformat",		0755,
	"/aroot/usr/etc/sysmap",		0755|S_ISUID|S_ISGID,
	"/aroot/usr/etc/textmap",		0755,
	NOSTR
};

/*
 ******	Anomalias da árvore "/lib" ******************************
 */
const ANOM	lib_anom[] =
{
	"/aroot/lib/cpp",			0755|S_IMETX,
	"/aroot/lib/gcc.486",			0755|S_IMETX,
	"/aroot/lib/gnu.cc.486.pc",		0755|S_IMETX,
	NOSTR
};

/*
 ******	Anomalias da árvore "/usr/xwin/bin" *********************
 */
const ANOM	usr_xwin_bin_anom[] =
{
	"/aroot/usr/xwin/bin/startx",		0755,
	"/aroot/usr/xwin/bin/xinit",		0755|S_ISUID|S_ISGID,
	"/aroot/usr/xwin/bin/superprobe",	0755|S_ISUID|S_ISGID,
	"/aroot/usr/xwin/bin/xterm",		0755|S_ISUID|S_ISGID,
	"/aroot/usr/xwin/bin/xserver",		0755|S_ISUID|S_ISGID,
	NOSTR
};

/*
 ******	Dispositivos de "/dev" **********************************
 */
const ANOM	dev_mode[] =
{
	"/aroot/dev/console",			0666,
	"/aroot/dev/ed0",			0600,
	"/aroot/dev/ed1",			0600,
	"/aroot/dev/fd0",			0766,
	"/aroot/dev/fd1",			0766,
	"/aroot/dev/grave",			0666,
	"/aroot/dev/itndaemon",			0666,
	"/aroot/dev/itnraw",			0666,
	"/aroot/dev/itntcp",			0666,
	"/aroot/dev/itnudp",			0666,
	"/aroot/dev/kmem",			0644,
	"/aroot/dev/lp",			0666,
	"/aroot/dev/lp0",			0666,
	"/aroot/dev/lp1",			0666,
	"/aroot/dev/md0",			0740,
	"/aroot/dev/md1",			0740,
	"/aroot/dev/md2",			0740,
	"/aroot/dev/mem",			0644,
	"/aroot/dev/mouse",			0666,
	"/aroot/dev/null",			0666,
	"/aroot/dev/ppp000",			0666,
	"/aroot/dev/ppp001",			0666,
	"/aroot/dev/ppp002",			0666,
	"/aroot/dev/ppp003",			0666,
	"/aroot/dev/ppp004",			0666,
	"/aroot/dev/ppp005",			0666,
	"/aroot/dev/ppp006",			0666,
	"/aroot/dev/ppp007",			0666,
	"/aroot/dev/ptyc00",			0666,
	"/aroot/dev/ptyc01",			0666,
	"/aroot/dev/ptyc02",			0666,
	"/aroot/dev/ptyc03",			0666,
	"/aroot/dev/ptyc04",			0666,
	"/aroot/dev/ptyc05",			0666,
	"/aroot/dev/ptyc06",			0666,
	"/aroot/dev/ptyc07",			0666,
	"/aroot/dev/ptyc08",			0666,
	"/aroot/dev/ptyc09",			0666,
	"/aroot/dev/ptyc10",			0666,
	"/aroot/dev/ptyc11",			0666,
	"/aroot/dev/ptyc12",			0666,
	"/aroot/dev/ptyc13",			0666,
	"/aroot/dev/ptyc14",			0666,
	"/aroot/dev/ptyc15",			0666,
	"/aroot/dev/ptys00",			0666,
	"/aroot/dev/ptys01",			0666,
	"/aroot/dev/ptys02",			0666,
	"/aroot/dev/ptys03",			0666,
	"/aroot/dev/ptys04",			0666,
	"/aroot/dev/ptys05",			0666,
	"/aroot/dev/ptys06",			0666,
	"/aroot/dev/ptys07",			0666,
	"/aroot/dev/ptys08",			0666,
	"/aroot/dev/ptys09",			0666,
	"/aroot/dev/ptys10",			0666,
	"/aroot/dev/ptys11",			0666,
	"/aroot/dev/ptys12",			0666,
	"/aroot/dev/ptys13",			0666,
	"/aroot/dev/ptys14",			0666,
	"/aroot/dev/ptys15",			0666,
	"/aroot/dev/ramd",			0740,
	"/aroot/dev/ramd0",			0740,
	"/aroot/dev/ramd1",			0740,
	"/aroot/dev/rfd0",			0666,
	"/aroot/dev/rfd1",			0666,
	"/aroot/dev/rmd0",			0640,
	"/aroot/dev/rmd1",			0640,
	"/aroot/dev/rmd2",			0640,
	"/aroot/dev/root",			0740,
	"/aroot/dev/rramd",			0640,
	"/aroot/dev/rramd0",			0640,
	"/aroot/dev/rramd1",			0640,
	"/aroot/dev/rroot",			0640,
   /***	"/aroot/dev/rzip",			0644, ***/
   /***	"/aroot/dev/rzip0",			0644, ***/
   /***	"/aroot/dev/rzip1",			0644, ***/
	"/aroot/dev/sb",			0666,
	"/aroot/dev/slip000",			0666,
	"/aroot/dev/slip001",			0666,
	"/aroot/dev/slip002",			0666,
	"/aroot/dev/slip003",			0666,
	"/aroot/dev/slip004",			0666,
	"/aroot/dev/slip005",			0666,
	"/aroot/dev/slip006",			0666,
	"/aroot/dev/slip007",			0666,
   /***	"/aroot/dev/swap",			0644, ***/
	"/aroot/dev/tty",			0666,
	"/aroot/dev/tty000",			0666,
	"/aroot/dev/tty001",			0666,
	"/aroot/dev/tty002",			0666,
	"/aroot/dev/tty003",			0666,
	"/aroot/dev/tty004",			0666,
	"/aroot/dev/tty005",			0666,
	"/aroot/dev/tty006",			0666,
	"/aroot/dev/tty007",			0666,
	"/aroot/dev/video1",			0666,
	"/aroot/dev/video2",			0666,
	"/aroot/dev/video3",			0666,
	"/aroot/dev/video4",			0666,
	"/aroot/dev/video5",			0666,
	"/aroot/dev/video6",			0666,
	"/aroot/dev/video7",			0666,
	"/aroot/dev/video8",			0666,
	"/aroot/dev/xconsole",			0644,
   /***	"/aroot/dev/zip",			0766, ***/
   /***	"/aroot/dev/zip0",			0766, ***/
   /***	"/aroot/dev/zip1",			0766, ***/
	NOSTR
};

/*
 ****** Protótipos de funções ***********************************
 */
void		check_mail_boxes (void);
int		check_tree_file (const char *, STAT *, int);
int		check_dev_disks (const char *, STAT *, int);
void		check_mode (const char *, STAT *, int uid, int gid, int mode, const ANOM *);
void		help (void);

/*
 ****************************************************************
 *	Atribui as proteções corretas aos arquivos do sistema	*
 ****************************************************************
 */
int
main (int argc, const char *argv[])
{
	int		opt;
	const char	*nm;
	const TREE	*tp;
	const ANOM	*ap;

	/*
	 *	Analisa as opções
	 */
	while ((opt = getopt (argc, argv, "ueafvH")) != EOF)
	{
		switch (opt)
		{
		    case 'u':			/* Atualiza */
			uflag++;
			break;

		    case 'e':			/* Existência */
			eflag++;
			break;

		    case 'a':			/* "/aroot/..." */
			path_offset = 0;
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
			putc ('\n', stderr);
			help ();

		}	/* end switch */

	}	/* end while */

	argv += optind;
	argc -= optind;

	/*
	 *	Verifica o Usuário Efetivo
	 */
	if (geteuid () != 0)
		error ("$Este comando pode ser executado apenas pelo SUPERUSUÁRIO");

	/*
	 *	Processa os diretórios e árvores
	 */
	for (tp = tree; tp->t_nm != NOSTR; tp++)
	{
		askyesno_set (1);

		nm = tp->t_nm + path_offset;

		/* Verifica se é um diretório */

		if (tp->t_file_mode == 0)
		{
			check_mode (nm, NOSTAT, 0, 0, tp->t_dir_mode, NOANOM);
			continue;
		}

		/* É uma árvore */

		global_uid	 = 0;
		global_gid	 = 0;
		global_dir_mode	 = tp->t_dir_mode;
		global_file_mode = tp->t_file_mode;
		global_anom_ptr	 = tp->t_anom;

		ftw (nm, check_tree_file);

	}	/* end for (árvores) */

	/*
	 *	Processa os anômalos
	 */
	for (tp = tree; tp->t_nm != NOSTR; tp++)
	{
		if ((ap = tp->t_anom) == NOANOM)
			continue;

		askyesno_set (1);

		for (/* acima */; ap->m_nm != NOSTR; ap++)
			check_mode (ap->m_nm + path_offset, NOSTAT, 0, 0, ap->m_mode, NOANOM);

	}	/* end for (anômalos) */

	/*
	 *	Processa os discos de "/dev"
	 */
	askyesno_set (1);

	ftw ("/aroot/dev" + path_offset, check_dev_disks);

	/*
	 *	Processa a lista de "/dev"
	 */
	askyesno_set (1);

	for (ap = dev_mode; ap->m_nm != NOSTR; ap++)
		check_mode (ap->m_nm + path_offset, NOSTAT, 0, 0, ap->m_mode, NOANOM);

	/*
	 *	Verifica as caixas postais
	 */
	check_mail_boxes ();

	/*
	 *	Termina
	 */
	return (0);

}	/* end setmode */

/*
 ****************************************************************
 *	Verifica as caixas postais				*
 ****************************************************************
 */
void
check_mail_boxes (void)
{
	const char	*mail_str = mail_box_path + path_offset;
	DIR		*dir_fp;
	const DIRENT	*dp;
	const PASSWD	*pwp;
	char		*user_mail_box_path, *cp_point;
	int		len;

	/*
	 *	Abre o diretório da caixa postal
	 */
	if ((dir_fp = opendir (mail_str)) == NODIR)
		{ error ("*Não consegui abrir \"%s\"", mail_str); return; }

	/*
	 *	Prepara o prefixo das caixas postais dos usuários
	 */
	len = strlen (mail_str); user_mail_box_path = alloca (len + MAXNAMLEN + 2);

	strcpy (user_mail_box_path, mail_str);
	user_mail_box_path[len] = '/';
	cp_point = user_mail_box_path + len + 1;

	/*
	 *	Analisa cada caixa postal
	 */
	while ((dp = readdir (dir_fp)) != NODIRENT)
	{
		if (dp->d_name[0] == '.')
			continue;

		askyesno_set (1);

		if ((pwp = getpwnam (dp->d_name)) == NOPASSWD)
		{
			error
			(	"Não consegui obter a identificação do usuário da caixa postal \"%s\"",
				dp->d_name
			);
			continue;
		}

		strcpy (cp_point, dp->d_name);

		global_uid	 = pwp->pw_uid;
		global_gid	 = pwp->pw_gid;
		global_dir_mode	 = 0700;
		global_file_mode = 0600;
		global_anom_ptr	 = NOANOM;

		ftw (user_mail_box_path, check_tree_file);

	}	/* end while (entradas do diretório) */

	closedir (dir_fp);

}	/* end check_mail_boxes */

/*
 ****************************************************************
 *	Verifica o modo de um arquivo de uma árvore		*
 ****************************************************************
 */
int
check_tree_file (const char *file_nm, STAT *sp, int file_type)
{
	switch (file_type)
	{
	    case FTW_D:
		check_mode (file_nm, sp, global_uid, global_gid, global_dir_mode, global_anom_ptr);
		break;

	    case FTW_F:
		check_mode (file_nm, sp, global_uid, global_gid, global_file_mode, global_anom_ptr);
		break;

	    case FTW_NS:
	    case FTW_DNR:
	    default:
		if (eflag)
			error ("*Não consegui obter o estado de \"%s\"", file_nm);

		break;

	}	/* end switch */

	return (0);

}	/* end check_tree */

/*
 ****************************************************************
 *	Verifica o modo dos discos de "/dev"			*
 ****************************************************************
 */
int
check_dev_disks (const char *file_nm, STAT *sp, int file_type)
{
	if
	(	!patmatch (file_nm, "*/hd*")  &&
		!patmatch (file_nm, "*/rhd*") &&
		!patmatch (file_nm, "*/sd*")  &&
		!patmatch (file_nm, "*/rsd*")
	)
		return (0);

#if (0)	/*******************************************************/
	if
	(	strncmp (file_nm, "/dev/hd", 7) &&
		strncmp (file_nm, "/dev/sd", 7) &&
		strncmp (file_nm, "/dev/rhd", 8) &&
		strncmp (file_nm, "/dev/rsd", 8)
	)
		return (0);
#endif	/*******************************************************/

	switch (file_type)
	{
	    case FTW_D:
		break;

	    case FTW_F:
		if (S_ISBLK (sp->st_mode))
			check_mode (file_nm, sp, 0, 0, 0740, NOANOM);
		if (S_ISCHR (sp->st_mode))
			check_mode (file_nm, sp, 0, 0, 0640, NOANOM);
		break;

	    case FTW_NS:
	    case FTW_DNR:
	    default:
		if (eflag)
			error ("*Não consegui obter o estado de \"%s\"", file_nm);

		break;

	}	/* end switch */

	return (0);

}	/* end check_dev_disks */

/*
 ****************************************************************
 *	Verifica o modo de um arquivo qualquer			*
 ****************************************************************
 */
void
check_mode (const char *file_nm, STAT *sp, int uid, int gid, int mode, const ANOM *ap)
{
	STAT		s;
	char		bad_mode = 0, bad_uid = 0, bad_gid = 0;

	/*
	 *	Inicialmente, obtém o estado do arquivo
	 */
	if (sp == NOSTAT && stat (file_nm, sp = &s) < 0)
	{
		if (eflag)
			error ("*Não consegui obter o estado de \"%s\"", file_nm);

		return;
	}

	/*
	 *	Inicialmente, confere o dono/grupo e modo
	 */
	if (sp->st_uid != uid)
		bad_uid++;

	if (sp->st_gid != gid)
		bad_gid++;

	if ((sp->st_mode & ~S_IFMT) != mode)
		bad_mode++;

	if (!bad_mode && !bad_uid && !bad_gid)
		return;

	/*
	 *	Verifica se é um anômalo
	 */
	if (ap != NOANOM) for (/* acima */; ap->m_nm != NOSTR; ap++)
	{
		if (streq (ap->m_nm + path_offset, file_nm))
			return;
	}

	/*
	 *	Emite a mensagem
	 */
	if (bad_uid)
		error ("O dono de \"%s\" não é o correto (%d :: %d)", file_nm, sp->st_uid, uid);

	if (bad_gid)
		error ("O grupo de \"%s\" não é o correto (%d :: %d)", file_nm, sp->st_gid, gid);

	if (bad_mode)
	{
		fprintf (stderr, "%s => ", modetostr (sp->st_mode) + 2 + 7);
		fprintf	(stderr, "%s\t%s\n",  modetostr (mode) + 2 + 7, file_nm);
	}

	/*
	 *	Conserta
	 */
	if (!uflag)
		return;

	if (!fflag)
	{
		fprintf (stderr, "Deseja consertar? (n): ");

		if (askyesno () <= 0)
			return;
	}

	if ((bad_uid || bad_gid) && chown (file_nm, uid, gid) < 0)
		error ("*Não consegui atribuir o dono/grupo de \"%s\"", file_nm);

	if (bad_mode && chmod (file_nm, mode) < 0)
		error ("*Não consegui atribuir o modo de \"%s\"", file_nm);

}	/* end check_mode */

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
		"%s - atribui as proteções corretas aos arquivos do sistema\n"
		"\n%s\n"
		"\nSintaxe:\n"
		"\t%s [-ueaf]\n",
		pgname, pgversion, pgname
	);
	fprintf
	(	stderr,
		"\nOpções:"
		"\t-u: Atualiza\n"
		"\t-e: Informa sobre os arquivos não encontrados\n"
		"\t-a: Analisa a raiz alternativa \"/aroot\"\n"
		"\t-f: Não consulta para atualizar\n"
	);

	exit (2);

}	/* end help */
