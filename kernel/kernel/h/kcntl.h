/*
 ****************************************************************
 *								*
 *			<sys/kcntl.h>				*
 *								*
 *	Controle do Kernel					*
 *								*
 *	Versão	1.0.0, de 02.02.87				*
 *		4.5.0, de 01.10.03				*
 *								*
 *	Módulo: Núcleo						*
 *		NÚCLEO do TROPIX para PC			*
 *		/usr/include/sys				*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 * 		Copyright © 2003 NCE/UFRJ - tecle "man licença"	*
 * 								*
 ****************************************************************
 */

#define	KCNTL_H			/* Para definir os protótipos */

/*
 ****************************************************************
 *	Comandos do KCNTL					*
 ****************************************************************
 */
typedef enum
{
	K_GETSYMGN = 1,		/* Obtem um SYM dado o nome */
	K_GETSYMGA,		/* Obtem um SYM dado o endereco */
	K_GETDEV,		/* Obtem o dev (MAJOR+MINOR) dado o devname */
	K_GETDEVNM,		/* Obtem o devname dado o dev (MAJOR+MINOR) */
	K_PUTCVTB,		/* Modifica a Tabela de conversão */
	K_SEMAFREE,		/* Libera o recurso de um processo */
	K_GETPSZ,		/* Obtém os tamanhos das áreas do processo */
	K_KTRACE,		/* Gera um trace dos recursos usados */
	K_GETMMU,		/* Obtem informações da gerencia de memória */
	K_SETPREEMPTION,	/* Habilita preemption no kernel */
	K_ERRTOSTR,		/* Obtém uma cadeia com o nome do erro */
	K_GET_DEV_TB,		/* Obtém uma entrada da tabela de discos */
	K_GET_PARTNM,		/* Obtém o nome de um tipo de partição */
	K_GET_FD_TYPE,		/* Obtém os tipos dos disquetes */
	K_MMU_LIMIT,		/* Obtém/atribui o limite da MMU */
	K_DEBUG,		/* Obtém/atribui o valor de DEBUG */
	K_CR0,			/* Obtém/atribui o valor de "cr0" */
	K_DMESG,		/* Obtém as mensagens do "boot" */

}	KENUM;

/*
 ******	Estrutura para "K_GETPSZ" *******************************
 */
typedef struct
{
	long	k_tsize;	/* Tamanho do texto do processo (bytes) */
	long	k_dsize;	/* Tamanho do data+bss do processo (bytes) */
	long	k_ssize;	/* Tamanho da pilha do processo (bytes) */
	long	k_res0[5];	/* Reservado para uso futuro */

}	KPSZ;

/*
 ******	Definições para "K_KTRACE" ******************************
 */
#define	PROCSWTIME	1	/* Tempo para troca de processo */
#define	CLKINTTIME	2	/* Tempo para atend. int. relógio */

/*
 ****** Tabela de tipos de sistemas de arquivos *****************
 */
typedef struct
{
	int	pt_type;	/* Tipo do sistema de arquivos */
	char	*pt_nm;		/* Nome do sistema de arquivos */

}	PARTNM;

#define	PARTNM_MAXSZ	20	/* Nenhum nome tem mais de 20 caracteres */
