/*
 ****************************************************************
 *								*
 *			<sys/kcntl.h>				*
 *								*
 *	Controle dos Processos e Threads			*
 *								*
 *	Versão	1.0.0, de 03.07.91				*
 *		3.2.0, de 06.05.99				*
 *								*
 *	Módulo: Núcleo						*
 *		NÚCLEO do TROPIX para PC			*
 *		/usr/include/sys				*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 * 		Copyright © 1999 NCE/UFRJ - tecle "man licença"	*
 * 								*
 ****************************************************************
 */

/*
 ****************************************************************
 *	Comandos do PCNTL					*
 ****************************************************************
 */
typedef enum
{
	P_PMUTIMES = 1,		/* Obtém tempos estimados de proc. paralelo */
	P_GETSTKRGSZ,		/* Obtém o tamanho da região de stack */
	P_SETSTKRGSZ,		/* Aumenta o tamanho da região de stack */	
	P_GETPRI,		/* Obtém a prioridade momentânea do processo */
	P_SETPRI,		/* Altera a prioridade momentânea do processo */
	P_SETRTPROC,		/* Transforma o proc. em proc. de tempo real */
	P_SETFPREGADDR,		/* Indica o end. de usu. dos regs fp de soft */
	P_ENABLE_USER_IO,	/* Permite o usuário ler/escrever nas portas */
	P_DISABLE_USER_IO,	/* Impede o usuário de ler/escrever nas portas */
	P_GET_PHYS_ADDR		/* Obtém um endereço físico */

}	PENUM;
