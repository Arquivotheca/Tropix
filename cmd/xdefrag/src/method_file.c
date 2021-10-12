/*
 ****************************************************************
 *								*
 *			method_file.c				*
 *								*
 *	Examina e desfragmenta o sistema de arquivos		*
 *								*
 *	Versão	4.3.0, de 25.10.02				*
 *		4.5.0, de 02.11.03				*
 *								*
 *	Módulo: xdefrag						*
 *		Utilitários Básicos				*
 *		Categoria B					*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 2003 NCE/UFRJ - tecle "man licença"	*
 * 								*
 ****************************************************************
 */

#include <sys/common.h>
#include <sys/syscall.h>
#include <sys/disktb.h>
#include <sys/t1.h>

#include <stdio.h>
#include <string.h>

#include "../h/xdefrag.h"

/*
 ******	Seleciona blocos que podem ser pulados ******************
 */
#define	BLOCK_to_SKIP(bit_map, bno, type)			\
								\
	((type = T1_GET_MAP (bit_map, bno)) == T1_MAP_MAP ||	\
	type == T1_MAP_INODE_EMPTY ||				\
	type == T1_MAP_INODE_FULL ||				\
	type == T1_MAP_BAD)

#define	TYPE_to_SKIP(type)					\
								\
	(type == T1_MAP_MAP ||					\
	type  == T1_MAP_INODE_EMPTY ||				\
	type  == T1_MAP_INODE_FULL ||				\
	type  == T1_MAP_BAD)

/*
 ******	Variáveis globais ***************************************
 */

/*
 ******	Protótipos de funções ***********************************
 */
int		method_hole_compact (void);
void		put_blocks_in_bitmap (const INOVEC *ip);

/*
 ****************************************************************
 *	Calcula os endereços desejados: compacta os arquivos	*
 ****************************************************************
 */
int
method_file_compact (void)
{
	daddr_t		bno;
	INOVEC		*ip;
	DOUBLE		*ap;
	const DOUBLE	*end_ap;
	int		i, type;
	char		*bp;

	/*
	 *	Verifica se cada arquivo está bem alocado
	 */
	for (ip = inovec; ip < end_inovec; ip++)
	{
		bno = NODADDR; ap = ip->i_double; end_ap = ap + ip->i_virtual_blkcnt;

		for (/* acima */; /* abaixo */; /* abaixo */)
		{
			if   (ap >= end_ap)
			{
				ip->i_compact = INO_COMPACT; compact_inodes++; break;
			}
			elif (ap->a_old_bno == NODADDR)
			{
				ap->a_new_bno = NODADDR; ap++;
			}
			elif (bno == NODADDR)
			{
				ap->a_new_bno = bno = ap->a_old_bno; bno++; ap++;
			}
			elif (ap->a_old_bno == bno)	/* Por enquanto está compacto */
			{
				ap->a_new_bno = bno++; ap++;
			}
			elif (BLOCK_to_SKIP (old_bit_map, bno, type))
			{
				bno++;			/* Pulando outros blocos */
			}
			else				/* Não está compacto */
			{
				ip->i_compact = INO_FRAGMENTED; fragmented_inodes++; break;
			}

		}	/* for (Blocos de um INODE) */

	}	/* end for (INODEs) */

	/*
	 *	Copia o mapa de BITs original
	 */
	memmove (bp = new_bit_map, old_bit_map, MAP_SZ (fs_sz));

	/*
	 *	Em primeiro lugar, remove os arquivos mal alocados
	 */
	for (ip = inovec; ip < end_inovec; ip++)
	{
		if (ip->i_compact != INO_FRAGMENTED)
			continue;

		ap = ip->i_double; end_ap = ap + ip->i_virtual_blkcnt;

		for (/* acima */; ap < end_ap; ap++)
		{
			if (ap->a_old_bno != NODADDR)
				T1_PUT_MAP (bp, ap->a_old_bno, T1_MAP_FREE);
		}
	}

	/*
	 *	Em segundo lugar, procura espaço contíguo para estes arquivos mal alocados
	 */
	for (ip = inovec; ip < end_inovec; ip++)
	{
		if (ip->i_compact != INO_FRAGMENTED)
			continue;

		if   ((bno = ip->i_double->a_old_bno) == NODADDR)
			bno = T1_ROOTNO;
		elif ((bno -= (ip->i_phys_blkcnt >> 1)) < T1_ROOTNO)
			bno = T1_ROOTNO;

		/* Procura os blocos contíguos */

		ap = ip->i_double; end_ap = ap + ip->i_virtual_blkcnt;

		for (i = 0; ap < end_ap; /* abaixo */)
		{
			if   (ap->a_old_bno == NODADDR)
			{
				ap->a_new_bno = NODADDR; ap++;
			}
			elif (bno >= fs_sz)
			{
				if (i++ > 0)
					{ xerror ("Método falhou"); return (-1); }

				bno = T1_ROOTNO; ap = ip->i_double;
			}
			elif ((type = T1_GET_MAP (bp, bno)) == T1_MAP_FREE)	/* Encontrou um bloco */
			{
				ap->a_new_bno = bno++; ap++;
			}
			elif (TYPE_to_SKIP (type)) /* Pula */
			{
				bno++;
			}
			else							/* Recomeça */
			{
				ap = ip->i_double; bno++;
			}
		}

		/* Registra os blocos no mapa de bits */

		put_blocks_in_bitmap (ip);

	   /***	ip->i_compact = INO_FRAGMENTED; ***/

	}	/* end for (INODEs) */

	/*
	 *	Verifica se deve elimitar as lacunas
	 */
	if (method_in_use == METHOD_FILE_AND_HOLE)
		return (method_hole_compact ());


	return (0);

}	/* end method_file_compact */

/*
 ****************************************************************
 *	Tenta eliminar as lacunas				*
 ****************************************************************
 */
int
method_hole_compact (void)
{
	daddr_t		begin_bno, bno;
	int		hole_size, max_size, type;
	char		*bp = new_bit_map;
	INOVEC		*ip, *max_ip;
	DOUBLE		*ap;
	const DOUBLE	*end_ap;

#if (0)	/*******************************************************/
#define	MIN_SZ	64	/* 256 KB */
#define	MIN_SZ	16	/* 64 KB */
#define	MIN_SZ	4	/* 16 KB */
#endif	/*******************************************************/

#define	MIN_SZ	8	/* 32 KB */

	/*
	 *	Procura a primeira lacuna de MIN_SZ blocos
	 */
	bno = T1_ROOTNO + 1;
    again:
	hole_size = 0; begin_bno = 0;

	for (/* acima */; /* abaixo */; bno++)
	{
		if (bno >= fs_sz)
			return (0);

		if   ((type = T1_GET_MAP (bp, bno)) == T1_MAP_FREE)	/* Encontrou um bloco */
		{
			if (begin_bno == 0)
				begin_bno = bno;

			hole_size++;
		}
		elif (TYPE_to_SKIP (type)) /* Pula */
		{
			/* vazio */;
		}
		else							/* Bloco ocupado */
		{
			if (hole_size >= MIN_SZ)	/* Encontrou */
				break;

			hole_size = 0; begin_bno = 0;
		}
	}

#undef	HOLE
#ifdef	HOLE
	printf ("method_hole_compact: Achei lacuna de %d blocos a partir de %d\n", hole_size, begin_bno);
#endif	HOLE

	/*
	 *	A partir deste ponto, procura o arquivo de tamanho máximo, mas de no mínimo MIN_SZ blocos
	 */
	max_size = 0; max_ip = NOINOVEC;

	for (ip = inovec; ip < end_inovec; ip++)
	{
		if (ip->i_phys_blkcnt < MIN_SZ)
			continue;

		if (ip->i_phys_blkcnt > hole_size)
			continue;

		ap = ip->i_double; end_ap = ap + ip->i_virtual_blkcnt;

		for (/* acima */; ap < end_ap; ap++)
		{
			if (ap->a_new_bno != 0)
				break;
		}

		if (ap >= end_ap)
			continue;

#ifdef	HOLE
		printf
		(	"method_hole_compact: Candidato (%d , %d), (buraco = %d, %d)\n",
			ip->i_phys_blkcnt, ap->a_new_bno, hole_size, begin_bno
		);
#endif	HOLE

		if (ap->a_new_bno <= begin_bno)
			continue;

		if (max_size < ip->i_phys_blkcnt)
			{ max_ip = ip;  max_size = ip->i_phys_blkcnt; }
	}

	if (max_ip == NOINOVEC)
		{ /*** bno = bno; ***/ goto again; }

#ifdef	HOLE
	printf ("method_hole_compact: Achei arquivo de %d blocos\n", max_size);
#endif	HOLE

	/*
	 *	Encontrou um arquivo próprio para a relocação
	 */
	ip = max_ip;

	ap = ip->i_double; end_ap = ap + ip->i_virtual_blkcnt;

	for (/* acima */; ap < end_ap; ap++)
	{
		if (ap->a_new_bno != NODADDR)
			T1_PUT_MAP (bp, ap->a_new_bno, T1_MAP_FREE);
	}

	bno = begin_bno;

	ap = ip->i_double; /*** end_ap = ap + ip->i_virtual_blkcnt; ***/

	for (/* acima */; ap < end_ap; /* abaixo */)
	{
		if   (ap->a_old_bno == NODADDR)
		{
			ap->a_new_bno = NODADDR; ap++;
		}
		elif (bno >= fs_sz)
		{
			{ xerror ("Método falhou"); return (-1); }
		}
		elif ((type = T1_GET_MAP (bp, bno)) == T1_MAP_FREE)	/* Encontrou um bloco */
		{
			ap->a_new_bno = bno++; ap++;
		}
		elif (TYPE_to_SKIP (type)) /* Pula */
		{
			bno++;
		}
		else							/* Recomeça */
		{
			{ xerror ("Método falhou"); return (-1); }
		}
	}

	/* Registra os blocos no mapa de bits */

	put_blocks_in_bitmap (ip);

	if (ip->i_compact != INO_FRAGMENTED)
		{ ip->i_compact = INO_FRAGMENTED; fragmented_inodes++; }

   /***	bno = bno; ***/

	goto again;

}	/* end method_hole_compact */

/*
 ****************************************************************
 *	Atualiza o mapa de bits					*
 ****************************************************************
 */
void
put_blocks_in_bitmap (const INOVEC *ip)
{
	DOUBLE		*ap = ip->i_double;
	const DOUBLE	*end_ap = ap + ip->i_virtual_blkcnt;
	int		index0, index1, index2;
	char		*bp = new_bit_map;

	/*
	 *	Prepara os blocos diretos
	 */
	for (index0 = 0; index0 < T1_NADDR - 3; index0++, ap++)
	{
		if (ap >= end_ap)
			return;

		if (ap->a_new_bno != NODADDR)
			T1_PUT_MAP (bp, ap->a_new_bno, T1_MAP_DATA);
	}

	if (ap >= end_ap)
		return;

	/*
	 *	Prepara os blocos indiretos de grau 1
	 */
	if (ap->a_new_bno != NODADDR)
	{
		T1_PUT_MAP (bp, ap->a_new_bno, T1_MAP_INDIR); ap++;

		for (index1 = 0; index1 < T1_INDIR_SZ; index1++, ap++)
		{
			if (ap >= end_ap)
				return;

			if (ap->a_new_bno != NODADDR)
				T1_PUT_MAP (bp, ap->a_new_bno, T1_MAP_DATA);
		}
	}
	else
	{
		ap += 1 + T1_INDIR_SZ;
	}

	if (ap >= end_ap)
		return;

	/*
	 *	Prepara os blocos indiretos de grau 2
	 */
	if (ap->a_new_bno != NODADDR)
	{
		T1_PUT_MAP (bp, ap->a_new_bno, T1_MAP_INDIR); ap++;

		if (ap >= end_ap)
			return;

		for (index2 = 0; index2 < T1_INDIR_SZ; index2++)
		{
			if (ap->a_new_bno != NODADDR)
			{
				T1_PUT_MAP (bp, ap->a_new_bno, T1_MAP_INDIR); ap++;

				for (index1 = 0; index1 < T1_INDIR_SZ; index1++, ap++)
				{
					if (ap >= end_ap)
						return;

					if (ap->a_new_bno != NODADDR)
						T1_PUT_MAP (bp, ap->a_new_bno, T1_MAP_DATA);
				}
			}
			else
			{
				ap += 1 + T1_INDIR_SZ;
			}
		}

		if (ap >= end_ap)
			return;
	}
	else
	{
		ap += 1 + T1_INDIR_SZ * (1 + T1_INDIR_SZ);
	}

   /***	if (ap >= end_ap) ***/
	   /***	return; ***/

	/*
	 *	Prepara os blocos indiretos de grau 3 (A FAZER)
	 */

}	/* end put_blocks_in_bitmap */

/*
 ****************************************************************
 *	Atualiza todos os INODEs (NÃO foram movidos)		*
 ****************************************************************
 */
void
update_all_inodes (void)
{
	INOVEC		*ip;

	/*
	 *	Atualiza todos os INODEs
	 */
	for (ip = inovec; ip < end_inovec; ip++)
	{
		if (ip->i_compact != INO_FRAGMENTED)
			continue;

		if (update_inode_addr (ip->i_ino, ip) < 0)
			return;

		percent_action_done++;

		draw_percent_percent
		(	percent_action_done,
			percent_action_block + percent_action_inode + percent_action_dir
		);

		do_XFlush ();
	}

	bflush (&inode_cache);

}	/* end update_all_inodes */
