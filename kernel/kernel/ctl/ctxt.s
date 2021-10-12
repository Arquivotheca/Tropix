|*
|****************************************************************
|*								*
|*			ctxt.s					*
|*								*
|*	Guarda e restaura o estado de um processo		*
|*								*
|*	Versão	3.0.0, de 12.11.94				*
|*		3.0.1, de 08.01.99				*
|*								*
|*	Módulo: Núcleo						*
|*		NÚCLEO do TROPIX para PC			*
|*								*
|*	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
|*		Copyright © 1999 NCE/UFRJ - tecle "man licença"	*
|*								*
|****************************************************************
|*

|*
|****************************************************************
|*	Salva o contexto de um processo				*
|****************************************************************
|*
|*	if (ctxtsto (u.u_save) == 0)
|*	{
|*		...........
|*	}
|*
	.global	_ctxtsto
_ctxtsto:
|*	clrb	_preemption_flag+CPUID	|* Impede PREEMPTION em modo SUP

	popl	r2			|* r2 = endereço de retorno

	movl	(sp),r1			|* r1 = &u.u_save

	movl	r3,0(r1)		|* Guarda os registradores
	movl	r4,4(r1)
	movl	r5,8(r1)
	movl	fp,12(r1)
	movl	sp,16(r1)
	movl	r2,20(r1)		|* Guarda o endereço de retorno

	clrl	r0			|* Retorna com 1
	incl	r0
	jmp	r2

|*
|****************************************************************
|*	Restaura o contexto de um processo			*
|****************************************************************
|*
|*	ctxtld (UPROC *u_new_up);
|*
|*	/* no return */
|*
	.global	_ctxtld
_ctxtld:
	disint				|* Impede interrupções

	movl	u.u_cpu,r2		|* r2 = CPU + Curpri

	movl	4(sp),r4
	movl	r4,r0			|* r4 = Nova UPROC (end. virtual)
	andl	#~SYS_ADDR,r0
	orl	#SRW,r0			|* r0 = Nova UPROC (end. físico)

	movl	_extra_page_table,r3	|* troca a UPROC (supondo USIZE == 2)
	movl	r0,(r3)
	addl	#PGSZ,r0
	movl	r0,4(r3)

|*	A UPROC foi trocada: a partir deste ponto, NÃO podemos mais usar a pilha do processo velho

	cmpb	#0,U_NO_USER_MMU(r4)	|* Verifica se o novo processo
	jne	1$			|*   executará somente em modo KERNEL

	movl	U_MMU_DIR(r4),r0	|* r0 = Diretório de pgs (end. físico)
	andl	#~SYS_ADDR,r0

	movl	cr3,r3			|* Verifica se, por acaso, é o mesmo
	cmpl	r0,r3
	jne	2$

|*	Não precisa trocar o diretório de páginas

1$:
	movl	#_u,r0			|* Invalida &u (supondo USIZE == 2)
	invlpg	(r0)
	invlpg	PGSZ(r0)

	clrb	U_CTXT_SW_TYPE(r4)	|* Indica que foi troca rápida
	jmp	3$

|*	Precisa trocar o diretório de páginas (toda a TLB será invalidada)

2$:
	movl	r0,cr3			|* Atualiza o "cr3"
	movb	#1,U_CTXT_SW_TYPE(r4)	|* Indica que a troca foi normal

|*
|******	Enfim, surge o novo processo ***************************
|*
3$:
	clrb	_mmu_dirty+CPUID

	movl	r2,u.u_cpu		|* Restaura CPU + Curpri

	leal	U_CTXT(r4),r1		|* Ponteiro para o contexto

	movl	0(r1),r3		|* Restaura os registradores
	movl	4(r1),r4
	movl	8(r1),r5
	movl	12(r1),fp
	movl	16(r1),sp

	enint				|* Libera as interrupções

	clrl	r0			|* Retorna com 0
	jmp	20(r1)

|*
|****************************************************************
|*	Prepara para um desvio longo (Sem troca de processo)	*
|****************************************************************
|*
|*	if (jmpset (u.u_qsave) == 0)
|*	{
|*		...........
|*	}
|*
	.global	_jmpset
_jmpset:
	popl	r2			|* r2 = endereço de retorno

	movl	(sp),r1			|* r1 = &u.u_save

	movl	r3,0(r1)		|* Guarda os registradores
	movl	r4,4(r1)
	movl	r5,8(r1)
	movl	fp,12(r1)
	movl	sp,16(r1)
	movl	r2,20(r1)		|* Guarda o endereço de retorno

	clrl	r0			|* Retorna com 1
	incl	r0
	jmp	r2

|*
|****************************************************************
|*	Desvio longo (Sem troca de processo)			*
|****************************************************************
|*
|*	jmpdo (u.u_qsave);
|*
|*	/* no return */
|*
	.global	_jmpdo
_jmpdo:
	movl	4(sp),r1		|* r1 = &u.u_save

	movl	0(r1),r3		|* Restaura os registradores
	movl	4(r1),r4
	movl	8(r1),r5
	movl	12(r1),fp
	movl	16(r1),sp

	clrl	r0			|* Retorna com 0
	jmp	20(r1)
