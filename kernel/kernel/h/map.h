/*
 ****************************************************************
 *								*
 *			<sys/map.h>				*
 *								*
 *	Estrutura do mapa de memoria/disco			*
 *								*
 *	Versão	3.0.0, de 07.09.94				*
 *		3.2.3, de 01.11.99				*
 *								*
 *	Módulo: Núcleo						*
 *		NÚCLEO do TROPIX para PC			*
 *		/usr/include/sys				*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 1999 NCE/UFRJ - tecle "man licença"	*
 *								*
 ****************************************************************
 */

#define	MAP_H			/* Para definir os protótipos */

/*
 ******	A estrutura *********************************************
 */
struct	map
{
	pg_t	a_area;		/* Endereço em páginas ou blocos */
	pg_t	a_size;		/* Tamanho em páginas ou blocos */
	MAP	*a_back;	/* Entrada anterior */
	MAP	*a_forw;	/* Entrada seguinte */
};

#define	NOMAP	(MAP *)NULL

enum {	M_CORE,	M_SWAP };	/* Para identificar o mapa */
