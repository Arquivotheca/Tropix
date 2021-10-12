/*
 ****************************************************************
 *								*
 *			do_mkfs.c				*
 *								*
 *	Cria um sistema de arquivos DOS				*
 *								*
 *	Versão	3.0.0, de 15.11.93				*
 *		4.9.0, de 06.09.06				*
 *								*
 *	Módulo: DOSMP						*
 *		Utilitários Sofisticados			*
 *		Categoria B					*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 2006 NCE/UFRJ - tecle "man licença"	*
 * 		Inspirado no PLDOS de Norival Ribeiro Figueira	*
 * 								*
 ****************************************************************
 */

#include <sys/common.h>
#include <sys/kcntl.h>
#include <sys/disktb.h>
#include <sys/stat.h>
#include <sys/syscall.h>

#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <time.h>

#include "../h/common.h"
#include "../h/dos.h"
#include "../h/proto.h"

/*
 ******	Definições globais **************************************
 */
typedef struct disktb	DISKTB;

/*
 ******	Valores Importantes *************************************
 */

/* No. de FATs (sempre 2) */
 
enum { N_FATs			= 2 };

/* No. total máximo de blocos de um sistema com FAT de 12 bits */
 
enum { MAX_TOTAL_FAT_12_BITS	= (1 + 2 * 12 + 32 + (0xFF1 - 2)) };

/* No. máximo de CLUSTERs de um sistema com FAT de 16 bits */
 
enum { MAX_CLUSTER_FAT_16_BITS	= (0xFFF1 - 2) };

/* No. máximo do início (DOSSB, FAT e ROOT) de um sistema com FAT de 16 bits */
 
enum { MAX_PRO_FAT_16_BITS	= (1 + 2 * 256 + 32) };

/* Bloco de informação  da FAT de 32 bits */
 
enum { FAT_32_SB_INFO		= 1 };

/* Duplicata do boot com FAT de 32 bits */
 
enum { FAT_32_SB_BACKUP		= 6 };

/* Começo da FAT1 com FAT de 12/16/32 bits */
 
enum {	FAT_12_OFFSET = 1, FAT_16_OFFSET = 1, FAT_32_OFFSET = FAT_32_SB_BACKUP + 1 };

/* Máscara para alinhar CLUSTERs */

enum {	BL4BLMASK		= BL4SZ/BLSZ - 1 };

/* Número do CLUSTER da RAIZ FAT de 32 bits */
 
enum { FAT_32_ROOT_CLUSTER	= 2 };

/*
 ******	Tabela de parâmetros de alguns dispositivos *************
 */
typedef struct
{
	daddr_t	d_size;		/* No. total de blocos */
	char	d_media;	/* Definição do meio */
	char	d_sec_clus;	/* Setores por cluster */
	char	d_sec_root;	/* Setores do diretório raiz */

}	DEVDEF;

const DEVDEF	devdef[] =
{
					/*	  blocos/FAT  CLUSTERs */

	 720,	0xFD,	2,	7,	/* 360  KB,	 2,	354 */
	1440,	0xF9,	2,	7,	/* 720  KB,	 3,	713 */
	2400,	0xF9,	1,	14,	/* 1.2  MB,	 7,	2371 */
	2880,	0xF0,	1,	14,	/* 1.44 MB,	 9,	2847 */
	3444,	0xF0,	1,	16,	/* 1.72 MB,	10,	3407 */
	0
};

/*
 ****** Superbloco com "boot" FAT16 *****************************
 */
const char	boot_code_16[BLSZ] =
{
/* s_reser0 */			0xEB , 0x3C, 0x90,

/* s_id */			'M', 'S', 'W', 'I', 'N', '4', '.', '1',
/* s_bytes_per_sector */	0, 2,
/* s_sectors_per_cluster */	0,
/* s_fat_offset */		0, 0, 
/* s_n_fats */			2,
/* s_root_dir_entries */	0, 0,
/* s_little_size */		0, 0,
/* s_media */			0,
/* s_little_sectors_per_fat */ 	0, 0, 
/* s_sectors_per_track */ 	0, 0,
/* s_n_heads */			0, 0,
/* s_hidden_sectors */		0, 0,
/* s_reser0 */			0, 0,
/* s_big_size */		0, 0, 0, 0,
/* s_phys_drive */		0x80,
/* s_reser1 */			0,
/* s_signature */		0x29,
/* s_vol_ser_no */		0, 0, 0, 0,
/* s_label */			' ',  ' ',  ' ',  ' ',  ' ', ' ',
				' ',  ' ',  ' ',  ' ',  ' ',
/* s_fat_msg */			'F',  'A', 'T',  '1',  '6',  ' ',  ' ',  ' ',

/* s_boot_code */		0x33, 0xC9, 
				0x8E, 0xD1, 0xBC, 0xFC, 0x7B, 0x16, 0x07, 0xBD,
				0x78, 0x00, 0xC5, 0x76, 0x00, 0x1E, 0x56, 0x16,
				0x55, 0xBF, 0x22, 0x05, 0x89, 0x7E, 0x00, 0x89,
				0x4E, 0x02, 0xB1, 0x0B, 0xFC, 0xF3, 0xA4, 0x06,
				0x1F, 0xBD, 0x00, 0x7C, 0xC6, 0x45, 0xFE, 0x0F,
				0x38, 0x4E, 0x24, 0x7D, 0x20, 0x8B, 0xC1, 0x99,
				0xE8, 0x7E, 0x01, 0x83, 0xEB, 0x3A, 0x66, 0xA1,
				0x1C, 0x7C, 0x66, 0x3B, 0x07, 0x8A, 0x57, 0xFC,
				
				0x75, 0x06, 0x80, 0xCA, 0x02, 0x88, 0x56, 0x02,
				0x80, 0xC3, 0x10, 0x73, 0xED, 0x33, 0xC9, 0xFE,
				0x06, 0xD8, 0x7D, 0x8A, 0x46, 0x10, 0x98, 0xF7,
				0x66, 0x16, 0x03, 0x46, 0x1C, 0x13, 0x56, 0x1E,
				0x03, 0x46, 0x0E, 0x13, 0xD1, 0x8B, 0x76, 0x11,
				0x60, 0x89, 0x46, 0xFC, 0x89, 0x56, 0xFE, 0xB8,
				0x20, 0x00, 0xF7, 0xE6, 0x8B, 0x5E, 0x0B, 0x03,
				0xC3, 0x48, 0xF7, 0xF3, 0x01, 0x46, 0xFC, 0x11,
				0x4E, 0xFE, 0x61, 0xBF, 0x00, 0x07, 0xE8, 0x28,
				0x01, 0x72, 0x3E, 0x38, 0x2D, 0x74, 0x17, 0x60,
				0xB1, 0x0B, 0xBE, 0xD8, 0x7D, 0xF3, 0xA6, 0x61,
				0x74, 0x3D, 0x4E, 0x74, 0x09, 0x83, 0xC7, 0x20,
				0x3B, 0xFB, 0x72, 0xE7, 0xEB, 0xDD, 0xFE, 0x0E,
				0xD8, 0x7D, 0x7B, 0xA7, 0xBE, 0x7F, 0x7D, 0xAC,
				0x98, 0x03, 0xF0, 0xAC, 0x98, 0x40, 0x74, 0x0C,
				0x48, 0x74, 0x13, 0xB4, 0x0E, 0xBB, 0x07, 0x00,
				
				0xCD, 0x10, 0xEB, 0xEF, 0xBE, 0x82, 0x7D, 0xEB,
				0xE6, 0xBE, 0x80, 0x7D, 0xEB, 0xE1, 0xCD, 0x16,
				0x5E, 0x1F, 0x66, 0x8F, 0x04, 0xCD, 0x19, 0xBE,
				0x81, 0x7D, 0x8B, 0x7D, 0x1A, 0x8D, 0x45, 0xFE,
				0x8A, 0x4E, 0x0D, 0xF7, 0xE1, 0x03, 0x46, 0xFC,
				0x13, 0x56, 0xFE, 0xB1, 0x04, 0xE8, 0xC2, 0x00,
				0x72, 0xD7, 0xEA, 0x00, 0x02, 0x70, 0x00, 0x52,
				0x50, 0x06, 0x53, 0x6A, 0x01, 0x6A, 0x10, 0x91,
				0x8B, 0x46, 0x18, 0xA2, 0x26, 0x05, 0x96, 0x92,
				0x33, 0xD2, 0xF7, 0xF6, 0x91, 0xF7, 0xF6, 0x42,
				0x87, 0xCA, 0xF7, 0x76, 0x1A, 0x8A, 0xF2, 0x8A,
				0xE8, 0xC0, 0xCC, 0x02, 0x0A, 0xCC, 0xB8, 0x01,
				0x02, 0x80, 0x7E, 0x02, 0x0E, 0x75, 0x04, 0xB4,
				0x42, 0x8B, 0xF4, 0x8A, 0x56, 0x24, 0xCD, 0x13,
				0x61, 0x61, 0x72, 0x0A, 0x40, 0x75, 0x01, 0x42,
				0x03, 0x5E, 0x0B, 0x49, 0x75, 0x77, 0xC3, 0x03,
				
				0x18, 0x01, 0x27,

/* s_msgs */			'\r', '\n',
				'I', 'n', 'v', 'a',  'l', 'i', 'd', ' ',
				's', 'y', 's', 't', 'e', 'm', ' ',
				'd', 'i', 's', 'k', 0xFF,
	
				'\r', '\n',
				'D', 'i', 's', 'k', ' ', 'I', '/', 'O', ' ',
				'e', 'r', 'r', 'o', 'r', 0xFF,
	
				'\r', '\n',
				'R', 'e', 'p', 'l', 'a', 'c', 'e', ' ',
				't', 'h', 'e', ' ', 'd', 'i', 's', 'k', ',', ' ',
				'a', 'n', 'd', ' ', 't', 'h', 'e', 'n', ' ', 
				'p', 'r', 'e', 's', 's', ' ', 'a', 'n', 'y', ' ',
				'k', 'e', 'y', '\r', '\n', '\0', '\0',
	
				'I', 'O', ' ', ' ', ' ', ' ', ' ', ' ', 'S', 'Y', 'S',
				'M', 'S', 'D', 'O', 'S', ' ', ' ', ' ', 'S', 'Y', 'S',
	
				0x7F, 0x01,
				0x00, 0x41, 0xBB, 0x00, 0x07, 0x60, 0x66, 0x6A,
				0x00, 0xE9, 0x3B, 0xFF, 0x00, 0x00, 0x55, 0xAA
};

/*
 ****** Superbloco com "boot" FAT32 *****************************
 */
const char	boot_code_32[BLSZ] =
{
/* s_reser0 */			0xEB, 0x58, 0x90,
/* s_id */ 			'M',  'S',  'W',  'I',  'N', '4', '.', '1',
/* s_bytes_per_sector */	0, 2,
/* s_sectors_per_cluster */	8,
/* s_fat_offset */		0, 0, 
/* s_n_fats */			2,
/* s_root_dir_entries */	0, 0,
/* s_little_size */		0, 0,
/* s_media */			0,
/* s_little_sectors_per_fat */ 	0, 0, 
/* s_sectors_per_track */ 	0, 0,
/* s_n_heads */			0, 0,
/* s_hidden_sectors */		0, 0,
/* s_reser_0 */			0, 0,
/* s_big_size */		0, 0, 0, 0,
/* s_big_sectors_per_fat */	0, 0, 0, 0,
/* s_flags */			0, 0,
/* s_version */			0, 0,
/* s_root_cluster */		2, 0, 0, 0,
/* s_fs_info */			1, 0,
/* s_sb_backup */		6, 0,
/* s_reser_1 */			0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,

/* s_phys_drive */		0x80,
/* s_reser_2 */			0,
/* s_signature */		0x29,
/* s_vol_ser_no */		0, 0, 0, 0,
/* s_label */			' ',  ' ',  ' ',  ' ',  ' ', ' ',
				' ',  ' ',  ' ',  ' ',  ' ',
/* s_fat_msg */			'F',  'A', 'T',  '3',  '2',  ' ',  ' ',  ' ',

/* s_boot_code */
				0xFA, 0x33, 0xC9, 0x8E, 0xD1, 0xBC,
				0xF8, 0x7B, 0x8E, 0xC1, 0xBD, 0x78, 0x00, 0xC5,
				0x76, 0x00, 0x1E, 0x56, 0x16, 0x55, 0xBF, 0x22,
				0x05, 0x89, 0x7E, 0x00, 0x89, 0x4E, 0x02, 0xB1,
				0x0B, 0xFC, 0xF3, 0xA4, 0x8E, 0xD9, 0xBD, 0x00,
				0x7C, 0xC6, 0x45, 0xFE, 0x0F, 0x8B, 0x46, 0x18,
				0x88, 0x45, 0xF9, 0x38, 0x4E, 0x40, 0x7D, 0x25,
				0x8B, 0xC1, 0x99, 0xBB, 0x00, 0x07, 0xE8, 0x97,
				0x00, 0x72, 0x1A, 0x83, 0xEB, 0x3A, 0x66, 0xA1,
				0x1C, 0x7C, 0x66, 0x3B, 0x07, 0x8A, 0x57, 0xFC,
				0x75, 0x06, 0x80, 0xCA, 0x02, 0x88, 0x56, 0x02,
				0x80, 0xC3, 0x10, 0x73, 0xED, 0xBF, 0x02, 0x00,
				0x83, 0x7E, 0x16, 0x00, 0x75, 0x45, 0x8B, 0x46,
				0x1C, 0x8B, 0x56, 0x1E, 0xB9, 0x03, 0x00, 0x49,
				0x40, 0x75, 0x01, 0x42, 0xBB, 0x00, 0x7E, 0xE8,
				0x5F, 0x00, 0x73, 0x26, 0xB0, 0xF8, 0x4F, 0x74,
				0x1D, 0x8B, 0x46, 0x32, 0x33, 0xD2, 0xB9, 0x03,
				0x00, 0x3B, 0xC8, 0x77, 0x1E, 0x8B, 0x76, 0x0E,
				0x3B, 0xCE, 0x73, 0x17, 0x2B, 0xF1, 0x03, 0x46,
				0x1C, 0x13, 0x56, 0x1E, 0xEB, 0xD1, 0x73, 0x0B,
				0xEB, 0x27, 0x83, 0x7E, 0x2A, 0x00, 0x77, 0x03,
				0xE9, 0xFD, 0x02, 0xBE, 0x7E, 0x7D, 0xAC, 0x98,
				0x03, 0xF0, 0xAC, 0x84, 0xC0, 0x74, 0x17, 0x3C,
				0xFF, 0x74, 0x09, 0xB4, 0x0E, 0xBB, 0x07, 0x00,
				0xCD, 0x10, 0xEB, 0xEE, 0xBE, 0x81, 0x7D, 0xEB,
				0xE5, 0xBE, 0x7F, 0x7D, 0xEB, 0xE0, 0x98, 0xCD,
				0x16, 0x5E, 0x1F, 0x66, 0x8F, 0x04, 0xCD, 0x19,
				0x41, 0x56, 0x66, 0x6A, 0x00, 0x52, 0x50, 0x06,
				0x53, 0x6A, 0x01, 0x6A, 0x10, 0x8B, 0xF4, 0x60,
				0x80, 0x7E, 0x02, 0x0E, 0x75, 0x04, 0xB4, 0x42,
				0xEB, 0x1D, 0x91, 0x92, 0x33, 0xD2, 0xF7, 0x76,
				0x18, 0x91, 0xF7, 0x76, 0x18, 0x42, 0x87, 0xCA,
				0xF7, 0x76, 0x1A, 0x8A, 0xF2, 0x8A, 0xE8, 0xC0,
				0xCC, 0x02, 0x0A, 0xCC, 0xB8, 0x01, 0x02, 0x8A,
				0x56, 0x40, 0xCD, 0x13, 0x61, 0x8D, 0x64, 0x10,
				0x5E, 0x72, 0x0A, 0x40, 0x75, 0x01, 0x42, 0x03,
				0x5E, 0x0B, 0x49, 0x75, 0xB4, 0xC3, 0x03, 0x18,
				0x01, 0x27,

/* s_msgs */			'\r', '\n',
				'I', 'n', 'v', 'a',  'l', 'i', 'd', ' ',
				's', 'y', 's', 't', 'e', 'm', ' ',
				'd', 'i', 's', 'k', 0xFF,

				'\r', '\n',
				'D', 'i', 's', 'k', ' ', 'I', '/', 'O', ' ',
				'e', 'r', 'r', 'o', 'r', 0xFF,

				'\r', '\n',
				'R', 'e', 'p', 'l', 'a', 'c', 'e', ' ',
				't', 'h', 'e', ' ', 'd', 'i', 's', 'k', ',', ' ',
				'a', 'n', 'd', ' ', 't', 'h', 'e', 'n', ' ', 
				'p', 'r', 'e', 's', 's', ' ', 'a', 'n', 'y', ' ',
				'k', 'e', 'y', '\r', '\n', 0x00, 0x00, 0x00,

				'I', 'O', ' ', ' ', ' ', ' ', ' ', ' ', 'S', 'Y', 'S',
				'M', 'S', 'D', 'O', 'S', ' ', ' ', ' ', 'S', 'Y', 'S',

				0x7E, 0x01, 0x00,

				'W', 'I', 'N', 'B', 'O', 'O', 'T', ' ', 'S', 'Y', 'S',

				0x00, 0x00, 0x55, 0xAA
};

const char	boot_code_32_cont[BLSZ] =
{
				0xFA, 0x66, 0x0F, 0xB6, 0x46, 0x10, 0x66, 0x8B,
				0x4E, 0x24, 0x66, 0xF7, 0xE1, 0x66, 0x03, 0x46,
				0x1C, 0x66, 0x0F, 0xB7, 0x56, 0x0E, 0x66, 0x03,
				0xC2, 0x33, 0xC9, 0x66, 0x89, 0x46, 0xFC, 0x66,
				0xC7, 0x46, 0xF8, 0xFF, 0xFF, 0xFF, 0xFF, 0xFA,
				0x66, 0x8B, 0x46, 0x2C, 0x66, 0x83, 0xF8, 0x02,
				0x0F, 0x82, 0xCF, 0xFC, 0x66, 0x3D, 0xF8, 0xFF,
				0xFF, 0x0F, 0x0F, 0x83, 0xC5, 0xFC, 0x66, 0x0F,
				0xA4, 0xC2, 0x10, 0xFB, 0x52, 0x50, 0xFA, 0x66,
				0xC1, 0xE0, 0x10, 0x66, 0x0F, 0xAC, 0xD0, 0x10,
				0x66, 0x83, 0xE8, 0x02, 0x66, 0x0F, 0xB6, 0x5E,
				0x0D, 0x8B, 0xF3, 0x66, 0xF7, 0xE3, 0x66, 0x03,
				0x46, 0xFC, 0x66, 0x0F, 0xA4, 0xC2, 0x10, 0xFB,
				0xBB, 0x00, 0x07, 0x8B, 0xFB, 0xB9, 0x01, 0x00,
				0xE8, 0xBE, 0xFC, 0x0F, 0x82, 0xAA, 0xFC, 0x38,
				0x2D, 0x74, 0x1E, 0xB1, 0x0B, 0x56, 0xBE, 0xD8,
				
				0x7D, 0xF3, 0xA6, 0x5E, 0x74, 0x19, 0x03, 0xF9,
				0x83, 0xC7, 0x15, 0x3B, 0xFB, 0x72, 0xE8, 0x4E,
				0x75, 0xD6, 0x58, 0x5A, 0xE8, 0x66, 0x00, 0x72,
				0xAB, 0x83, 0xC4, 0x04, 0xE9, 0x64, 0xFC, 0x83,
				0xC4, 0x04, 0x8B, 0x75, 0x09, 0x8B, 0x7D, 0x0F,
				0x8B, 0xC6, 0xFA, 0x66, 0xC1, 0xE0, 0x10, 0x8B,
				0xC7, 0x66, 0x83, 0xF8, 0x02, 0x72, 0x3B, 0x66,
				0x3D, 0xF8, 0xFF, 0xFF, 0x0F, 0x73, 0x33, 0x66,
				0x48, 0x66, 0x48, 0x66, 0x0F, 0xB6, 0x4E, 0x0D,
				0x66, 0xF7, 0xE1, 0x66, 0x03, 0x46, 0xFC, 0x66,
				0x0F, 0xA4, 0xC2, 0x10, 0xFB, 0xBB, 0x00, 0x07,
				0x53, 0xB9, 0x04, 0x00, 0xE8, 0x52, 0xFC, 0x5B,
				0x0F, 0x82, 0x3D, 0xFC, 0x81, 0x3F, 0x4D, 0x5A,
				0x75, 0x08, 0x81, 0xBF, 0x00, 0x02, 0x42, 0x4A,
				0x74, 0x06, 0xBE, 0x80, 0x7D, 0xE9, 0x0E, 0xFC,
				0xEA, 0x00, 0x02, 0x70, 0x00, 0x03, 0xC0, 0x13,
				
				0xD2, 0x03, 0xC0, 0x13, 0xD2, 0xE8, 0x18, 0x00,
				0xFA, 0x26, 0x66, 0x8B, 0x01, 0x66, 0x25, 0xFF,
				0xFF, 0xFF, 0x0F, 0x66, 0x0F, 0xA4, 0xC2, 0x10,
				0x66, 0x3D, 0xF8, 0xFF, 0xFF, 0x0F, 0xFB, 0xC3,
				0xBF, 0x00, 0x7E, 0xFA, 0x66, 0xC1, 0xE0, 0x10,
				0x66, 0x0F, 0xAC, 0xD0, 0x10, 0x66, 0x0F, 0xB7,
				0x4E, 0x0B, 0x66, 0x33, 0xD2, 0x66, 0xF7, 0xF1,
				0x66, 0x3B, 0x46, 0xF8, 0x74, 0x44, 0x66, 0x89,
				0x46, 0xF8, 0x66, 0x03, 0x46, 0x1C, 0x66, 0x0F,
				0xB7, 0x4E, 0x0E, 0x66, 0x03, 0xC1, 0x66, 0x0F,
				0xB7, 0x5E, 0x28, 0x83, 0xE3, 0x0F, 0x74, 0x16,
				0x3A, 0x5E, 0x10, 0x0F, 0x83, 0xA4, 0xFB, 0x52,
				0x66, 0x8B, 0xC8, 0x66, 0x8B, 0x46, 0x24, 0x66,
				0xF7, 0xE3, 0x66, 0x03, 0xC1, 0x5A, 0x52, 0x66,
				0x0F, 0xA4, 0xC2, 0x10, 0xFB, 0x8B, 0xDF, 0xB9,
				0x01, 0x00, 0xE8, 0xB4, 0xFB, 0x5A, 0x0F, 0x82,
				
				0x9F, 0xFB, 0xFB, 0x8B, 0xDA, 0xC3, 0x00, 0x00,
				0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
				0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
				0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
				0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
				0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
				0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
				0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
				0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
				0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
				0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
				0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
				0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
				0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
				0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
				0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x55, 0xAA
};

/*
 ******	Protótipos de funções ***********************************
 */
void		mk_one_fs (const char *, daddr_t, int);
int		is_power_of_2 (int n);
void		do_mkfs_help (void);

/*
 ****************************************************************
 *	Cria um sistema de arquivos DOS				*
 ****************************************************************
 */
void
do_mkfs (int argc, const char *argv[])
{
	int		opt, i;
	const char	*dev_str = NOSTR, *sz_str = NOSTR, *cluster_str = NOSTR;
	char		dev_no_nm[4];
	daddr_t		sz;
	int		cluster_sz;
	const char	*str;

	/*
	 *	Analisa as opções
	 */
	while ((opt = cmd_getopt (argv, "s:c:v0123456789H")) >= 0)
	{
		if (opt >= '0' && opt <= '9')
		{
			if (dev_str != NOSTR)
				do_mkfs_help ();

			dev_no_nm[0] = '-';
			dev_no_nm[1] = opt;
			dev_no_nm[2] = '\0';

			dev_str = dev_no_nm;
		}
		else switch (opt)
		{
		    case 'v':			/* Verboso */
			cmd_vflag++;
			break;

		    case 's':			/* Tamanho do sistema de arquivos */
			sz_str = cmd_optarg;
			break;

		    case 'c':			/* Tamanho do CLUSTER */
			cluster_str = cmd_optarg;
			break;

		    case 'H':			/* Help */
			do_mkfs_help ();
			return;

		    default:			/* Erro */
			fprintf (stderr, "\n");
			do_mkfs_help ();
			return;
		}
	}

	/*
	 *	Pequena consistência
	 */
	argc -= cmd_optind;
	argv += cmd_optind;

#if (0)	/*******************************************************/
	if (!S_ISBLK (dev_mode) && !S_ISCHR (dev_mode))
	{
		printf
		(	"%s: Não posso criar sistemas de "
			"arquivos DOS, pois não é um arquivo "
			"especial de blocos ou caracteres\n",
			cmd_nm
		);
		return;
	}
#endif	/*******************************************************/

	if (rflag)
	{
		printf
		(	"%s: Não posso criar sistemas de "
			"arquivos DOS, pois foi dada a opção \"-r\"\n",
			cmd_nm
		);
		return;
	}

	if   (argc == 1)
	{
		if (dev_str != NOSTR)
			{ do_mkfs_help (); return; }

		dev_str = argv[0];
	}
	elif (argc > 1)
	{
		do_mkfs_help ();
		return;
	}

	/*
	 *	Analisa os números dados
	 */
	sz = 0;			/* Tamanho ainda desconhecido */

	if (sz_str)
	{
		if ((i = strtol (sz_str, &str, 0)) <= 0 || *str)
		{
			printf
			(	"%s: Tamanho inválido: \"%s\"\n",
				cmd_nm, sz_str
			);
			return;
		}

		sz = i;
	}

	cluster_sz = 0;		/* Tamanho do CLUSTER ainda desconhecido */

	if (cluster_str)
	{
		if ((i = strtol (cluster_str, &str, 0)) <= 0 || !is_power_of_2 (i) || *str)
		{
			printf
			(	"%s: Tamanho inválido de CLUSTER: \"%s\"\n",
				cmd_nm, cluster_str
			);
			return;
		}

		cluster_sz = i;
	}

	/*
	 *	Cria o sistema de arquivos
	 */
	mk_one_fs (dev_str, sz, cluster_sz);

}	/* end do_mkfs */

/*
 ****************************************************************
 *	Cria o sistema de arquivos				*
 ****************************************************************
 */
void
mk_one_fs (const char *dev_str, daddr_t sz, int cluster_sz)
{
	DOSSB		*sp = &sb;
	const char	*boot_code_ptr;
	const DEVDEF	*dp;
	int		i;
	long		n_clus, sec_fat, fat_bits;
	int		u_root_dir_entries, n_hidden;
	int		overflow = 0, not_used, fat_offset;
	int		one_fat_complex, residual_blocks;
	DEVDEF		dd;
	DISKTB		ud;
	char		fs_partnm[32];

	/*
	 *	Consistência
	 */
	if (simple_open (dev_str, GET_SHORT (sp->s_n_heads)) < 0)
		return;

	if (dev_ro)
	{
		printf
		(	"%s: Não posso criar o sistema de arquivos DOS "
			"(proteção de escrita)\n",
			cmd_nm
		);
		return;
	}

	if (!S_ISBLK (dev_mode) && !S_ISCHR (dev_mode))
	{
		printf
		(	"%s: Não posso criar sistemas de "
			"arquivos DOS, pois não é um arquivo "
			"especial de blocos ou caracteres\n",
			cmd_nm
		);
		return;
	}

	/*
	 *	Obtém alguns dados
	 */
	if (kcntl (K_GET_DEV_TB, dev_dev, &ud) < 0)
	{
		printf
		(	"%s: Não consegui obter o tamanho do dispositivo \"%s\"\n",
			cmd_nm, dev_nm
		);
		return;
	}

	/*
	 *	Procura se o dispositivo está na tabela
	 */
	for (dp = devdef; /* abaixo */; dp++)
	{
		if (dp->d_size == 0)		/* Não está na tabela */
		{
			if   (sz == 0)
			{
				sz = ud.p_size;
			}
			elif (sz > ud.p_size)
			{
				printf
				(	"%s: O tamanho dado (%d) é maior do que o tamanho "
					"de \"%s\" (%d)\n",
					cmd_nm, sz, dev_nm, ud.p_size
				);
				return;
			}

			dd.d_size	= sz;
			dd.d_media	= 0xF8;		/* Para disco rígido */
			dd.d_sec_clus	= 0;		/* A ser determinado */
			dd.d_sec_root	= 32;		/* Valor padrão */

			break;
		}

		if (dp->d_size == ud.p_size && ud.p_head == 2)	/* Disquete */
		{
			dd.d_size	= dp->d_size;
			dd.d_media	= dp->d_media;
			dd.d_sec_clus	= dp->d_sec_clus;
			dd.d_sec_root	= dp->d_sec_root;

			break;
		}

	}	/* end for (procurando o dispositivo na tabela) */

	/*
	 *	Determina o tipo de FAT (32, 16 ou 12)
	 */
	if   (ud.p_type == DOS_FAT32_G || ud.p_type == DOS_FAT32_L)
		{ fat_bits = 32; fat_offset = FAT_32_OFFSET;    dd.d_sec_root = 0; }
	elif (dd.d_size > MAX_TOTAL_FAT_12_BITS)
		{ fat_bits = 16; fat_offset = FAT_16_OFFSET; /* dd.d_sec_root = 32; */ }
	else
		{ fat_bits = 12; fat_offset = FAT_12_OFFSET; /* dd.d_sec_root = ...; */ }

	/*
	 *	Prepara o ponteiro para o superbloco com o protótipo
	 */
	if (fat_bits == 32)
		boot_code_ptr = boot_code_32;
	else
		boot_code_ptr = boot_code_16;

	/*
	 *	Calcula o No. de blocos por cluster (se não dado)
	 */
	if   (fat_bits == 32)
	{
		if ((dd.d_sec_clus = cluster_sz) == 0)
			dd.d_sec_clus = 8;
	}
	elif (dd.d_sec_clus == 0)
	{
		i = dd.d_size - MAX_PRO_FAT_16_BITS;
		i += MAX_CLUSTER_FAT_16_BITS - 1;
		i /= MAX_CLUSTER_FAT_16_BITS;

		/* Aparentemente, DOS começa com 4 blocos */

		for (dd.d_sec_clus = 4; /* abaixo */; dd.d_sec_clus <<= 1)
		{
			if (dd.d_sec_clus >= i)
				break;
		}
	}

	if (dd.d_sec_clus > 64)
	{
		fprintf
		(	stderr,
			"\n%s: CUIDADO: Partição DOS com CLUSTER maior do que 32 KB\n",
			pgname
		);
	}

	/*
	 *	Determina o tamanho da FAT (e o número de CLUSTERs)
	 */
	if   (fat_bits != 12)		 /* FAT de 16/32 bits */
	{
		dd.d_size &= ~BL4BLMASK;	/* Retira os blocos não alinhados */

		one_fat_complex = N_FATs + (BLSZ * 8 / fat_bits) * dd.d_sec_clus;
 
		sec_fat		= (dd.d_size - dd.d_sec_root - fat_offset) / one_fat_complex;
		residual_blocks	= (dd.d_size - dd.d_sec_root - fat_offset) % one_fat_complex;

		n_clus		= sec_fat * (BLSZ * 8 / fat_bits);
#ifdef	DEBUG
		printf
		(	"d_size = %d, one_fat_complex = %d, sec_fat = %d, residual_blocks = %d, n_clus = %d\n",
			dd.d_size, one_fat_complex, sec_fat, residual_blocks, n_clus
		);
#endif	DEBUG
		if (residual_blocks >= N_FATs + dd.d_sec_clus)
		{
			sec_fat += 1; n_clus += (residual_blocks - N_FATs) / dd.d_sec_clus;
#ifdef	DEBUG
			printf ("Aumentei de %d CLUSTERs\n", (residual_blocks - N_FATs) / dd.d_sec_clus);
#endif	DEBUG
		}

		fat_offset = dd.d_size - (n_clus * dd.d_sec_clus) - dd.d_sec_root - (N_FATs * sec_fat); 
#ifdef	DEBUG
		printf ("fat_offset = %d\n", fat_offset);
#endif	DEBUG
	}
	else				/* FAT de 12 bits */
	{
		sec_fat = 12;	/* Tamanho máximo para 12 bits */

		for (EVER)
		{
			n_clus  = dd.d_size;
			n_clus -= 1 + N_FATs * sec_fat + dd.d_sec_root;
			n_clus /= dd.d_sec_clus;

			not_used = sec_fat * BLSZ - ((n_clus + 2) * 3 + 1) / 2;
#ifdef	DEBUG
			printf ("sec_fat = %d, not_used = %d\n", sec_fat, not_used);
#endif	DEBUG
			if   (not_used < 0)
				{ sec_fat++; overflow++; }
			elif (not_used < BLSZ)
				break;
			elif (overflow)
				break;
			else /* not_used >= BLSZ  sem overflow */
				sec_fat--;
		}
	}

	/*
	 *	obtém o nome do tipo da partição do disco
	 */
	fs_partnm[0] = '\0';
	kcntl (K_GET_PARTNM, ud.p_type, fs_partnm);

	/*
	 *	Alguns cálculos
	 */
	if (fat_bits == 32)
		u_root_dir_entries = 0;
	else
		u_root_dir_entries = dd.d_sec_root * BLSZ / sizeof (DOSDIR);

	n_hidden = (ud.p_head == 2) ? 1 : ud.p_sect;

	/*
	 *	Imprime os valores obtidos
	 */
	if (cmd_vflag || ud.p_head != 2)
	{
		printf ("Dispositivo:        = \"%s\"", dev_nm);

		if (fs_partnm[0] != '\0')
			printf (" (%s)", fs_partnm);

		printf
		(	"\n"
			"Identificação:      = %-8.8s, meio = 0x%02X\n"
			"Tamanho do sistema: = %d blocos (%s)\n"
			"FAT:                = FAT%d (%d x %d blocos)\n"
			"CLUSTER:            = %d CLUSTERs de %d bloco%s (%d bytes)\n",

			&boot_code_ptr[offsetof (DOSSB, s_id)], dd.d_media,
			dd.d_size, cv_blocks_to_KB_MB_or_GB (dd.d_size),
			fat_bits, N_FATs, sec_fat,
			n_clus, dd.d_sec_clus, plural (dd.d_sec_clus),
			BLTOBY (dd.d_sec_clus)
		);

		if (fat_bits != 32)
		{
			printf
			(	"Diretório RAIZ:     = %d entradas, setor inicial = %d\n",
				u_root_dir_entries, 1 + N_FATs * sec_fat
			);
		}
		else
		{
			printf
			(	"Diretório RAIZ:     = CLUSTER inicial = %d\n",
				FAT_32_ROOT_CLUSTER
			);
		}

		printf
		(	"Setores, trilhas:   = %d bytes, %d setores, %d cabeças\n\n",
			BLSZ, ud.p_sect, ud.p_head
		);
	}

	/*
	 *	Verifica se o usuário concorda
	 */
	fprintf
	(	stderr,
		"%s: O conteúdo do dispositivo \"%s\" será perdido. "
		"Continua (n) ? ",
		cmd_nm, dev_nm
	);

	if (askyesno () <= 0)
		return;

	/*
	 *	Preenche os diversos campos
	 */
	memmove (sp, boot_code_ptr, BLSZ);

   /***	PUT_SHORT (BLSZ, sp->s_bytes_per_sector); ***/
	sp->s_sectors_per_cluster = dd.d_sec_clus;
	PUT_SHORT (fat_offset, sp->s_fat_offset);
   /***	sp->s_n_fats = N_FATs; ***/
	PUT_SHORT (u_root_dir_entries, sp->s_root_dir_entries);
   /***	PUT_SHORT (dd.d_size, sp->s_little_size); ***/
	sp->s_media = dd.d_media;
   /***	PUT_SHORT (sec_fat, sp->s_little_sectors_per_fat); ***/
	PUT_SHORT (ud.p_sect, sp->s_sectors_per_track);
	PUT_SHORT (ud.p_head, sp->s_n_heads);
	PUT_SHORT (n_hidden, sp->s_hidden_sectors);
   /***	PUT_LONG  (dd.d_size, sp->s_big_size); ***/

	if (dd.d_size > 0xFFFF)
		PUT_LONG  (dd.d_size, sp->s_big_size);
	else
		PUT_SHORT (dd.d_size, sp->s_little_size);

	/*
	 *	Ajuste finais
	 */
	switch (fat_bits)
	{
	    case 32:
		PUT_LONG  (sec_fat, sp->s_big_sectors_per_fat);
	   /***	PUT_LONG  (FAT_32_ROOT_CLUSTER, sp->s_root_cluster); ***/
	   /***	PUT_SHORT (FAT_32_SB_INFO, sp->s_fs_info); ***/
	   /***	PUT_SHORT (FAT_32_SB_BACKUP, sp->s_sb_backup); ***/
		PUT_LONG (time ((time_t *)NULL), sp->s_32_vol_ser_no);
		break;

	    case 16:
		PUT_SHORT (sec_fat, sp->s_little_sectors_per_fat);
		PUT_LONG (time ((time_t *)NULL), sp->s_16_vol_ser_no);
		break;

	    case 12:
		PUT_SHORT (sec_fat, sp->s_little_sectors_per_fat);
		sp->s_16_fat_msg[4] = '2';	/* FAT12 */
		PUT_LONG (time ((time_t *)NULL), sp->s_16_vol_ser_no);
		break;
	}

	/*
	 *	Escreve o SB
	 */
	bwrite (0, sp);

	if (fat_bits == 32)
		bwrite (FAT_32_SB_BACKUP, sp);

	/*
	 *	Grava o bloco de informações da FAT32
	 */
	if (fat_bits == 32)
	{
		FSINFO		*fsinfo;

		fsinfo = alloca (sizeof (FSINFO));

		memset (fsinfo, 0, sizeof (FSINFO));

		memmove (fsinfo->fs_isig1, "RRaA", 4);
		memmove (fsinfo->fs_isig2, "rrAa", 4);
		memmove (fsinfo->fs_isig3, "\0\0\x55\xAA", 4);
		memmove (fsinfo->fs_isig4, "\0\0\x55\xAA", 4);

		PUT_LONG (n_clus - 1, fsinfo->fs_infree);
		PUT_LONG (2,          fsinfo->fs_inxtfree);

		bwrite (FAT_32_SB_INFO,     fsinfo);
	   	bwrite (FAT_32_SB_INFO + 1, boot_code_32_cont);
	}

	/*
	 *	Atualiza o UNI bloco
	 */
	if (get_uni_block () < 0)
		error ("do_mkfs: erro de inicialização do SB");

	/*
	 *	Zera todo o diretório RAIZ
	 */
	if (fat_bits == 32)
		cluster_block_clr (2);
	else
		cluster_block_clr (0);

	/*
	 *	Aloca a FAT da memória (ela pode mudar de tamanho)
	 */
	alloc_fat ();

	/*
	 *	Inicializa as duas entradas iniciais da FAT
	 */
	put_fat_value (0xFFFFFF00|dd.d_media, 0);	/* Cluster == 0 */
	put_fat_value (0xFFFFFFFF, 1);			/* Cluster == 1 */

	if (fat_bits == 32)
		put_fat_value (STD_EOF, 2);		/* Cluster == 2 (raiz) */

	/*
	 *	Escreve a FAT no disco
	 */
	put_whole_fat ();

	/*
	 *	Inicializações
	 */
   /***	fs_status = veja abaixo			***/
   /***	dev_nm[0] = acertado por "simple_open"	***/
	vol_nm[0] = '\0';
   /***	cwd_cluster = 0; ***/

	fs_status = FS_MOUNT_RW;

}	/* end mk_one_fs */

/*
 ****************************************************************
 *    Verifica se um número é potência de 2 menor do que 128	*
 ****************************************************************
 */
int
is_power_of_2 (int n)
{
	int		i;

	for (i = 1; i < (1 << 8); i <<= 1)
	{
		if (i == n)
			return (1);
	}

	return (0);

}	/* end is_power_of_2 */

/*
 ****************************************************************
 *	Resumo de utilização do programa			*
 ****************************************************************
 */
void
do_mkfs_help (void)
{
	fprintf
	(	stderr,
		"%s - cria um sistema de arquivos DOS\n"
		"\nSintaxe:\n"
		"\t%s [-v] [-%s] [-c <b>] [<dispositivo>]\n",
		cmd_nm, cmd_nm, num_dev_list
	);

	fprintf
	(	stderr,
		"\nOpções:"
		"\t-v: Informa os parâmetros do sistema de arquivos\n"
		"\t-c: O tamanho do CLUSTER da partição FAT32 terá <b> setores de 512 bytes\n"
#if (0)	/*******************************************************/
		"\t-s: O sistema de arquivos terá <sz> blocos\n"
		"\t    (ao invés do tamanho total da partição)\n"
#endif	/*******************************************************/
		"%s",
		num_dev_option
	);

}	/* end do_mkfs_help */
