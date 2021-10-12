/*
 ****************************************************************
 *								*
 *			<sys/dirent.h>				*
 *								*
 *   Formato do diretório independente de sistema de arquivos	*
 *								*
 *	Versão	4.0.0, de 02.07.01				*
 *		4.8.0, de 02.11.05				*
 *								*
 *	Módulo: Núcleo						*
 *		NÚCLEO do TROPIX para PC			*
 *		/usr/include/sys				*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 2005 NCE/UFRJ - tecle "man licença"	*
 *								*
 ****************************************************************
 */

#define	DIRENT_H 		/* Para definir os protótipos */

/*
 ******	Definições da estrutura *********************************
 */
#define	MAXNAMLEN	255	/* Tamanho máximo do nome */

#define	NODIRENT	(DIRENT *)NULL

typedef struct dirent	DIRENT;

struct dirent
{
	off_t	d_offset;		/* No diretório original */
	int	d_ino;			/* Número do nó-índice */
	int	d_namlen;		/* Tamanho do identificador */
	char	d_name[MAXNAMLEN+1];	/* O identificador */
};

/*
 ******	Protótipos de funções ***********************************
 */
extern int	getdirent (int, DIRENT *, int, int *);
extern ino_t	getdirpar (dev_t dev, ino_t ino, DIRENT *dep);
