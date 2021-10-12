/*
 ****************************************************************
 *								*
 *			do_param.c				*
 *								*
 *	Imprime os parâmetros do sistema de arquivos		*
 *								*
 *	Versão	3.0.0, de 28.10.93				*
 *		3.0.8, de 27.10.97				*
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

#include <sys/common.h>
#include <sys/syscall.h>
#include <sys/kcntl.h>

#include <stdio.h>
#include <stdlib.h>

#include "../h/common.h"
#include "../h/dos.h"
#include "../h/proto.h"

/*
 ******	Protótipos de funções ***********************************
 */
void	print_sb (int);
void	print_uni (void);
void	do_param_help (void);

/*
 ****************************************************************
 *	Imprime os parâmetros do sistema de arquivos		*
 ****************************************************************
 */
void
do_param (int argc, const char *argv[])
{
	int		opt;
	const DOSSB	*sp = &sb;
	const UNI	*up = &uni;
	char		fs_partnm[32];

	/*
	 *	Analisa as opções
	 */
	while ((opt = cmd_getopt (argv, "vH")) >= 0)
	{
		switch (opt)
		{
		    case 'v':			/* Verboso */
			cmd_vflag++;
			break;

		    case 'H':			/* Help */
			do_param_help ();
			return;

		    default:			/* Erro */
			fprintf (stderr, "\n");
			do_param_help ();
			return;
		}
	}

	/*
	 *	Pequena consistência
	 */
	argc -= cmd_optind;
	argv += cmd_optind;

	if (argc != 0)
		{ do_param_help (); return; }


	/*
	 *	obtém o nome do tipo da partição do disco
	 */
	fs_partnm[0] = '\0';
	kcntl (K_GET_PARTNM, dev_type, fs_partnm);

	/*
	 *	Imprime os parâmetros
	 */
	printf ("Dispositivo:        = \"%s\"", dev_nm);

	if (fs_partnm[0] != '\0')
		printf (" (%s)", fs_partnm);

	printf
	(	"\n"
		"Identificação:      = %-8.8s, meio = 0x%02X\n"
		"Tamanho do sistema: = %d blocos (%s)\n"
		"FAT:                = FAT%d (%d x %d blocos)\n"
		"CLUSTER:            = %d CLUSTERs de %d bloco%s (%d bytes)\n",

		sp->s_id, sp->s_media,
		up->u_end_fs_blkno, cv_blocks_to_KB_MB_or_GB (up->u_end_fs_blkno),
		up->u_fat_bits, up->u_n_fats, up->u_sectors_per_fat,
		up->u_n_clusters, up->u_sectors_per_cluster, plural (up->u_sectors_per_cluster),
		BLTOBY (up->u_sectors_per_cluster)
	);

	if (up->u_fat_bits != 32)
	{
		printf
		(	"Diretório RAIZ:     = %d entradas, setor inicial = %d\n",
			up->u_root_dir_entries, up->u_root_dir_blkno
		);
	}
	else
	{
		printf
		(	"Diretório RAIZ:     = CLUSTER inicial = %d\n",
			up->u_root_cluster
		);
	}

	printf
	(	"Setores, trilhas:   = %d bytes, %d setores, %d cabeças\n",
		up->u_bytes_per_sector, up->u_sectors_per_track, up->u_n_heads
	);

	if (cmd_vflag)
		{ print_sb (up->u_fat_bits); print_uni (); }

}	/* end do_param */

/*
 ****************************************************************
 *	Imprime o Super-bloco					*
 ****************************************************************
 */
void
print_sb (int fat_bits)
{
	DOSSB		*sp = &sb;

	printf
	(
		"\n"
		"(Reservado)         = %02X %02X %02X\n"
		"Identificação       = %-8.8s\n"

		"Bytes por setor     = %d\n"
		"Setores por CLUSTER = %d\n"
		"Início da FAT1      = %d\n"
		"No. de FATs         = %d\n"
		"Entradas da RAIZ    = %d\n"
		"Tamanho (curto)     = %d\n"
		"Tipo do meio        = %02X\n"
		"Setores/FAT (curto) = %d\n"
		"Setores/Trilha      = %d\n"
		"No. de cabeças      = %d\n"
		"Setores ocultos     = %d\n"
		"(Reservado)         = %04X\n"
		"Tamanho (longo)     = %d (%s)\n",

		sp->s_reser0[0], sp->s_reser0[1], sp->s_reser0[2],
		sp->s_id,
		GET_SHORT (sp->s_bytes_per_sector),
		sp->s_sectors_per_cluster,
		GET_SHORT (sp->s_fat_offset),
		sp->s_n_fats,
		GET_SHORT (sp->s_root_dir_entries),
		GET_SHORT (sp->s_little_size),
		sp->s_media,
		GET_SHORT (sp->s_little_sectors_per_fat),
		GET_SHORT (sp->s_sectors_per_track),
		GET_SHORT (sp->s_n_heads),
		GET_SHORT (sp->s_hidden_sectors),
		GET_SHORT (sp->s_reser_0),
		GET_LONG (sp->s_big_size), cv_blocks_to_KB_MB_or_GB (GET_LONG (sp->s_big_size))
	);

	if (fat_bits == 32) printf
	(
		"Setores/FAT (longo) = %d\n"
		"Indicadores         = %04X\n"
		"Versão              = %d\n"
		"Início da raiz      = %d\n"
		"Bloco de info       = %d\n"
		"Cópia do boot       = %d\n"

		"Dispositivo físico  = %02X\n"
		"Assinatura          = %02X\n"
		"Volume              = %P\n"
		"Rótulo              = %-11.11s\n"
		"Mensagem da FAT     = %-5.5s\n",

		GET_LONG (sp->s_big_sectors_per_fat),
		GET_SHORT (sp->s_flags),
		GET_SHORT (sp->s_version),
		GET_LONG (sp->s_root_cluster),
		GET_SHORT (sp->s_fs_info),
		GET_SHORT (sp->s_sb_backup),

		sp->s_32_phys_drive,
		sp->s_32_signature,
		long_endian_cv (*(long *)sp->s_32_vol_ser_no),
		sp->s_32_label,
		sp->s_32_fat_msg
	);
	else printf
	(
		"Dispositivo físico  = %02X\n"
		"Assinatura          = %02X\n"
		"Volume              = %P\n"
		"Rótulo              = %-11.11s\n"
		"Mensagem da FAT     = %-5.5s\n",

		sp->s_16_phys_drive,
		sp->s_16_signature,
		long_endian_cv (*(long *)sp->s_16_vol_ser_no),
		sp->s_16_label,
		sp->s_16_fat_msg
	);

}	/* print_sb */

/*
 ****************************************************************
 *	Imprime o Bloco de controle				*
 ****************************************************************
 */
void
print_uni (void)
{
	const UNI	*up = &uni;

	printf
	(
		"\n"
		"dev_type         	= %02X\n"
		"u_sectors_per_track	= %d\n"
		"u_bytes_per_sector	= %d\n"
		"u_n_heads		= %d\n"
		"u_fat1_blkno		= %d\n"
		"u_end_fat1_blkno	= %d\n"
		"u_fat2_blkno		= %d\n"
		"u_end_fat2_blkno	= %d\n"
		"u_root_dir_blkno	= %d\n"
		"u_end_root_dir_blkno	= %d\n"
		"u_end_fs_blkno		= %d (%s)\n"
		"u_n_fats		= %d\n"
		"u_sectors_per_fat	= %d\n"
		"u_n_clusters		= %d\n"
		"u_sectors_per_cluster	= %d\n"
		"u_root_dir_entries	= %d\n"
		"u_cluster_0_blkno	= %d\n"
		"u_bytes_per_cluster	= %d\n"
		"u_fat_bits		= %d\n"
		"u_root_cluster		= %d\n"
		"u_sb_backup		= %d\n"
		"u_fs_info		= %d\n"
		"u_disk_infree		= %d\n"
		"u_disk_inxtfree		= %d\n"
		"u_infree		= %d\n"
		"u_inxtfree		= %d\n",

		dev_type,
		up->u_sectors_per_track,
		up->u_bytes_per_sector,
		up->u_n_heads,
		up->u_fat1_blkno,
		up->u_end_fat1_blkno,
		up->u_fat2_blkno,
		up->u_end_fat2_blkno,
		up->u_root_dir_blkno,
		up->u_end_root_dir_blkno,
		up->u_end_fs_blkno,
		cv_blocks_to_KB_MB_or_GB (up->u_end_fs_blkno),
		up->u_n_fats,
		up->u_sectors_per_fat,
		up->u_n_clusters,
		up->u_sectors_per_cluster,
		up->u_root_dir_entries,
		up->u_cluster_0_blkno,
		up->u_bytes_per_cluster,
		up->u_fat_bits,
		up->u_root_cluster,
		up->u_sb_backup,
		up->u_fs_info,
		up->u_disk_infree,
		up->u_disk_inxtfree,
		up->u_infree,
		up->u_inxtfree
	);

}	/* end print_uni */

/*
 ****************************************************************
 *	Resumo de utilização do programa			*
 ****************************************************************
 */
void
do_param_help (void)
{
	fprintf
	(	stderr,
		"%s - imprime os parâmetros do sistema de arquivos DOS\n"
		"\nSintaxe:\n"
		"\t%s [-v]\n",
		cmd_nm, cmd_nm
	);

	fprintf
	(	stderr,
		"\nOpções:"
		"\t-v: Imprime também os parâmetros originais\n"
	);

}	/* end help */
