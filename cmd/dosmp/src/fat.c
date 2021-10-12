/*
 ****************************************************************
 *								*
 *			fat.c					*
 *								*
 *	Gerência da FAT e clusters				*
 *								*
 *	Versão	3.0.0, de 28.10.93				*
 *		3.0.5, de 14.02.97				*
 *								*
 *	Módulo: DOSMP						*
 *		Utilitários Sofisticados			*
 *		Categoria B					*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 2000 NCE/UFRJ - tecle "man licença"	*
 * 		Inspirado no PLDOS de Norival Ribeiro Figueira	*
 * 								*
 ****************************************************************
 */

#include <sys/types.h>
#include <sys/syscall.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

#include "../h/common.h"
#include "../h/dos.h"
#include "../h/proto.h"

/*
 ****************************************************************
 *	Variáveis globais					*
 ****************************************************************
 */
entry ulong	*global_fat;	/* As entradas da FAT */

entry char	*global_fat_status; /* O vetor de estado das entradas da FAT */

entry char	global_got_whole_fat;	/* Está com a FAT toda na memória */

/*
 ****** Indicadores do estado da FAT ****************************
 */
#define	FAT_PRESENT	0x01	/* Esta entrada da FAT está no vetor */

/*
 ****************************************************************
 *	Aloca memória para a FAT				*
 ****************************************************************
 */
void
alloc_fat (void)
{
	const UNI	*up = &uni;

	/*
	 *	Desaloca a FAT anterior
	 */
	if (global_fat_status != NOVOID)
		free (global_fat_status);

	if (global_fat != NOVOID)
		free (global_fat);

	/*
	 *	Aloca a FAT atual (É bom ter dois de folga ...)
	 */
	if ((global_fat = malloc ((up->u_n_clusters + 4) * sizeof (ulong))) == NOVOID)
	{
		fprintf
		(	stderr,
			"%s: Não consegui alocar memória para a FAT\n",
			pgname
		);
		exit (1);
	}

	if ((global_fat_status = malloc ((up->u_n_clusters + 4) * sizeof (char))) == NOVOID)
	{
		fprintf
		(	stderr,
			"%s: Não consegui alocar memória para a FAT\n",
			pgname
		);
		exit (1);
	}

	/*
	 *	Zera a FAT atual
	 */
	memset (global_fat,	   0, (up->u_n_clusters + 4) * sizeof (ulong));
	memset (global_fat_status, 0, (up->u_n_clusters + 4) * sizeof (char));

	global_got_whole_fat = 0;	/* Ainda NÃO tem a FAT */

}	/* end alloc_fat */

/*
 ****************************************************************
 *	Obtém uma entrada da FAT				*
 ****************************************************************
 */
int
get_fat_value (int old_clusno)
{
	const UNI	*up = &uni;
	unsigned long	l;
	int		offset;
	char		area[4];
	int		value;

	/*
	 *	Pequena consistência
	 */
	if (old_clusno < 0 || old_clusno >= up->u_n_clusters + 2)
	{
		error
		(	"get_fat_value: No. de cluster inválido: %d\n",
			old_clusno
		);
		exit (1);
	}

	/*
	 *	Se já estiver na memória, ...
	 */
	if (global_fat_status[old_clusno])	/* & FAT_PRESENT */
		return (global_fat[old_clusno]);

	/*
	 *	Calcula o deslocamento no arquivo da FAT
	 */
	if   (up->u_fat_bits == 12)
		offset = (old_clusno * 3) >> 1;
	elif (up->u_fat_bits == 16)
		offset = old_clusno << 1;
	else	/* fat_bits == 32 */
		offset = old_clusno << 2;

	/*
	 *	Le a região da FAT do disco
	 */
	lseek (dev_fd, BLSZ * up->u_fat1_blkno + offset, L_SET);

	if ((read (dev_fd, area, sizeof (area))) != sizeof (area))
	{
		fprintf
		(	stderr,
			"%s: Erro de leitura da FAT (%s)\n",
			pgname, strerror (errno)
		);
		exit (1);
	}

	/*
	 *	Retira o conteúdo da FAT
	 */
	if (up->u_fat_bits == 12)
	{
		l = GET_LONG (area);

		if (old_clusno & 1)
			value = (l >> 4) & 0xFFF;
		else
			value =  l	 & 0xFFF;
	}
	elif (up->u_fat_bits == 16)
	{
		value = GET_SHORT (area);
	}
	else	/* fat_bits == 32 */
	{
		value = GET_LONG (area);
	}

	global_fat[old_clusno] = value;
	global_fat_status[old_clusno] = FAT_PRESENT;	/* |= */

	return (value);

}	/* end get_fat_value */

/*
 ****************************************************************
 *	Armazena uma entrada da FAT				*
 ****************************************************************
 */
void
put_fat_value (int value, int clusno)
{
	const UNI	*up = &uni;
	int		offset, size;
	char		area[4];

	/*
	 *	Pequena consistência
	 */
	if (clusno < 0 || clusno >= up->u_n_clusters + 2)
	{
		error
		(	"put_fat_value: No. de cluster inválido: %d\n",
			clusno
		);
		return;
	}

	/*
	 *	Armazena o valor no vetor
	 */
	global_fat[clusno] = value;
	global_fat_status[clusno] = FAT_PRESENT;	/* |= */

	/*
	 *	Insere o conteúdo na FAT
	 */
	if (up->u_fat_bits == 12)
	{
		if (clusno & 1)
		{
			PUT_SHORT (value << 4, area);

			get_fat_value (clusno - 1);
			area[0] |= (global_fat[clusno - 1] >> 8) & 0x0F;
		}
		else
		{
			PUT_SHORT (value, area);

			if (clusno < up->u_n_clusters + 2 - 1)
			{
				get_fat_value (clusno + 1);
				area[1] |= (global_fat[clusno + 1] << 4) & 0xF0;
			}
		}

		size = sizeof (short); offset = (clusno * 3) >> 1;
	}
	elif (up->u_fat_bits == 16)
	{
		PUT_SHORT (value, area);

		size = sizeof (short); offset = clusno << 1;
	}
	else	/* fat_bits == 32 */
	{
		PUT_LONG (value, area);

		size = sizeof (long); offset = clusno << 2;
	}

	/*
	 *	Escreve a entrada na FAT 1 no disco
	 */
	lseek (dev_fd, BLSZ * up->u_fat1_blkno + offset, L_SET);

	if (write (dev_fd, area, size) != size)
	{
		fprintf
		(	stderr,
			"%s: Erro de leitura da FAT (%s)\n",
			pgname, strerror (errno)
		);
		exit (1);
	}

	/*
	 *	Escreve a entrada na FAT 2 no disco
	 */
	if (up->u_n_fats < 2)
		return;

	lseek (dev_fd, BLSZ * up->u_fat2_blkno + offset, L_SET);

	if (write (dev_fd, area, size) != size)
	{
		fprintf
		(	stderr,
			"%s: Erro de leitura da FAT (%s)\n",
			pgname, strerror (errno)
		);
		exit (1);
	}

}	/* end put_fat_value */

/*
 ****************************************************************
 *	Lê a FAT inteira de uma vez do disco			*
 ****************************************************************
 */
void
get_whole_fat (void)
{
	const UNI	*up = &uni;
	const char	*cp, *end_cp;
	ulong		l;
	ulong		*fat;
	const ulong	*end_fat;
	int		sectors;
	char		area[3 * BLSZ];

	/*
	 *	Verifica se já leu a FAT toda
	 */
	if (global_got_whole_fat)
		return;

	/*
	 *	Lê a FAT1 do disco de 3 em 3 setores
	 */
	fat = global_fat; end_fat = fat + up->u_n_clusters + 2;

	end_cp = area + sizeof (area);

	lseek (dev_fd, BLSZ * up->u_fat1_blkno, L_SET);

	for (sectors = up->u_sectors_per_fat; sectors > 0; sectors -= 3)
	{
		if (read (dev_fd, area, sizeof (area)) != sizeof (area))
		{
			fprintf
			(	stderr,
				"%s: Erro de leitura da FAT (%s)\n",
				pgname, strerror (errno)
			);
			exit (1);
		}

		cp = area;

		if (up->u_fat_bits == 12)
		{
			for (/* acima */; fat < end_fat && cp < end_cp; cp += 3)
			{
				l = GET_LONG (cp);

				*fat++ =  l	   & 0xFFF;
				*fat++ = (l >> 12) & 0xFFF;
			}
		}
		elif (up->u_fat_bits == 16)
		{
			for (/* acima */; fat < end_fat && cp < end_cp; cp += 2)
			{
				*fat++ = GET_SHORT (cp);
			}
		}
		else	/* fat_bits == 32 */
		{
#if (0)	/*******************************************************/
			ulong		tst_value = (4 * MBSZ) / CLUSZ;
#endif	/*******************************************************/

			for (/* acima */; fat < end_fat && cp < end_cp; cp += 4)
			{
				l = GET_LONG (cp);

#if (0)	/*******************************************************/
				if (!C_ISBAD_OR_EOF (l))
				{
					if ((l >> KBSHIFT) >= tst_value)
						error ("$Partição com arquivos acima de 4 GB");
				}
#endif	/*******************************************************/

				*fat++ = l;
			}
		}

	}

	/*
	 *	A FAT está toda presente
	 */
	memset (global_fat_status, FAT_PRESENT, (up->u_n_clusters + 2) * sizeof (char));

	global_got_whole_fat++;

}	/* end get_whole_fat */

/*
 ****************************************************************
 *	Escreve a FAT inteira de uma vez no disco		*
 ****************************************************************
 */
void
put_whole_fat (void)
{
	const UNI	*up = &uni;
	char		*cp;
	const char	*end_cp;
	long		l;
	ulong		*fat;
	const ulong	*end_fat;
	int		sectors, size;
	char		area[3 * BLSZ];

	/*
	 *	Escreve a FAT1 do disco de 3 em 3 setores
	 */
	fat = global_fat; end_fat = fat + up->u_n_clusters + 2;

	end_cp = area + sizeof (area);

	lseek (dev_fd, BLSZ * up->u_fat1_blkno, L_SET);

	for (sectors = up->u_sectors_per_fat; sectors > 0; sectors -= 3)
	{
		cp = area; memsetl (cp, 0, sizeof (area) / sizeof (long));

		if (up->u_fat_bits == 12)
		{
			for (/* acima */; fat < end_fat && cp < end_cp; /* abaixo */)
			{
				l =  *fat++; 		*cp++ = l;	l >>= 8;
				l |= *fat++ << 4;	*cp++ = l;	l >>= 8;
							*cp++ = l;
			}
		}
		elif (up->u_fat_bits == 16)
		{
			for (/* acima */; fat < end_fat && cp < end_cp; cp += 2)
			{
				PUT_SHORT (*fat++, cp);
			}
		}
		else	/* fat_bits == 32 */
		{
			for (/* acima */; fat < end_fat && cp < end_cp; cp += 4)
			{
				PUT_LONG (*fat++, cp);
			}
		}

		if (sectors >= 3)
			size = 3 * BLSZ;
		else
			size = sectors * BLSZ;

		if (write (dev_fd, area, size) != size)
		{
			fprintf
			(	stderr,
				"%s: Erro de escrita da FAT (%s)\n",
				pgname, strerror (errno)
			);
			exit (1);
		}
	}

	/*
	 *	Escreve a FAT2
	 */
	if (up->u_n_fats < 2)
		goto got;

	fat = global_fat; /*** end_fat = fat + up->u_n_clusters + 2; ***/

	/*** end_cp = area + sizeof (area); ***/

	lseek (dev_fd, BLSZ * up->u_fat2_blkno, L_SET);

	for (sectors = up->u_sectors_per_fat; sectors > 0; sectors -= 3)
	{
		cp = area; memsetl (cp, 0, sizeof (area) / sizeof (long));

		if (up->u_fat_bits == 12)
		{
			for (/* acima */; fat < end_fat && cp < end_cp; /* abaixo */)
			{
				l =  *fat++; 		*cp++ = l;	l >>= 8;
				l |= *fat++ << 4;	*cp++ = l;	l >>= 8;
							*cp++ = l;
			}
		}
		elif (up->u_fat_bits == 16)
		{
			for (/* acima */; fat < end_fat && cp < end_cp; cp += 2)
			{
				PUT_SHORT (*fat++, cp);
			}
		}
		else	/* fat_bits == 32 */
		{
			for (/* acima */; fat < end_fat && cp < end_cp; cp += 4)
			{
				PUT_LONG (*fat++, cp);
			}
		}

		if (sectors >= 3)
			size = 3 * BLSZ;
		else
			size = sectors * BLSZ;

		if (write (dev_fd, area, size) != size)
		{
			fprintf
			(	stderr,
				"%s: Erro de escrita da FAT (%s)\n",
				pgname, strerror (errno)
			);
			exit (1);
		}
	}

	/*
	 *	A FAT está toda presente
	 */
    got:
	memset (global_fat_status, FAT_PRESENT, (up->u_n_clusters + 2) * sizeof (char));

	global_got_whole_fat++;

}	/* end put_whole_fat */

/*
 ****************************************************************
 *	Obtém um novo cluster					*
 ****************************************************************
 */
int
get_new_cluster (int old_clusno)
{
	UNI		*up = &uni;
	int		index, end_index, low_index;

	/*
	 *	É o primeiro bloco do arquivo
	 */
	end_index = up->u_n_clusters + 2;

	if (old_clusno == 0)
	{
		for (index = 2; index < end_index; index++)
		{
			if (get_fat_value (index) == 0)
			{
				put_fat_value (C_STD_EOF, index);
				up->u_infree--;
				return (index);
			}
		}

		errno = ENOSPC; return (-1);
	}

	/*
	 *	Pequena consistência
	 */
	if (old_clusno < 2 || old_clusno >= up->u_n_clusters + 2)
	{
		error
		(	"get_new_cluster: No. de cluster inválido: %d\n",
			old_clusno
		);
		return (-1);
	}

	/*
	 *	Procura uma entrada vaga perto da anterior
	 */
   /***	end_index = up->u_n_clusters + 2; ***/

	for
	(	low_index = old_clusno - 1,	/* Um abaixo */
		index	  = old_clusno + 1;	/* Um acima */
		/* abaixo */;
		/* abaixo */
	)
	{
		if (index < end_index)
		{
			if (get_fat_value (index) == 0)
			{
				put_fat_value (C_STD_EOF, index);
				put_fat_value (index, old_clusno);
				up->u_infree--;
				return (index);
			}

			index++;
		}

		if (low_index >= 2)
		{
			if (get_fat_value (low_index) == 0)
			{
				put_fat_value (C_STD_EOF, low_index);
				put_fat_value (low_index, old_clusno);
				up->u_infree--;
				return (low_index);
			}

			low_index--;
		}
		else
		{
			if (index >= end_index)
				{ errno = ENOSPC; return (-1); }
		}
	}

}	/* end get_new_cluster */

/*
 ****************************************************************
 *	Remove todos os CLUSTERs de um arquivo			*
 ****************************************************************
 */
void
put_cluster_list (int clusno_head)
{
	UNI		*up = &uni;
	int		clusno = clusno_head, next_clusno;

	if (clusno == 0)
		return;

	for (EVER)
	{
		if (C_ISEOF (clusno))
			return;

		if (clusno < 2 || clusno >= up->u_n_clusters + 2)
		{
			error ("put_cluster_list: CLUSTER inválido: %d", clusno);
			return;
		}

		next_clusno = get_fat_value (clusno);

		put_fat_value (C_FREE, clusno);
		up->u_infree++;

		clusno = next_clusno;

	}	/* end for (EVER) */

}	/* end put_cluster_list */

/*
 ****************************************************************
 *	Obtém clusters contíguos para um arquivo		*
 ****************************************************************
 */
int
alloc_file_clusters (int n_cluster)
{
	UNI		*up = &uni;
	int		index;
	int		n;
	int		first_index, last_index;

	/*
	 *	Em primeiro lugar, procura uma área contígua do tamanho desejado
	 */
	for (index = up->u_n_clusters + 2 - 1; /* abaixo */; index--)
	{
		if (index < 2)
			return (-1);

		if (get_fat_value (index) != 0)
			continue;

		/* Encontrou o final de uma área */

		last_index = index;

		for (n = 0; /* abaixo */; index--)
		{
			if (index < 2)
				return (-1);

			if (get_fat_value (index) != 0)
				break;

			if (++n < n_cluster)
				continue;

			/* Encontrou! */

			first_index = index; goto found;

		}	/* end for (examinando o tamanho da área) */

	}	/* end for (procurando finais de áreas) */

	/*
	 *	Achou a área - agora aloca
	 */
    found:
	index = last_index;
	put_fat_value (C_STD_EOF, index); up->u_infree--;
	n = index;

	for (index--; index >= first_index; index--, n--)
	{
		if (get_fat_value (index) != 0)
		{
			fprintf
			(	stderr,
				"%s: alloc_file_clusters: "
				"alocando CLUSTER ocupado\n",
				pgname
			);
			return (-1);
		}

		put_fat_value (n, index); up->u_infree--;
	}

	return (n);

}	/* end alloc_file_clusters */
