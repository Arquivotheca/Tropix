/*
 ****************************************************************
 *								*
 *			mkfst1.h				*
 *								*
 *	Definições universais					*
 *								*
 *	Versão	4.3.0, de 04.07.02				*
 *		4.3.0, de 04.07.02				*
 *								*
 *	Módulo: mkfst1						*
 *		Utilitários especiais				*
 *		Categoria D					*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 2002 NCE/UFRJ - tecle "man licença"	*
 * 								*
 ****************************************************************
 */

/*
 ****************************************************************
 *	Variáveis e Definições globais				*
 ****************************************************************
 */
#define	NOVOID	(void *)NULL
#define	elif	else if
#define	EVER	;;
#define STR(x)	# x
#define XSTR(x)	STR (x)

/*
 ******	Definições relativas ao sistema de arquivos *************
 */
#define	PMODE	0755

/*
 ******	Variáveis externas **************************************
 */
extern const char	pgversion[];	/* Versão do programa */

extern int		exit_code;	/* Código de retorno */

extern int		rflag;		/* Cria o arquivo regular */
extern int		fflag;		/* Forca: não pede confirmação */
extern int		vflag;		/* Verbose */

extern const char	*fs_nm;		/* Nome do Dispositivo */
extern char		fs_partnm[];	/* Nome da partição */

extern int		fs_fd;		/* Descritor para leitura e escritas do FS */

extern T1SB		sb;		/* Superbloco */
extern char		bitmap[BL4SZ];	/* Área para o Mapa de bits */

#ifdef	DISKTB_H
extern  DISKTB		dev_disktb;	/* Entrada da tabela de partições */
#endif	DISKTB_H

#ifdef	STAT_H
extern STAT		dev_s;		/* O estado do dispositivo/arquivo */
#endif	STAT_H

extern time_t		now_time;	/* Tempo de Criação */

extern daddr_t		root_dir_bno;

extern daddr_t		inode_bno;	/* No. do bloco dos INODEs */

extern T1DINO		inode_block[T1_INOinBL4];

extern char		root_dir_image[BL4SZ];

extern char		lost_dir_image[BL4SZ];
extern daddr_t		lost_dir_bno;

/*
 ****** Protótipos de funções ***********************************
 */
extern void		analyse_arguments (const char *[]);
extern char		*cv_blocks_to_KB_MB_or_GB (daddr_t);
extern void		make_empty_bitmaps (void);
extern void		fs_read (daddr_t, void *);
extern void		fs_write (daddr_t, const void *);
extern void		fs_clear (daddr_t);
extern void		get_dev (const char *);
extern const char	*get_boot1_nm (void);
extern void		make_root_and_lost_dir (void);
extern daddr_t		get_empty_block (int type);
extern T1DINO		*ialloc (void);
extern void		help (void);
extern void		write_boot_block (const char *);
