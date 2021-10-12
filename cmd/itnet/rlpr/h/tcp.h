/*
 ****************************************************************
 *								*
 *			tcp.h					*
 *								*
 *	Definições acerca da biblioteca TCP			*
 *								*
 *	Versão	4.2.0, de 22.05.02				*
 *								*
 *	Módulo: rlpr						*
 *		Utilitários Básicos				*
 *		Categoria B					*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright (c) 2002 TROPIX Technologies Ltd.	*
 * 								*
 ****************************************************************
 */

typedef	long	IPADDR;


extern	int	tcp_open (int port);
extern	int	tcp_listen (int fd, char *name);
extern	int	tcp_connect (int port, const char *machine);
extern	int	tcp_nread (int fd, int *pend);
extern	int	tcp_read (int fd, void *buf, int size);
extern	int	tcp_write (int fd, void *buf, int size);
extern	int	tcp_close (int fd);
extern	int	tcp_abort (int fd);
