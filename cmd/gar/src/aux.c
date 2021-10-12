/*
 ****************************************************************
 *								*
 *			aux.c					*
 *								*
 *	General Archiver -- Utilitario para Fitas/Disquettes	*
 *								*
 *	Versão	1.0.0, de 06.03.87				*
 *		4.2.0, de 27.04.02				*
 *								*
 *	Módulo: GAR						*
 *		Utilitários Básicos				*
 *		Categoria B					*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 2002 NCE/UFRJ - tecle "man licença"	*
 * 								*
 ****************************************************************
 */

#include <sys/common.h>
#include <sys/ioctl.h>
#include <sys/kcntl.h>
#include <sys/disktb.h>
#include <sys/stat.h>
#include <sys/syscall.h>

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>

#include "../h/common.h"
#include "../h/gar.h"
#include "../h/tar.h"

/*
 ****************************************************************
 *	Variáveis e Definições globais				*
 ****************************************************************
 */
#define	CRCSZ	8

#define	BYTOKB_INT(x)	((unsigned)(x) >> KBSHIFT)
#define	BYTOKB_DEC(x)	((((x) & (KBSZ - 1)) * 100) >> KBSHIFT)

entry long	totalbytes;	/* No. Total de Bytes lidos/escritos */

entry off_t	col_offset;	/* Tamanho da coleção */

entry off_t	col_max_offset;	/* Tamanho máximo da coleção */

/*
 ****************************************************************
 *	Blocador da Saida					*
 ****************************************************************
 */
void
bwrite (const void *area, int nbytes)
{
	int		count;

	totalbytes += nbytes; 

	while (nbytes > 0)
	{
		while (col_area_avail_sz <= 0)
		{
			if   (col_max_offset && col_offset + col_area_sz > col_max_offset)
			{
				vol++;
				volout ();
			}
			elif (write (colfd, b->buf, col_area_sz) == col_area_sz)
			{
				col_area_avail_sz = col_area_sz;
				col_offset += col_area_sz;
			}
			elif (errno == ENXIO)
			{
				vol++;
				volout ();
			}
			else
			{
				error ("$*Erro de escrita na coleção");
			}
		}

		count = MIN (nbytes, col_area_avail_sz);

		memmove (b->buf + col_area_sz - col_area_avail_sz, area, count);
		area += count;

		col_area_avail_sz -= count;
		nbytes -= count;
	}

}	/* end bwrite */

/*
 ****************************************************************
 *	"Flush" da Saida					*
 ****************************************************************
 */
void
bflush (void)
{
	char		dummy;
	long		bytes;

	bytes = totalbytes;

	memset (b->buf+col_area_sz-col_area_avail_sz, ' ', col_area_avail_sz);

	bwrite (b->buf+col_area_sz-col_area_avail_sz, col_area_avail_sz);
	bwrite (&dummy, sizeof (dummy));

	totalbytes = bytes;

}	/* end bflush */

/*
 ****************************************************************
 *	Desblocador da Entrada  				*
 ****************************************************************
 */
void
bread (void *area, int nbytes)
{
	long		count, n;
	long		oldpos;

	totalbytes += nbytes; 

	/*
	 *	Se area == NOSTR, apenas le sem copiar (skip).
	 */
	while (nbytes > 0)
	{
		while (col_area_avail_sz <= 0)
		{
			if (area == NOSTR && !rflag && !col_is_a_tape && nbytes >= col_area_sz)
			{
				if ((oldpos = lseek (colfd, 0, SEEK_CUR)) >= 0)
				{
					count = (nbytes / col_area_sz) * col_area_sz; 
					lseek (colfd, count, SEEK_CUR);

					if ((n = read (colfd, b->buf, col_area_sz)) > 0)
					{
						col_area_avail_sz = n - (nbytes-count);
						return;
					}

					lseek (colfd, oldpos, SEEK_SET);
				}

				n = read (colfd, b->buf, BLSZ);

				if (col_area_sz != BLSZ)
				{
					if (lseek (colfd, col_area_sz-BLSZ, SEEK_CUR) < 0)
						error ("$*Erro em SEEK");
				}

				count = col_area_sz;
			}
			else
			{
				n = read (colfd, b->buf, col_area_sz);

				if (n > 0 && n < col_area_sz)	/* Completa o último bloco */
					{ memset (b->buf + n, 0, col_area_sz - n); n = col_area_sz; }

				count = n;
			}

			if   (n > 0)
			{
				col_area_avail_sz = count;
			}
			elif (n == 0 || n < 0 && errno == ENXIO)
			{
				vol++;
				volin ();
			}
			else
			{
				error ("$*Erro de leitura da coleção");
			}

		}	/* end while (col_area_avail_sz <= 0) */

		count = MIN (nbytes, col_area_avail_sz);

		if (area != NOSTR)
		{
			memmove (area, b->buf + col_area_sz - col_area_avail_sz, count);
			area		+= count;
		}

		col_area_avail_sz -= count;
		nbytes		  -= count;

	}	/* end while (nbytes > 0) */

}	/* end bread */

/*
 ****************************************************************
 *	Tenta voltar para trás o buffer de entrada		*
 ****************************************************************
 */
int
bback (int nbytes, int rw)
{
	/*
	 *	Verifica se ainda está dentro do buffer corrente
	 */
	if (nbytes + col_area_avail_sz > col_area_sz)
	{
		/*
		 *	Não está; vai ter de reler o bloco anterior
		 */
		if (lseek (colfd, -(2 * col_area_sz), L_CUR) < 0)
		{
			error ("*Erro de seek na coleção");
			return (-1);
		}
	
		if (read (colfd, b->buf, col_area_sz) != col_area_sz)
		{
			error ("*Erro de leitura da coleção");
			return (-1);
		}
	
		col_area_avail_sz -= col_area_sz;
	}

	col_area_avail_sz += nbytes; totalbytes -= nbytes; 

	if (rw && lseek (colfd, -col_area_sz, L_CUR) < 0)
	{
		error ("*Erro de seek na coleção");
		return (-1);
	}

	return (0);

}	/* end bback */

/*
 ****************************************************************
 *	Tenta voltar ao início do arquivo 			*
 ****************************************************************
 */
int
file_rew (void)
{
	off_t		nbytes, oldpos, newpos, n, off;

	nbytes = khead.k_size;

	if   (type == GAR)
		nbytes += CRCSZ * BYTOBL (nbytes);
	elif (type == TAR)
		nbytes = BLROUND (nbytes);
	elif (type == CPIO || type == CPIOR)
		nbytes = WDROUND (nbytes);

	/*
	 *	Verifica se ainda está dentro do buffer corrente
	 */
	if (nbytes + col_area_avail_sz <= col_area_sz)
	{
		col_area_avail_sz += nbytes;
		totalbytes -= nbytes; 

		return (0);
	}

	/*
	 *	Não está; vai ter de reler um bloco anterior
	 */
	if (col_is_a_tape)
		return (-1);

	if ((oldpos = lseek (colfd, 0, SEEK_CUR)) < 0)
		return (-1);

	if ((newpos = oldpos - col_area_avail_sz - nbytes) < 0)
		return (-1);

	off = newpos % col_area_sz;

	lseek (colfd, newpos - off, SEEK_SET);

	if ((n = read (colfd, b->buf, col_area_sz)) == col_area_sz)
	{
		col_area_avail_sz = col_area_sz - off;
		totalbytes -= nbytes; 
		return (0);
	}

	/*
	 *	Não conseguiu: recoloca o ponteiro onde estava
	 */
	lseek (colfd, oldpos - col_area_sz, SEEK_SET);

	totalbytes -= col_area_sz - col_area_avail_sz;
	col_area_avail_sz = 0;

	return (-1);

}	/* end file_rew */

/*
 ****************************************************************
 *	Cria os Diretorios Faltando				*
 ****************************************************************
 */
int
verdir (char *file_nm)
{
	char		*cp;

	for (cp = file_nm + 1; *cp != '\0'; cp++) if (*cp == '/')
	{
		*cp = '\0';

		if (access (file_nm, F_OK) < 0)
		{
			if (mkdir (file_nm, 0777) < 0)
			{
				msg ("*Não consegui criar o diretório \"%s\"", file_nm);
				*cp = '/'; return (-1);
			}

			if (mflag && chown (file_nm, khead.k_uid, khead.k_gid) < 0)
				msg ("*Não consegui alterar o dono de \"%s\"", file_nm);
		}

		*cp = '/';
	}

	return (0);

}	/* end verdir */

/*
 ****************************************************************
 *	Cálculo do checksum do TAR				*
 ****************************************************************
 */
int
tchecksum (THEAD *th)
{
	int		i;
	const char	*cp;

	i = 0;

	for (cp = (char *)th; cp < th->t_chksum; cp++)
		i += *cp;

	i += sizeof (th->t_chksum) * ' ';	/* Do próprio "checksum" */

	for (cp = &th->t_chksum[sizeof (th->t_chksum)]; cp < (char *)&th[1]; cp++)
		i += *cp;

	return (i);

}	/* end tchecksum */

/*
 ****************************************************************
 *	Troca de dispositivo					*
 ****************************************************************
 */
int
newdev (FUNC func, int fd)
{
	const char	*cp;
	char		nome[64];
	STAT		s;

	close (fd);

	for (EVER)
	{
		fprintf (stderr, "\n\nFinal do volume nr. %d\n", vol);

		fprintf (stderr, "Dê o nome do arquivo/dispositivo ");
		fprintf (stderr, "do volume %d: ", vol+1);
		fflush  (stderr);

		cp = nome;

		if (fngets (nome, sizeof (nome), ttyfp) == NOSTR || cp[0] == '\0')
		{
			if (device_nm != NOSTR)
				cp = device_nm;
			else
				continue;
		}

		if
		((fd =  open
			(	cp,
				func == IN ? O_RDONLY|O_LOCK : O_WRONLY|O_LOCK
			))
				>= 0
		)
		{
			if (cp != device_nm)
				device_nm = strdup (cp);

			fstat (fd, &s);
			coldev = s.st_dev;
			colino = s.st_ino;

			get_dev_size (fd, &s);

			return (fd);
		}

		error ("*Não consegui abrir \"%s\"", cp);

	}	/* end for EVER */

}	/* end newdev */

/*
 ****************************************************************
 *	Escreve o Header do Volume (para troca de Volume)	*
 ****************************************************************
 */
void
volout (void)
{
	GVOL		*gp;

	colfd = newdev (OUT, colfd);

	gp = &b->convol;

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

	if (write (colfd, &b->convol, col_area_sz) != col_area_sz)
		error ("$*Erro de escrita na coleção");

	col_offset += col_area_sz;

	memmove (b->buf, b->buf+col_area_sz-sizeof (GVOL), sizeof (GVOL) );

	col_area_avail_sz = col_area_sz - sizeof (GVOL);

}	/* end volout */

/*
 ****************************************************************
 *	Le o Header do Volume (para troca de Volume)		*
 ****************************************************************
 */
void
volin (void)
{
	GVOL		*gv;
	int		i;

	gv = (GVOL *)b->buf;

	for (EVER)
	{
		colfd = newdev (IN, colfd);

		if (type != GAR)
			return;

		if   (read (colfd, b->buf, col_area_sz) != col_area_sz)
		{
			error ("*Erro de leitura da coleção");
		}
		elif (strncmp (gv->g_magic, garvolume, GARMSZ) != 0)
		{
			fprintf (stderr, "Isto não é um volume GAR\n");
		}
		elif (atoi (gv->g_date) != date)
		{
			fprintf (stderr, "A data do volume GAR não confere\n");
		}
		elif ((i = atoi (gv->g_vol)) != vol)
		{
			fprintf
			(	stderr,
				"Este é o volume %d (e não %d)\n",
				i + 1,
				vol + 1
			);
		}
		else
		{
			if (atoi (gv->g_vcrc) != crc16 ((char *)gv, GVOLCRCSZ) )
			{
			    fprintf (stderr, "Erro de CRC no volume GAR\n");
			}

			col_area_avail_sz = col_area_sz - sizeof (GVOL);

			return;
		}

	}	/* end for (EVER) */

}	/* end volin */

/*
 ****************************************************************
 *	Obtém o tamanho do dispositivo				*
 ****************************************************************
 */
void
get_dev_size (int fd, STAT *sp)
{
	struct disktb	d;

	col_max_offset = 0; col_offset = 0; col_is_a_tape = 0;

	if (!S_ISBLK (sp->st_mode) && !S_ISCHR (sp->st_mode))
		return;

	if (ioctl (fd, MTISATAPE, 0) > 0)
		{ col_is_a_tape = 1; return; }

	if (kcntl (K_GET_DEV_TB, sp->st_rdev, &d) < 0)
	{
#if (0)	/*******************************************************/
		erro
		(	"Não consegui obter o tamanho do dispositivo \"%s\"",
			device_nm
		);
#endif	/*******************************************************/
		return;
	}

	col_max_offset = d.p_size * BLSZ;

#ifdef	DEBUG
	fprintf (stderr, "col_max_offset = %d\n", col_max_offset);
#endif	DEBUG

}	/* end get_dev_size */

/*
 ****************************************************************
 *	Conversão octal/binário					*
 ****************************************************************
 */
int
otoi (const char *cp)
{
	int		c, i;

	i = 0;

	while (*cp == ' ')
		cp++;

	while ((c = *cp++ - '0') >= 0 && c <= 7)
		i = i << 3 | c;

	return (i);

}	/* end otoi */

/*
 ****************************************************************
 *	Emite uma mensagem de comparação de datas		*
 ****************************************************************
 */
void
time_cmp (time_t file_mtime, time_t col_mtime)
{
	if   (file_mtime == col_mtime)
	{
		msg ("As versão têm a mesma data de modificação");
	}
	elif (file_mtime > col_mtime)
	{
		msg
		(	"A versão mais recente é a do sistema de arquivos (%s)",
			print_interval (file_mtime - col_mtime)
		);
	}
	else
	{
		msg
		(	"A versão mais recente é a da coleção (%s)",
			print_interval (file_mtime - col_mtime)
		);
	}
		
}	/* end time_cmp */

/*
 ****************************************************************
 *	Emite uma noção de intervalo de tempo			*
 ****************************************************************
 */
char *
print_interval (time_t inter)
{
	static char	buf[32];

	if (inter < 0)
		inter = -inter;

	if   (inter < 60)
		sprintf (buf, "%d segundo(s)", inter);
	elif ((inter /= 60) < 60)
		sprintf (buf, "%d minuto(s)", inter);
	elif ((inter /= 60) < 24)
		sprintf (buf, "%d hora(s)", inter);
	elif ((inter /= 24) < 30)
		sprintf (buf, "%d dia(s)", inter);
	elif (inter < 365)
		sprintf (buf, "%d mes(es)", inter / 30);
	else
		sprintf (buf, "%d ano(s)", inter / 365);

	return (buf);

}	/* end print_interval */

/*
 ****************************************************************
 *	Emite uma mensagem					*
 ****************************************************************
 */
void
msg (char *fmt, ...)
{
	va_list		args;
	char		err;
	int		save_errno = errno;

	va_start (args, fmt);

	if   (g_filenmput == 0)
		{ fprintf (stderr, "%s:\n", g_filenm); g_filenmput++; }
	elif (g_nlput == 0)
		fprintf (stderr, "\n");

	if ((err = *fmt) == '*')
		fmt++;

	fprintf (stderr, "\t**** ");
	vfprintf (stderr, fmt, args);

	if (err == '*'  &&  save_errno > 0)
		fprintf (stderr, " (%s)", strerror (save_errno));

	putc ('\n', stderr);

	g_nlput++;

	va_end (args);

}	/* end msg */

/*
 ****************************************************************
 *	Imprime o nome do arquivo com a cor correta		*
 ****************************************************************
 */
void
print_file_nm (const char *file_nm, const STAT *sp, const char *str, FILE *fp)
{
	fprintf
	(	fp,
		"%s%s%s",
		color_vector[(sp->st_mode & S_IFMT) >> 12],
		file_nm,
		color_vector[MC_DEF]
	);

	if (str)
		fputs (str, fp);

}	/* end print_file_nm */

/*
 ****************************************************************
 *  Coloca o número adequado de "tabs" após o nome do arquivo	*
 ****************************************************************
 */
void
put_tabs (const char *file_nm, FILE *fp)
{
	int		n;

	n = 2 - ((strlen (file_nm) + 3) >> 3);

	while (n-- > 0)
		putc ('\t', fp);

	fprintf (fp, "\t    "); 	fflush (fp);

}	/* end put_tabs */

/*
 ****************************************************************
 *	Coloca o <nl> se necessário				*
 ****************************************************************
 */
void
putnl (void)
{
	if (g_nlput == 0 && vflag)
		{ putc ('\n', stderr); g_nlput++; }

}	/* end putnl */

/*
 ****************************************************************
 *	Prepara o texto com o no. de KBs			*
 ****************************************************************
 */
const char *
edit_BY_to_KB (ulong bytes)
{
	static char	area[16];

	sprintf (area, "%d.%02d", BYTOKB_INT (bytes), BYTOKB_DEC (bytes));

	return (area);

}	/* end edit_BL_to_KB */

/*
 ****************************************************************
 *	Edita uma percentagem					*
 ****************************************************************
 */
char *
editpercent (int quoc)
{
	static char	buf[8];
	char		*sinal;

	if (quoc < 0)
		{ sinal = "-"; quoc = -quoc; }
	else
		sinal = "";


	sprintf (buf, "%s%d.%02d", sinal, quoc / 100, quoc % 100);
	
	return (buf);

}	/* end editpercent */

/*
 ****************************************************************
 *	Calcula uma percentagem					*
 ****************************************************************
 */
int
getpercent (long numer, long denom)
{
	int		quoc;

	/*
	 *	Tenta evitar overflow
	 */
	if (numer > (0x7FFFFFFF / 10000))
		quoc = numer / (denom / 10000);
	else
		quoc = 10000 * numer / denom;

	return (quoc);

}	/* end percent */

/*
 ****************************************************************
 *	Encerra o programa					*
 ****************************************************************
 */
void
quit (int code)
{
	if (reduce_nm1)
	{
		unlink (reduce_nm0);
		unlink (reduce_nm1);
		unlink (reduce_nm2);
	}

	exit (exit_code + code);

}	/* end quit */
