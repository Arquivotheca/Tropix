|*
|****************************************************************
|*								*
|*			common.s				*
|*								*
|*	Cabeçalho para programas em "assembly"			*
|*								*
|*	Versão	3.0.0, de 12.08.94				*
|*		4.6.0, de 16.06.04				*
|*								*
|*	Módulo: Núcleo						*
|*		NÚCLEO do TROPIX para PC			*
|*								*
|*	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
|*		Copyright © 2004 NCE/UFRJ - tecle "man licença"	*
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
SYS_ADDR	= 2 * GBSZ			|* 0x80000000
UPROC_ADDR	= [2048 - 4] * MBSZ		|* 0x7FC00000
SVGA_ADDR	= [2048 - 8] * MBSZ		|* 0x7F800000

PG_DIR_SHIFT	= 20		|* Para deslocamento do dir. de pg.

SRW		= 3		|* Página presente, R/W, supervisor
URW		= 7		|* Página presente, R/W, user

USER_TEXT_VA	= 	     4   * MBSZ
USER_DATA_VA	= 	     256 * MBSZ
USER_STACK_VA	= 1 * GBSZ + 252 * MBSZ

|*
|****** Definições da CPU ***************************************
|*
CPUID		= 0

|*
|****** Definições da UPROC *************************************
|*
USIZE		= 2		|* Tamanho (PG) da UPROC

SRUN		= 5		|* Depende do "enum" do "proc.h"

	.global	_u
_u		= UPROC_ADDR

|*	Deslocamentos

U_CPU			= 0			(short)
U_PRI			= 2			(short)

U_CTXT_SW_TYPE		= 4			(char)
U_NO_USER_MMU		= 5			(char)

U_STATE			= 8
U_SIG			= 12
U_MMU_DIR		= 16

U_DONTPANIC		= 20
U_FRAME			= 24

U_CTXT			= 28

|*	Endereço virtual

u.u_cpu			= _u + U_CPU		(short)
u.u_pri			= _u + U_PRI		(short)

u.u_ctxt_sw_type	= _u + U_CTXT_SW_TYPE	(char)
u.u_no_user_mmu		= _u + U_NO_USER_MMU	(char)

u.u_state		= _u + U_STATE
u.u_sig			= _u + U_SIG
u.u_mmu_dir		= _u + U_MMU_DIR

u.u_dontpanic		= _u + U_DONTPANIC
u.u_frame		= _u + U_FRAME

u.u_ctxt		= _u + U_CTXT

|*
|****** Definições do VIDEO *************************************
|*
VGA_ADDR	= SYS_ADDR + 736 * KBSZ

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

VEC_UNIT	= 0		|* Unidade (informação para o driver)
VEC_FUNC	= 4		|* Função de interrupção

VECLINESZ	= 8		|* Compartilhando até 7 unidades

VEC_SIZEOF	= 8		|* Tamanho de uma estrutura

VECLINE_SIZEOF	= VEC_SIZEOF * VECLINESZ |* Tamanho da estrutura total

|*
|****** Endereços de portas *************************************
|*
IO_NOP		= 0x84		|* Dispositivo inexistente para NOP

IO_ICU1		= 0x20		|* Controlador 8259A No. 1
IO_ICU2		= 0xA0		|* Controlador 8259A No. 2

|*
|****** Habilita/desabilita as "lampadinhas" ********************
|*
IDLE_PATTERN	= 1		|* Padrão do "idle"
INTR_PATTERN	= 1		|* Padrão da interrupção
