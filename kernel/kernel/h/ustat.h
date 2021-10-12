/*
 ****************************************************************
 *								*
 *			<sys/ustat.h>				*
 *								*
 *	Estrutura de estado de um systema de arquivos		*
 *								*
 *	Versão	1.0.0, de 28.01.87				*
 *		4.5.0, de 21.05.03				*
 *								*
 *	Módulo: Núcleo						*
 *		NÚCLEO do TROPIX para PC			*
 *		/usr/include/sys				*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 2003 NCE/UFRJ - tecle "man licença"	*
 *								*
 ****************************************************************
 */

#define	USTAT_H			/* Para definir os protótipos */

/*
 ******	A estrutura *********************************************
 */
typedef	struct	ustat	USTAT;

struct	ustat
{
	int	f_type;		/* Tipo/subtipo do sistema de arquivos */
	int	f_sub_type;
	int	f_bsize;	/* Tamanho do bloco */
	int	f_nmsize;	/* Tamanho máximo do identificador */
	daddr_t	f_fsize;	/* No. total de blocos */
	daddr_t	f_tfree;	/* No. total de blocos livres */
	ino_t	f_isize;	/* No. total de nós-índice */
	ino_t	f_tinode;	/* No. total de nós-índice livres */
	char	f_fname[16];	/* Nome do sistema de arquivos */
	char	f_fpack[16];	/* Nome do volume */
	short	f_m, f_n;	/* Fator de entrelaçamento */
	int	f_symlink_sz;	/* Tamanho a partir do qual está no disco */

	int	f_reser[3];
};

/*
 ******	Protótipos de funções ***********************************
 */
extern int	ustat (dev_t, USTAT *);
