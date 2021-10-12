/*
 ****************************************************************
 *								*
 *			df.c					*
 *								*
 *	Informa sobre o número de blocos e nós-índice livres	*
 *								*
 *	Versão	1.0.0, de 01.07.86				*
 *		4.6.0, de 10.09.04				*
 *								*
 *	Módulo: df						*
 *		Utilitários Básicos				*
 *		Categoria B					*
 *								*
 *	TROPIX: Sistema Operacional Multiprocessado		*
 *		Copyright © 2004 NCE/UFRJ - tecle "man licença"	*
 * 								*
 ****************************************************************
 */

#include <sys/common.h>
#include <sys/sync.h>
#include <sys/scb.h>
#include <sys/kcntl.h>
#include <sys/ioctl.h>
#include <sys/mntent.h>
#include <sys/sb.h>
#include <sys/t1.h>
#include <sys/v7.h>
#include <sys/syscall.h>

#include <stdio.h>
#include <stdlib.h>
#include <a.out.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <stat.h>
#include <ustat.h>
#include <errno.h>

/*
 ****************************************************************
 *	Variáveis e Definições globais				*
 ****************************************************************
 */
const char	pgversion[] =  "Versão:	4.6.0, de 10.09.04";

#define	elif	else if

entry int	dflag;		/* Debug */
entry int	vflag;		/* Verbose */

entry char	unit;		/* Unidade (MB, KB, blocos ou nós-índice */

const char	*fs_names[] = FS_NAMES;

/*
 ******	Definições relativas à ordem de bytes da CPU ************
 */
#ifdef	SB_ENDIAN_CV
#define	SB_ENDIAN_LONG(x)	long_endian_cv (x)
#define	SB_ENDIAN_SHORT(x)	short_endian_cv (x)
#else
#define	SB_ENDIAN_LONG(x)	(x)
#define	SB_ENDIAN_SHORT(x)	(x)
#endif	SB_ENDIAN_CV

/*
 ******	Conversões para KB **************************************
 */
#define	BLTOKB_INT(x)	((unsigned)(x) >> (KBSHIFT-BLSHIFT))
#define	BLTOKB_DEC(x)	(((x) & (KBSZ/BLSZ-1)) * (100 / (KBSZ/BLSZ)))

/*
 ******	Conversões para MB **************************************
 */
#define	BLTOMB_ROUND(x)	(x += (MBSZ/BLSZ/(2 * 100)))
#define	BLTOMB_INT(x)	((unsigned)(x) >> (MBSHIFT-BLSHIFT))
#define	BLTOMB_DEC(x)	((100 * ((unsigned)(x) & (MBSZ/BLSZ - 1)))	\
					>> (MBSHIFT-BLSHIFT))
/*
 ******	Estruturas usadas ***************************************
 */
entry SB		*sb_list;	/* Lista de SBs (ordem normal) */

#define	sb_printed	sb_res[0]

entry int		dev_max = 15,	/* Larguras máximas */
			dir_max = 11;

/*
 ******	Protótipos de funções ***********************************
 */
void		create_SB_list (void);
int		proc_name (const char *);
int		proc_entry (SB *);
int		print_entry (SB *, USTAT *);
const char	*edit_value (unsigned long);
void		help (void);

/*
 ****************************************************************
 *	Programa de informar espaço livre no disco		*
 ****************************************************************
 */
int
main (int argc, const char *argv[])
{
	int		opt, i, ret = 0;
	SB		*sp;
	const char	*str = NOSTR;

	/*
	 *	Analisa as opções
	 */
	while ((opt = getopt (argc, argv, "kbivdH")) != EOF)
	{
		switch (opt)
		{
		    case 'k':			/* KB ao invés de MB */
			if (unit)
				help ();
			unit = 'k';
			break;

		    case 'b':			/* Blocos */
			if (unit)
				help ();
			unit = 'b';
			break;

		    case 'i':			/* Nós-Índice */
			if (unit)
				help ();
			unit = 'i';
			break;

		    case 'v':			/* Verbose */
			vflag++;
			break;

		    case 'd':			/* Debug */
			dflag++;
			break;

		    case 'H':			/* Help */
			help ();

		    default:			/* Erro */
			putc ('\n', stderr);
			help ();

		}	/* end switch */

	}	/* end while */

	argv += optind;

	if (unit == 0)
		unit = 'm';

	/*
	 *	Inicialização das tabela
	 */
	create_SB_list ();

	/*
	 *	Imprime o cabeçalho
	 */
	switch (unit)
	{
	    case 'm':	/* MB */
		str = "---- MB ----";
		break;

	    case 'k':	/* KB */
		str = "---- KB ----";
		break;

	    case 'b':	/* Blocos */
		str = "-- BLOCOS --";
		break;

	    case 'i':	/* Nós Índice */
		str = " NÓS-ÍNDICE ";
		break;
	}

	for (i = dev_max + dir_max + 7; i > 0; i--)
		putchar ('-');

	printf
	(	" ---------------%s----------------\n"
		"TIPO  DISPOSITIVO%*sDIRETÓRIO%*s"
		"TOTAL     OCUPADO       LIVRE      %%\n",
		str,
		dev_max - 10, "", dir_max - 3, ""
	);

	/*
	 *	Analisa os argumentos
	 */
	if (*argv == NOSTR)
	{
		/* Não foram dados argumentos: processa a MTAB */

		for (sp = sb_list; sp != NOSB; sp = sp->sb_forw)
			ret += proc_entry (sp);
	}
	else
	{
		/* Foram dados argumentos */

		for (/* vazio */; *argv; argv++)
			ret += proc_name (*argv);
	}

	return (ret);

}	/* end df */

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

		memmove (sp, phys_sp, sizeof (SB)); sp->sb_printed = 0;

		if (sb_list == NOSB)
			sb_list = sp;
		else
			last_sp->sb_forw = sp;

		last_sp = sp; sp->sb_forw = NOSB;
			
		next_phys_sp = phys_sp->sb_forw;

		if ((int)phys (phys_sp, 0, 0) == -1)
			error ("*Não consegui desmapear um SB");

		/* Guarda os tamanhos máximos */

		if ((len = strlen (sp->sb_dev_nm)) > dev_max)
			dev_max = len;

		if ((len = strlen (sp->sb_dir_nm)) > dir_max)
			dir_max = len;
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
 *	Processa um nome 					*
 ****************************************************************
 */
int
proc_name (const char *filenm)
{
	SB		*sp;
	USTAT		*up;
	dev_t		dev;
	int		fd;
	V7SB		v7sb;
	T1SB		t1sb;
	SB		sb;
	STAT		s;
	USTAT		u;

	/*
	 *	Procura pelo nome na tabela
	 */
	for (sp = sb_list; sp != NOSB; sp = sp->sb_forw)
	{
		if (streq (filenm, sp->sb_dev_nm))
			return (proc_entry (sp));

		if (streq (filenm, sp->sb_dir_nm))
			return (proc_entry (sp));
	}

	/*
	 *	Verifica de que arquivo se trata
	 */
	if (stat (filenm, &s) < 0)
		{ error ("*Não consegui obter o estado de \"%s\"", filenm); return (1); }

	switch (s.st_mode & S_IFMT)
	{
	    case S_IFREG: 
	    case S_IFDIR:
	    case S_IFIFO:
	    default:
		dev = s.st_dev;
		break;

	    case S_IFBLK:
	    case S_IFCHR:
		dev = s.st_rdev;
		break;
	}

	/*
	 *	Procura pelo dev_t na tabela
	 */
	for (sp = sb_list; sp != NOSB; sp = sp->sb_forw)
	{
		if (sp->sb_dev == dev)
			return (proc_entry (sp));
	}

	/*
	 *	O sistema procurado não está montado
	 */
	switch (s.st_mode & S_IFMT)
	{
	    case S_IFREG: 
	    case S_IFDIR:
	    case S_IFIFO:
		error ("\"%s\" não corresponde a um arquivo montado", filenm);  
		return (1);
	}

	/*
	 *	Tenta descobrir o tipo do sistema de arquivos
	 */
	if ((fd = open (filenm, O_RDONLY)) < 0)
		{ error ("*Não consegui abrir \"%s\"", filenm); return (1); }

	if (ioctl (fd, DKISADISK) < 0)
		{ error ("O dispositivo \"%s\" NÃO é um disco", filenm); close (fd); return (1); }

	up = (USTAT *)NULL;

	if (up == (USTAT *)NULL)
	{
		lseek (fd, BL4TOBY (T1_SBNO), L_SET);

		if (read (fd, &t1sb, sizeof (t1sb)) != sizeof (t1sb))
			{ error ("*Não consegui ler o superbloco de \"%s\"", filenm); return (1); }

		if (t1sb.s_magic == T1_SBMAGIC)
		{
			up = &u;

			up->f_type   = FS_T1;
			up->f_bsize  = t1sb.s_bl_sz;
			up->f_fsize  = t1sb.s_fs_sz;
			up->f_tfree  = t1sb.s_fs_sz - t1sb.s_busy_bl;
			up->f_isize  = t1sb.s_fs_sz * T1_INOinBL4;
			up->f_tinode = up->f_isize - t1sb.s_busy_ino;
		}
	}

	if (up == (USTAT *)NULL)
	{
		int		isize;

		lseek (fd, BLTOBY (V7_SBNO), L_SET);

		if (read (fd, &v7sb, sizeof (v7sb)) != sizeof (v7sb))
			{ error ("*Não consegui ler o superbloco de \"%s\"", filenm); return (1); }

		if (SB_ENDIAN_LONG (v7sb.s_magic) == V7_SBMAGIC)
		{
			isize = SB_ENDIAN_SHORT (v7sb.s_isize);

			up = &u;

			up->f_type   = FS_V7;
			up->f_bsize  = BLSZ;
			up->f_fsize  = SB_ENDIAN_LONG (*(daddr_t *)v7sb.s_fsize) - isize;
			up->f_tfree  = SB_ENDIAN_LONG (*(daddr_t *)v7sb.s_tfblk);
			up->f_isize  = (isize - (V7_SBNO + 1)) * V7_INOPBL;
			up->f_tinode = SB_ENDIAN_SHORT (v7sb.s_tfdino);
		}
	}

	close (fd);

	/*
	 *	Imprime a linha
	 */
	strcpy (sb.sb_dev_nm, filenm);
	sb.sb_dir_nm[0]	= '\0';
	sb.sb_printed	= 0;

	return (print_entry (&sb, up));

}	/* end proc_name */

/*
 ****************************************************************
 *	Processa uma entrada da tabela				*
 ****************************************************************
 */
int
proc_entry (SB *sp)
{
	USTAT		u;

	if (ustat (sp->sb_dev, &u) < 0)
	{
		error ("*Não consegui obter o \"ustat\" de \"%s\"", sp->sb_dev_nm);  
		return (1);
	}

	return (print_entry (sp, &u));

}	/* end proc_entry */

/*
 ****************************************************************
 *	Imprime a linha						*
 ****************************************************************
 */
int
print_entry (SB *sp, USTAT *up)
{
	long		total, usados, livres;

	/*
	 *	Verifica se já foi processado
	 */
	if (sp->sb_printed++)
		return (0);

	/*
	 *	Nomes
	 */
	if (up == (USTAT *)NULL)
		printf ("      ");
	else
		printf ("%-6.6s",   fs_names[up->f_type]);

	printf ("%-*.*s ", dev_max, dev_max, sp->sb_dev_nm);
	printf ("%-*.*s",  dir_max, dir_max, sp->sb_dir_nm);

	if (up == (USTAT *)NULL)
	{
		printf ("\n");
		return (1);
	}

	/*
	 *	Calculas os valores
	 */
	if (unit == 'i')
	{
		if (up->f_isize == 0)
			{ printf ("\n"); return (0); }

		total  = up->f_isize;
		livres = up->f_tinode;
		usados = total - livres;
	}
	else
	{
		int		ratio = up->f_bsize >> BLSHIFT;

		total  = up->f_fsize * ratio;
		livres = up->f_tfree * ratio;

		usados = total - livres;
	}

	printf ("%11s ", edit_value (total));
	printf ("%11s ", edit_value (usados));
	printf ("%11s ", edit_value (livres));
	printf ("%8.2f\n", 100.0 *(double)usados/(double)total + 0.005);

	return (0);

}	/* end print_entry */

/*
 ****************************************************************
 *	Prepara o texto com a unidade pedida			*
 ****************************************************************
 */
const char *
edit_value (unsigned long value)
{
	static char	area[16];

	switch (unit)
	{
	    case 'm':
		sprintf (area, "%d.%02d", BLTOMB_INT (value), BLTOMB_DEC (value));
		break;

	    case 'k':
		sprintf (area, "%d.%02d", BLTOKB_INT (value), BLTOKB_DEC (value));
		break;

	    default:
		sprintf (area, "%d", value);
		break;
	}

	return (area);

}	/* end edit_value */

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
		"%s - informa sobre o espaço disponível de sistemas de arquivos\n"
		"\n%s\n"
		"\nSintaxe:\n"
		"\t%s [-kbi] [<nome> ...]\n",
		pgname, pgversion, pgname
	);

	fprintf
	(	stderr,
		"\nOpções:"
		"\t--: Normalmente, o espaço é dado em MB\n"
		"\t-k: Dá o espaço em KB\n"
		"\t-b: Dá o espaço em blocos (de 512 bytes)\n"
		"\t-i: Informa sobre os nós-índice\n"
		"\nObs.:\t<nome> pode ser um dispositivo ou diretório.\n"
		"\tSe não for dado nenhum <nome>, serão considerados\n"
		"\ttodos os dispositivos montados\n"
	);

	exit (2);

}	/* end help */
