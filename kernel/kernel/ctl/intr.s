|*
|****************************************************************
|*								*
|*			intr.s					*
|*								*
|*	Gerência de interrupções				*
|*								*
|*	Versão	3.0.0, de 25.08.94				*
|*		4.9.0, de 09.08.06				*
|*								*
|*	Módulo: Núcleo						*
|*		NÚCLEO do TROPIX para PC			*
|*								*
|*	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
|*		Copyright © 2006 NCE/UFRJ - tecle "man licença"	*
|*								*
|****************************************************************
|*

|*
|****** Variáveis globais ***************************************
|*
	.data
	.global	_cpl_level
_cpl_level:
	.long	7		|* PL corrente

	.global	_pl_mask
_pl_mask:
	.long	0xFFFF		|* Máscara para o nível 0
	.long	0xFFFF		|* Máscara para o nível 1
	.long	0xFFFF		|* Máscara para o nível 2
	.long	0xFFFF		|* Máscara para o nível 3
	.long	0xFFFF		|* Máscara para o nível 4
	.long	0xFFFF		|* Máscara para o nível 5
	.long	0xFFFF		|* Máscara para o nível 6
	.long	0xFFFF		|* Máscara para o nível 7

.if	INTR_PATTERN
|*
|****** Tabela dos padrões da interrupção ***********************
|*
COLOR1	= {  7 << 8 } | CHAR	|* Branco
COLOR2	= {  6 << 8 } | CHAR	|* Marrom
COLOR3	= {  2 << 8 } | CHAR	|* Verde
COLOR4	= {  9 << 8 } | CHAR	|* Azul escuro brilhante
COLOR5	= { 14 << 8 } | CHAR	|* Amarelo
COLOR6	= {  4 << 8 } | CHAR	|* Vermelho
COLOR7	= {  5 << 8 } | CHAR	|* Magenta

	.data
	.global	_intr_pattern
_intr_pattern:
	.word	W,	W,	W,	W		|* 0
	.word	W,	W,	W,	SPACE

	.word	COLOR1,	W,	W,	W		|* 1
	.word	W,	W,	W,	SPACE

	.word	COLOR2,	COLOR2,	W,	W		|* 2
	.word	W,	W,	W,	SPACE

	.word	COLOR3,	COLOR3,	COLOR3,	W		|* 3
	.word	W,	W,	W,	SPACE

	.word	COLOR4,	COLOR4,	COLOR4,	COLOR4		|* 4
	.word	W,	W,	W,	SPACE

	.word	COLOR5,	COLOR5,	COLOR5,	COLOR5		|* 5
	.word	COLOR5,	W,	W,	SPACE

	.word	COLOR6,	COLOR6,	COLOR6,	COLOR6		|* 6
	.word	COLOR6,	COLOR6,	W,	SPACE

	.word	COLOR7,	COLOR7,	COLOR7,	COLOR7		|* 7
	.word	COLOR7,	COLOR7,	COLOR7,	SPACE

	.global	_intr_pattern_addr
_intr_pattern_addr:
	.long	_intr_pattern + 0 * INTR_PAT_SZ
	.long	_intr_pattern + 1 * INTR_PAT_SZ
	.long	_intr_pattern + 2 * INTR_PAT_SZ
	.long	_intr_pattern + 3 * INTR_PAT_SZ
	.long	_intr_pattern + 4 * INTR_PAT_SZ
	.long	_intr_pattern + 5 * INTR_PAT_SZ
	.long	_intr_pattern + 6 * INTR_PAT_SZ
	.long	_intr_pattern + 7 * INTR_PAT_SZ
.endif	INTR_PATTERN

|*
|******	Atribui a prioridade 0 **********************************
|*
|*	int	spl0 (void);
|*
|*	Retorna o valor antigo de PL
|*
	.text
	.global _spl0
_spl0:
	movl	#0,r1			|* r1 = prioridada nova
	movl	_pl_mask+0*4,r0		|* r0 = máscara do nível #0

	jmp	splset

|*
|******	Atribui a prioridade 1 **********************************
|*
|*	int	spl1 (void);
|*
|*	Retorna o valor antigo de PL
|*
	.text
	.global _spl1
_spl1:
	movl	#1,r1			|* r1 = prioridada nova
	movl	_pl_mask+1*4,r0		|* r0 = máscara do nível #1

	jmp	splset

|*
|******	Atribui a prioridade 2 **********************************
|*
|*	int	spl2 (void);
|*
|*	Retorna o valor antigo de PL
|*
	.text
	.global _spl2
_spl2:
	movl	#2,r1			|* r1 = prioridada nova
	movl	_pl_mask+2*4,r0		|* r0 = máscara do nível #2

	jmp	splset

|*
|******	Atribui a prioridade 3 **********************************
|*
|*	int	spl3 (void);
|*
|*	Retorna o valor antigo de PL
|*
	.text
	.global _spl3
_spl3:
	movl	#3,r1			|* r1 = prioridada nova
	movl	_pl_mask+3*4,r0		|* r0 = máscara do nível #3

	jmp	splset

|*
|******	Atribui a prioridade 4 **********************************
|*
|*	int	spl4 (void);
|*
|*	Retorna o valor antigo de PL
|*
	.text
	.global _spl4
_spl4:
	movl	#4,r1			|* r1 = prioridada nova
	movl	_pl_mask+4*4,r0		|* r0 = máscara do nível #4

	jmp	splset

.if 0	|*******************************************************|
|*
|******	Atribui a prioridade 5 **********************************
|*
|*	int	spl5 (void);
|*
|*	Retorna o valor antigo de PL
|*
	.text
	.global _spl5
_spl5:
	movl	#5,r1			|* r1 = prioridada nova
	movl	_pl_mask+5*4,r0		|* r0 = máscara do nível #5

	jmp	splset

|*
|******	Atribui a prioridade 6 **********************************
|*
|*	int	spl6 (void);
|*
|*	Retorna o valor antigo de PL
|*
	.text
	.global _spl6
_spl6:
	movl	#6,r1			|* r1 = prioridada nova
	movl	_pl_mask+6*4,r0		|* r0 = máscara do nível #6

	jmp	splset
.endif	|*******************************************************|


|*
|******	Atribui a prioridade 7 **********************************
|*
|*	int	spl7 (void);
|*
|*	Retorna o valor antigo de PL
|*
	.text
	.global _spl7
_spl7:
	movl	#7,r1			|* r1 = prioridada nova
	movl	_pl_mask+7*4,r0		|* r0 = máscara do nível #7

	jmp	splset

|*
|******	Atribui uma prioridade dada *****************************
|*
|*	int	splx (int new_pl);
|*
|*	Retorna o valor antigo de PL
|*
	.text
	.global _splx
_splx:
	movl	4(sp),r1		|* r1 = PL dado
	movl	_pl_mask(*,r1@l),r0	|* r0 = máscara do nível dado
splset:
	disint				|* Desabilita as interruções

	cmpl	r1,_cpl_level		|* Já está com a máscara correta?
	jeq	2$

	outb	r0,#IO_ICU1+1		|* Atualiza ICU #1
	inb	#IO_NOP,r0

	movb	h0,r0			|* Atualiza ICU #2
	outb	r0,#IO_ICU2+1
	inb	#IO_NOP,r0
2$:
	movl	_cpl_level,r0		|* Retorna a prioridade anterior
	movl	r1,_cpl_level		|* Atribui a prioridada nova

.if	INTR_PATTERN
	cmpl    #0,_idle_intr_active
	jz	9$

	pushl	r5
	pushl	r4

	movl	_intr_pattern_addr(*,r1@l),r4	|* Padrão de interrupção
	movl	#INTR_VGA_ADDR,r5
	movl	#INTR_PAT_SZ/4,r1

	up
	rep
	movsl

	popl	r4
	popl	r5

9$:
.endif	INTR_PATTERN

	enint
	rts

|*
|******	Desabilita todas as interrupções ************************
|*
|*	void	disable_int (void);
|*
	.text
	.global _disable_int
_disable_int:
	disint
	rts

|*
|******	Habilita as interrupções ********************************
|*
|*	void	enable_int (void);
|*
	.text
	.global _enable_int
_enable_int:
	enint
	rts
