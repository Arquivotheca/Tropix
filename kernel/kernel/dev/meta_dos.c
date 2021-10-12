/*
 ****************************************************************
 *								*
 *			meta_dos.c				*
 *								*
 *	Meta-driver para arquivos DOS simulando partições	*
 *								*
 *	Versão	3.0.0, de 06.03.95				*
 *		4.3.0, de 19.09.02				*
 *								*
 *	Módulo: Núcleo						*
 *		NÚCLEO do TROPIX para PC			*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 2002 NCE/UFRJ - tecle "man licença" *   
 *								*
 ****************************************************************
 */

#include "../h/common.h"
#include "../h/sync.h"
#include "../h/scb.h"
#include "../h/region.h"

#include "../h/disktb.h"
#include "../h/bhead.h"
#include "../h/iotab.h"
#include "../h/map.h"
#include "../h/devhead.h"
#include "../h/devmajor.h"
#include "../h/kfile.h"
#include "../h/inode.h"
#include "../h/ioctl.h"
#include "../h/signal.h"
#include "../h/uproc.h"
#include "../h/uerror.h"

#include "../h/extern.h"
#include "../h/proto.h"

/*
 ****************************************************************
 *	Variáveis e Definições globais				*
 ****************************************************************
 */
#undef	DEBUG		/* Controla a depuração */

#define	NMD	3	/* 0: "swap", 1: "root", 2: "home" */

#define	GET_SHORT(cp)		(*(unsigned short *)(cp))
#define	GET_LONG(cp)		(*(unsigned long *)(cp))

#define	C_ISBAD_OR_EOF(c)	((c) >= BAD_BEGIN)

/*
 ******	Bloco 0 do disco ****************************************
 */
typedef struct
{
	char	s_reser0[3];
	char	s_id[8];		/* Identificação: "DOS  2.0" */

	char	s_bytes_per_sector[2];	/* Bytes por setor */
	char	s_sectors_per_cluster;	/* Setores por cluster */
	char	s_fat_offset[2];	/* Deslocamento da FAT */
	char	s_n_fats;		/* No. de FATs */
	char	s_root_dir_entries[2];	/* No. de ent. do dir. raiz */
	char	s_little_size[2];	/* No. de setores do dispositivo */
	char	s_media;		/* Tipo do meio */
	char	s_little_sectors_per_fat[2]; /* No. de setores por FAT */
	char	s_sectors_per_track[2];	/* No. de setores por trilha */
	char	s_n_heads[2];		/* No. de lados */
	char	s_hidden_sectors[2];	/* No. de setores ocultos */
	char	s_reser_0[2];
	char	s_big_size[4];		/* Tamanho (se >= 32 MB) */

  union
  {
    struct	/* Continuação para FAT32 */
    {
	char	s_32_big_sectors_per_fat[4]; /* No. de setores por FAT (32) */
	char	s_32_flags[2];		/* Indicadores */
	char	s_32_version[2];	/* Versão */
	char	s_32_root_cluster[4];	/* Início do diretório raiz */
	char	s_32_fs_info[2];	/* Setor do bloco de Informações  */
	char	s_32_boot_backup[2];	/* Cópia do setor de boot */

	char	s_reser_1[12];
	char	s_phys_drive;		/* Dispositivo */
	char	s_reser_2;
	char	s_signature;		/* Assinatura (0x29) */
	char	s_vol_ser_no[4];	/* No. serial */
	char	s_label[11];		/* Rótulo */
	char	s_fat_msg[5];		/* Tipo da FAT */

    }	fat_32;

    struct	/* Continuação para FAT12 / FAT16 */
    {
	char	s_phys_drive;		/* Dispositivo */
	char	s_reser_2;
	char	s_signature;		/* Assinatura (0x29) */
	char	s_vol_ser_no[4];	/* No. serial */
	char	s_label[11];		/* Rótulo */
	char	s_fat_msg[5];		/* Tipo da FAT */

    }	fat_16;

  }	u;

	char	s_boot[512-87-2];
	char	s_boot_valid[2];	/* Indica o o boot está válido */

}	DOSSB;

/*
 ******	Equivalencias convenientes ******************************
 */
#define	s_big_sectors_per_fat	u.fat_32.s_32_big_sectors_per_fat
#define	s_flags			u.fat_32.s_32_flags
#define	s_version		u.fat_32.s_32_version
#define	s_root_cluster		u.fat_32.s_32_root_cluster
#define	s_fs_info		u.fat_32.s_32_fs_info
#define	s_boot_backup		u.fat_32.s_32_boot_backup

#define	s_32_phys_drive		u.fat_32.s_phys_drive
#define	s_32_signature		u.fat_32.s_signature
#define	s_32_vol_ser_no		u.fat_32.s_vol_ser_no
#define	s_32_label		u.fat_32.s_label
#define	s_32_fat_msg		u.fat_32.s_fat_msg

#define	s_16_phys_drive		u.fat_16.s_phys_drive
#define	s_16_signature		u.fat_16.s_signature
#define	s_16_vol_ser_no		u.fat_16.s_vol_ser_no
#define	s_16_label		u.fat_16.s_label
#define	s_16_fat_msg		u.fat_16.s_fat_msg

/*
 ******	Entrada de diretório do DOS *****************************
 */
typedef	struct
{
	char	d_name[8];		/* Nome do arquivo */
	char	d_ext[3];		/* Extensão do nome */
	char	d_mode;			/* Atributos do arquivo */

	char	d_reser;		/* Ainda não usado */
	char	d_ccenti;		/* Centésimos de segundo da criação */
	char	d_ctime[2];		/* Tempo de criação */
	char	d_cdate[2];		/* Data de criação */
	char	d_adate[2];		/* Data de acesso */
	char	d_high_cluster[2];	/* Parte alta do CLUSTER */

	char	d_mtime[2];		/* Tempo de atualização */
	char	d_mdate[2];		/* Data de atualização */
	char	d_low_cluster[2];	/* Parte baixa do CLUSTER */
	char	d_size[4];		/* Tamanho do arquivo */

}	DOSDIR;

#define NODOSDIR	(DOSDIR *)0

/*
 ******	Indicadores dos arquivos DOS ****************************
 */
#define	Z_2		0x80		/* Não usado */
#define	Z_1		0x40		/* Não usado */
#define	Z_MODIF		0x20		/* Modificado */
#define	Z_DIR		0x10		/* Diretório */
#define	Z_VOL		0x08		/* Volume */
#define	Z_SYS		0x04		/* Sistema */
#define	Z_HIDDEN	0x02		/* Oculto */
#define	Z_RO		0x01		/* Read only */

#define	Z_IFMT		(Z_DIR|Z_VOL)	/* Tipo do arquivo */
#define	Z_REG		0		/* Arquivo regular */
#define	Z_LFN		(Z_VOL|Z_SYS|Z_HIDDEN|Z_RO) /* Nome longo */

#define	Z_ISREG(m)	(((m) & Z_IFMT) == Z_REG)
#define	Z_ISDIR(m)	(((m) & Z_IFMT) == Z_DIR)
#define	Z_ISVOL(m)	(((m) & Z_IFMT) == Z_VOL)
#define	Z_ISLFN(m)	((m) == Z_LFN)

#define	Z_EMPTY		0xE5		/* Entrada de dir. vazia */

#define Z_NMSZ		(sizeof (dp->d_name) + sizeof (dp->d_ext))

/*
 ******	Macros para a FAT ***************************************
 */
entry int	FAT_ENT_PER_BLOCK;
entry int	BAD_BEGIN;

#define FIRST_BLK(clusno)						\
		clus_begin + (clusno) * sec_clus

#define FIRST_AND_LAST_BLK(clusno, blkno, end_blkno)			\
		blkno = clus_begin + (clusno) * sec_clus,		\
		end_blkno = blkno + sec_clus

#define	GET_CLUSTER(dp, cluster)					\
	cluster = GET_SHORT ((dp)->d_low_cluster);			\
	if (fat_bits == 32)						\
		cluster |= ((GET_SHORT ((dp)->d_high_cluster)) << 16)
/*
 ******	Estrutura de controle dos meta-dispositivos *************
 */
typedef struct
{
	char	m_open;		/* O meta-dispositivo foi encontrado */
	int	m_dev;		/* O dispositivo físico do meta-dispositivo */
	int	m_offset;	/* O início dentro do dispositivo físico */
	int	m_size;		/* O Tamanho do meta-dispositivo */

	short	m_head;		/* No. de cabeças do dispositivo */
	short	m_sect;		/* No. de setores por trilha */
	ushort	m_cyl;		/* No. de cilindros */

}	META_STATUS;

entry META_STATUS	meta_status[NMD];

/*
 ******	Variáveis globais ***************************************
 */
entry int	md_open_files;	/* Arquivos encontrados até agora */

entry int	md_is_open;	/* Todos os meta discos alocados */

entry DEVHEAD	mdtab;		/* Cabeca da lista de dp's e do major */

entry BHEAD	rmdbuf;		/* Para as operações "raw" */

entry DISKTB	*md_disktb;	/* Começo das entradas de "md" */

/*
 ****** Protótipos de funções ***********************************
 */
int		mdsearch (const DISKTB *);
long		mdcontig (const char *, int, const DOSDIR *, int, int, int);
int		mdstrategy (BHEAD *);
void		mdstart (void);

/*
 ****************************************************************
 *   Verifica se a unidade existe e prepara a interrupção	*
 ****************************************************************
 */
int
mdattach (int major)
{
	DISKTB		*pp;

	/*
	 *	Confere o MAJOR
	 */
	if (major != MD_MAJOR)
		panic ("mdattach: MAJOR %d errado", major);

	/*
	 *	Prepara a entrada para "md"
	 */
	for (pp = disktb; /* abaixo */; pp++)
	{
		if (pp->p_name[0] == '0')
			{ printf ("NÃO achei \"/dev/md0\" na DISKTB\n"); return (-1); }

		if (MAJOR (pp->p_dev) == MD_MAJOR && pp->p_target == MD_SWAP_MINOR)
			{ md_disktb = pp; break; }
	}

	return (0);

}	/* end mdattach */

#if (0)	/*******************************************************/
	/*
	int		minor, i;
	 *	Verifica se há espaço na DISKTB
	 */
	if (next_disktb + NMD >= end_disktb)
	{
		printf ("mdopen: Tabela de Discos/Partições cheia\n");
		u.u_error = ENXIO; return (-1);
	}

	/*
	 *	Cria as entradas da "disktb"
	 */
	pp = next_disktb; minor = pp - disktb;

	for (i = 0; i < NMD; i++, pp++, minor++)
	{
		pp->p_name[0]	= 'm';
		pp->p_name[1]	= 'd';
		pp->p_name[2]	= '0' + i;

	   /***	pp->p_offset	= ...; ***/
	   /***	pp->p_size	= ...; ***/

		pp->p_dev	= MAKEDEV (MD_MAJOR, minor);
	   /***	pp->p_unit	= 0; ***/
		pp->p_target	= i;

	   /***	pp->p_type	= 0; ***/
	   /***	pp->p_flags	= 0; ***/

	   /***	pp->p_head	= ...; ***/
	   /***	pp->p_sect	= ...; ***/
	   /***	pp->p_cyl	= ...; ***/

	   	pp->p_nopen	= 0;
	   	pp->p_lock	= 0;

	}	/* end criando as entradas da "disktb" */

	pp->p_name[0] = '\0'; md_disktb = next_disktb; next_disktb = pp;
#endif	/*******************************************************/

/*
 ****************************************************************
 *	Função de "open"					*
 ****************************************************************
 */
int
mdopen (dev_t dev, int oflag)
{
	DISKTB		*up, *pp;
	META_STATUS	*mp;
	SCB		*sp = &scb;
	int		i;

	/*
	 *	Prólogo
	 */
	if ((up = disktb_get_entry (dev)) == NODISKTB)
		{ u.u_error = ENXIO; return (-1); }

	/*
	 *	Se já estava aberto, apenas incrementa
	 */
	if (md_is_open)
	{
		if (md_disktb == NODISKTB)
			printf ("mdopen: Ponteiro \"md_disktb\" NULO!\n");

		up->p_nopen++;

		return (0);
	}

	/*
	 *	Procura uma partição DOS na tabela de partições
	 */
	for (pp = disktb; /* abaixo */; pp++)
	{
		if (md_open_files >= NMD) 	/* Se conseguiu todos, ... */
			break;

		if (pp->p_name[0] == '\0') 	/* A tabela acabou */
		{
			/* Verifica se ficou faltando apenas o SWAP */

			if (md_open_files >= NMD - 1)
			{
				mp = meta_status;

				if (mp[MD_ROOT_MINOR].m_open && mp[MD_HOME_MINOR].m_open)
					break;
			}

			printf ("mdopen: Arquivo(s) DOS faltando\n");

			for (mp = &meta_status[0]; mp < &meta_status[NMD]; mp++)
			{
				if (mp->m_open)
					(*biotab[MAJOR (mp->m_dev)].w_close) (mp->m_dev, 0);
			}

			memclr (meta_status, sizeof (meta_status));
			md_open_files = 0;
		   /***	md_is_open = 0; ***/

			u.u_error = ENXIO; return (-1);
		}

		if (pp->p_type != PAR_DOS_FAT16 && pp->p_type != PAR_DOS_FAT32 && pp->p_type != PAR_DOS_FAT32_L)
			continue;

		if ((i = mdsearch (pp)) > 0)
			md_open_files += i;
	}

	/*
	 *	Encontrou ROOT, HOME e talvez SWAP - atualiza as entradas da "disktb"
	 */
	pp = md_disktb; mp = &meta_status[0];

	for (i = 0; i < NMD; i++, pp++, mp++)
	{
	   /***	pp->p_name[0]	= ...; ***/
	   /***	pp->p_name[1]	= ...; ***/
	   /***	pp->p_name[2]	= ...; ***/

		pp->p_offset	= mp->m_offset;
		pp->p_size	= mp->m_size;

	   /***	pp->p_dev	= ...; ***/
	   /***	pp->p_unit	= ...; ***/
	   /***	pp->p_target	= ...; ***/

		pp->p_type	= (i == 0) ? PAR_TROPIX_SWAP : PAR_TROPIX_T1;
	   /***	pp->p_flags	= ...; ***/

		pp->p_head	= mp->m_head;
		pp->p_sect	= mp->m_sect;
		pp->p_cyl	= mp->m_cyl;

	   /***	pp->p_nopen	= ...; ***/
	   /***	pp->p_lock	= ...; ***/

	}	/* end criando as entradas da "disktb" */

	up->p_nopen++;

	/*
	 *	Verifica se precisa atualizar o SWAP
	 */
	if (sp->y_swapdev == NODEV)
	{
		mp = &meta_status[MD_SWAP_MINOR];

		if (mp->m_open)
		{
			sp->y_swapdev = md_disktb[MD_SWAP_MINOR].p_dev;

			if ((sp->y_nswap = mp->m_size - sp->y_swaplow) > 0)
				mrelease (M_SWAP, sp->y_nswap, SWAP_OFFSET);
		}
	}

	md_is_open++;

	return (0);

}	/* end mdopen */

/*
 ****************************************************************
 *	Procura um dos 3 arquivos em um dispositivo dado	*
 ****************************************************************
 */
int
mdsearch (const DISKTB *pp)
{
	BHEAD		*bp;
	const DOSSB	*sp;
	const DOSDIR	*dp;
	const DOSDIR	*end_dp;
	META_STATUS	*mp;
	int		fat_bits;
	int		sec_fat, n_root_dir, fat1, end_fat1;
	int		root_dir, end_root_dir, sec_clus;
	int		byte_clus, clus_begin;
	int		end_fs, n_clus;
	int		blkno, end_blkno, clusno, d_cluster;
	int		fat_blkno, fat_index;
	int		open_meta_files = 0;
	long		blsz;
	DOSDIR		SWAP_dir, ROOT_dir, HOME_dir;

	/*
	 *	Em primeiro lugar, abre o dispositivo físico
	 */
	if (((*biotab[MAJOR (pp->p_dev)].w_open) (pp->p_dev, O_RW)) < 0)
	{
		printf ("mdopen: Não consegui abrir \"%s\"\n", pp->p_name);
		return (0);
	}

	/*
	 *	Le o superbloco do sistema de arquivos DOS
	 */
	bp = bread (pp->p_dev, 0, 0); sp = (DOSSB *)bp->b_addr;

	if (sp->s_boot_valid[0] != 0x55 || sp->s_boot_valid[1] != 0xAA)
	{
		printf
		(	"mdopen: Assinatura do superbloco DOS inválido em \"%s\"\n",
			pp->p_name
		);
		block_put (bp); goto bad;
	}

	if   (memeq (sp->s_32_fat_msg, "FAT32", 5))
	{
		fat_bits = 32;
	}
	elif (memeq (sp->s_16_fat_msg, "FAT16", 5))
	{
		fat_bits = 16;
	}
	else
	{
		printf
		(	"mdopen: O sistema de arquivos \"%s\" NÃO é nem FAT32 nem FAT16\n",
			pp->p_name
		);
		block_put (bp); goto bad;
	}

	printf
	(	"mdopen: Examinando a partição DOS FAT%d \"%s\"\n",
		fat_bits, pp->p_name
	);

	if (sp->s_n_fats != 2)
	{
		printf
		(	"mdopen: O sistema de arquivos DOS \"%s\" NÃO tem 2 FATs\n",
			pp->p_name
		);
		block_put (bp); goto bad;
	}

	if (GET_SHORT (sp->s_bytes_per_sector) != BLSZ)
	{
		printf
		(	"mdopen: O sistema de arquivos DOS \"%s\" NÃO tem bloco de %d bytes\n",
			pp->p_name, BLSZ
		);
		block_put (bp); goto bad;
	}

	if ((end_fs = GET_SHORT (sp->s_little_size)) == 0)
		end_fs = GET_LONG (sp->s_big_size);

	/*
	 *	Calcula alguns parametros
	 */
	if (fat_bits == 32)	/* FAT32 */
	{
		if
		(	GET_SHORT (sp->s_root_dir_entries) != 0 ||
			GET_SHORT (sp->s_little_size) != 0 ||
			GET_SHORT (sp->s_little_sectors_per_fat) != 0 ||
			GET_SHORT (sp->s_version) != 0
		)
		{
			printf
			(	"mdopen: Superbloco de FAT 32 inválido em \"%s\"\n",
				pp->p_name
			);
			block_put (bp); goto bad;
		}

		FAT_ENT_PER_BLOCK = (BLSZ / sizeof (ulong));
		BAD_BEGIN	  = 0x0FFFFFF1;

		sec_fat		= GET_LONG (sp->s_big_sectors_per_fat);
		n_root_dir	= 0;
	   	fat1		= GET_SHORT (sp->s_fat_offset);
		end_fat1	= fat1 + sec_fat;
		root_dir	= end_fat1 + sec_fat;
		end_root_dir	= root_dir;
		d_cluster	= GET_LONG (sp->s_root_cluster);
	}
	else			/* FAT16 */
	{
		FAT_ENT_PER_BLOCK = (BLSZ / sizeof (ushort));
		BAD_BEGIN	  = 0xFFF1;

		sec_fat		= GET_SHORT (sp->s_little_sectors_per_fat);
		n_root_dir	= GET_SHORT (sp->s_root_dir_entries);
		fat1		= GET_SHORT (sp->s_fat_offset);
		end_fat1	= fat1 + sec_fat;
		root_dir	= end_fat1 + sec_fat;
		end_root_dir	= root_dir + n_root_dir * sizeof (DOSDIR) / BLSZ;
		d_cluster	= 0;
	}

	sec_clus	= sp->s_sectors_per_cluster;
	byte_clus	= sec_clus * BLSZ;
	clus_begin	= end_root_dir - 2 * sec_clus;
	n_clus		= (end_fs - end_root_dir) / sec_clus;

	block_put (bp);

#ifdef	DEBUG
	printf
	(	"mdopen: sec_fat = %d,\tn_root_dir = %d\n"
		"mdopen: fat1 = %d,\tend_fat1 = %d\n"
		"mdopen: root_dir = %d,\tend_root_dir = %d,\tsec_clus = %d\n"
		"mdopen: byte_clus = %d,\tclus_begin = %d\n"
		"mdopen: end_fs = %d,\tn_clus = %d\n",
		sec_fat, n_root_dir,
		fat1, end_fat1,
		root_dir, end_root_dir, sec_clus,
		byte_clus, clus_begin,
		end_fs, n_clus
	);
#endif	DEBUG

	/*
	 *	Le a raiz procurando "tropix"
	 */
	if (fat_bits == 32) for (clusno = blkno = end_blkno = 0; /* abaixo */; blkno++)
	{
		/* FAT32 */

		if (blkno >= end_blkno)
		{
			if (clusno == 0)
			{
				clusno = d_cluster;
			}
			else
			{
				fat_blkno = fat1 + clusno / FAT_ENT_PER_BLOCK;
				fat_index = clusno % FAT_ENT_PER_BLOCK;
		
				bp = bread (pp->p_dev, fat_blkno, 0);

				clusno = GET_LONG ((ulong *)bp->b_addr + fat_index);

				block_put (bp);
			}
#ifdef	DEBUG
			printf ("mdopen: Cluster seguinte = %d\n", clusno);
#endif	DEBUG
			if (C_ISBAD_OR_EOF (clusno))
				goto tropix_not_found;

			if (clusno < 2 || clusno >= n_clus + 2)
			{
				printf
				(	"mdopen: Cluster inválido %d em "
					"\"%s:/tropix\"\n",
					clusno, pp->p_name
				);
				goto bad;
			}

			FIRST_AND_LAST_BLK (clusno, blkno, end_blkno);

		}	/* end o cluster terminou */

		bp = bread (pp->p_dev, blkno, 0);

		dp = (DOSDIR *)bp->b_addr; end_dp = dp + BLSZ / sizeof (DOSDIR);

		for (/* acima */; dp < end_dp; dp++)
		{
			if (dp->d_name[0] == '\0')
				{ block_put (bp); goto tropix_not_found; }

			if (dp->d_name[0] == Z_EMPTY || Z_ISLFN (dp->d_mode))
				continue;

			if (memeq (dp->d_name, "TROPIX     ", sizeof (dp->d_name) + sizeof (dp->d_ext)))
				goto tropix_found;

		}	/* percorrendo as entradas de diretórios de um bloco */

		block_put (bp);

	}
	else for (blkno = root_dir; blkno < end_root_dir; blkno++)
	{
		/* FAT16 */

		bp = bread (pp->p_dev, blkno, 0);

		dp = (DOSDIR *)bp->b_addr; end_dp = dp + BLSZ / sizeof (DOSDIR);

		for (/* acima */; dp < end_dp; dp++)
		{
			if (dp->d_name[0] == '\0')
				{ block_put (bp); goto tropix_not_found; }

			if (dp->d_name[0] == Z_EMPTY || Z_ISLFN (dp->d_mode))
				continue;

			if (memeq (dp->d_name, "TROPIX     ", sizeof (dp->d_name) + sizeof (dp->d_ext)))
				goto tropix_found;

		}

		block_put (bp);

	}	/* end percorrendo os blocos de diretório */

    tropix_not_found:
#ifdef	DEBUG
	printf
	(	"mdopen: Não encontrei o diretório \"%s:/tropix\"\n",
		pp->p_name
	);
#endif	DEBUG
	goto bad;

	/*
	 *	Encontrou o arquivo "/tropix"
	 */
    tropix_found:
	if (!Z_ISDIR (dp->d_mode))
	{
#ifdef	DEBUG
		printf
		(	"mdopen: O arquivo \"%s:/tropix\" NÃO é um diretório\n",
			pp->p_name
		);
#endif	DEBUG
		block_put (bp); goto bad;
	}

	GET_CLUSTER (dp, d_cluster);

	block_put (bp);

	printf
	(	"mdopen: Examinando o diretório \"%s:/tropix\"\n",
		pp->p_name
	);

	SWAP_dir.d_mode = Z_IFMT;
	ROOT_dir.d_mode = Z_IFMT;
	HOME_dir.d_mode = Z_IFMT;

	/*
	 *	Le o diretório "tropix" procurando "root", "swap" e "home"
	 */
	for (clusno = blkno = end_blkno = 0; /* abaixo */; blkno++)
	{
		if (blkno >= end_blkno)
		{
			if (clusno == 0)
			{
				clusno = d_cluster;
			}
			else
			{
				fat_blkno = fat1 + clusno / FAT_ENT_PER_BLOCK;
				fat_index = clusno % FAT_ENT_PER_BLOCK;
		
				bp = bread (pp->p_dev, fat_blkno, 0);

				if (fat_bits == 32)
					clusno = GET_LONG ((ulong *)bp->b_addr + fat_index);
				else
					clusno = GET_SHORT ((ushort *)bp->b_addr + fat_index);

				block_put (bp);
			}
#ifdef	DEBUG
			printf ("mdopen: Cluster seguinte = %d\n", clusno);
#endif	DEBUG
			if (C_ISBAD_OR_EOF (clusno))
				goto examine;

			if (clusno < 2 || clusno >= n_clus + 2)
			{
				printf
				(	"mdopen: Cluster inválido %d em "
					"\"%s:/tropix\"\n",
					clusno, pp->p_name
				);
				goto bad;
			}

			FIRST_AND_LAST_BLK (clusno, blkno, end_blkno);

		}	/* end o cluster terminou */

		bp = bread (pp->p_dev, blkno, 0);

		dp = (DOSDIR *)bp->b_addr; end_dp = dp + BLSZ / sizeof (DOSDIR);

		for (/* acima */; dp < end_dp; dp++)
		{
			if (dp->d_name[0] == '\0')
				{ block_put (bp); goto examine; }

			if (dp->d_name[0] == Z_EMPTY || Z_ISLFN (dp->d_mode))
				continue;

			if   (memeq (dp->d_name, "SWAP       ", Z_NMSZ)) 
				memmove (&SWAP_dir, dp, sizeof (DOSDIR));
			elif (memeq (dp->d_name, "ROOT       ", Z_NMSZ))
				memmove (&ROOT_dir, dp, sizeof (DOSDIR));
			elif (memeq (dp->d_name, "HOME       ", Z_NMSZ))
				memmove (&HOME_dir, dp, sizeof (DOSDIR));

		}	/* percorrendo as entradas de diretórios de um bloco */

		block_put (bp);

	}	/* end percorrendo os blocos de um cluster do diretório) */

	/*
	 *	Analisa os arquivos encontrados
	 */
    examine:
	mp = &meta_status[MD_SWAP_MINOR];

	if (!mp->m_open && Z_ISREG (SWAP_dir.d_mode))
	{
#ifdef	DEBUG
		GET_CLUSTER (&SWAP_dir, d_cluster);
		printf
		(	"mdopen: SWAP encontrado, size = %d, cluster = %d\n",
			GET_LONG (SWAP_dir.d_size), d_cluster
		);
#endif	DEBUG
		if ((blsz = mdcontig ("swap", fat_bits, &SWAP_dir, fat1, pp->p_dev, byte_clus)) >= 0)
		{
			mp->m_open   = 1;
			mp->m_dev    = pp->p_dev;
			GET_CLUSTER (&SWAP_dir, d_cluster);
			mp->m_offset = FIRST_BLK (d_cluster);
		   	mp->m_size   = blsz;

			mp->m_head   = pp->p_head;
			mp->m_sect   = pp->p_sect;
			mp->m_cyl    = pp->p_cyl;

			printf
			(	"mdopen: Arquivo \"%s:/tropix/swap\": %d MB, bloco %d\n",
				pp->p_name, blsz >> (MBSHIFT - BLSHIFT), mp->m_offset
			);

			(*biotab[MAJOR (pp->p_dev)].w_open) (pp->p_dev, O_RW);
			open_meta_files++;
		}
	}

	mp++;			/* ROOT */

	if (!mp->m_open && Z_ISREG (ROOT_dir.d_mode))
	{
#ifdef	DEBUG
		GET_CLUSTER (&ROOT_dir, d_cluster);
		printf
		(	"mdopen: ROOT encontrado, size = %d, cluster = %d\n",
			GET_LONG (ROOT_dir.d_size), d_cluster
		);
#endif	DEBUG
		if ((blsz = mdcontig ("root", fat_bits, &ROOT_dir, fat1, pp->p_dev, byte_clus)) >= 0)
		{
			mp->m_open   = 1;
			mp->m_dev    = pp->p_dev;
			GET_CLUSTER (&ROOT_dir, d_cluster);
			mp->m_offset = FIRST_BLK (d_cluster);
		   	mp->m_size   = blsz;

			mp->m_head   = pp->p_head;
			mp->m_sect   = pp->p_sect;
			mp->m_cyl    = pp->p_cyl;

			printf
			(	"mdopen: Arquivo \"%s:/tropix/root\": %d MB, bloco %d\n",
				pp->p_name, blsz >> (MBSHIFT - BLSHIFT), mp->m_offset
			);

			(*biotab[MAJOR (pp->p_dev)].w_open) (pp->p_dev, O_RW);
			open_meta_files++;
		}
	}

	mp++;			/* HOME */

	if (!mp->m_open && Z_ISREG (HOME_dir.d_mode))
	{
#ifdef	DEBUG
		GET_CLUSTER (&HOME_dir, d_cluster);
		printf
		(	"mdopen: HOME encontrado, size = %d, cluster = %d\n",
			GET_LONG (HOME_dir.d_size), d_cluster
		);
#endif	DEBUG
		if ((blsz = mdcontig ("home", fat_bits, &HOME_dir, fat1, pp->p_dev, byte_clus)) >= 0)
		{
			mp->m_open   = 1;
			mp->m_dev    = pp->p_dev;
			GET_CLUSTER (&HOME_dir, d_cluster);
			mp->m_offset = FIRST_BLK (d_cluster);
		   	mp->m_size   = blsz;

			mp->m_head   = pp->p_head;
			mp->m_sect   = pp->p_sect;
			mp->m_cyl    = pp->p_cyl;

			printf
			(	"mdopen: Arquivo \"%s:/tropix/home\": %d MB, bloco %d\n",
				pp->p_name, blsz >> (MBSHIFT - BLSHIFT), mp->m_offset
			);

			(*biotab[MAJOR (pp->p_dev)].w_open) (pp->p_dev, O_RW);
			open_meta_files++;
		}
	}

	/*
	 *	Verifica o retorno
	 */
    bad:
	(*biotab[MAJOR (pp->p_dev)].w_close) (pp->p_dev, 0);

	return (open_meta_files);

}	/* end mdsearch */

/*
 ****************************************************************
 *	Função para confirmar a contiguidade do arquivo		*
 ****************************************************************
 */
long
mdcontig (const char *nm, int fat_bits, const DOSDIR *dp, int fat1, int dev, int byte_clus)
{
	BHEAD		*bp = NOBHEAD;
	int		fat_blkno, fat_index, bp_blkno = 0;
	int		clusno, new_clusno;
	long		size = byte_clus;

	/*
	 *	Percorre toda a lista de CLUSTERs
	 */
	GET_CLUSTER (dp, clusno);

	for (/* vazio */; /* vazio */; clusno = new_clusno)
	{
		fat_blkno = fat1 + clusno / FAT_ENT_PER_BLOCK;
		fat_index = clusno % FAT_ENT_PER_BLOCK;

		if (fat_blkno != bp_blkno)
		{
			if (bp_blkno != 0)
				block_put (bp);

			bp = bread (dev, fat_blkno, 0);

			bp_blkno = fat_blkno;
		}

		if (fat_bits == 32)
			new_clusno = GET_LONG ((ulong *)bp->b_addr + fat_index);
		else
			new_clusno = GET_SHORT ((ushort *)bp->b_addr + fat_index);

		if (C_ISBAD_OR_EOF (new_clusno))
			break;

		if (new_clusno != clusno + 1)
		{
			printf
			(	"mdopen: O arquivo \"%s\" NÃO é contíguo "
				"(%d :: %d)\n", nm, clusno, new_clusno
			);

			if (bp_blkno != 0)
				block_put (bp);

			return (-1);
		}

		size += byte_clus;

	}	/* end percorrendo os clusters */

	if (bp_blkno != 0)
		block_put (bp);

	if (size != GET_LONG (dp->d_size))
	{
		printf
		(	"mdopen: Arquivo \"%s\" com tamanho incorreto (%d :: %d)\n",
			nm, size, GET_LONG (dp->d_size)
		);
		return (-1);
	}

	return (size >> BLSHIFT);

}	/* end mdcontig */

/*
 ****************************************************************
 *	Função de close						*
 ****************************************************************
 */
int
mdclose (dev_t dev, int flag)
{
	DISKTB			*up;
	const META_STATUS	*mp;
	SCB			*sp = &scb;
	DISKTB			*pp;
	int			i;

	/*
	 *	Verifica se foi o último
	 */
	if (!md_is_open)
		{ printf ("mdclose: Chamada em estado já fechado\n"); return (-1); }

	if (md_disktb == NODISKTB)
		{ printf ("mdclose: Ponteiro \"md_disktb\" NULO!\n"); return (-1); }

	up = &disktb[MINOR (dev)]; up->p_nopen--;

	for (pp = &md_disktb[0]; pp < &md_disktb[NMD]; pp++)
	{
		if (pp->p_nopen)
			return (0);
	}

	/*
	 *	Verifica se precisa atualizar o SWAP
	 */
	if (MAJOR (sp->y_swapdev) == MD_MAJOR)
	{
		mrelease_all (M_SWAP);
		sp->y_swapdev = NODEV;
	}

	/*
	 *	Fecha os diversos dispositivos
	 */
	for (mp = &meta_status[0]; mp < &meta_status[NMD]; mp++)
	{
		if (mp->m_open)
			(*biotab[MAJOR (mp->m_dev)].w_close) (mp->m_dev, 0);
	}

	/*
	 *	Zera os diversos campos
	 */
	memclr (meta_status, sizeof (meta_status));
	md_open_files = 0;
	md_is_open = 0;

	/*
	 *	Zera alguns campos
	 */
	for (i = 0, pp = md_disktb; i < NMD; i++, pp++)
	{
	   /***	pp->p_name[0]	= ...; ***/
	   /***	pp->p_name[1]	= ...; ***/
	   /***	pp->p_name[2]	= ...; ***/

		pp->p_offset	= 0;
		pp->p_size	= 0;

	   /***	pp->p_dev	= ...; ***/
	   /***	pp->p_unit	= ...; ***/
	   /***	pp->p_target	= ...; ***/

		pp->p_type	= 0;
	   /***	pp->p_flags	= ...; ***/

		pp->p_head	= 0;
		pp->p_sect	= 0;
		pp->p_cyl	= 0;

	   /***	pp->p_nopen	= ...; ***/
	   /***	pp->p_lock	= ...; ***/

	}	/* end criando as entradas da "disktb" */

	return (0);

}	/* end mdclose */

/*
 ****************************************************************
 *	Executa uma operação de entrada/saida			*
 ****************************************************************
 */
int
mdstrategy (BHEAD *bp)
{
	const DISKTB		*up;
	const META_STATUS	*mp;
	daddr_t			bn;

	/*
	 *	Verifica a validade do pedido
	 */
	up = &disktb[MINOR (bp->b_dev)];

	mp = &meta_status[up->p_target];

	if ((bn = bp->b_blkno) < 0 || bn + BYTOBL (bp->b_base_count) > mp->m_size)
		{ bp->b_error = ENXIO; goto bad; }

	/*
	 *	Faz a conversão de "meta" para "físico"
	 */
	bp->b_phdev   = mp->m_dev;
	bp->b_phblkno = bp->b_blkno + mp->m_offset;

	return ((*biotab[MAJOR (bp->b_phdev)].w_strategy) (bp));

	/*
	 *	Houve algum erro
	 */
    bad:
	bp->b_flags |= B_ERROR;
	EVENTDONE (&bp->b_done);
	return (-1);

}	/* end mdstrategy */

/*
 ****************************************************************
 *	Leitura de modo "raw"					*
 ****************************************************************
 */
int
mdread (IOREQ *iop)
{
	const DISKTB		*up;
	int			dma;
	const META_STATUS	*mp;

	up = &disktb[MINOR (iop->io_dev)];

	mp = &meta_status[up->p_target];

	dma = biotab[MAJOR (mp->m_dev)].w_tab->v_flags & V_DMA_24;

	if (iop->io_offset_low & BLMASK || iop->io_count & BLMASK || iop->io_count > 255 * KBSZ)
		u.u_error = EINVAL;
	else
		physio (iop, mdstrategy, &rmdbuf, B_READ, dma);

	return (UNDEF);

}	/* end mdread */

/*
 ****************************************************************
 *	Escrita de modo "raw"					*
 ****************************************************************
 */
int
mdwrite (IOREQ *iop)
{
	const DISKTB		*up;
	int			dma;
	const META_STATUS	*mp;

	up = &disktb[MINOR (iop->io_dev)];

	mp = &meta_status[up->p_target];

	dma = biotab[MAJOR (mp->m_dev)].w_tab->v_flags & V_DMA_24;

	if (iop->io_offset_low & BLMASK || iop->io_count & BLMASK || iop->io_count > 255 * KBSZ)
		u.u_error = EINVAL;
	else
		physio (iop, mdstrategy, &rmdbuf, B_WRITE, dma);

	return (UNDEF);

}	/* end mdwrite */

/*
 ****************************************************************
 *	Rotina de IOCTL						*
 ****************************************************************
 */
int
mdctl (dev_t dev, int cmd, int arg, int flag)
{
	const DISKTB		*up;
	META_STATUS		*mp;

	/*
	 *	Verifica a validade do pedido e converte de "meta" para "físico"
	 */
	up = &disktb[MINOR (dev)];

	mp = &meta_status[up->p_target]; dev = mp->m_dev;

	/*
	 *	Chama a rotina de IOCTL do Dispositivo.
	 */
	switch (u.u_fileptr->f_inode->i_mode & IFMT)
	{
	    case IFBLK:
		return ((*biotab[MAJOR(dev)].w_ioctl) (dev, cmd, arg, flag));

	    case IFCHR:
		return ((*ciotab[MAJOR(dev)].w_ioctl) (dev, cmd, arg, flag));

	    default:
		u.u_error = ENODEV;
		return (-1);
	}

}	/* end mdctl */
