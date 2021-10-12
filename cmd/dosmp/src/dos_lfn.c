/*
 ****************************************************************
 *								*
 *			dos_lfn.c				*
 *								*
 *	Funções auxiliares para os "Long File Names" do W 95	*
 *								*
 *	Versão	3.0.0, de 20.02.96				*
 *		4.5.0, de 02.10.03				*
 *								*
 *	Módulo: DOSMP						*
 *		Utilitários Sofisticados			*
 *		Categoria B					*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 2003 NCE/UFRJ - tecle "man licença"	*
 * 		Inspirado no PLDOS de Norival Ribeiro Figueira	*
 * 								*
 ****************************************************************
 */

#include <sys/types.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>

#include "../h/common.h"
#include "../h/dos.h"
#include "../h/proto.h"

/*
 ******	Variáveis globais ***************************************
 */
#define	LFN_MAX_CHAR	13

const int	dos_lfn_locat_vec[LFN_MAX_CHAR + 2] =
{
	0, 1, 3, 5, 7, 9, 14, 16, 18, 20, 22, 24, 28, 30, 0
};

/*
 ******	Protótipos de funções ***********************************
 */
int	dos_lfn_can_old_format (DOSSTAT *);
int	dos_lfn_can_new_format (DOSSTAT *, char **, int *);
char	dos_lfn_w95_checksum (const char *);

/*
 ****************************************************************
 *	Adiciona uma entrada de LFN ao nome longo		*
 ****************************************************************
 */
void
dos_lfn_add (DOSSTAT *zp)
{
	char		*bp;
	int		i, locat;
	ushort		w;

	/*
	 *	Confere algumas coisas
	 */
    again:
	bp = zp->z_lfn_nm;

	if (bp == zp->z_lfn_area + LFN_SZ - 1)
	{
		/* Primeira entrada do LFN (na realidade o final do nome) */
#define	DEBUG
#ifdef	DEBUG
		if ((zp->z_name[0] & 0x40) == 0)
			printf ("Primeira entrada do LFN sem 0x40!\n");
#endif	DEBUG
		zp->z_lfn_index    = (zp->z_name[0] & ~0x40);
		zp->z_lfn_checksum =  zp->z_name[13];
	}
	else	/* Entradas seguintes do LFN */
	{
		if ((zp->z_name[0] & 0x40) != 0)
		{
#ifdef	DEBUG
			printf ("Entrada seguinte do LFN com 0x40!\n");
#endif	DEBUG
			dos_lfn_reset (zp);
			goto again;
		}

		if (--zp->z_lfn_index != zp->z_name[0])
		{
#ifdef	DEBUG
			printf
			(	"Numeração LFN NÃO confere (%d :: %d)\n",
				zp->z_lfn_index, zp->z_name[0]
			);
#endif	DEBUG
			dos_lfn_reset (zp);
			goto again;
		}

		if (zp->z_lfn_checksum != zp->z_name[13])
		{
#ifdef	DEBUG
			printf
			(	"Checksum LFN NÃO confere (%02X :: %02X)\n",
				zp->z_lfn_checksum, zp->z_name[13]
			);
#endif	DEBUG
			dos_lfn_reset (zp);
			goto again;
		}
	}

	/*
	 *	Agora adiciona os carateres
	 */
	for (i = LFN_MAX_CHAR; (locat = dos_lfn_locat_vec[i]) != 0; i--)
	{
		w = GET_SHORT ((int)&zp->z_d + locat);

		if (w == 0x0000 || w == 0xFFFF)
			continue;

		*--bp = w;
	}

	zp->z_lfn_nm = bp;

}	/* end dos_lfn_add */

/*
 ****************************************************************
 *	Obtém o nome composto					*
 ****************************************************************
 */
void
dos_lfn_get_nm (DOSSTAT *zp)
{
	const char	*cp, *end_cp;
	char		*bp;
	char		checksum;

	/*
	 *	NÃO tem nome longo
	 */
	if (*zp->z_lfn_nm == '\0')
	{
		bp = zp->z_lfn_area;

		if (Z_ISVOL (zp->z_mode))
		{
			end_cp = zp->z_name + sizeof (zp->z_name) + sizeof (zp->z_ext) - 1;

			for (/* acima */; end_cp >= zp->z_name; end_cp--)
			{
				if (*end_cp != '\0' && *end_cp != ' ')
					break;
			}

			for (cp = zp->z_name; cp <= end_cp; /* abaixo */)
				*bp++ = *cp++;
		}
		else
		{
			for (end_cp = zp->z_name + sizeof (zp->z_name) - 1; end_cp >= zp->z_name; end_cp--)
			{
				if (*end_cp != '\0' && *end_cp != ' ')
					break;
			}

			for (cp = zp->z_name; cp <= end_cp; /* abaixo */)
				*bp++ = *cp++;

			for (end_cp = zp->z_ext + sizeof (zp->z_ext) - 1; end_cp >= zp->z_ext; end_cp--)
			{
				if (*end_cp != '\0' && *end_cp != ' ')
					{ *bp++ = '.'; break; }
			}

			for (cp = zp->z_ext; cp <= end_cp; /* abaixo */)
				*bp++ = *cp++;
		}

		*bp = '\0'; zp->z_lfn_nm = zp->z_lfn_area;

		return;
	}

	/*
	 *	TEM nome longo: confere o "checksum"
	 */
	checksum = dos_lfn_w95_checksum (zp->z_name);

	if (zp->z_lfn_checksum != checksum)
	{
#ifdef	DEBUG
		printf
		(	"CHECKSUM LFN NÃO confere (%02X :: %02X): %s\n",
			zp->z_lfn_checksum, checksum, zp->z_lfn_nm
		);
#endif	DEBUG
	}

}	/* end dos_lfn_get_nm */

/*
 ****************************************************************
 *	Cria uma entrada de diretório				*
 ****************************************************************
 */
int
dos_lfn_dir_put (DOSSTAT *zp)
{
	const UNI	*up = &uni;
	const char	*cp;
	int		c, i;
	const DOSDIR	*dp, *end_dp;
	const char	*entry_cp;
	char		*digit_point = NOSTR;
	int		nm_len, entry_no;
	int		old_clusno = 0, clusno;
	int		blkno, end_blkno = 0;
	int		is_lfn;
	int		slot_clusno = 0, slot_blkno = 0, slot_end_blkno = 0;
	int		slot_offset = 0, slot_sz = 0;
	int		entries_needed = 0;
	int		found_slot = 0, found_entry = 0;
	int		found_zero = 0, found_eof = 0;
	int		base_sz = 0, locat;
	char		digit_vec[10];
	DOSSTAT		par_z;
	char		checksum;
	char		area[BLSZ];

	/*
	 *	Na estrutura DOSSTAT "*zp" devem estar definidos:
	 *
	 *	z_lfn_clusno:	No. do cluster do diretório pai
	 *	z_lfn_nm:	Nome do arquivo filho
	 *	z_d:		Dados do filho (exceto o nome)
	 */

	/*
	 *	Inicialmente investiga se é um LFN
	 */
	if (lfn_mode)
		is_lfn = dos_lfn_can_new_format (zp, &digit_point, &base_sz);
	else
		is_lfn = dos_lfn_can_old_format (zp);

	nm_len = strlen (zp->z_lfn_nm);

	if (is_lfn)
	{
		entries_needed += (nm_len + (LFN_MAX_CHAR-1)) / LFN_MAX_CHAR;
		memset (digit_vec, 0, sizeof (digit_vec));
	}

	entries_needed += 1;	/* Não esquecer a entrada tradicional */

	/*
	 *	Percorre o diretório PAI
	 */
	dos_lfn_init (&par_z);

	if ((clusno = zp->z_lfn_clusno) == 0)
		ROOT_FIRST_AND_LAST_BLK (blkno, end_blkno);
	else
		FIRST_AND_LAST_BLK (clusno, blkno, end_blkno);

	dp = bread (blkno, area);
	end_dp = dp + BLSZ / sizeof (DOSDIR);

	for (/* acima */; /* abaixo */; dp++)
	{
		if (dp >= end_dp)
		{
			if (++blkno >= end_blkno)
			{
				if (clusno == 0)	/* RAIZ do DOS */
					{ found_eof++; break; }

				if (C_ISBAD_OR_EOF (clusno = get_fat_value (old_clusno = clusno)))
					{ found_eof++; break; }

				FIRST_AND_LAST_BLK (clusno, blkno, end_blkno);
			}

			dp = bread (blkno, area);
		   /***	end_dp = dp + BLSZ / sizeof (DOSDIR); ***/
		}

		if (dp->d_name[0] == '\0')
			{ found_zero++; break; }

		if (dp->d_name[0] == Z_EMPTY)
		{
			if (!found_slot)
			{
				if (slot_sz++ == 0)
				{
					slot_clusno	= clusno;
					slot_blkno	= blkno;
					slot_end_blkno  = end_blkno;
					slot_offset	= (int)dp - (int)area;
				}

				if (slot_sz >= entries_needed)
					found_slot++;
			}

			dos_lfn_reset (&par_z); continue;
		}

		slot_sz = 0;

		memmove (&par_z.z_d, dp, sizeof (DOSDIR));

		if (Z_ISLFN (dp->d_mode))
			{ dos_lfn_add (&par_z); continue; }

		dos_lfn_get_nm (&par_z);

		if   (streq (zp->z_lfn_nm, par_z.z_lfn_nm))
		{
			found_entry++; break;
		}
		elif (is_lfn && memeq (zp->z_name, par_z.z_name, base_sz))
		{
			if (memeq (zp->z_ext, par_z.z_ext, sizeof (zp->z_ext)))
			{
				c = par_z.z_name[base_sz] - '0';

				if ((unsigned)c < 10)
					digit_vec[c] = 1;
			}
		}

		dos_lfn_reset (&par_z);

	}	/* end for (lendo o diretório) */

	/*
	 *	Prepara o endereço de escrita da entrada do diretório
	 */
	if   (found_entry)	/* Encontrou a entrada com o nome desejado */
	{
	   /***	clusno = ...; ***/

	   /***	blkno = ...; ***/
	   /***	end_blkno = ...; ***/

		zp->z_blkno  = blkno;
		zp->z_offset = (int)dp - (int)area;
	}
	elif (found_slot)	/* Entradas vagas do tamanho necessário */
	{
		clusno = slot_clusno;

	   	blkno	  = slot_blkno;
	   	end_blkno = slot_end_blkno;

		zp->z_blkno  = slot_blkno;
		zp->z_offset = slot_offset;
	}
	elif (found_zero)	/* Encontrou o final "lógico" do diretório */
	{
	   /***	clusno = ...; ***/

	   /***	blkno = ...; ***/
	   /***	end_blkno = ...; ***/

		zp->z_blkno  = blkno;
		zp->z_offset = (int)dp - (int)area;
	}
	elif (found_eof)	/* Encontrou o final "físico" do diretório */
	{
		if (clusno == 0)
			{ errno = ENOSPC; return (-1); }

		if ((clusno = get_new_cluster (old_clusno)) < 0)
			{ errno = ENOSPC; return (-1); }

		cluster_block_clr (clusno);

		FIRST_AND_LAST_BLK (clusno, blkno, end_blkno);

		zp->z_blkno  = blkno;
		zp->z_offset = 0;
	}
	else
	{
		printf ("dos_lfn_dir_put: Nem ENTRY, ZERO, SLOT nem EOF?\n");
		{ errno = ENOSPC; return (-1); }
	}

	/*
	 *	Se já tinha a entrada ou NÃO for LFN,
	 *	escreve a entrada regular e termina
	 */
	if (found_entry || !is_lfn)
		{ dos_dir_write (zp); return (0); }

	/*
	 *	Escolhe o dígito depois do "~"
	 */
	for (i = 1; /* abaixo */; i++)
	{
		if (i >= sizeof (digit_vec))
		{
			printf
			(	"dos_lfn_dir_put: "
				"Não há mais dígitos disponíveis\n"
			);

			{ errno = ENOSPC; return (-1); }
		}

		if (digit_vec[i] == 0)
			{ *digit_point = '0' + i; break; }
	}

	/*
	 *	Prepara os parametros do LFN
	 */
	entry_no = (entries_needed - 1) | 0x40;

	if ((i = nm_len % LFN_MAX_CHAR) == 0)
		i = LFN_MAX_CHAR;

	entry_cp = zp->z_lfn_nm + nm_len - i;

	par_z.z_blkno  = zp->z_blkno;
	par_z.z_offset = zp->z_offset;

	checksum = dos_lfn_w95_checksum (zp->z_name);

	/*
	 *	Escreve as diversas entradas do LFN
	 */
	for (EVER)
	{
		char		end_of_str;

		if (par_z.z_offset >= BLSZ)
		{
			if (++blkno >= end_blkno)
			{
				if (clusno == 0)
					{ errno = ENOSPC; return (-1); }

				if ((clusno = get_new_cluster (clusno)) < 0)
					{ errno = ENOSPC; return (-1); }

				cluster_block_clr (clusno);

				FIRST_AND_LAST_BLK (clusno, blkno, end_blkno);
			}

			par_z.z_blkno  = blkno;
			par_z.z_offset = 0;
		}

		/* Escreve a entrada regular */

		if ((entry_no & ~0x40) <= 0)
		{
			zp->z_blkno  = par_z.z_blkno;
			zp->z_offset = par_z.z_offset;

			dos_dir_write (zp);
			break;
		}

		clr_dir_ent (&par_z.z_d);

		cp = entry_cp; end_of_str = 0;

		for (i = 1; (locat = dos_lfn_locat_vec[i]) != 0; i++)
		{
			if   (end_of_str)
				c = 0xFFFF;
			elif ((c = *cp) == '\0')
				end_of_str++;
			else
				cp++;

			PUT_SHORT (c, (int)&par_z.z_d + locat);
		}

		par_z.z_name[0]  = entry_no;
		par_z.z_name[13] = checksum;
		par_z.z_mode     = Z_LFN;

		dos_dir_write (&par_z);

		if ((entry_no & ~0x40) == 1)
		{
			if ((i = entry_cp - zp->z_lfn_nm) != 0)
			{
				printf
				(	"dos_lfn_dir_put: "
					"Escreveu o número errado de entradas LFN: %d\n",
					i
				);
			}
		}

		par_z.z_offset += sizeof (DOSDIR);

		entry_no &= ~0x40; entry_no--;
		entry_cp -= LFN_MAX_CHAR;

	}	/* end for (pelas várias entradas LFN) */

	return (0);

}	/* end dos_lfn_dir_put */

/*
 ****************************************************************
 *	Cria o nome no formato tradicional (8.3)		*
 ****************************************************************
 */
int
dos_lfn_can_old_format (DOSSTAT *zp)
{
	char		*bp, *end_bp;
	const char	*cp;
	const char	*last_dot;
	int		c;

	memset (zp->z_name, ' ', sizeof (zp->z_name) + sizeof (zp->z_ext));

	bp = zp->z_name; end_bp = bp + sizeof (zp->z_name);

	for (cp = zp->z_lfn_nm; *cp == '.'; cp++)
		/* vazio */;

	last_dot = strrchr (cp, '.');

	for (/* acima */; (c = *cp) != '\0'; cp++)
	{
		if (c == '.')
		{
			if (cp == last_dot)
			   { bp = zp->z_ext; end_bp = bp + sizeof (zp->z_ext); }
		}
		else	/* c != '.' */
		{
			if (bp < end_bp)
				*bp++ = toupper (c);
		}
	}

	return (0);	/* Não é LFN */

}	/* end dos_lfn_can_old_format */

/*
 ****************************************************************
 *	Cria o nome no formato novo (W95)			*
 ****************************************************************
 */
int
dos_lfn_can_new_format (DOSSTAT *zp, char **digit_point, int *base_sz)
{
	char		*bp, *end_bp;
	const char	*cp;
	const char	*last_dot;
	int		c;
	int		is_lfn = 0, dot = 0;

	/*
	 *	Inicialmente investiga se é um LFN
	 */
	memset (zp->z_name, ' ', sizeof (zp->z_name) + sizeof (zp->z_ext));

	bp = zp->z_name; end_bp = bp + sizeof (zp->z_name);

	for (cp = zp->z_lfn_nm; *cp == '.'; cp++)
		/* vazio */;

	last_dot = strrchr (cp, '.');

	for (/* acima */; (c = *cp) != '\0'; cp++)
	{
		if (islower (c))
			is_lfn++;
#if (0)	/*******************************************************/
		if (isupper (c))
			is_lfn++;
#endif	/*******************************************************/

		if (c == '.')
		{
			if (dot++ > 0)
				is_lfn++;

			if (cp == last_dot)
			   { bp = zp->z_ext; end_bp = bp + sizeof (zp->z_ext); }
		}
		else	/* c != '.' */
		{
			if (bp < end_bp)
				*bp++ = toupper (c);
			else
				is_lfn++;
		}
	}

	/*
	 *	Se for LFN, prepara o nome com "~"
	 */
	if (is_lfn)
	{
		bp = zp->z_name; end_bp = bp + sizeof (zp->z_name);

		for (/* acima */; bp < end_bp && *bp != ' '; bp++)
			/* vazio */;

		end_bp -= 2;	/* Espaço para "~1" */

		if (bp > end_bp)
			bp = end_bp;

		*base_sz = bp - zp->z_name + 1;	/* Inclui o "~" */

		*bp++ = '~'; *bp = '1'; *digit_point = bp;
	}

	return (is_lfn);

}	/* end dos_lfn_can_new_format */

/*
 ****************************************************************
 *	Remove uma entrada de diretório				*
 ****************************************************************
 */
int
dos_lfn_dir_clr (DOSSTAT *zp)
{
	const UNI	*up = &uni;
	int		clusno;
	int		end_blkno;
	int		entries_count;
	DOSSTAT		zero_z;

	/*
	 *	Na estrutura DOSSTAT "*zp" devem estar definidos:
	 *
	 *	z_lfn_entries:	 No. de entradas DOSDIR
	 *	z_lfn_clusno:	 No. do cluster do diretório pai
   	 *	z_lfn_blkno:	 No. do bloco inicial
   	 *	z_lfn_end_blkno: No. do bloco final do cluster
	 *	z_lfn_offset:	 Deslocamento dentro do bloco
	 *
	 *	A função "dos_lfn_dir_clr" não altera o conteúdo de "zp"
	 */

#undef	DEBUG
#ifdef	DEBUG
	printf
	(	"dir_clr: entries = %d, clusno = %d\n",
		zp->z_lfn_entries, zp->z_lfn_clusno
	);

	printf
	(	"dir_clr: blkno = %d, end_blkno = %d, offset = %d\n",
		zp->z_lfn_blkno, zp->z_lfn_end_blkno, zp->z_lfn_offset
	);
#endif	DEBUG

	entries_count	= zp->z_lfn_entries; 
	clusno		= zp->z_lfn_clusno;
   	zero_z.z_blkno	= zp->z_lfn_blkno;
   	end_blkno	= zp->z_lfn_end_blkno;
	zero_z.z_offset	= zp->z_lfn_offset;

	clr_dir_ent (&zero_z.z_d); zero_z.z_name[0] = Z_EMPTY;

	/*
	 *	Zera as diversas entradas do diretório
	 */
	for (/* acima */; entries_count > 0; entries_count--)
	{
		if (zero_z.z_offset >= BLSZ)
		{
			if (++zero_z.z_blkno >= end_blkno)
			{
				if (clusno == 0)
					goto not_found;

				if (C_ISBAD_OR_EOF (clusno = get_fat_value (clusno)))
					goto not_found;

				FIRST_AND_LAST_BLK (clusno, zero_z.z_blkno, end_blkno);
			}

			zero_z.z_offset = 0;
		}

		dos_dir_write (&zero_z);

		zero_z.z_offset += sizeof (DOSDIR);

	}	/* end for (pelas várias entradas LFN) */

	return (0);

	/*
	 *	Encontrou inexplicavelmente o final do diretório!
	 */
    not_found:
	printf
	(	"dos_lfn_dir_put: "
		"NÃO encontrei a entrada de remoção!\n"
	);

	errno = ENOENT; return (-1);

}	/* end dos_lfn_dir_clr */

/*
 ****************************************************************
 *	Calcula o "checksum" do Win95				*
 ****************************************************************
 */
char
dos_lfn_w95_checksum (const char *nm)
{
	int		i;
	char		checksum = 0;

	for (i = 11; i > 0; i--)
		checksum = ((checksum << 7) | (checksum >> 1)) + *nm++;

	return (checksum);

}	/* end dos_lfn_w95_checksum */
