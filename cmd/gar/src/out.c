/*
 ****************************************************************
 *								*
 *			out.c					*
 *								*
 *	General Archiver -- Utilitario para Fitas/Disquettes	*
 *								*
 *	Versão	1.1.1, de 15.01.88				*
 *		4.9.0, de 20.07.06				*
 *								*
 *	Módulo: gar						*
 *		Utilitários Básicos				*
 *		Categoria B					*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 2006 NCE/UFRJ - tecle "man licença"	*
 * 								*
 ****************************************************************
 */

#include <sys/common.h>
#include <sys/syscall.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stat.h>
#include <ustat.h>
#include <ftw.h>
#include <fcntl.h>
#include <time.h>

#include "../h/common.h"
#include "../h/reduce.h"
#include "../h/gar.h"
#include "../h/tar.h"

/*
 ****************************************************************
 *	Variáveis e Definições globais				*
 ****************************************************************
 */
typedef struct litem	LITEM;

struct litem
{
	char	*l_name;	/* Nome do link */
	dev_t	l_dev;		/* Dispositivo */
	ino_t	l_ino;		/* No. do Inode */
	LITEM	*l_next;	/* o Proximo */
};

entry LITEM	*linklist;		/* Comeco da Lista */

entry char	*reduce_nm0,	/* Nomes do arquivo temporário */
		*reduce_nm1,
		*reduce_nm2;

entry int	ftw_file_cnt;	/* Contador dos arquivos gerados por "ftw" */

/*
 ****************************************************************
 *	Saída							*
 ****************************************************************
 */
void
out (const char *argv[])
{
	int		i;
	GHEAD		ghead;

	/*
	 *	Imprime o tipo da coleção
	 */
	if (modif != APPEND)
	{
		if (Tflag == 0)
			fprintf (stderr, "Coleção GAR de %-24.24s, ", btime (&date));
		else
			fprintf (stderr, "Coleção TAR, ");

		if   (col_max_offset != 0)
			fprintf (stderr, "dispositivo de %s KB, ", edit_BY_to_KB (col_max_offset));

		if (col_area_sz & (BLSZ - 1))
			fprintf (stderr, "tamanho do bloco = %d bytes (fracionada)\n", col_area_sz);
		else
			fprintf (stderr, "tamanho do bloco = %s KB\n", edit_BY_to_KB (col_area_sz));
	}

	/*
	 *	Processa cada nome dado
	 */
	if (*argv == NOSTR) 		/* Não Foram Dados Argumentos. Le do Stdin */
	{
		char		*area = alloca (1024);

		while (fngets (area, 1024, stdin) != NOSTR)
		{
			ftw_file_cnt = 0;

			if ((i = ftw (area, outfile)) != 0)
				error ("\"ftw\" retornou o valor %d", i);
		}
	}
	else				/* Foram Dados Argumentos. Expande-os */
	{
		for (/* vazio */; *argv; argv++)
		{
			ftw_file_cnt = 0;

			if ((i = ftw (*argv, outfile)) != 0)
				error ("\"ftw\" retornou o valor %d", i);
		}
	}

	/*
	 *	Escreve o TRAILER
	 */
	if (Tflag == 0)			/* Formato GAR */
	{
		memset (&ghead, ' ', sizeof (GHEAD) );

		strncpy (ghead.g_magic, gartrailer, GARMSZ);

		bwrite (&ghead, sizeof (GHEAD) );
	}
	else 				/* Formato TAR */
	{
		char		*area = alloca (BLSZ);

		memset (area, 0, BLSZ);

		bwrite (area, BLSZ);
	}

	bflush ();

	fprintf
	(	stderr,
		"A coleção contém %d bytes (%s KB)\n",
		totalbytes,
		edit_BY_to_KB (ROUND (totalbytes, col_area_sz))
	); 

}	/* end out */

/*
 ****************************************************************
 *	Saida de Um Arquivo					*
 ****************************************************************
 */
int
outfile (const char *file_nm, STAT *sp, int status)
{
	GHEAD		*gp;
	int		len;
	GHEAD		ghead;
	const char	*nm, *linknm;
	int		reduce_sz = 0;
	char		*reduce_nm = NOSTR;

	/*
	 *	Contador de nomes de arquivos fornecidos por "ftw"
	 */
	ftw_file_cnt++;

	/*
	 *	Tira o "./" supérfluo
	 */
	if (file_nm[0] == '.' && file_nm[1] == '/' && file_nm[2] != '\0')
		file_nm += 2;

	/*
	 *	Inicializa variáveis para diagnósticos
	 */
	g_filenm = file_nm; 	g_filenmput = 0; 	g_nlput = 0;

	/*
	 *	Processa arquivos inacessíveis
	 */
	if (status == FTW_NS || status == FTW_DNR)
	{
		if (vflag || nflag)
		{
			fprintf (stderr, "%c ", status == FTW_NS ? '?' : 'D');
			fprintf (stderr, "%6d   ", 0 /* Tamanho NULO */); 
		}

		fprintf (stderr, "%s:\n", file_nm);
		g_filenmput++;	g_nlput++;

		if (status == FTW_NS)
			msg ("*Não consegui obter o estado de \"%s\"", file_nm);
		else
			msg ("*Não consegui percorrer o diretório \"%s\"", file_nm);

		return (0);
	}

	/*
	 *	Verifica se é para ignorar ".old"
	 */
	if (dotflag == 0)
	{
		if (patmatch (file_nm, ".!.*") || patmatch (file_nm, "*/.!.*"))
		{
			if (status == FTW_D)
				return (1);	/* Pula toda subárvore */
			else
				return (0);
		}
	}

	/*
	 *	Estuda o caso dos diretórios
	 */
	if (status == FTW_D)
	{
		if (dflag) 		/* Se foi dado "-d", não coloca os diretórios na coleção */
			return (0);
	}
	else
	{
		if (!pattern_accept (file_nm))
			return (0);
	}

#if (0)	/*******************************************************/
	/*
	 *	Verifica se é para não entrar em subdiretórios
	 */
	if (dflag  &&  ftw_file_cnt > 1  &&  status == FTW_D)
		return (1);
#endif	/*******************************************************/

#if (0)	/*******************************************************/
	/*
	 *	Diretórios só são incluídos com a opção "-." ou interativamente
	 */
	if (status == FTW_D  &&  !nflag  &&  !dotflag)
		return (0);
#endif	/*******************************************************/

#if (0)	/*******************************************************/
	/*
	 *	Analisa os padrões (exceto para diretórios)
	 */
	if (status != FTW_D && !pattern_accept (file_nm))
		return (0);
#endif	/*******************************************************/

	/*
	 ****** O arquivo foi aceito: começa o processamento ****
	 */
	if (vflag)
	{
		fprintf (stderr, "%c ", (modetostr (sp->st_mode))[0]);

		if (S_ISBLK (sp->st_mode) || S_ISCHR (sp->st_mode))
			fprintf (stderr, "  %2d/%-2d ", MAJOR (sp->st_rdev), MINOR (sp->st_rdev)); 
		else
			fprintf (stderr, "%7d ", sp->st_size); 

		fprintf (stderr, "  ");
		print_file_nm (file_nm, sp, NOSTR, stderr);
		put_tabs (file_nm, stderr);
		fflush  (stderr);
		g_filenmput++;
	}

	if
	(	S_ISREG (sp->st_mode) &&
		sp->st_dev == coldev &&
		sp->st_ino == colino
	)
	{
		msg ("O arquivo é a própria coleção!");
		return (0);
	}

	if (nflag)
	{
		fprintf (stderr, "%c %6d   ", (modetostr (sp->st_mode))[0], sp->st_size);
		print_file_nm (file_nm, sp, "? (n): ", stderr);
		g_filenmput++; g_nlput++;

		if (askyesno () <= 0)
		{
			if (S_ISDIR (sp->st_mode))
				return (1);
			else
				return (0);
		}
	}

	/*
	 *	Verifica se tem Links
	 */
	linknm = link_proc (file_nm, sp);

	/*
	 *	Verifica se é para gravar no formato TAR
	 */
	if (Tflag)
	{
		out_tar (file_nm, sp, linknm);
		putnl ();
		return (0);
	}

	/*
	 *	Comprime ou não o arquivo
	 */
	if   (!Dflag || linknm != NOSTR  || !S_ISREG (sp->st_mode))
	{
		nm = file_nm;
	}
	else	/* ZIP */
	{
		int		file_fd;
		FILE		*reduce_fp;

		reduce_nm = get_tmp_nm (BYTOBL (sp->st_size));

		if (Gflag)
			printf ("\nreduce_nm = \"%s\"\n", reduce_nm);

		if ((file_fd = open (file_nm, O_RDONLY)) == 0)
			{ msg ("*Não consegui abrir \"%s\"", file_nm); return (0); }

		if ((reduce_fp = fopen (reduce_nm, "w")) == NOFILE)
		{
			msg ("*Não consegui abrir \"%s\"", reduce_nm);
			close (file_fd);
			return (0);
		}

		if ((reduce_sz = deflate (file_fd, reduce_fp)) < 0)
		{
			msg ("Erro na compressão de \"%s\"", file_nm);
			fclose (reduce_fp);
			close (file_fd);
			return (0);
		}

		close (file_fd); fclose (reduce_fp);

		if (reduce_sz < sp->st_size)
		{
			nm = reduce_nm;

			if (vflag)
			{
				fprintf
				(	stderr,
					"(%s %%, D %d)",
					editpercent (getpercent (sp->st_size - reduce_sz, sp->st_size)),
					ZIP_LEVEL
				);
			}
		}
		else
		{
			nm = file_nm;
		}
	}

	/*
	 *	Prepara o Header do Arquivo
	 */
	gp = &ghead;

	len = strlen (file_nm),

	sprintf
	(	(char *)gp,
		"%12s%12u%12d%12d%6d%12u%12u%12u%12d%12d%12d%12d"
		"%6d%2s%12d%3d%2s%3s%16s",
		garheader,
		sp->st_dev,
		sp->st_ino,
		sp->st_mode,
		sp->st_nlink,
		sp->st_uid,
		sp->st_gid,
		sp->st_rdev,
		nm == reduce_nm ? reduce_sz : sp->st_size,
		sp->st_atime,
		sp->st_mtime,
		sp->st_ctime,
		len,
		linknm == NOSTR ? "  " : " *",
		nm == reduce_nm ? sp->st_size : 0, 
		nm == reduce_nm ? ZIP_LEVEL : 0, 
		nm == reduce_nm ? " R" : "  ", 
		nm == reduce_nm ? "D0" : "  ",
		""
	);

	sprintf
	(	gp->g_crc,
		"%7d",
		crc16 (gp, GHEADCRCSZ)
	);

	/*
	 *	Escreve o Header do Arquivo
	 */
	bwrite (gp, sizeof (GHEAD));

	/*
	 *	Escreve o Nome do Arquivo
	 */
	bwrite (file_nm, len);

	/*
	 *	Escreve o CRC do Nome do Arquivo
	 */
	sprintf
	(	gp->g_crc,
		"%7d",
		crc16 (file_nm, len)
	);

	bwrite (gp->g_crc, sizeof (gp->g_crc));

	/*
	 *	Verifica se deve escrever os Blocos ou o Nome do Link
	 */
	if (linknm == NOSTR)
	{
		outblocks (nm, sp);
		putnl ();
		return (0);
	}

	/*
	 *	Deve apenas escrever um LINK
	 */
	len = strlen (linknm),

	sprintf
	(	(char *)gp,
		"%12s%12u%12d%12d%6d%12u%12u%12u%12d%12d%12d%12d%6d%2s%36s",
		garlink,
		sp->st_dev,
		sp->st_ino,
		sp->st_mode,
		sp->st_nlink,
		sp->st_uid,
		sp->st_gid,
		sp->st_rdev,
		sp->st_size,
		sp->st_atime,
		sp->st_mtime,
		sp->st_ctime,
		len,
		"  ",
		""
	);

	sprintf
	(	gp->g_crc,
		"%7d",
		crc16 (gp, GHEADCRCSZ)
	);

	/*
	 *	Escreve o Header do LINK
	 */
	bwrite (gp, sizeof (GHEAD));

	/*
	 *	Escreve o Nome do LINK
	 */
	bwrite (linknm, len);

	/*
	 *	Escreve o CRC do Nome do LINK
	 */
	sprintf
	(	gp->g_crc,
		"%7d",
		crc16 (linknm, len)
	);

	bwrite (gp->g_crc, sizeof (gp->g_crc));

	putnl ();

	return (0);

}	/* end outfile */

/*
 ****************************************************************
 *	Obtém um arquivo temporário apropriado			*
 ****************************************************************
 */
char *
get_tmp_nm (int bl_sz)
{
	STAT		s;
	USTAT		us;
	static int	dev0 = -1, dev1 = -1;
	int		ratio;

	if (reduce_nm1 == NOSTR)
	{
		reduce_nm1 = mktemp ("/var/tmp/garXXXXXX");

		reduce_nm0 = reduce_nm1 + 4;	/* "/tmp/garXXXXXX" */
		reduce_nm2 = reduce_nm0 + 5;	/* "garXXXXXX" */

		if (stat ("/tmp", &s) >= 0)
			dev0 = s.st_dev;

		if (stat ("/var/tmp", &s) >= 0)
			dev1 = s.st_dev;
	}

	if (reduce_nm1 == NOSTR)
		error ("$ERRO em \"mktemp\"");

	if (dev0 != -1  &&  ustat (dev0, &us) >= 0)
	{
		ratio = us.f_bsize >> BLSHIFT;

		if (us.f_tfree * ratio >= bl_sz)
			return (reduce_nm0);
	}

	if (dev1 != -1  &&  ustat (dev1, &us) >= 0)
	{
		ratio = us.f_bsize >> BLSHIFT;

		if (us.f_tfree * ratio >= bl_sz)
			return (reduce_nm1);
	}

	return (reduce_nm2);

}	/* end get_tmp_nm */

/*
 ****************************************************************
 *	Saída no formato TAR					*
 ****************************************************************
 */
void
out_tar (const char *file_nm, STAT *sp, const char *linknm)
{
	THEAD		*tp;
	THEAD		thead;

	tp = &thead;

	if (strlen (file_nm) + 1 > sizeof (thead.t_name))
		{ msg ("Nome comprido demais: \"%s\"", file_nm); return; }

	memset (tp, 0, sizeof (THEAD));
	strcpy (tp->t_name, file_nm);

	sprintf (tp->t_mode,  "%6o ",   sp->st_mode & 0xFFFF);
	sprintf (tp->t_uid,   "%6o ",   sp->st_uid);
	sprintf (tp->t_gid,   "%6o ",   sp->st_gid);
	sprintf (tp->t_size,  "%11lo ", sp->st_size);
	sprintf (tp->t_mtime, "%11lo ", sp->st_mtime);

	/*
	 *	Verifica se é um LINK
	 */
	if (linknm)
	{
		tp->t_typeflag = '1';
		if (strlen (linknm) + 1 > sizeof (thead.t_linkname))
			{ msg ("*Nome comprido demais: \"%s\"", linknm); return; }
		strcpy (tp->t_linkname, linknm);
		sprintf (tp->t_chksum, "%6o", tchecksum (tp));
		bwrite (tp, sizeof (THEAD));
		return;
	}

	/*
	 *	Analisa o tipo de arquivo
	 */
	switch (sp->st_mode & S_IFMT)
	{
	    case S_IFREG:
		tp->t_typeflag = '0';
		sprintf (tp->t_chksum, "%6o", tchecksum (tp));
		bwrite (tp, sizeof (THEAD));
		outblocks (file_nm, sp);
		return;

	    case S_IFDIR:
		tp->t_typeflag = '5';
		sprintf (tp->t_chksum, "%6o", tchecksum (tp));
		bwrite (tp, sizeof (THEAD));
		return;

	    case S_IFCHR:
		tp->t_typeflag = '3';
		sprintf (tp->t_devmajor, "%6o ", MAJOR (sp->st_rdev));
		sprintf (tp->t_devminor, "%6o ", MINOR (sp->st_rdev));
		sprintf (tp->t_chksum,   "%6o", tchecksum (tp));
		bwrite (tp, sizeof (THEAD));
		return;

	    case S_IFBLK:
		tp->t_typeflag = '4';
		sprintf (tp->t_devmajor, "%6o ", MAJOR (sp->st_rdev));
		sprintf (tp->t_devminor, "%6o ", MINOR (sp->st_rdev));
		sprintf (tp->t_chksum,   "%6o", tchecksum (tp));
		bwrite (tp, sizeof (THEAD));
		return;

	    case S_IFIFO:
		tp->t_typeflag = '6';
		sprintf (tp->t_chksum,   "%6o", tchecksum (tp));
		bwrite (tp, sizeof (THEAD));
		return;
	}

}	/* end out_tar */

/*
 ****************************************************************
 *	Processa LINKs 						*
 ****************************************************************
 */
char *
link_proc (const char *file_nm, STAT *sp)
{
	LITEM		*lp;

	/*
	 *	Se tem apenas 1 LINK, nada precisa ser feito
	 */
	if (sp->st_nlink == 1)
		return (NOSTR);

	/*
	 *	Verifica se um outro nome já foi visto
	 */
	for (lp = linklist; lp != (LITEM *)NULL; lp = lp->l_next)
	{
		if (lp->l_dev == sp->st_dev && lp->l_ino == sp->st_ino)
			return (lp->l_name);
	}

	/*
	 *	É o primeiro da série: insere na lista
	 */
	if ((lp = (LITEM *)malloc (sizeof (LITEM))) == (LITEM *)NULL)
	{
		error ("Não obtive memória para item de elo físico");
	}
	elif ((lp->l_name = strdup (file_nm)) == NOSTR)
	{
		error ("Não obtive memória para item de elo físico");
	}
	else
	{
		lp->l_dev = sp->st_dev;
		lp->l_ino = sp->st_ino;

		lp->l_next = linklist;
		linklist = lp;
	}

	return (NOSTR);

}	/* end link_proc */

/*
 ****************************************************************
 *	Escreve os Blocos do Arquivo				*
 ****************************************************************
 */
void
outblocks (const char *file_nm, STAT *sp)
{
	int		fd, n;
	char		buf[BLSZ];
	char		crc[8];

	if   (S_ISREG (sp->st_mode))
	{
		if ((fd = open (file_nm, O_RDONLY)) < 0)
			{ msg ("*Não consegui abrir \"%s\"", file_nm); return; }

		while ((n = read (fd, buf, BLSZ)) > 0)
		{
			if (Tflag == 0) 			/* Formato GAR */
			{
				bwrite (buf, n);
				sprintf (crc, "%7d", crc16 (buf, n) );
				bwrite (crc, sizeof (crc));
			}
			else 					/* Formato TAR */
			{
				if (n < BLSZ)
					memset (buf+n, 0, BLSZ-n);
	
				bwrite (buf, BLSZ);
			}
		}

		close (fd);

		if (n != 0)
			msg ("*Erro de leitura do arquivo");
	}
	elif (S_ISLNK (sp->st_mode))
	{
		char		*symlink_nm;

		n = sp->st_size; symlink_nm = alloca (n);

		if (readlink (file_nm, symlink_nm, n) < 0)
			msg ("*Não consegui obter o conteúdo do elo simbólico de \"%s\"", file_nm);

		if (Tflag == 0) 			/* Formato GAR */
		{
			bwrite (symlink_nm, n);
			sprintf (crc, "%7d", crc16 (symlink_nm, n) );
			bwrite (crc, sizeof (crc));
		}
		else while (n > 0)			/* Formato TAR */
		{
			int		count;

			if ((count = n) > BLSZ)
				count = BLSZ;

			memmove (buf, symlink_nm, count);

			if (count < BLSZ)
				memset (buf + count, 0, BLSZ - count);

			bwrite (buf, BLSZ);

			n		-= count;
			symlink_nm	+= count;
		}
	}

}	/* end outblocks */ 

/*
 ****************************************************************
 *	Escreve o Header do Volume				*
 ****************************************************************
 */
void
volhwrite (void)
{
	GVOL		gvol;
	GVOL		*gp;

	if (Tflag)
		return;

	gp = &gvol;

	time (&date);

	sprintf
	(	(char *)gp,
		"%12s%12d%6d%9s%13s",
		garvolume,
		date,
		vol,
		headerversion,
		""
	);

	sprintf
	(	gp->g_vcrc,
		"%7d",
		crc16 ((char *)gp, GVOLCRCSZ)
	);

	bwrite ((char *)gp, sizeof (GVOL));

}	/* end volhwrite */
