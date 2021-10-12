/*
 ****************************************************************
 *								*
 *			<sys/iotab.h>				*
 *								*
 *	Estruturas das chamadas aos drivers			*
 *								*
 *	Versão	3.0.0, de 06.10.94				*
 *		4.2.0, de 07.09.01				*
 *								*
 *	Módulo: Núcleo						*
 *		NÚCLEO do TROPIX para PC			*
 *		/usr/include/sys				*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 2001 NCE/UFRJ - tecle "man licença"	*
 *								*
 ****************************************************************
 */

#define	IOTAB_H			/* Para definir os protótipos */

/*
 ******	Dispositivos estruturados (blocos) **********************
 */
struct	biotab
{
	int	(*w_open) (int, int);
	int	(*w_close) (int, int);
	int	(*w_strategy) (BHEAD *);
	int	(*w_ioctl) (int, int, int, int);
	DEVHEAD	*w_tab;
};

/*
 ******	Dispositivos não-estruturados (caracteres) **************
 */
struct	ciotab
{
	int	(*w_attach) (int);
	int	(*w_open) (int, int);
	int	(*w_close) (int, int);
	int	(*w_read) (IOREQ *);
	int	(*w_write) (IOREQ *);
	int	(*w_ioctl) (int, int, int, int);
};
