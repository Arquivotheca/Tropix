/*
 ****************************************************************
 *								*
 *			<sys/select.h>				*
 *								*
 *	Definição dos tipos usados pela chamada "select"	*
 *								*
 *	Versão	4.2.0, de 08.10.01				*
 *		4.2.0, de 08.10.01				*
 *								*
 *	Módulo: Núcleo						*
 *		NÚCLEO do TROPIX para PC			*
 *		/usr/include/sys				*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 2001 NCE/UFRJ - tecle "man licença"	*
 * 								*
 ****************************************************************
 */

#define	SELECT_H			/* Para incluir protótipos */

/*
 ******	Definições de tamanhos **********************************
 */
#define	FD_SETSIZE	32		/* Número máximo de descritores */

/*
 ******	Definições de tipos *************************************
 */
typedef	int		fd_mask;
typedef	int		fd_set;

/*
 ******	Macros úteis ********************************************
 */
#define	FD_SET(fd, mask_ptr)	(*(mask_ptr) |= (1 << (fd)))
#define	FD_CLR(fd, mask_ptr)	(*(mask_ptr) &= ~(1 << (fd)))
#define	FD_ISSET(fd, mask_ptr)	(*(mask_ptr) & (1 << (fd)))
#define	FD_ZERO(mask_ptr)	(*(mask_ptr) = 0)

/*
 ****** Protótipos **********************************************
 */
extern int	select (int, fd_set *, fd_set *, fd_set *, const MUTM *);

