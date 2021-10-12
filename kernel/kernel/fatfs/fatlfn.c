/*
 ****************************************************************
 *								*
 *			fatlfn.c				*
 *								*
 *	Tratamento dos nomes longos (padrão Win95)		*
 *								*
 *	Versão	4.2.0, de 26.12.01				*
 *		4.9.0, de 28.08.07				*
 *								*
 *	Módulo: Núcleo						*
 *		NÚCLEO do TROPIX para PC			*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 2007 NCE/UFRJ - tecle "man licença"	*
 * 								*
 ****************************************************************
 */

#include "../h/common.h"
#include "../h/sync.h"
#include "../h/scb.h"
#include "../h/region.h"

#include "../h/mntent.h"
#include "../h/sb.h"
#include "../h/kfile.h"
#include "../h/fat.h"
#include "../h/sysdirent.h"
#include "../h/inode.h"
#include "../h/stat.h"
#include "../h/bhead.h"
#include "../h/uerror.h"
#include "../h/signal.h"
#include "../h/uproc.h"

#include "../h/extern.h"
#include "../h/proto.h"

/*
 ******	Variáveis globais ***************************************
 */
#define	LFN_MAX_CHAR	13

#define IS_BIT(vector, index)	(vector[index >> 3] &  (1 << (index & 7)))
#define SET_BIT(vector, index)	(vector[index >> 3] |= (1 << (index & 7)))

const int	fat_lfn_locat_vec[LFN_MAX_CHAR + 2] =
{
	0, 1, 3, 5, 7, 9, 14, 16, 18, 20, 22, 24, 28, 30, 0
};

/*
 ******	Protótipos de funções ***********************************
 */
int	fat_lfn_can_format (const DIRENT *dep, FATDIR *dp, char **digit_point, int *base_sz);
void	fat_8_3_can_format (const DIRENT *dep, FATDIR *dp);
void	fat_dir_write (const FATDIR *, clusno_t clusno, daddr_t blkno, daddr_t end_blkno, off_t offset);
char	fat_lfn_w95_checksum (const char *);

/*
 ****************************************************************
 *	Adiciona uma entrada de LFN ao nome longo		*
 ****************************************************************
 */
void
fat_lfn_add (const FATDIR *dp, FATLFN *zp)
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
		if ((dp->d_name[0] & 0x40) == 0)
			printf ("Primeira entrada do LFN sem 0x40!\n");
#endif	DEBUG
		zp->z_lfn_index    = (dp->d_name[0] & ~0x40);
		zp->z_lfn_checksum =  dp->d_name[13];
	}
	else	/* Entradas seguintes do LFN */
	{
		if ((dp->d_name[0] & 0x40) != 0)
		{
#ifdef	DEBUG
			printf ("Entrada seguinte do LFN com 0x40!\n");
#endif	DEBUG
			FAT_LFN_RESET (zp);
			goto again;
		}

		if (--zp->z_lfn_index != dp->d_name[0])
		{
#ifdef	DEBUG
			printf
			(	"Numeração LFN NÃO confere (%d :: %d)\n",
				zp->z_lfn_index, dp->d_name[0]
			);
#endif	DEBUG
			FAT_LFN_RESET (zp);
			goto again;
		}

		if (zp->z_lfn_checksum != dp->d_name[13])
		{
#ifdef	DEBUG
			printf
			(	"Checksum LFN NÃO confere (%02X :: %02X)\n",
				zp->z_lfn_checksum, dp->d_name[13]
			);
#endif	DEBUG
			FAT_LFN_RESET (zp);
			goto again;
		}
	}

	/*
	 *	Agora adiciona os carateres
	 */
	for (i = LFN_MAX_CHAR; (locat = fat_lfn_locat_vec[i]) != 0; i--)
	{
		w = FAT_GET_SHORT ((int)dp + locat);

		if (w == 0x0000 || w == 0xFFFF)
			continue;

		*--bp = w;
	}

	zp->z_lfn_nm = bp;

}	/* end fat_lfn_add */

/*
 ****************************************************************
 *	Obtém o nome composto					*
 ****************************************************************
 */
void
fat_lfn_get_nm (const SB *unisp, const FATDIR *dp, FATLFN *zp)
{
	const char	*cp, *end_cp;
	char		*bp;
	char		checksum;

	/*
	 *	NÃO tem nome longo
	 */
	if (*zp->z_lfn_nm == '\0' || (unisp->sb_mntent.me_flags & SB_LFN) == 0)
	{
		bp = zp->z_lfn_area;

		if (FAT_ISVOL (dp->d_mode))
		{
			end_cp = dp->d_name + sizeof (dp->d_name) + sizeof (dp->d_ext) - 1;

			for (/* acima */; end_cp >= dp->d_name; end_cp--)
			{
				if (*end_cp != '\0' && *end_cp != ' ')
					break;
			}

			for (cp = dp->d_name; cp <= end_cp; /* abaixo */)
				*bp++ = *cp++;
		}
		else
		{
			for (end_cp = dp->d_name + sizeof (dp->d_name) - 1; end_cp >= dp->d_name; end_cp--)
			{
				if (*end_cp != '\0' && *end_cp != ' ')
					break;
			}

			for (cp = dp->d_name; cp <= end_cp; /* abaixo */)
				*bp++ = *cp++;

			for (end_cp = dp->d_ext + sizeof (dp->d_ext) - 1; end_cp >= dp->d_ext; end_cp--)
			{
				if (*end_cp != '\0' && *end_cp != ' ')
					{ *bp++ = '.'; break; }
			}

			for (cp = dp->d_ext; cp <= end_cp; /* abaixo */)
				*bp++ = *cp++;
		}

		*bp = '\0'; zp->z_lfn_nm = zp->z_lfn_area;

		return;
	}

	/*
	 *	TEM nome longo: confere o "checksum"
	 */
	checksum = fat_lfn_w95_checksum (dp->d_name);

	if (zp->z_lfn_checksum != checksum)
	{
#ifdef	DEBUG
		printf
		(	"CHECKSUM LFN NÃO confere (%02X :: %02X): %s\n",
			zp->z_lfn_checksum, checksum, zp->z_lfn_nm
		);
#endif	DEBUG
	}

}	/* end fat_lfn_get_nm */

/*
 ****************************************************************
 *	Cria uma entrada de diretório				*
 ****************************************************************
 */
ino_t
fat_lfn_dir_put (INODE *ip, const DIRENT *dep)
{
	FATSB		*sp = ip->i_sb->sb_ptr;
	BHEAD		*bp = NOBHEAD;
	FATDIR		*dp;
	const FATDIR	*end_dp;
	char		is_lfn;
	char		*digit_point = NOSTR;
	int		base_sz = 0;
	int		entries_needed = 1;	/* A entrada convencional */
	char		digit_vec[16];		/* Para 100 números */
	clusno_t	clusno, log_clusno;
	int		blkno, end_blkno;
	char		found_zero = 0, found_eof = 0, found_slot = 0;
	int		slot_clusno = 0, slot_blkno = 0, slot_end_blkno = 0;
	off_t		slot_offset = 0, offset = 0;
	int		slot_sz = 0;
	int		i, entry_no;
	const char	*entry_cp;
	const char	*cp;
	char		checksum;
	int		locat;
	FATDIR		d, lfn_d;

	/*
	 *	ip:		Dados do diretório pai
	 *	dep:		Nome do arquivo filho
	 */

	/*
	 *	Inicialmente investiga se é um LFN
	 */
	memclr (&d, sizeof (FATDIR));

	FAT_PUT_CLUSTER (C_STD_EOF, &d);

	if ((ip->i_sb->sb_mntent.me_flags & SB_LFN) == 0)
		{ is_lfn = 0; fat_8_3_can_format (dep, &d); }
	else
		is_lfn = fat_lfn_can_format (dep, &d, &digit_point, &base_sz);

	if (is_lfn)
	{
		entries_needed += (dep->d_namlen + (LFN_MAX_CHAR - 1)) / LFN_MAX_CHAR;
		memclr (digit_vec, sizeof (digit_vec));
	}

	/*
	 *	Obtém os blocos iniciais
	 */
	log_clusno = 0;

	if ((clusno = ip->i_first_clusno) == 0)
		ROOT_FIRST_AND_LAST_BLK (blkno, end_blkno);
	else
		FIRST_AND_LAST_BLK (clusno, blkno, end_blkno);

	blkno -= 1;	/* É incrementado antes da primeira leitura */

	/*
	 *	Malha principal
	 */
	for (end_dp = dp = NOFATDIR; /* abaixo */; dp++)
	{
		if (dp >= end_dp)
		{
			/* Lê um bloco de diretório */

			if (bp != NOBHEAD)
				{ block_put (bp); bp = NOBHEAD; }

			if (++blkno >= end_blkno)
			{
				if (clusno == 0)		/* RAIZ */
					{ found_eof++; break; }

				clusno = fat_cluster_read_map (ip, ++log_clusno);

				if (C_ISBAD_OR_EOF (clusno))
					{ found_eof++; break; }

				FIRST_AND_LAST_BLK (clusno, blkno, end_blkno);
			}

			if (blkno < end_blkno - 1)
				bp = breada (ip->i_dev, blkno, blkno + 1, 0);
			else
				bp = bread (ip->i_dev, blkno, 0);

			dp = bp->b_addr; end_dp = (FATDIR *)((int)dp + BLSZ);
		}

		if (dp->d_name[0] == '\0')		/* Final do diretório */
			{ offset = (int)dp - (int)bp->b_addr; found_zero++; break; }

		if (dp->d_name[0] == FAT_EMPTY)		/* Entrada vazia */
		{
			if (!found_slot)
			{
				if (slot_sz++ == 0)
				{
					slot_clusno	= clusno;
					slot_blkno	= blkno;
					slot_end_blkno  = end_blkno;
					slot_offset	= (int)dp - (int)bp->b_addr;
				}

				if (slot_sz >= entries_needed)
					found_slot++;
			}

			continue;
		}

		slot_sz = 0;

		if (FAT_ISLFN (dp->d_mode))
			continue;

		/*
		 *	Coleta os nomes DOS da forma "...~1" e "...~11"
		 */
		if (is_lfn && memeq (dp->d_ext, d.d_ext, sizeof (d.d_ext)))
		{
			if   (dp->d_name[base_sz - 1] == '~' && memeq (dp->d_name, d.d_name, base_sz - 1))
			{
				int		index;

				index = dp->d_name[base_sz] - '0';

				if ((unsigned)index < 10)
					SET_BIT (digit_vec, index);
			}
			elif (dp->d_name[base_sz - 2] == '~' && memeq (dp->d_name, d.d_name, base_sz - 2))
			{
				int		digit, index;

				digit = dp->d_name[base_sz - 1] - '0';

				if (digit <= 0 || digit >= 10)
					continue;

				index = digit * 10;

				digit = dp->d_name[base_sz] - '0';

				if ((unsigned)digit >= 10)
					continue;

				index += digit;

				SET_BIT (digit_vec, index);
			}
		}

	}	/* end for (dp = ...) */

	if (bp != NOBHEAD)
		{ block_put (bp); bp = NOBHEAD; }

	/*
	 *	Prepara o endereço de escrita da entrada do diretório
	 */
	if   (found_slot)	/* Entradas vagas do tamanho necessário */
	{
		clusno		= slot_clusno;

	   	blkno		= slot_blkno;
	   	end_blkno	= slot_end_blkno;

		offset		= slot_offset;
	}
	elif (found_zero)	/* Encontrou o final "lógico" do diretório */
	{
	   /***	clusno		= ...; ***/

	   /***	blkno		= ...; ***/
	   /***	end_blkno	= ...; ***/

	   /***	offset		= (int)dp - (int)bp->b_addr;	***/
	}
	elif (found_eof)	/* Encontrou o final "físico" do diretório */
	{
		if (clusno == 0)
			{ u.u_error = ENOSPC; return (-1); }

		/* Repare que "log_clusno" já foi incrementado */

		if ((clusno = fat_cluster_write_map (ip, log_clusno)) < 0)
			{ u.u_error = ENOSPC; return (-1); }

		FIRST_AND_LAST_BLK (clusno, blkno, end_blkno);

		offset		= 0;
	}
	else
	{
		printf ("fat_lfn_dir_put: Nem ZERO, SLOT nem EOF?\n");
		{ u.u_error = ENOSPC; return (-1); }
	}

	/*
	 *	Se já tinha a entrada ou NÃO for LFN,
	 *	escreve a entrada regular e termina
	 */
	if (!is_lfn)
	{
		bp = bread (ip->i_dev, blkno, 0);

		memmove (bp->b_addr + offset, &d, sizeof (FATDIR));

		bdwrite (bp);

		return (FAT_MAKE_INO (blkno, offset));
	}

	/*
	 *	Escolhe os dígitos depois do "~"
	 */
	for (i = 1; /* abaixo */; i++)
	{
		if (i >= 100)
		{
			printf
			(	"fat_lfn_dir_put: "
				"Não há mais dígitos disponíveis\n"
			);

			{ u.u_error = ENOSPC; return (-1); }
		}

		if (IS_BIT (digit_vec, i))
			continue;

		if (i < 10)
		{
			digit_point[0] = '0' + i;
		}
		else
		{
			digit_point[-2] = '~';
			digit_point[-1] = '0' + i / 10;
			digit_point[0]  = '0' + i % 10;
		}

		break;
	}

	/*
	 *	Prepara os parametros do LFN
	 */
	entry_no = (entries_needed - 1) | 0x40;

	if ((i = dep->d_namlen % LFN_MAX_CHAR) == 0)		/* PROVISÓRIO */
		i = LFN_MAX_CHAR;

	entry_cp = dep->d_name + dep->d_namlen - i;

	checksum = fat_lfn_w95_checksum (d.d_name);

	/*
	 *	Inicialização da malha
	 */
	bp = bread (ip->i_dev, blkno, 0);

	dp = bp->b_addr + offset; end_dp = (FATDIR *)((int)bp->b_addr + BLSZ);

	/*
	 *	Escreve as diversas entradas do LFN
	 */
	for (/* acima */; /* abaixo */; dp++)
	{
		char		end_of_str;

		if (dp >= end_dp)
		{
			/* Lê um bloco de diretório */

			bdwrite (bp);

			if (++blkno >= end_blkno)
			{
				if (clusno == 0)		/* RAIZ */
					{ u.u_error = ENOSPC; return (-1); }

				if ((clusno = fat_cluster_write_map (ip, ++log_clusno)) < 0)
					{ u.u_error = ENOSPC; return (-1); }

				FIRST_AND_LAST_BLK (clusno, blkno, end_blkno);
			}

			bp = bread (ip->i_dev, blkno, 0);

			dp = bp->b_addr; end_dp = (FATDIR *)((int)dp + BLSZ);
		}

		/* Escreve a entrada regular */

		if ((entry_no & ~0x40) <= 0)
		{
			memmove (dp, &d, sizeof (FATDIR));

			bdwrite (bp);

			return (FAT_MAKE_INO (blkno, (int)dp - (int)bp->b_addr));
		}

		/* Escreve uma entrada LFN */

		memclr (&lfn_d, sizeof (FATDIR));

		cp = entry_cp; end_of_str = 0;

		for (i = 1; (locat = fat_lfn_locat_vec[i]) != 0; i++)
		{
			int		c;

			if   (end_of_str)
				c = 0xFFFF;
			elif ((c = *cp) == '\0')
				end_of_str++;
			else
				cp++;

			FAT_PUT_SHORT (c, (int)&lfn_d + locat);
		}

		lfn_d.d_name[0]  = entry_no;
		lfn_d.d_name[13] = checksum;
		lfn_d.d_mode     = FAT_LFN;

		memmove (dp, &lfn_d, sizeof (FATDIR));

		if ((entry_no & ~0x40) == 1)
		{
			if ((i = entry_cp - dep->d_name) != 0)
			{
				printf
				(	"fat_lfn_dir_put: "
					"Escreveu o número errado de entradas LFN: %d\n",
					i
				);
			}
		}

		entry_no &= ~0x40; entry_no--;
		entry_cp -= LFN_MAX_CHAR;

	}	/* end for (pelas várias entradas LFN) */

}	/* end fat_lfn_dir_put */

/*
 ****************************************************************
 *	Cria o nome no formato novo (W95)			*
 ****************************************************************
 */
int
fat_lfn_can_format (const DIRENT *dep, FATDIR *dp, char **digit_point, int *base_sz)
{
	char		*bp, *end_bp;
	const char	*cp;
	const char	*last_dot = NOSTR;
	int		c;
	int		is_lfn = 0, dot = 0;

	/*
	 *	Retorna 1 se for LFN, 0 se for DOS 8.3
	 *
	 *	Inicialmente investiga se é um LFN
	 */

	/*
	 *	Acha o último ponto
	 */
	for (cp = dep->d_name; *cp != '\0'; cp++)
	{ 
		if (*cp == '.')
			last_dot = cp;
	}

	/*
	 *	Tenta compor o nome DOS 8.3
	 */
	memset (dp->d_name, ' ', sizeof (dp->d_name) + sizeof (dp->d_ext));

	bp = dp->d_name; end_bp = bp + sizeof (dp->d_name);

	for (cp = dep->d_name; (c = *cp) != '\0'; cp++)
	{
		if (lowertoupper[c] != c)
			is_lfn++;

		if   (c == '.')
		{
			if (cp == dep->d_name || dot++ > 0)
				is_lfn++;

			if (cp == last_dot)
				{ bp = dp->d_ext; end_bp = bp + sizeof (dp->d_ext); }
		}
		elif (c == ' ' || bp >= end_bp)
		{
			is_lfn++;
		}
		else	/* c != '.' && c != ' ' */
		{
			*bp++ = lowertoupper[c];
		}
	}

	if (!is_lfn)
		return (0);

	/*
	 *	É LFN: prepara o nome com "~"
	 *
	 *	O resumo DOS 8.3 já foi feito acima
	 */
	bp = dp->d_name + sizeof (dp->d_name) - 1; end_bp = bp - 1;	/* Espaço para "~1" */

	while (bp >= dp->d_name && *bp == ' ')
		bp--;

	bp++;

	if (bp > end_bp)
		bp = end_bp;

	*base_sz = bp - dp->d_name + 1;	/* Inclui o "~" */

	*bp++ = '~'; *bp = '1'; *digit_point = bp;

	return (1);

}	/* end fat_lfn_can_format */

/*
 ****************************************************************
 *	Cria o nome no formato novo (8.3)			*
 ****************************************************************
 */
void
fat_8_3_can_format (const DIRENT *dep, FATDIR *dp)
{
	const char	*cp;
	char		*bp, *end_bp;
	const char	*last_dot = NOSTR;
	int		c, base_len;

	/*
	 *	Acha o último ponto
	 */
	for (cp = dep->d_name; cp[0] != '\0'; cp++)
	{ 
		if (cp[0] == '.' && cp[1] != '\0')
			last_dot = cp;
	}

	if (last_dot == NOSTR)
		base_len = dep->d_namlen;
	else
		base_len = last_dot - dep->d_name;

	/*
	 *	Prepara o nome 8.3
	 */
	memset (dp->d_name, ' ', sizeof (dp->d_name) + sizeof (dp->d_ext));

	bp = dp->d_name; end_bp = bp + sizeof (dp->d_name);

	for (cp = dep->d_name; (c = *cp) != '\0'; cp++)
	{
		if   (cp == last_dot)
		{
			bp = dp->d_ext; end_bp = bp + sizeof (dp->d_ext);
		}
		elif (bp < end_bp)
		{
			if (c == '.' || c == ' ')
				c = '_';

			*bp++ = lowertoupper[c];
		}
		elif (cp == dep->d_name + sizeof (dp->d_name) && end_bp == dp->d_name + sizeof (dp->d_name))
		{
			bp = dp->d_name  + sizeof (dp->d_name) / 2;
			cp = dep->d_name + base_len - sizeof (dp->d_name) / 2 - 1;
		}
	}

}	/* end fat_8_3_can_format */

/*
 ****************************************************************
 *	Calcula o "checksum" do Win95				*
 ****************************************************************
 */
char
fat_lfn_w95_checksum (const char *nm)
{
	int		i;
	char		checksum = 0;

	for (i = 11; i > 0; i--)
		checksum = ((checksum << 7) | (checksum >> 1)) + *nm++;

	return (checksum);

}	/* end fat_lfn_w95_checksum */
