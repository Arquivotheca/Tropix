|*
|****************************************************************
|*								*
|*			common.s				*
|*								*
|*	Cabeçalho para programas em "assembly"			*
|*								*
|*	Versão	3.0.0, de 12.08.94				*
|*		4.5.0, de 09.06.03				*
|*								*
|*	Módulo: Núcleo						*
|*		NÚCLEO do TROPIX para PC			*
|*								*
|*	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
|*		Copyright © 2003 NCE/UFRJ - tecle "man licença"	*
|*								*
|****************************************************************
|*

|*
|****************************************************************
|*	Definições globais					*
|****************************************************************
|*
KBSZ		= 1024
KBSHIFT		= 10

MBSZ		= 1024 * KBSZ
MBSHIFT		= 20

GBSZ		= 1024 * MBSZ

PGSZ		= 4096
PGSHIFT		= 12

|*
|****** Definições da MMU ***************************************
|*
SYS_ADDR	= 3 * GBSZ			|* 0xC0000000
USER_ADDR	= SYS_ADDR + 512 * MBSZ		|* 0xE0000000

PG_DIR_SHIFT	= 20		|* Para deslocamento do dir. de pg.

SPROT		= 3		|* Página presente, R/W, supervisor
UPROT		= 7		|* Página presente, R/W, user

USER_TEXT_VA	= 	     4   * MBSZ
USER_DATA_VA	= 	     512 * MBSZ
USER_PHYS_VA	= 1 * GBSZ + 512 * MBSZ
USER_IPC_VA	= 1 * GBSZ + 640 * MBSZ
USER_STACK_VA	= 2 * GBSZ + 512 * MBSZ

|*
|****** Definições da CPU ***************************************
|*
CPUID		= 0

|*
|****** Definições da UPROC *************************************
|*
USIZE		= 1		|* Tamanho (PG) da UPROC
CTXT_SW_TYPE	= 16		|* Deslocamento para u.u_ctxt_sw_type
NO_USER_MMU	= 17		|* Deslocamento para u.u_no_user_mmu

	.global	_u
_u		=	USER_ADDR

u.u_cpu		=	_u+0
u.u_pri		=	_u+2
u.u_dontpanic	=	_u+4
u.u_proc	=	_u+8
u.u_frame	=	_u+12

|*
|****** Definições da PROC **************************************
|*
proc.p_state	=	0
proc.p_sig	=	4
proc.p_pri	=	8
proc.p_mmu_dir	=	72

SRUN		=	5	|* Depende do "enum" do "proc.h"

|*
|****** Definições do VIDEO *************************************
|*
VIDEOADDR	= SYS_ADDR + 736 * KBSZ

LINE		= 24
COL		= 80

|*
|****** Seletores ***********************************************
|*
KERNEL_CS	= { 1 * 8 + 0 }
KERNEL_DS	= { 2 * 8 + 0 }

USER_CS		= { 3 * 8 + 3 }
USER_DS		= { 4 * 8 + 3 }

KERNEL_TSS	= { 5 * 8 + 0 }
SYS_CALL	= { 6 * 8 + 3 }

|*
|****** Definição da estrutura VECDEF ***************************
|*

|* Esta definição deve ser igual à de "h/intr.h"

VEC_PL		= 0	 	|* Prioridade da interrupção (1 a 7)
VEC_UNIT	= 4		|* Unidade (informação para o driver)
VEC_FUNC	= 8		|* Função de interrupção

VECLINESZ	= 8		|* Compartilhando até 7 unidades

VEC_SIZEOF	= 12		|* Tamanho de uma estrutura

VECLINE_SIZEOF	= VEC_SIZEOF * VECLINESZ |* Tamanho da estrutura total

|*
|****** Endereços de portas *************************************
|*
IO_NOP	= 0x84			|* Dispositivo inexistente para NOP

IO_ICU1	= 0x20			|* Controlador 8259A No. 1
IO_ICU2	= 0xA0			|* Controlador 8259A No. 2

|*
|****** Habilita/desabilita as "lampadinhas" ********************
|*
IDLE_PATTERN	= 1		|* Padrão do "idle"
INTR_PATTERN	= 1		|* Padrão da interrupção
|*
|****************************************************************
|*								*
|*			pat.s					*
|*								*
|*	Definições do padrão de interrupções			*
|*								*
|*	Versão	3.0.0, de 05.09.94				*
|*		3.0.0, de 05.09.94				*
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
|******	Definições do padrão da interrupção *********************
|*
CHAR		= '*'

SPACE		= { 0 << 8 } | ' '
COLOR		= { 4 << 8 } | CHAR	|* Vermelho
W		= SPACE

.if 0	/*******************************************************/
W		= { 7 << 8 } | CHAR	|* Branco
.endif	/*******************************************************/

INTR_PAT_LEN	= 8			|* Em caracteres na tela
INTR_PAT_LOC	= COL - 20 - INTR_PAT_LEN |* Offset na última linha

INTR_PAT_SZ	= 16			|* Em bytes
INTR_PAT_SHIFT	= 4			|* Log (2) do acima

INTR_VIDEOADDR	= VIDEOADDR + 2 * { LINE * COL + INTR_PAT_LOC }
|*
|****************************************************************
|*								*
|*			vector.s				*
|*								*
|*	Pontos de entradas das interrupções dos dispositivos	*
|*								*
|*	Versão	3.0.0, de 26.08.94				*
|*		4.5.0, de 09.06.03				*
|*								*
|*	Módulo: Núcleo						*
|*		NÚCLEO do TROPIX para PC			*
|*								*
|*	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
|*		Copyright © 2003 NCE/UFRJ - tecle "man licença"	*
|*								*
|****************************************************************
|*

|*
|****** Tabela de controle das interrupções *********************
|*
	.bss
	.global	_vecdef

|*		PL   UNIT	 FUNC
_vecdef:
	.long	0,	0,	0		|* IRQ0
	.long	0,	0,	0
	.long	0,	0,	0
	.long	0,	0,	0
	.long	0,	0,	0
	.long	0,	0,	0
	.long	0,	0,	0
	.long	0,	0,	0

	.long	0,	0,	0		|* IRQ1
	.long	0,	0,	0
	.long	0,	0,	0
	.long	0,	0,	0
	.long	0,	0,	0
	.long	0,	0,	0
	.long	0,	0,	0
	.long	0,	0,	0

	.long	0,	0,	0		|* IRQ2
	.long	0,	0,	0
	.long	0,	0,	0
	.long	0,	0,	0
	.long	0,	0,	0
	.long	0,	0,	0
	.long	0,	0,	0
	.long	0,	0,	0

	.long	0,	0,	0		|* IRQ3
	.long	0,	0,	0
	.long	0,	0,	0
	.long	0,	0,	0
	.long	0,	0,	0
	.long	0,	0,	0
	.long	0,	0,	0
	.long	0,	0,	0

	.long	0,	0,	0		|* IRQ4
	.long	0,	0,	0
	.long	0,	0,	0
	.long	0,	0,	0
	.long	0,	0,	0
	.long	0,	0,	0
	.long	0,	0,	0
	.long	0,	0,	0

	.long	0,	0,	0		|* IRQ5
	.long	0,	0,	0
	.long	0,	0,	0
	.long	0,	0,	0
	.long	0,	0,	0
	.long	0,	0,	0
	.long	0,	0,	0
	.long	0,	0,	0

	.long	0,	0,	0		|* IRQ6
	.long	0,	0,	0
	.long	0,	0,	0
	.long	0,	0,	0
	.long	0,	0,	0
	.long	0,	0,	0
	.long	0,	0,	0
	.long	0,	0,	0

	.long	0,	0,	0		|* IRQ7
	.long	0,	0,	0
	.long	0,	0,	0
	.long	0,	0,	0
	.long	0,	0,	0
	.long	0,	0,	0
	.long	0,	0,	0
	.long	0,	0,	0

	.long	0,	0,	0		|* IRQ8
	.long	0,	0,	0
	.long	0,	0,	0
	.long	0,	0,	0
	.long	0,	0,	0
	.long	0,	0,	0
	.long	0,	0,	0
	.long	0,	0,	0

	.long	0,	0,	0		|* IRQ9
	.long	0,	0,	0
	.long	0,	0,	0
	.long	0,	0,	0
	.long	0,	0,	0
	.long	0,	0,	0
	.long	0,	0,	0
	.long	0,	0,	0

	.long	0,	0,	0		|* IRQ10
	.long	0,	0,	0
	.long	0,	0,	0
	.long	0,	0,	0
	.long	0,	0,	0
	.long	0,	0,	0
	.long	0,	0,	0
	.long	0,	0,	0

	.long	0,	0,	0		|* IRQ11
	.long	0,	0,	0
	.long	0,	0,	0
	.long	0,	0,	0
	.long	0,	0,	0
	.long	0,	0,	0
	.long	0,	0,	0
	.long	0,	0,	0

	.long	0,	0,	0		|* IRQ12
	.long	0,	0,	0
	.long	0,	0,	0
	.long	0,	0,	0
	.long	0,	0,	0
	.long	0,	0,	0
	.long	0,	0,	0
	.long	0,	0,	0

	.long	0,	0,	0		|* IRQ13
	.long	0,	0,	0
	.long	0,	0,	0
	.long	0,	0,	0
	.long	0,	0,	0
	.long	0,	0,	0
	.long	0,	0,	0
	.long	0,	0,	0

	.long	0,	0,	0		|* IRQ14
	.long	0,	0,	0
	.long	0,	0,	0
	.long	0,	0,	0
	.long	0,	0,	0
	.long	0,	0,	0
	.long	0,	0,	0
	.long	0,	0,	0

	.long	0,	0,	0		|* IRQ15
	.long	0,	0,	0
	.long	0,	0,	0
	.long	0,	0,	0
	.long	0,	0,	0
	.long	0,	0,	0
	.long	0,	0,	0
	.long	0,	0,	0

|*
|****** Tabela dos endereços dos vetores ************************
|*
	.const
	.global	_vecdefaddr
_vecdefaddr:
	.long	_irq0_vector
	.long	_irq1_vector
	.long	_irq2_vector
	.long	_irq3_vector
	.long	_irq4_vector
	.long	_irq5_vector
	.long	_irq6_vector
	.long	_irq7_vector
	.long	_irq8_vector
	.long	_irq9_vector
	.long	_irq10_vector
	.long	_irq11_vector
	.long	_irq12_vector
	.long	_irq13_vector
	.long	_irq14_vector
	.long	_irq15_vector

|*
|******	Vetores IRQ0-7 ******************************************
|*
	.text
	.align	4
	.global	_irq0_vector
_irq0_vector:
	pushal
	movl	#_vecdef+0*VECLINE_SIZEOF,r3	|* Entrada VECDEF IRQ0
	jmp	int_common_0_7

	.align	4
	.global	_irq1_vector
_irq1_vector:
	pushal
	movl	#_vecdef+1*VECLINE_SIZEOF,r3	|* Entrada VECDEF IRQ1
	jmp	int_common_0_7

	.align	4
	.global	_irq2_vector
_irq2_vector:
	pushal
	movl	#_vecdef+2*VECLINE_SIZEOF,r3	|* Entrada VECDEF IRQ2
	jmp	int_common_0_7

	.align	4
	.global	_irq3_vector
_irq3_vector:
	pushal
	movl	#_vecdef+3*VECLINE_SIZEOF,r3	|* Entrada VECDEF IRQ3
	jmp	int_common_0_7

	.align	4
	.global	_irq4_vector
_irq4_vector:
	pushal
	movl	#_vecdef+4*VECLINE_SIZEOF,r3	|* Entrada VECDEF IRQ4
	jmp	int_common_0_7

	.align	4
	.global	_irq5_vector
_irq5_vector:
	pushal
	movl	#_vecdef+5*VECLINE_SIZEOF,r3	|* Entrada VECDEF IRQ5
	jmp	int_common_0_7

	.align	4
	.global	_irq6_vector
_irq6_vector:
	pushal
	movl	#_vecdef+6*VECLINE_SIZEOF,r3	|* Entrada VECDEF IRQ6
	jmp	int_common_0_7

	.align	4
	.global	_irq7_vector
_irq7_vector:
	pushal
	movl	#_vecdef+7*VECLINE_SIZEOF,r3	|* Entrada VECDEF IRQ7
|*	jmp	int_common_0_7

|*
|******	Código comum para os IRQs de 0-7 ************************
|*
int_common_0_7:
|***	inb	#IO_NOP,r0

	movb	#0x20,r0		|* Dá ACK da interrupção unidade #1
	outb	r0,#IO_ICU1
	inb	#IO_NOP,r0

	jmp	int_common_0_15

|*
|******	Vetores IRQ8-15 *****************************************
|*
	.text
	.align	4
	.global	_irq8_vector
_irq8_vector:
	pushal
	movl	#_vecdef+8*VECLINE_SIZEOF,r3	|* Entrada VECDEF IRQ8
	jmp	int_common_8_15

	.align	4
	.global	_irq9_vector
_irq9_vector:
	pushal
	movl	#_vecdef+9*VECLINE_SIZEOF,r3	|* Entrada VECDEF IRQ9
	jmp	int_common_8_15

	.align	4
	.global	_irq10_vector
_irq10_vector:
	pushal
	movl	#_vecdef+10*VECLINE_SIZEOF,r3	|* Entrada VECDEF IRQ10
	jmp	int_common_8_15

	.align	4
	.global	_irq11_vector
_irq11_vector:
	pushal
	movl	#_vecdef+11*VECLINE_SIZEOF,r3	|* Entrada VECDEF IRQ11
	jmp	int_common_8_15

	.align	4
	.global	_irq12_vector
_irq12_vector:
	pushal
	movl	#_vecdef+12*VECLINE_SIZEOF,r3	|* Entrada VECDEF IRQ12
	jmp	int_common_8_15

	.align	4
	.global	_irq13_vector
_irq13_vector:
	pushal
	movl	#_vecdef+13*VECLINE_SIZEOF,r3	|* Entrada VECDEF IRQ13
	jmp	int_common_8_15

	.align	4
	.global	_irq14_vector
_irq14_vector:
	pushal
	movl	#_vecdef+14*VECLINE_SIZEOF,r3	|* Entrada VECDEF IRQ14
	jmp	int_common_8_15

	.align	4
	.global	_irq15_vector
_irq15_vector:
	pushal
	movl	#_vecdef+15*VECLINE_SIZEOF,r3	|* Entrada VECDEF IRQ15
|*	jmp	int_common_8_15

|*
|******	Código comum para os IRQs de 8-15 ***********************
|*
int_common_8_15:
|***	inb	#IO_NOP,r0

	movb	#0x20,r0		|* Dá ACK da interrupção unidade #1
	outb	r0,#IO_ICU1
	inb	#IO_NOP,r0

	movb	#0x20,r0		|* Dá ACK da interrupção unidade #2
	outb	r0,#IO_ICU2
	inb	#IO_NOP,r0

|*	jmp	int_common_0_15

|*
|******	Código comum para todos os IRQs *************************
|*
|*	r3 = entrada em VECDEFLINE correspondente à IRQ sendo atendida 
|*
int_common_0_15:
	pushl	ds
	pushl	es

	movw	#KERNEL_DS,r0
	movw	r0,ds
	movw	r0,es

|***	inb	#IO_NOP,r0

|***	movl	.....,r3
	movl	VEC_PL(r3),r0			|* r0 = Prioridade nova

.if	INTR_PATTERN
	movl	_intr_pattern_addr(*,r0@l),r4	|* Padrão de interrupção
	movl	_intr_videoaddr,r5
	movl	#INTR_PAT_SZ/4,r1

	up
	rep
	movsl
.endif	INTR_PATTERN

	pushl	_cpl_level		|* PL corrente
	pushl	VEC_UNIT(r3)		|* Unidade

	movl	r0,_cpl_level		|* Atribui a prioridada nova
	movl	r0,_cpl_mask		|* Atribui a máscara nova
	movl	_pl_mask(*,r0@l),r0	|* r0 = máscara do nível novo

   	outb	r0,#IO_ICU1+1		|* Atualiza ICU #1
	inb	#IO_NOP,r0
|***	inb	#IO_NOP,r0

	movb	h0,r0			|* Atualiza ICU #2
	outb	r0,#IO_ICU2+1
|***	inb	#IO_NOP,r0
|***	inb	#IO_NOP,r0

	pushl	#1			|* Código da interrupção

	testb	#3,14*4(sp)		|* Guarda o "frame" se for de usuário
	jeq	8$
	movl	sp,u.u_frame
8$:
	enint				|* Habilita as interruções
9$:
	call	VEC_FUNC(r3)		|* Atende a interrupção

	addl	#VEC_SIZEOF,r3		|* Aponta para a próxima unidade compartilhada
	cmpl	#0,VEC_FUNC(r3)
	jeq	int_coda

	movl	VEC_UNIT(r3),r0		|* coloca a nova unidade
	movl	r0,4(sp)

	jmp	9$

|*
|****** Epílogo da interrupção **********************************
|*
int_coda:
	disint				|* Desabilita as interrupções

|*	popl	r3			|* Remove o código
|*	popl	r3			|* Remove a unidade
|*	popl	r3			|* Obtém a prioridade anterior
	movl	8(sp),r3		|* Obtém a prioridade anterior

	movl	_pl_mask(*,r3@l),r0	|* r0 = máscara do nível anterior

	outb	r0,#IO_ICU1+1		|* Atualiza ICU #1
	inb	#IO_NOP,r0

	movb	h0,r0			|* Atualiza ICU #2
	outb	r0,#IO_ICU2+1
|***	inb	#IO_NOP,r0

	movl	r3,_cpl_level		|* Atribui a prioridada nova
	movl	r3,_cpl_mask		|* Atribui a máscara nova

.if	INTR_PATTERN
	movl	_intr_pattern_addr(*,r3@l),r4	|* Padrão de interrupção
	movl	_intr_videoaddr,r5
	movl	#INTR_PAT_SZ/4,r1

	up
	rep
	movsl
.endif	INTR_PATTERN

	tstl	r3			|* Não troca de processo se SPL > 0
	jne	9$

	movl	u.u_proc,r0		|* Pega endereço da tabela PROC
	movw	proc.p_pri(r0),r1	|* Pega curpri
	cmpw	_bestpri,r1		|* Retorna se curpri >= bestpri
	jge	9$

|*	testb	#3,44(sp)		|* testa se é modo USER
	testb	#3,14*4(sp)		|* testa se é modo USER
	jne	1$			|* se for USER troca de processo logo

|*	Interrupção foi em modo supervisor

|*	movl	#_preemption_flag,r0	|* Se flag == 0 não troca
|*	movb	CPUID(r0),r1		|* de processo neste momento
|*	movl	#_preemption_mask,r0	|* Se mask == 0 não troca
|*	testb	r1,CPUID(r0)		|* de processo em modo supervisor
|*	jeq	9$			|* Preemption só quando (flag & mask)

	|* Falta o CPUID

	cmpb	#0,_preemption_flag	|* Se flag == 0 não troca
	jeq	9$			|* de processo neste momento

	movl	u.u_proc,r0		|* Não troca para um processo que
	cmpl	#SRUN,proc.p_state(r0)	|* já não vai mais rodar mesmo
	jne	9$			|* (estado dele já não é mais SRUN)

	cmpb	#26,_CSW
	jne	1$

	pushl	#'@'			|* Imprime "@" se houve preemtion
	call	_putchar		|* em modo supervisor
	addl	#4,sp

|*	Efetiva a troca do processo corrente

1$:
	enint				|* Habilita as interrupções
	call	_swrunproc		|* Troca de processo

	movl	u.u_proc,r0		|* Pega de novo endereço da tabela PROC
	cmpl	#0,proc.p_sig(r0)	|* Verifica se há sinais pendentes
	jeq	9$			|* Retorna se não há sinais pendentes

|*	pushl	40(sp)			|* Põe PC do usuário na stack
	pushl	52(sp)			|* Põe PC do usuário na stack
	pushl	#0			|* Põe fault address = 0 na stack
	call	_sigexec		|* Processa os sinais
	addl	#8,sp

9$:
.if 0	|*******************************************************|
	pushfl
	andl	#0x00004000,(sp)	|* Apara o bit NT
	jeq	10$
	call	_nt_bit_on		|* Imprime a mensagem
10$:
	addl	#4,sp			|* Tira o Eflags
.endif	|*******************************************************|

	cmpb	#0,_mmu_dirty+CPUID	|* Carrega a gerência de memória
	jeq	30$

	movl	u.u_proc,r0
	movl	proc.p_mmu_dir(r0),r0
	subl	#SYS_ADDR,r0
	movl	r0,cr3	

	clrb	_mmu_dirty+CPUID
30$:
	addl	#12,sp			|* Retira código, máscara e prioridade
	popl	es
	popl	ds

	popal

	pushfl
	andl	#~0x00004000,(sp)	|* Apara o bit NT
	popfl

	rte

|*
|******	Vetor da interrupção espúria 32 *************************
|*
	.global	_spurious_32_vector
	.align	4
_spurious_32_vector:
	pushl	#32-16			|* No. do IRQ
	jmp	_spurious_common

|*
|******	Vetor da interrupção espúria 33 *************************
|*
	.global	_spurious_33_vector
	.align	4
_spurious_33_vector:
	pushl	#33-16			|* No. do IRQ
	jmp	_spurious_common

|*
|******	Vetor da interrupção espúria 34 *************************
|*
	.global	_spurious_34_vector
	.align	4
_spurious_34_vector:
	pushl	#34-16			|* No. do IRQ
	jmp	_spurious_common

|*
|******	Vetor da interrupção espúria 35 *************************
|*
	.global	_spurious_35_vector
	.align	4
_spurious_35_vector:
	pushl	#35-16			|* No. do IRQ
	jmp	_spurious_common

|*
|******	Vetor da interrupção espúria 36 *************************
|*
	.global	_spurious_36_vector
	.align	4
_spurious_36_vector:
	pushl	#36-16			|* No. do IRQ
	jmp	_spurious_common

|*
|******	Vetor da interrupção espúria 37 *************************
|*
	.global	_spurious_37_vector
	.align	4
_spurious_37_vector:
	pushl	#37-16			|* No. do IRQ
	jmp	_spurious_common

|*
|******	Vetor da interrupção espúria 38 *************************
|*
	.global	_spurious_38_vector
	.align	4
_spurious_38_vector:
	pushl	#38-16			|* No. do IRQ
	jmp	_spurious_common

|*
|******	Vetor da interrupção espúria 39 *************************
|*
	.global	_spurious_39_vector
	.align	4
_spurious_39_vector:
	pushl	#39-16			|* No. do IRQ
	jmp	_spurious_common

|*
|******	Vetor da interrupção espúria 40 *************************
|*
	.global	_spurious_40_vector
	.align	4
_spurious_40_vector:
	pushl	#40-16			|* No. do IRQ
	jmp	_spurious_common

|*
|******	Vetor da interrupção espúria 41 *************************
|*
	.global	_spurious_41_vector
	.align	4
_spurious_41_vector:
	pushl	#41-16			|* No. do IRQ
	jmp	_spurious_common

|*
|******	Vetor da interrupção espúria 42 *************************
|*
	.global	_spurious_42_vector
	.align	4
_spurious_42_vector:
	pushl	#42-16			|* No. do IRQ
	jmp	_spurious_common

|*
|******	Vetor da interrupção espúria 43 *************************
|*
	.global	_spurious_43_vector
	.align	4
_spurious_43_vector:
	pushl	#43-16			|* No. do IRQ
	jmp	_spurious_common

|*
|******	Vetor da interrupção espúria 44 *************************
|*
	.global	_spurious_44_vector
	.align	4
_spurious_44_vector:
	pushl	#44-16			|* No. do IRQ
	jmp	_spurious_common

|*
|******	Vetor da interrupção espúria 45 *************************
|*
	.global	_spurious_45_vector
	.align	4
_spurious_45_vector:
	pushl	#45-16			|* No. do IRQ
	jmp	_spurious_common

|*
|******	Vetor da interrupção espúria 46 *************************
|*
	.global	_spurious_46_vector
	.align	4
_spurious_46_vector:
	pushl	#46-16			|* No. do IRQ
	jmp	_spurious_common

|*
|******	Vetor da interrupção espúria 47 *************************
|*
	.global	_spurious_47_vector
	.align	4
_spurious_47_vector:
	pushl	#47-16			|* No. do IRQ
	jmp	_spurious_common

|*
|******	Vetor das interrupções espúrias 48-255 ******************
|*
	.global	_spurious_255_vector
	.align	4
_spurious_255_vector:
	pushl	#255-16			|* No. do IRQ
	jmp	_spurious_common

|*
|****** Parte comum das	interrupções espúrias *************************
|*
SPURIOUS_PL	= 7

	.text
_spurious_common:
	popl	_spurious_unit		|* Guarda o no. do IRQ

	pushal
	inb	#IO_NOP,r0

	movb	#0x03,r0		|* Examina o ISR #1
	outb	r0,#IO_ICU1
	inb	#IO_NOP,r0

	movb	#0x03,r0		|* Examina o ISR #2
	outb	r0,#IO_ICU2
	inb	#IO_NOP,r0

	inb	#IO_ICU1,r0		|* Obtém o ISR #1
	movb	r0,r2
	inb	#IO_NOP,r0

	movb	#0x02,r0		|* Examina o IRR #1
	outb	r0,#IO_ICU1
	inb	#IO_NOP,r0

	movb	#0x02,r0		|* Examina o IRR #2
	outb	r0,#IO_ICU2
	inb	#IO_NOP,r0

	inb	#IO_ICU2,r0		|* Obtém o ISR #2
	movb	r0,h2
	inb	#IO_NOP,r0

	movb	#0x20,r0		|* Dá ACK da interrupção unidade #1
	outb	r0,#IO_ICU1
	inb	#IO_NOP,r0

	movb	#0x20,r0		|* Dá ACK da interrupção unidade #2
	outb	r0,#IO_ICU2
	inb	#IO_NOP,r0

	pushl	ds
	pushl	es

	movw	#KERNEL_DS,r0
	movw	r0,ds
	movw	r0,es

	pushl	_cpl_level		|* PL corrente
	movl	r2,r0
	lsll	#16,r0
	movw	_spurious_unit,r0
	pushl	r0			|* Unidade

	movl	#SPURIOUS_PL,_cpl_level	|* Atribui a prioridada nova
	movl	#SPURIOUS_PL,_cpl_mask	|* Atribui a máscara nova
	movl	_pl_mask+SPURIOUS_PL*4,r0 |* r0 = máscara do nível novo

	outb	r0,#IO_ICU1+1		|* Atualiza ICU #1
	inb	#IO_NOP,r0

	movb	h0,r0			|* Atualiza ICU #2
	outb	r0,#IO_ICU2+1
	inb	#IO_NOP,r0

	pushl	#1			|* Código da interrupção

	testb	#3,14*4(sp)		|* Guarda o "frame" se for de usuário
	jeq	1$
	movl	sp,u.u_frame
1$:
	enint				|* Habilita as interruções

	call	_spurious_int

	jmp	int_coda

|*
|****** Variáveis ***********************************************
|*
	.data
	.global	_spurious_unit
_spurious_unit:
	.long	0
