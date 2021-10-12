/*
 ****************************************************************
 *								*
 *			<fcntl.h>				*
 *								*
 *	Controle de Arquivos					*
 *								*
 *	Versão	1.0.0, de 15.01.87				*
 *		3.0.0, de 05.02.96				*
 *								*
 *	Módulo: fcntl.h						*
 *		/usr/include					*
 *		Categoria B					*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 1995 NCE/UFRJ - tecle "man licença"	*
 * 								*
 ****************************************************************
 */

/*
 ******	Indicadores do OPEN *************************************
 */
#define	O_RDONLY	0x0000		/* Leitura */
#define	O_WRONLY	0x0001		/* Escrita */
#define	O_RDWR		0x0002		/* Leitura e Escrita */ 

#define	O_NDELAY	0x0004		/* Nao espere em leitura/escrita */ 
#define	O_NONBLOCK	O_NDELAY
#define	O_APPEND	0x0008		/* Escreva no final */ 
#define	O_FORMAT	0x0010		/* Formatação */ 

#define	O_CREAT		0x0100		/* Crie o arquivo se não existe */ 
#define	O_TRUNC		0x0200		/* Trunque o arquivo */ 
#define	O_EXCL		0x0400		/* Erro no open se já existe */

#define	O_PHYS		0x0000		/* Mapeamento físico */
#define	O_KERNEL	0x1000		/* Mapeamento do kernel */

#define	O_LOCK		0x4000		/* Open com uso exclusivo */

/*
 ******	Indicadores do FCNTL ************************************
 */
#define	F_DUPFD	0	/* Duplica o descritor */
#define	F_GETFD	1	/* Obtem indicadores do descritor do arquivo */
#define	F_SETFD	2	/* Atribui indicadores do descritor do arquivo */
#define	F_GETFL	3	/* Obtem indicadores do estado do arquivo */
#define	F_SETFL	4	/* Atribui indicadores do estado do arquivo */
