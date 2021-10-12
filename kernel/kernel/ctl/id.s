|*
|****************************************************************
|*								*
|*			id.s					*
|*								*
|*	Manipulação de IDs					*
|*								*
|*	Versão	3.0.0, de 29.08.94				*
|*		4.0.0, de 15.08.01				*
|*								*
|*	Módulo: Núcleo						*
|*		NÚCLEO do TROPIX para PC			*
|*								*
|*	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
|*		Copyright © 2001 NCE/UFRJ - tecle "man licença"	*
|*								*
|****************************************************************
|*


|*
|****************************************************************
|*	Converte no formato canônico				*
|****************************************************************
|*
|*	idp_t	idcan (idp_t id1, const char *id2);
|*
	.global	_idcan
_idcan:
	pushl	r5
	pushl	r4

	movl	12(sp),r5	|* Zera os 16 bytes de "id1"
	movl	#4,r1
	clrl	r0
	up
	rep
	stosl

	movl	12(sp),r5	|* "id1"
	movl	r5,r0		|* Retorna "id1"
	movl	16(sp),r4	|* "id2"

	movl	#14,r1		|* Examina 14 caracteres
   |***	up
1$:
	cmpb	#0,(r4)
	movsb
	loopnz	1$

	popl	r4
	popl	r5
	rts

|*
|****************************************************************
|*	Zera um "id"						*
|****************************************************************
|*
|*	idp_t	idclr (idp_t id1);
|*
	.global	_idclr
_idclr:
	pushl	r5

	movl	8(sp),r5	|* Zera os 16 bytes de "id1"
	movl	#4,r1
	clrl	r0
	up
	rep
	stosl

	movl	8(sp),r0	|* Retorna "id1"
	popl	r5
	rts

|*
|****************************************************************
|*	Copia identificadores					*
|****************************************************************
|*
|*	idp_t	idcpy (idp_t id1, const idp_t id2);
|*
	.global	_idcpy
_idcpy:
	pushl	r5
	pushl	r4

	movl	12(sp),r5	|* "id1"
	movl	r5,r0		|* Retorna "id1"
	movl	16(sp),r4	|* "id2"
	movl	#4,r1		|* 4 longos

	up
	rep
	movsl

	popl	r4
	popl	r5
	rts

.if 0	|*******************************************************|
|*
|****************************************************************
|*	Compara identificadores (14 bytes)			*
|****************************************************************
|*
|*	int	ideq (const idp_t id1, const idp_t id2);
|*
	.global	_ideq
_ideq:
	pushl	r5
	pushl	r4

	movl	12(sp),r5	|* "id1"
	movl	16(sp),r4	|* "id2"
	movl	#3,r1		|* 3 longos + 1 short
	clrl	r0		|* Por enquanto o resultado é NÃO

	up
	repe
	cmpsl
	jne	1$

	cmpsw
	jne	1$

	incl	r0		|* Retorna "1"
1$:
	popl	r4
	popl	r5
	rts

|*
|****************************************************************
|*	Converte um identificador longo para a forma canônica	*
|****************************************************************
|*
|*	idp_t	lidcan (idp_t lid1, const char *id2);
|*
	.global	_lidcan
_lidcan:
	pushl	r5
	pushl	r4

	movl	12(sp),r5	|* Zera os 32 bytes de "lid1"
	movl	#8,r1
	clrl	r0
	up
	rep
	stosl

	movl	12(sp),r5	|* "lid1"
	movl	r5,r0		|* Retorna "lid1"
	movl	16(sp),r4	|* "lid2"

	movl	#31,r1		|* Examina 31 caracteres
   |***	up
1$:
	cmpb	#0,(r4)
	movsb
	loopnz	1$

	popl	r4
	popl	r5
	rts

|*
|****************************************************************
|*	Compara identificadores longos (31 bytes)		*
|****************************************************************
|*
|*	int	lideq (const idp_t lid1, const idp_t lid2);
|*
	.global	_lideq
_lideq:
	pushl	r5
	pushl	r4

	movl	12(sp),r5	|* "lid1"
	movl	16(sp),r4	|* "lid2"
	movl	#8,r1		|* 8 longos
	clrl	r0		|* Por enquanto o resultado é NÃO

	up
	repe
	cmpsl
	jne	1$

	incl	r0		|* Retorna "1"
1$:
	popl	r4
	popl	r5
	rts
.endif	|*******************************************************|
