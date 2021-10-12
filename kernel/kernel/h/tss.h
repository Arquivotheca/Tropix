/*
 ****************************************************************
 *								*
 *			<sys/tss.h>				*
 *								*
 *	Definição da estrutura de TASK				*
 *								*
 *	Versão	3.0.0, de 28.08.94				*
 *		3.0.0, de 28.08.94				*
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

#define	DINO_H			/* Para definir os protótipos */

/*
 ******	Estrutura "task state segment" **************************
 */
struct tss
{
	long	tss_link;	/* TSS anterior */
	void	*tss_sp0; 	/* Pilha do nível 0 */
	long	tss_ss0;	/* Segmento de pilha nível 0 */
	void	*tss_sp1; 	/* Pilha do nível 1 */
	long	tss_ss1;	/* Segmento de pilha nível 1 */
	void	*tss_sp2; 	/* Pilha do nível 2 */
	long	tss_ss2;	/* Segmento de pilha nível 2 */

	long	tss_cr3; 	/* Diretório da tabela de páginas */
	void	*tss_pc; 	/* Contador do programa */
	long	tss_sr; 	/* Estado do processador */

	long	tss_r0; 	/* Registros do usuário */
	long	tss_r1; 
	long	tss_r2; 
	long	tss_r3; 
	long	tss_sp;
	long	tss_fp;
	long	tss_r4; 
	long	tss_r5; 

	long	tss_es;		/* Registros de segmentação */
	long	tss_cs;
	long	tss_ss;
	long	tss_ds;
	long	tss_fs;
	long	tss_gs;
	long	tss_ldt;
	long	tss_iomap;
};
