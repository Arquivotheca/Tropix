/*
 ****************************************************************
 *								*
 *			statetostr.c				*
 *								*
 *	Devolve o nome de um estado				*
 *								*
 *	Versão	1.0.0, de 29.05.97				*
 *		1.0.1, de 28.03.98				*
 *								*
 *	Módulo: xc/lib/aux					*
 *		X Windows - Bibliotecas				*
 *		Baseado no software homônimo do XFree86		*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright (c) 1998 TROPIX Technologies Ltd.	*
 * 								*
 ****************************************************************
 */

#include <xti.h>

static char	*stname[] =
{
	"T_UNINIT",	/* Não inicializado */
	"T_UNBND",	/* "unbound" */
	"T_IDLE",	/* Inerte */
	"T_OUTCON",	/* Enviando pedido de conexão */
	"T_INCON",	/* Recebendo pedido de conexão */
	"T_DATAXFER",	/* Transferindo dados */
	"T_OUTREL",	/* Enviando pedido de desconexão */
	"T_INREL"	/* Recebendo pedido de desconexão */
};

char *
statetostr (T_STATE state)
{
	if (state < T_UNINIT || state > T_INREL)
		return ("ESTADO INVÁLIDO");

	return (stname[state]);

}	/* end statetostr */
