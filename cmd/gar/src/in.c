/*
 ****************************************************************
 *								*
 *			in.c					*
 *								*
 *	General Archiver -- Utilitario para Fitas/Disquettes	*
 *								*
 *	Versão	1.1.2, de 15.02.88				*
 *		4.2.0, de 27.02.02				*
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
#include <sys/syscall.h>
#include <sys/uerror.h>
#include <sys/times.h>

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <stat.h>
#include <setjmp.h>
#include <pwd.h>
#include <time.h>

#include "../h/common.h"
#include "../h/gar.h"
#include "../h/reduce.h"

/*
 ****************************************************************
 *	Variáveis e Definições globais				*
 ****************************************************************
 */
entry int	ndiff;		/* Nr. de diferenças na comparação */

entry jmp_buf	q_env;		/* Para interromper a comparação */

/*
 ******	Variáveis globais referentes aos blocos comprimidos *****
 */
static int	reduce_file_fd;	/* Descritor do arquivo */

static int	reduce_file_block; /* No. do bloco do arquivo */

static int	reduce_file_errors; /* No. de diferenças do arquivo */

/*
 ******	Estrutura de modos **************************************
 */
typedef struct mode	MODE;

struct mode
{
	const char	*m_name;	/* Nome */
	KHEAD		m_head;		/* O estado */
	MODE		*m_next;	/* o Proximo */
};

entry MODE	*modelist;		/* Começo da Lista */

#define	NOMODE	(MODE *)NULL	

/*
 ****************************************************************
 *	Entrada							*
 ****************************************************************
 */
void
in (void)
{
	/*
	 *	Analisa o tipo da coleção
	 */
	prein ();

	/*
	 *	Analisa o Conteudo do Arquivo
	 */
	while (getheader () >=  0)
		procfile ();

	/*
	 *	Acerta os modos dos diretórios
	 */
	proc_mode_list ();

	/*
	 *	Mensagens finais	
	 */
	fprintf
	(	stderr,
		"A coleção contém %d bytes (%s KB))\n",
		totalbytes,
		edit_BY_to_KB (ROUND (totalbytes, col_area_sz))
	); 

	if (modif == COMP  ||  uflag)
	{
		if   (ndiff > 1)
		{
			fprintf
			(	stderr,
				"Foram encontradas diferenças em %d arquivos\n",
				ndiff
			); 
		}
		elif (ndiff == 1)
		{
			fprintf
			(	stderr,
				"Foram encontradas diferenças em 1 arquivo\n"
			); 
		}
		else
		{
			fprintf
			(	stderr,
				"Não foram encontradas diferenças\n"
			); 
		}
	}

	quit (0);

}	/* end gar */

/*
 ****************************************************************
 *	Preâmbulo da entrada					*
 ****************************************************************
 */
void
prein (void)
{
	/*
	 *	Le um Bloco e descobre o tamanho do bloco
	 */
	if ((col_area_sz = read (colfd, b->buf, col_area_sz)) <= 0)
		error ("$*Erro de leitura da coleção");

	switch (type = typeid ())
	{
	    case GAR:
		fprintf (stderr, "Coleção GAR de ");
		if (date)
			fprintf (stderr, "%-24.24s, ", btime (&date) ); 
		else
			fprintf (stderr, "(data desconhecida), ");
		break;

	    case CPIO:
		fprintf (stderr, "Coleção CPIO, ");
		break;

	    case CPIOR:
		fprintf (stderr, "Coleção CPIO Reverso, ");
		break;

	    case TAR:
		fprintf (stderr, "Coleção TAR, ");
		break;
	}

	if   (col_max_offset != 0)
		fprintf (stderr, "dispositivo de %s KB, ", edit_BY_to_KB (col_max_offset));

	if (col_area_sz & (BLSZ - 1))
		fprintf (stderr, "tamanho do bloco = %d bytes (fracionada)\n", col_area_sz);
	else
		fprintf (stderr, "tamanho do bloco = %s KB\n", edit_BY_to_KB (col_area_sz));

}	/* end prein */

/*
 ****************************************************************
 *	Obtem um Header						*
 ****************************************************************
 */
int
getheader (void)
{
	switch (type)
	{
	    case GAR:
		return (gartok (&khead));

	    case CPIO:
	    case CPIOR:
		return (cpiotok (&khead));

	    case TAR:
		return (tartok (&khead));
	}

	error ("Tipo indefinido");

	return (-1);

}	/* end getheader */

/*
 ****************************************************************
 *	Processa um Arquivo 					*
 ****************************************************************
 */
void
procfile (void)
{
	int		file_fd;
	char		*cp;
	long		mode;
	KHEAD		*kh = &khead;
	const char	*linknm;
	off_t		origsz, reduce_sz;
	STAT		old_s, linkstat;

	g_filenm = kh->k_name;
	g_filenmput = 0;
	g_nlput = 0;

	if ((origsz = kh->k_origsz) == 0)
		origsz = kh->k_size;

	/*
	 ******	Analisa se processa este arquivo ****************
	 */
	if (modif == CREATE && gflag)
	{
		static char	buf[512];
		static int	avail;

		if (avail == 0)
		{
			if (fngets (buf, sizeof (buf), gfp) == NOSTR)
				{ inskip (0); return; }
			avail++;
		}

		if (strcmp (kh->k_name, buf))
			{ inskip (0); return; }

		avail = 0;

	} 	/* end if (modif == CREATE && gflag) */

	/*
	 *	Se for comparação com atualização, começa com comparação
	 */
	if (uflag)
		modif = COMP;

	/*
	 *	Testa os padrões, se dados
	 */
	if (!S_ISDIR (kh->k_mode) && !pattern_accept (kh->k_name))
		{ inskip (0); return; }

	/*
	 ******	Impressão da Tabela *****************************
	 */
	if (modif == TAB)
		{ tbline (); inskip (0); return; }

	/*
	 ******	Analisa os demais casos *************************
	 *
	 ****** modif == CREATE, COMP ou PIPE *******************
	 */
	file_fd = -1;

	if (vflag)
	{
		fprintf (stderr, "%c ", (modetostr (kh->k_mode))[0]);

		if (S_ISBLK (kh->k_mode) || S_ISCHR (kh->k_mode))
			fprintf (stderr, "  %2d/%-2d ", MAJOR (kh->k_rdev), MINOR (kh->k_rdev)); 
		else
			fprintf (stderr, "%7d ", origsz); 

		fprintf (stderr, "  "); print_file_nm (kh->k_name, &kh->k_stat, NOSTR, stderr);

		if (kh->k_origsz)
		{
			put_tabs (kh->k_name, stderr);

			fprintf
			(	stderr,
				"(%s %%, %s%d)",
				editpercent (getpercent (kh->k_origsz - kh->k_size, kh->k_origsz)),
				kh->k_method == 'D' ? "D " : "", kh->k_bits
			);
		}

		fprintf (stderr, "\n");

		g_filenmput++; g_nlput++;
	}

	cp = kh->k_name;

	/*
	 *	Com a opção "-d", utiliza o final do caminho
	 */
	if (dflag)
	{
		if ((cp = strrchr (cp, '/')) == NOSTR)
			cp = kh->k_name;
		else
			cp++;
	}

	/*
	 ******	Criação *****************************************
	 */
	if (modif == CREATE)
	{
		if (nflag)
		{
			fprintf (stderr, "%c %6d ", (modetostr (kh->k_mode))[0], origsz);
			print_file_nm (kh->k_name, &kh->k_stat, "? (n): ", stderr);
			g_filenmput++; g_nlput++;

			if (askyesno () <= 0)
				{ inskip (0); return; }
		}

		/*
		 *	Se for comparação com atualização, continua com criação
		 */
	    update:
		modif = CREATE;

		if (lstat (cp, &old_s) >= 0)
		{
			if (fflag == 0  &&  uflag == 0)
			{
				if (g_filenmput == 0)
				{
					print_file_nm (kh->k_name, &kh->k_stat, "\n", stderr);
					g_filenmput++; g_nlput++;
				}
	
				fprintf
				(	stderr,
					"\t**** O arquivo já existe. "
					"Apaga/reescreve? (n): "
				);
	
				if (askyesno () <= 0)
					{ inskip (0); return; }
			}

			if
			(	(kh->k_mode & S_IFMT) != (old_s.st_mode & S_IFMT)
				|| !S_ISREG (kh->k_mode) || kh->k_linkflag != ' '
			)
			{
				if   (!S_ISDIR (old_s.st_mode))
				{
					if (unlink (cp) < 0)
					{
						msg ("*Não consegui remover");
						inskip (0);
						return;
					}
				}
				elif (!S_ISDIR (kh->k_mode))
				{
					if (rmdir (cp) < 0)
					{
						msg ("*Não consegui remover");
						inskip (0);
						return;
					}
				}
			}

		}	/* end if (o arquivo já existe) */

		if (verdir (cp) < 0)
			{ inskip (0); return; }

		/*
		 *	Verifica se é elo físico
		 */
		if (kh->k_linkflag != ' ')
		{
			if (dflag == 0 && link (kh->k_linkname, cp) < 0)
				msg ("*Não consegui criar o elo físico com \"%s\"", kh->k_linkname);

			putnl (); inskip (0); return;
		}

		if ((file_fd = mkfile (cp)) < 0)
			{ inskip (0); return; }

		if   (kh->k_origsz == 0)
		{
			inblocks (file_fd);
		}
		else 	/* kh->k_method == 'D' */
		{
			crcflag = kh->k_compcrc;

			reduce_file_fd = file_fd;
			reduce_file_block = -1;
			reduce_file_errors = 0;

			reduce_sz = inflate (red_block_write, kh->k_size);

			if (reduce_file_errors == 0  &&  reduce_sz != origsz)
			{
				msg
				(	"O tamanho %d do arquivo expandido "
					"NÃO confere",
					reduce_sz
				);
			}
		}

		if (file_fd > 0)
			close (file_fd);

		/*
		 *	Atribui o dono/modo
		 */
		if (mflag)
		{
			if (S_ISDIR (kh->k_mode))
				add_file_mode (cp);
			else
				set_file_mode (cp, kh);
		}

		putnl ();
		return;

	}	/* end CREAT */

	/*
	 ******	Pipe ********************************************
	 */
	if (modif == PIPE)
	{
		if (S_ISDIR (kh->k_mode))
			{ inskip (0); return; }

		if (kh->k_linkflag != ' ')
			{ msg ("Este arquivo é um elo físico"); inskip (0); return; }

		if (!S_ISREG (kh->k_mode))
			{ msg ("Este arquivo NÃO é regular"); inskip (0); return; }

		if   (kh->k_origsz == 0)
		{
			inblocks (STDOUT);
		}
		else 	/* kh->k_method == 'D' */
		{
			crcflag = kh->k_compcrc;

			reduce_file_fd = STDOUT;
			reduce_file_block = -1;
			reduce_file_errors = 0;

			reduce_sz = inflate (red_block_write, kh->k_size);

			if (reduce_file_errors == 0  &&  reduce_sz != origsz)
			{
				msg
				(	"O tamanho %d do arquivo expandido "
					"NÃO confere",
					reduce_sz
				);
			}
		}

		putnl ();

		return;
	}

	/*
	 ******	modif == COMP ***********************************
	 */
	if (nflag)
	{
		fprintf (stderr, "%c %6d ", (modetostr (kh->k_mode))[0], origsz);
		print_file_nm (kh->k_name, &kh->k_stat, "? (n): ", stderr);
		g_filenmput++; g_nlput++;

		if (askyesno () <= 0)
			{ inskip (0); return; }
	}

	if (lstat (cp, &old_s) < 0)
	{
		msg ("*Não consegui obter o estado do arquivo");

	    not_equal:
		ndiff++;

		if (uflag)
		{
			fprintf (stderr, "\t**** Atualiza? (n): ");

			if (fflag)
				{ fprintf (stderr, "sim\n"); goto update; }

			g_nlput++;

			if (askyesno () > 0)
				goto update;
		}

		inskip (0); putgfile (); return;
	}

	if (kh->k_linkflag != ' ')
	{
		if   (lstat (kh->k_linkname, &linkstat) < 0)
		{
			msg
			(	"*Não consegui obter o estado do elo físico \"%s\"",
				kh->k_linkname
			);

			goto not_equal;
		}
		elif (old_s.st_dev != linkstat.st_dev || old_s.st_ino != linkstat.st_ino)
		{
			msg ("O elo físico \"%s\" NÃO confere", kh->k_linkname);

			goto not_equal;
		}

		putnl (); return;
	}

	/*
	 *	Verifica se tem links espúrios
	 */
	if ((linknm = link_proc (cp, &old_s)) != NOSTR && !streq (linknm, cp))
	{
		msg ("O arquivo tem um elo físico espúrio com \"%s\"", linknm);

		ndiff++;

		if (uflag)
		{
			fprintf (stderr, "\t**** Atualiza? (n): ");

			if (fflag)
				{ fprintf (stderr, "sim\n"); goto un_link; }

			g_nlput++;

			if (askyesno () > 0)
			{
			    un_link:
				if (unlink (cp) < 0)
				{
					msg ("*Não consegui remover \"%s\"", cp);
					inskip (0);
					return;
				}
		
				goto update;
			}
		}

		inskip (0); putgfile (); return;
	}

	/*
	 *	Analisa o tipo do arquivo
	 */
	mode = kh->k_mode & S_IFMT;

	if ((old_s.st_mode & S_IFMT) != mode)
	{
		msg
		(	"Os arquivos têm tipos diferentes: (%c :: %c)",
			(modetostr (mode))[0],
			(modetostr (old_s.st_mode))[0]
		);

		goto not_equal;
	}

	switch (mode)
	{
	    case S_IFREG: 
		if (old_s.st_size != origsz)
		{
			msg
			(	"As versões têm tamanhos diferentes (%d :: %d)",
				origsz, old_s.st_size
			);

			time_cmp (old_s.st_mtime, kh->k_mtime);

			goto not_equal;
		}

		if (old_s.st_size == 0)
		{
			msg ("Ambas versões têm tamanho NULO");
			return;
		}

		if ((file_fd = open (cp, O_RDONLY)) < 0)
		{
			msg ("*Não consegui abrir o arquivo");
			goto not_equal;
		}

		if   (kh->k_origsz == 0)
		{
			if (inblocks (file_fd) > 0)
			{
				time_cmp (old_s.st_mtime, kh->k_mtime);

				close (file_fd);
				ndiff++;

				if (uflag)
				{
					fprintf (stderr, "\t**** Atualiza? (n): ");

					if (fflag)
						{ fprintf (stderr, "sim\n"); goto n_rew; }

					g_nlput++;

					if (askyesno () > 0)
					{
					    n_rew:
						if (file_rew () >= 0)
							goto update;
						else
							msg ("Não consegui dar o rewind");
					}
				}

				putgfile ();
			}
		}
		else 	/* kh->k_method == 'D' */
		{
			crcflag = kh->k_compcrc;

			reduce_file_fd = file_fd;
			reduce_file_block = -1;
			reduce_file_errors = 0;

			if (setjmp (q_env) != 0)
				{ close (file_fd); putnl (); return; }

			reduce_sz = inflate (red_block_cmp, kh->k_size);

			if (reduce_file_errors == 0)
			{
				if (reduce_sz != origsz) msg
				(	"O tamanho %d do arquivo expandido "
					"NÃO confere",
					reduce_sz
				);
			}
			else
			{
				time_cmp (old_s.st_mtime, kh->k_mtime);

				close (file_fd);
				ndiff++;

				if (uflag)
				{
					fprintf (stderr, "\t**** Atualiza? (n): ");

					if (fflag)
						{ fprintf (stderr, "sim\n"); goto d_rew; }

					g_nlput++;

					if (askyesno () > 0)
					{
					    d_rew:
						if (file_rew () >= 0)
							goto update;
						else
							msg ("Não consegui dar o rewind");
					}
				}

				putgfile ();
			}
		}

		close (file_fd);
		putnl ();
		return;

	    case S_IFCHR:
	    case S_IFBLK:
		if (old_s.st_rdev != kh->k_rdev)
		{
			msg
			(	"São dispositivos diferentes (%d/%d :: %d/%d)",
				MAJOR (kh->k_rdev), 
				MINOR (kh->k_rdev), 
				MAJOR (old_s.st_rdev),
				MINOR (old_s.st_rdev)
			);
			goto not_equal;
		}
		break;

	    case S_IFLNK:
	    {
		char		*symlink_nm = alloca (old_s.st_size + 1);

		if (readlink (kh->k_name, symlink_nm, old_s.st_size + 1) < 0)
		{
			msg ("*Não consegui ler o conteúdo do elo simbólico de \"%s\"", kh->k_name);
			goto not_equal;
		}

		if (!streq (kh->k_linkname, symlink_nm))
		{
			msg
			(	"Os conteúdos dos elos simbólicos NÃO são iguais \"%s\" :: \"%s\"",
				kh->k_linkname, symlink_nm
			);

			goto not_equal;
		}


		break;
	    }

	    case S_IFDIR:
	    case S_IFIFO:
		break;

	}	/* end switch */

	putnl ();

}	/* end procfile */

/*
 ****************************************************************
 *	Adiciona um MODO na lista				*
 ****************************************************************
 */
void
add_file_mode (const char *nm)
{
	MODE		*mp;
	const char	*cp;

	if ((mp = malloc (sizeof (MODE))) == NOMODE || (cp = strdup (nm)) == NOSTR)
	{
		fprintf (stderr, "%s: Não obtive memória para item de modo", pgname);
	}
	else
	{
		mp->m_name = cp;
		memmove (&mp->m_head, &khead, sizeof (KHEAD));

		mp->m_next = modelist;
		modelist = mp;
	}

}	/* end add_file_mode */

/*
 ****************************************************************
 *	Altera os modos dos diretórios da lista de modos	*
 ****************************************************************
 */
void
proc_mode_list (void)
{
	const MODE	*mp;

	for (mp = modelist; mp != NOMODE; mp = mp->m_next)
		set_file_mode (mp->m_name, &mp->m_head);

}	/* end proc_mode_list */

/*
 ****************************************************************
 *	Atribui o modo do arquivo				*
 ****************************************************************
 */
void
set_file_mode (const char *cp, const KHEAD *kh)
{
	/*
	 *	Atribui o dono/modo
	 */
	if (chmod (cp, kh->k_mode & ~S_IFMT) < 0)
		msg ("*Não consegui restaurar a proteção de \"%s\"", cp);

	if (utime (cp, &kh->k_atime) < 0)
		msg ("*Não consegui restaurar os tempos de \"%s\"", cp);

	if (chown (cp, kh->k_uid, kh->k_gid) < 0)
		msg ("*Não consegui restaurar o dono de \"%s\"", cp);

}	/* end set_file_mode */

/*
 ****************************************************************
 *	Grava o nome do arquivo em "garfield"			*
 ****************************************************************
 */
void
putgfile (void)
{
	if (gflag)
		fprintf (gfp, "%s\n", g_filenm);

}	/* end putgfile */

/*
 ****************************************************************
 *	Cria o Arquivo						*
 ****************************************************************
 */
int
mkfile (const char *file_nm)
{
	int		fd;
	const KHEAD	*kh = &khead;

	switch (kh->k_mode & S_IFMT)
	{
	    case S_IFREG:
		if ((fd = creat (file_nm, kh->k_mode)) < 0)
		{
			msg ("*Não consegui criar o arquivo \"%s\"", file_nm);
			return (-1);
		}
		return (fd);

	    case S_IFBLK:
	    case S_IFCHR:
	    case S_IFIFO:
		if (mknod (file_nm, kh->k_mode, kh->k_rdev) < 0)
		{
			msg ("*Não consegui criar o arquivo \"%s\"", file_nm);
			return (-1);
		}
		break;

	    case S_IFDIR:
		if (access (file_nm, F_OK) < 0 && mkdir (file_nm, kh->k_mode) < 0)
		{
			msg ("*Não consegui criar o diretório \"%s\"", file_nm);
			return (-1);
		}

		break;

	    case S_IFLNK:
		if (symlink (kh->k_linkname, file_nm) < 0)
		{
			msg ("*Não consegui criar o elo simbólico \"%s\"", file_nm);
			return (-1);
		}

		break;

	    default:
		msg ("Arquivo com formato inválido: %06X", kh->k_mode);
		return (-1);
	}

	return (0);

}	/* end mkfile */

/*
 ****************************************************************
 *	Le os Blocos de Um arquivo				*
 ****************************************************************
 */
int
inblocks (int file_fd)
{
	int		n, count, bloco;
	const KHEAD	*kh = &khead;
	char		colbuf[BLSZ];
	char		filebuf[BLSZ];
	char		crc[8];
	int		erros;

	/*
	 *	Chamado com CREAT, COMP, PIPE
	 */
	erros = 0;

	if (kh->k_linkflag != ' ' || !S_ISREG (kh->k_mode))
		return (0);

	bloco = 0;

	n = kh->k_size;

	while (n > 0)
	{
		count = MIN (n, BLSZ);

		bread (colbuf, count);
		n -= count;

		if (type == GAR)
		{
			bread (crc, sizeof (crc) );

			if (atoi (crc) != crc16 (colbuf, count) )
			{
				msg ("Erro de crc no bloco %d", bloco);
				erros++;
			}
		}

		if ((modif == CREATE || modif == PIPE) && file_fd >= 0)
		{
			if (write (file_fd, colbuf, count) != count)
			{
				msg ("*Erro de escrita no bloco %d", bloco);
				erros++;
				file_fd = -1;
			}
		}
		elif (modif == COMP && file_fd >= 0)
		{
			if   (read (file_fd, filebuf, count) != count)
			{
				msg ("*Erro de leitura no bloco %d", bloco);
				erros++;
				file_fd = -1;
			}
			else
			{
				if (memeq (colbuf, filebuf, count) == 0)
				{
					if   (erros < 10)
						msg ("Diferença no bloco %d", bloco);
					elif (erros == 10)
						msg ("Número excessivo de diferenças");
					erros++;
				}

			}	/* end read sem erro */

		}	/* end elif (modif == COMP && file_fd >= 0) */

		bloco++;

	}	/* end 	while (n > 0) */

	n = kh->k_size;

	if   (type == TAR)
		bread (NOSTR, BLROUND (n) - n);
	elif (type == CPIO || type == CPIOR)
		bread (NOSTR, WDROUND (n) - n);

	return (erros == 0 ? 0 : 1);

}	/* end inblocks */ 

/*
 ****************************************************************
 *	Faz a comparação de um arquivo comprimido		*
 ****************************************************************
 */
void
red_block_cmp (char *col_cp, int col_cnt)
{
	char		filebuf[BLSZ];

	reduce_file_block++;

	if (reduce_file_fd < 0)
		return;

	if (read (reduce_file_fd, filebuf, col_cnt) != col_cnt)
	{
		msg ("*Erro de leitura no bloco %d", reduce_file_block);
		reduce_file_errors++;
		reduce_file_fd = -1;
	}

	if (memeq (col_cp, filebuf, col_cnt) == 0)
	{
		if   (reduce_file_errors < 10)
			msg ("Diferença no bloco %d", reduce_file_block);
		elif (reduce_file_errors == 10)
			msg ("Número excessivo de diferenças");

		reduce_file_errors++;
	}

}	/* end red_block_cmp */

/*
 ****************************************************************
 *	Escreve os blocos de um arquivo comprimido		*
 ****************************************************************
 */
void
red_block_write (char *col_cp, int col_cnt)
{
	reduce_file_block++;

	if (reduce_file_fd < 0)
		return;

	if (write (reduce_file_fd, col_cp, col_cnt) != col_cnt)
	{
		msg ("*Erro de escrita no bloco %d", reduce_file_block);
		reduce_file_errors++;
		reduce_file_fd = -1;
	}

}	/* end red_block_write */

/*
 ****************************************************************
 *	Pula um Arquivo						*
 ****************************************************************
 */
void
inskip (int i)
{
	int		n, count, bloco;
	KHEAD		*kh = &khead;
	char		buf[BLSZ];
	char		crc[8];

	/*
	 *	Verifica se o Arquivo Realmente tem Blocos
	 */
	if (kh->k_linkflag != ' ' || !S_ISREG (kh->k_mode))
		return;

	bloco = 0;

	n = kh->k_size - i;

	if (!rflag && !col_is_a_tape)
	{
		/*
		 *	Skip rápido para disco
		 */
		if (type == GAR)
			bread (NOSTR, n + sizeof (crc) * BYTOBL (n));
		else
			bread (NOSTR, n);
	}
	else
	{
		/*
		 *	Skip normal, com CRC
		 */
		while (n > 0)
		{
			count = MIN (n, BLSZ);
	
			bread (buf, count);
			n -= count;
	
			if (type == GAR)
			{
				bread (crc, sizeof (crc) );
	
				if (atoi (crc) != crc16 ((char *)buf, count) )
					msg ("Erro de crc no bloco %d", bloco);
			}
	
			bloco++;
		}
	}

	/*
	 *	Arredondamento para TAR e CPIO
	 */
	n = kh->k_size;

	if   (type == TAR)
		bread (NOSTR, BLROUND (n) - n);
	elif (type == CPIO || type == CPIOR)
		bread (NOSTR, WDROUND (n) - n);

}	/* end inskip */ 

/*
 ****************************************************************
 *	Imprime uma Linha da Tabela				*
 ****************************************************************
 */
void
tbline (void)
{
	const char	*cp;
	const KHEAD	*kh = &khead;

	/*
	 *	Escreve o tipo do arquivo modo
	 */
	cp = modetostr (kh->k_mode); printf ("%-2.2s", cp);

	/*
	 *	Se for o caso, escreve mais informações
	 */
	if (vflag)
	{
		if (vflag > 1)
		{
			printf ("%s  ",	cp + 2);
			printf ("%5d/%-5d    ", kh->k_uid, kh->k_gid);
		}
		else
		{
			printf ("%s  ",	cp + 14);

			if ((cp = pwcache (kh->k_uid)) != NOSTR)
				printf ("%-14.14s ", cp);
			else
				printf ("%-14d ", kh->k_uid);
		}

		printf ("%-24.24s   ",	btime (&kh->k_mtime)); 
	}

	/*
	 *	Verifica se está comprimido ou não
	 */
	if (kh->k_origsz)			/* Comprimido */
	{
		printf ("%7d   ", kh->k_origsz); 
		print_file_nm (kh->k_name, &kh->k_stat, NOSTR, stdout);

		if (!vflag)
		{
			put_tabs (kh->k_name, stdout);

			printf
			(	"(%s %%, %s%d)",
				editpercent (getpercent (kh->k_origsz - kh->k_size, kh->k_origsz)),
				kh->k_method == 'D' ? "D " : "", kh->k_bits
			);
		}
	}
	else					/* NÃO Comprimido */
	{
		if (S_ISBLK (kh->k_mode) || S_ISCHR (kh->k_mode))
			printf ("  %2d/%-2d   ", MAJOR (kh->k_rdev), MINOR (kh->k_rdev)); 
		else
			printf ("%7d   ", kh->k_size); 

		print_file_nm (kh->k_name, &kh->k_stat, NOSTR, stdout);

		if (S_ISLNK (kh->k_mode) && kh->k_linkflag == ' ')
			printf (" => %s", kh->k_linkname);
	}

	/*
	 *	Imprime o elo físico
	 */
	if (kh->k_linkflag != ' ')
		printf (" (%s)", kh->k_linkname);

	printf ("\n"); 

}	/* end tbline */
