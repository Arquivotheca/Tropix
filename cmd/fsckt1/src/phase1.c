/*
 ****************************************************************
 *								*
 *			phase1.c				*
 *								*
 *	Primeira fase da verificação				*
 *								*
 *	Versão	4.3.0, de 10.07.02				*
 *		4.3.0, de 18.09.02				*
 *								*
 *	Módulo: fsckt1						*
 *		Utilitários Especiais				*
 *		Categoria D					*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 2002 NCE/UFRJ - tecle "man licença"	*
 * 								*
 ****************************************************************
 */

#include <sys/common.h>
#include <sys/sync.h>
#include <sys/stat.h>
#include <sys/t1.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <setjmp.h>

#include "../h/fsckt1.h"

/*
 ******	Definições globais **************************************
 */
T1DINO		zero_ino;	/* Para testar se é igual a ZERO */

/*
 ****************************************************************
 *	Primeira fase						*
 ****************************************************************
 */
int
phase1 (void)
{
	T1SB		*sp = (T1SB *)sb.b_area;
	daddr_t		bno;
	char		*bp;
	T1DINO		*np;
	int		index;

	printf ("*** Fase 1: Verifica os blocos e tamanhos\n");

	/*
	 *	Lê os mapas de bits do disco
	 */
	bp = disk_block_map;

	for (bno = T1_BMNO; bno < sp->s_fs_sz; bno += T1_ZONE_SZ, bp += BL4SZ)
	{
		bread (bp, bno, BL4SZ);

		if (T1_GET_MAP (bp, T1_BMNO) != T1_MAP_MAP)
		{
			printf ("Código inválido do próprio mapa no bloco %d\n", bno);

			T1_PUT_MAP (bp, T1_BMNO, T1_MAP_MAP);
		}

		T1_PUT_MAP (mem_block_map, bno, T1_MAP_MAP);

		fs_busy_bl++;
	}

	T1_PUT_MAP (mem_block_map, T1_BOOTNO, T1_MAP_OUT);
	T1_PUT_MAP (mem_block_map, T1_SBNO,   T1_MAP_OUT);

	fs_busy_bl += 2;

	/*
	 *	Verifica se é o modo de PÂNICO ou não
	 */
	if (pflag)	/* Modo de PÂNICO: Procura blocos de INODEs, ignorando o Mapa de Bits */
	{
		for (bno = T1_BMNO + 1; bno < sp->s_fs_sz; bno++)
		{
			bget (&inoblk, bno);

			for (np = (T1DINO *)inoblk.b_area, index = 0; /* abaixo */; index++, np++)
			{
				if (index >= T1_INOinBL4)
					{ inode_block_scan (bno); break; }

				if (np->n_magic != T1_SBMAGIC)
					break;
			}
		}
	}
	else		/* Modo NORMAL: Consulta o Mapa de Bits para percorrer os blocos de INODEs */
	{
		for (bno = T1_BMNO; bno < sp->s_fs_sz; bno++)
		{
			int	code;

			switch (code = T1_GET_MAP (disk_block_map, bno))
			{
			    case T1_MAP_INODE_EMPTY:
			    case T1_MAP_INODE_FULL:
				break;

			    case T1_MAP_FREE:
			    case T1_MAP_MAP:
			    case T1_MAP_DATA:
			    case T1_MAP_INDIR:
			    case T1_MAP_BAD:
			    case T1_MAP_OUT:
				continue;

			    default:
				printf ("Código inválido %d do bloco %d\n", code, bno);
				continue;
			}

			bget (&inoblk, bno);

			inode_block_scan (bno);

		}	/* end for (percorrendo todos os blocos de INODEs) */
	}

	return (CONTINUE);

}	/* end phase1 */

/*
 ****************************************************************
 *	Examina um bloco de INODE				*
 ****************************************************************
 */
void
inode_block_scan (daddr_t bno)
{
	T1DINO		*np;
	int		index, code;
	ino_t		ino;
	long		mode;
	INO_HASH	*ip;
	int		inode_count = 0;
	off_t		sz, good_sz;
	const char	*str;

	/*
	 *	Examina cada INODE
	 */
	for (np = (T1DINO *)inoblk.b_area, index = 0; index < T1_INOinBL4; index++, np++)
	{
		ino = T1_MAKE_INO (bno, index);

		if (np->n_magic != T1_SBMAGIC)
		{
			print_inode (ino, np);
			printf
			(	"Número mágico do INODE incorreto (%X :: %X). ",
				np->n_magic, T1_SBMAGIC
			);

			if (faskyesno ("Ajusta") > 0)
			{
				np->n_magic = T1_SBMAGIC;

				inoblk.b_dirty++;
			}
		}

		if ((mode = np->n_mode) == 0)
		{
			if
			(	!memeq 
				(	(void *)np + sizeof (np->n_magic),
					(void *)&zero_ino + sizeof (np->n_magic),
					sizeof (T1DINO) - sizeof (np->n_magic)
				)
			)
			{
#define	DETAILS
#ifdef	DETAILS
				if (!memeq (np->n_addr, zero_ino.n_addr, sizeof (np->n_addr)))
					printf ("ENDEREÇOS NÃO zerados\n");

				if (np->n_uid != 0)
					printf ("UID NÃO zerado\n");

				if (np->n_gid != 0)
					printf ("GID NÃO zerado\n");

				if (np->n_nlink != 0)
					printf ("NLINK NÃO zerado\n");

				if (np->n_size != 0)
					printf ("SIZE NÃO zerado\n");

				if (np->n_atime != 0)
					printf ("ATIME NÃO zerado\n");

				if (np->n_mtime != 0)
					printf ("MTIME NÃO zerado\n");

				if (np->n_ctime != 0)
					printf ("CTIME NÃO zerado\n");
#endif	DETAILS
				print_inode (ino, np);
				printf ("INODE não usado & NÃO zerado. ");

				if (faskyesno ("Zera") > 0)
				{
					memsetl (np, 0, sizeof (T1DINO)/sizeof (long));
					np->n_magic = T1_SBMAGIC;

					inoblk.b_dirty++;
				}
			}

			continue;
		}

		inode_count++;

		mode &= S_IFMT;

		/*
		 *	Se for um arquivo de tipo desconhecido, ...
		 */
		switch (mode)
		{
		    case S_IFDIR:
		    case S_IFREG:
		    case S_IFBLK:
		    case S_IFCHR:
		    case S_IFIFO:
		    case S_IFLNK:
			break;

		    default:
			print_inode (ino, np);
			printf ("Tipo de arquivo inválido. ");

			if   (faskyesno ("\tConverte para REGULAR") > 0)
			{
				mode = S_IFREG; np->n_mode = S_IFREG|0755;

				inoblk.b_dirty++;
			}
			elif (faskyesno ("\t\t\t\tRemove") > 0)
			{
				memset (np, 0, sizeof (T1DINO));
				np->n_magic = T1_SBMAGIC;

				inoblk.b_dirty++;

				continue;
			}

			break;
		}

		/*
		 *	É um tipo válido: registra o arquivo
		 */
		fs_busy_ino++;

		if ((ip = malloc (sizeof (INO_HASH))) == NOVOID)
			error (NOSTR);

		if (ino_hash (ino) != NO_INO_HASH)
			printf ("Erro interno: INO duplicado?\n");

		*link_place = ip;

		ip->i_ino	= ino;
		ip->i_state	= (mode == S_IFDIR) ? DIR_STATE : FILE_STATE;
		ip->i_mem_link	= 0;
		ip->i_disk_link	= np->n_nlink;
		ip->i_next	= NO_INO_HASH;

		/*
		 *	Analisa os blocos do arquivo
		 */
		fs_file_block_sz = 0;

		file_block_scan (np, ip, search_bad_or_dup_blocks);

		/*
		 *	Confere o tamanho real do arquivo
		 */
		sz = np->n_size; good_sz = BL4TOBY (fs_file_block_sz); str = "Arquivo";

		if   (mode == S_IFDIR)
		{
			if (sz != good_sz)
				{ str = "Diretório"; goto adjust_question; }
		}
		elif (mode == S_IFREG)
		{
			if (BYTOBL4 (sz) < fs_file_block_sz)
				goto adjust_question;
		}
		elif (mode == S_IFLNK)
		{
			if   (sz < sizeof (np->n_addr))
			{
				good_sz = strlen ((char *)np->n_addr);

				if (sz != good_sz)
					goto adjust_question;
			}
			elif (good_sz != BL4SZ || sz >= BL4SZ)
			{
				print_inode (ip->i_ino, np);
				printf ("Elo simbólico com tamanhos (%d, %d) inválidos\n\n", sz, good_sz);

				ip->i_state = CLR_STATE;
			}
		}
		elif (sz != 0)	/* Especiais de bloco/caracteres */
		{
			good_sz = 0;

		    adjust_question:
			print_inode (ino, np);
			printf ("%s com tamanho inválido (%d :: %d). ", str, sz, good_sz);

			if (faskyesno ("Ajusta") > 0)
			{
				np->n_size = good_sz;

				inoblk.b_dirty++;
			}
		}

	}	/* end (for (INODEs deste bloco) */

	/*
	 *	Armazena o código correto deste bloco de INODEs
	 */
	code = (inode_count >= T1_INOinBL4) ? T1_MAP_INODE_FULL : T1_MAP_INODE_EMPTY;

	T1_PUT_MAP (mem_block_map, bno, code);

	fs_busy_bl++;

}	/* end inode_block_scan */

/*
 ****************************************************************
 *	Executa uma função para cada bloco de um arquivo	*
 ****************************************************************
 */
int
file_block_scan (T1DINO *np, INO_HASH *ip, int (*func) (T1DINO *, INO_HASH *, daddr_t, int))
{
	daddr_t		*bp, bno;
	int		level, ret;

	/*
	 *	Analisa o tipo do INODE
	 */
	switch (np->n_mode & S_IFMT)
	{
	    case S_IFREG:
	    case S_IFDIR:
		break;

	    case S_IFLNK:
		if (np->n_size < sizeof (np->n_addr))
			return (CONTINUE);

		break;

	    default:
		return (CONTINUE);
	}

	/*
	 *	Chama a função para os blocos diretos
	 */
	for (bp = &np->n_addr[0]; bp < &np->n_addr[T1_NADDR - 3]; bp++)
	{
		if ((bno = *bp) == NODADDR)
			continue;

		if ((ret = (*func) (np, ip, bno, T1_MAP_DATA)) & STOP)
			return (ret);
	}

	/*
	 *	Chama a função para os blocos indiretos
	 */
	for (level = 1; level <= 3; level++, bp++)
	{
		if ((bno = *bp) == NODADDR)
			continue;

		if ((ret = (*func) (np, ip, bno, T1_MAP_INDIR)) & STOP)
			return (ret);

		if ((ret = file_ind_block_scan (np, ip, bno, level, func)) & STOP)
			return (ret);
	}

	return (CONTINUE);

}	/* end file_block_scan */

/*
 ****************************************************************
 *	Executa a função para os bloco indiretos		*
 ****************************************************************
 */
int
file_ind_block_scan
(
	T1DINO		*np,
	INO_HASH	*ip,
	daddr_t		indir_bno,
	int		level,
	int		(*func) (T1DINO *, INO_HASH *, daddr_t, int)
)
{
	daddr_t		*bp, bno;
	int		ret, type;
	daddr_t		iblk[T1_INDIR_SZ];

	/*
	 *	Confere o número do bloco
	 */
	if (indir_bno <= T1_BMNO || indir_bno >= ((T1SB *)sb.b_area)->s_fs_sz)
		return (SKIP);

	bread (iblk, indir_bno, BL4SZ);

	level--; type = (level <= 0) ? T1_MAP_DATA : T1_MAP_INDIR;

	/*
	 *	Processa cada endereço do bloco de endereços indiretos
	 */
	for (bp = &iblk[0]; bp < &iblk[T1_INDIR_SZ]; bp++)
	{
		if ((bno = *bp) == NODADDR)
			continue;

		if ((ret = (*func) (np, ip, bno, type)) & STOP)
			return (ret);

		if (level <= 0)
			continue;

		if ((ret = file_ind_block_scan (np, ip, bno, level, func)) & STOP)
			return (ret);
	}

	return (CONTINUE);

}	/* end file_ind_block_scan */

/*
 ****************************************************************
 *	Analisa cuidadosamente um bloco de um arquivo		*
 ****************************************************************
 */
int
search_bad_or_dup_blocks (T1DINO *np, INO_HASH *ip, daddr_t bno, int type)
{
	/*
	 *	Verifica se o número do bloco é válido
	 */
	if (bno <= T1_BMNO || bno >= ((T1SB *)sb.b_area)->s_fs_sz)
	{
		print_inode (ip->i_ino, np);
		printf ("Arquivo com bloco %d fora dos limites\n\n", bno);

		ip->i_state = CLR_STATE;
		return (CONTINUE);
	}

	/*
	 *	O arquivo tem mais um bloco
	 */
	if (type == T1_MAP_DATA)
		fs_file_block_sz++;

	/*
	 *	Verifica se o bloco é duplicado
	 */
	if (T1_GET_MAP (mem_block_map, bno) != T1_MAP_FREE)
	{
		print_inode (ip->i_ino, np);
		printf ("Arquivo com bloco %d duplicado\n\n", bno);

		put_in_dup_block_tb (bno);
		return (CONTINUE);
	}

	/*
	 *	Chegamos aos blocos legítimos
	 */
	T1_PUT_MAP (mem_block_map, bno, type);

	fs_busy_bl++;

	return (CONTINUE);

}	/* end search_bad_or_dup_blocks */

/*
 ****************************************************************
 *	Insere um bloco na tabela de blocos duplicados		*
 ****************************************************************
 */
void
put_in_dup_block_tb (daddr_t bno)
{
	DUPBLK		*dup_p;
	int		i;

	/*
	 *	Em primeiro lugar, verifica se o bloco já está na tabela
	 */
	for (dup_p = dup_block_tb, i = 0; i < dup_block_sz; dup_p++, i++)
	{
		if (dup_p->dup_bno == bno)
			{ dup_p->dup_n++; return; }
	}

	/*
	 *	Ainda não está
	 */
	if (++dup_block_sz > dup_block_elem)
	{
		dup_block_elem += 32;

		if ((dup_block_tb = realloc (dup_block_tb, dup_block_elem * sizeof (DUPBLK))) == NODUPBLK)
			error (NOSTR);
	}

	dup_p = &dup_block_tb[dup_block_sz - 1];

	dup_p->dup_bno	= bno;
	dup_p->dup_n	= 1;

}	/* put_in_dup_block_tb */
