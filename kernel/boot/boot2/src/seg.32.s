|*
|****************************************************************
|*								*
|*			seg.32.s				*
|*								*
|*	Liga a paginação e funções auxiliares			*
|*								*
|*	Versão	3.0.0, de 10.04.94				*
|*		4.9.0, de 08.08.07				*
|*								*
|*	Módulo: Boot2						*
|*		NÚCLEO do TROPIX para PC			*
|*								*
|*	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
|*		Copyright © 2007 NCE/UFRJ - tecle "man licença"	*
|*								*
|****************************************************************
|*

	.seg32

|*
|****************************************************************
|*	Definições globais					*
|****************************************************************
|*
KBSZ		= 1024
MBSZ		= 1024 * 1024
GBSZ		= 1024 * 1024 * 1024

BOOT2LOAD	= 64 * KBSZ + 80	|* Deve ser múltiplo de 16

BOOT2EXEC	= 1  * MBSZ
DIFF		= BOOT2EXEC - BOOT2LOAD

STACKLOAD	= BOOT2LOAD
STACKEXEC	= BOOT2EXEC + 8 * MBSZ
MMUTABLE	= BOOT2EXEC + 8 * MBSZ		|* Alinhado em 4 KB

MEM_MB_SZ	= 2048		|* Endereçamento da memória Física

PGSZ		= 4096

SYS_ADDR	= 2 * GBSZ	|* 0x80000000

SVGA_ADDR	= 2040 * MBSZ	|* Deve ser igual à "h/mmu.h"

PG_DIR_SHIFT	= 20		|* Para deslocamento do dir. de pg.

PGPROT		= 3		|* Página presente, R/W, supervisor

IO_NOP		= 0x84		|* Dispositivo inexistente para NOP

|*
|****** Parâmetros do video *************************************
|*
LINE		= 24
COL		= 80

|*
|****** Variáveis globais ***************************************
|*
	.data
_boot_directory_table:
	.long	0

	.global	_malloc_area_begin
_malloc_area_begin:
	.long	0

	.global	_cpu_vendor	|* Cadeia de identificação do fabricante da CPU
_cpu_vendor:
	.dsl	4

|*
|****************************************************************
|*	Início do procesamento segmentado			*
|****************************************************************
|*
	.text
	.global	begin_of_segmentation
begin_of_segmentation:
	movw	#2*8,r0		|* Segmento no. 2 (data, read/write)
	movw	r0,ds
	movw	r0,es
	movw	r0,ss

	movl	#STACKLOAD,sp

|*
|****** Liga o A20 **********************************************
|*
1$:
	inb	#IO_NOP,r0	|* Espera a entrada ficar pronta
	inb	#0x64,r0
	testb	#2,r0
	jnz	1$

	movb	#0xD1,r0	|* comando de escrita
	outb	r0,#0x64
	inb	#IO_NOP,r0
2$:
	inb	#IO_NOP,r0	|* Espera a entrada ficar pronta
	inb	#0x64,r0
	testb	#2,r0
	jnz	2$

	movb	#0xDF,r0	|* Liga A20
	outb	r0,#0x60
	inb	#IO_NOP,r0
3$:
	inb	#IO_NOP,r0	|* Espera a entrada ficar pronta
	inb	#0x64,r0
	testb	#2,r0
	jnz	3$

|*
|****** Espera o A20 ativar *************************************
|*
a20_tst:
1$:
	movl	#0,r0
	movl	#MBSZ,r1

	movl	#10000000,r2
2$:
	movl	#0,(r0)
	movl	#1,(r1)

	cmpl	#0,(r0)
	jz	9$

	decl	r2
	jgt	2$

	jmp	1$
9$:

|*
|******	Auto cópia para 1 MB ************************************
|*
copy_up:
	movl	#BOOT2EXEC,r4
	movl	#_edata,r1
	subl	r4,r1
	lsrl	#2,r1

   	movl	#BOOT2LOAD,r4
   	movl	#BOOT2EXEC,r5
   	up
	rep
	movsl

	bral	up_point+DIFF	|* Desvia para acima de 1 MB
up_point:
	movl	#STACKEXEC,sp

|*
|****** Zera o BSS **********************************************
|*
	clrl	r0
	movl	#_edata,r5
	movl	#_end,r1
	subl	r5,r1
	lsrl	#2,r1
	up
	rep
	stosl

|*
|****** Coloca o cursor de verdade fora da tela *****************
|*
	cmpb    #0,_video_mode
	jne	cursor_end

	movl	#COL*[LINE+1],r3
	call	set_cursor

cursor_end:

|*
|****** Inicializa a console ************************************
|*
	call	_init_video

	pushl	#_pgversion
	call	_printf
	addl	#4,sp

|*
|****** Guarda o endereço do vetor de interrupções **************
|*
VESA =	0
.if	VESA

	sidt	_intr_addr

	.data
	.global	_intr_addr_limit, _intr_addr_base
	.align	4
	.word	0
_intr_addr:
_intr_addr_limit:
	.word	0xAAAA
_intr_addr_base:
	.long	0xBBBBCCCC

	.text
.endif	VESA

|*
|******	Verifica se é um 386, 486 ou PENTIUM ********************
|*
|*	Indicadores de "cr0":
|*
|*	PG	0x80000000	/* PaGing enable */
|*	CD	0x40000000	/* Cache Disable */
|*	NW	0x20000000	/* Not Write-through */
|*	
|*	AM	0x00040000	/* Alignment Mask (set to enable AC flag) */
|*	WP	0x00010000	/* Write Protect (honor page protect in all modes) */
|*	
|*	NE	0x00000020	/* Numeric Error enable (EX16 vs IRQ13) */
|*	ET	0x00000010	/* Extension Type (387 (if set) vs 287) */
|*	TS	0x00000008	/* Task Switched (if MP, trap ESC and WAIT) */
|*	EM	0x00000004	/* EMulate non-NPX coproc. (trap ESC only) */
|*	MP	0x00000002	/* "Math" Present (NPX or NPX emulator) */
|*	PE	0x00000001	/* Protected mode Enable */
|*	
	pushfl			|* push EFLAGS
	popl	r0		|* get EFLAGS
	andl	#~0x4000,r0	|* Apaga o bit NT
	movl	r0,r1		|* save original EFLAGS
	pushl	r1
	eorl	#0x40000,r0	|* flip AC bit in EFLAGS
	pushl	r0		|* copy to EFLAGS
	popfl			|* set EFLAGS
	pushfl			|* get new EFLAGS
	popl	r0		|* put it in r0
	eorl	r1,r0		|* change in flags
	andl	#0x40000,r0	|* check if AC bit changed
	jnz	10$		|* Desvia se 486 ou Pentium

|*	É um 386

	popfl			|* restore original EFLAGS

	movl	cr0,r0		|* 386
	andl	#0x80000011,r0	|* Save PG,PE,ET
	orl	#2,r0		|* set MP
	movl	r0,cr0

	movl	#386,r0		|* 386

	jmp	90$	

|*	Verifica se tem a instrução "cpuid"

10$:
	movl	r1,r0		|* get original EFLAGS
	eorl	#0x200000,r0	|* flip ID bit in EFLAGS
	pushl	r0		|* copy to EFLAGS
	popfl			|* set EFLAGS
	pushfl			|* get new EFLAGS
	popl	r0		|* put it in r0
	eorl	r1,r0		|* change in flags
	andl	#0x200000,r0	|* check if ID bit changed
	jnz	20$		|* Desvia se tem a instrução "cpuid"

|*	Não tem a instrução "cpuid": é um 486

15$:
	popfl			|* restore original EFLAGS

	movl	cr0,r0		|* 486
	andl	#0x80000011,r0	|* Save PG,PE,ET
	orl	#0x50022,r0	|* set AM, WP, NE and MP
	movl	r0,cr0

	movl	#486,r0		|* 486

	jmp	90$	

|*	Tem a instrução "cpuid": verifica se é um 486 ou PENTIUM

20$:
	clrl	r0
	cpuid

	movl	#_cpu_vendor,r5
	movl	r3,0(r5)
	movl	r2,4(r5)
	movl	r1,8(r5)

	clrl	r0
	incl	r0
	cpuid

	movl	r0,_cpu_id
	movl	r2,_cpu_features

|*	Examina a família

	andw	#0x0F00,r0	|* Desvia se ID < 5 ou seja é 486
	cmpw	#0x0500,r0
	jlo	15$

|*	É um PENTIUM

	popfl			|* restore original EFLAGS

	movl	cr0,r0		|* Pentium
	andl	#0x80000011,r0	|* Save PG,PE,ET
	orl	#0x50022,r0	|* set AM, WP, NE and MP
	movl	r0,cr0

	movl	#586,r0		|* Pentium

   |***	jmp	90$	

90$:
	movl	r0,_cpu_type

|*
|******	Prepara a paginação *************************************
|*
paging:
	movl	#MMUTABLE,r5	|* Espaço para dir_table + 16 page_table
   |***	addl	#PGSZ-1,r5
   |***	andl	#~{PGSZ-1},r5
   	movl	r5,_boot_directory_table

|*	Zera a "boot_directory_table"

	movl	#PGSZ/4,r1
   |***	movl	_boot_directory_table,r5
	clrl	r0
   	up
	rep
	stosl

|*	Prepara as entradas válidas da  "_boot_directory_table"

   |***	movl	r5,_boot_page_table
	movl	r5,r0 		 	|* == _boot_page_table
	orl	#PGPROT,r0		|* R/W, Supervisor, Present

   	movl	_boot_directory_table,r2
	movl	#[MEM_MB_SZ/4],r1
26$:
   	movl	r0,(r2)				|* End. virtual = 0
	movl	r0,SYS_ADDR>>PG_DIR_SHIFT(r2)	|* End. virtual = 2 Gb

	addl	#PGSZ,r0
	addl	#4,r2
	loop	26$

|*	Prepara as tabelas de páginas para MEM_MB_SZ MB de memória física

   |***	movl	_boot_page_table,r5
	movl	#[MEM_MB_SZ/4]*1024,r1	|* r1 = No. de páginas (== PTEs)
	movl	#0|PGPROT,r0		|* R/W, Supervisor, Present
   |***	up
30$:
	stosl
	addl	#PGSZ,r0
	loop	30$

	movl	r5,_malloc_area_begin	|* Guarda o final da tabela

|*	Prepara as entradas para a memória de vídeo SVGA

   	movl	_boot_directory_table,r2

   |***	movl	_malloc_area_begin,r5
	movl	r5,r0 		 	|* == _malloc_area_begin
	orl	#PGPROT,r0		|* R/W, Supervisor, Present

	movl	r0,SVGA_ADDR>>PG_DIR_SHIFT(r2)

   |***	movl	_malloc_area_begin,r5
	movl	#PGSZ/4,r1
	movl	#_supervga_info,r0
	movl	40(r0),r0
	orl	#PGPROT,r0		|* R/W, Supervisor, Present
   |***	up
40$:
	stosl
	addl	#PGSZ,r0
	loop	40$

	movl	r5,_malloc_area_begin	|* Guarda o final da tabela

|*	Liga a paginação

	movl	_boot_directory_table,r0
	movl	r0,cr3	

	movl	cr0,r0
	orl	#0x80000000,r0
	movl	r0,cr0

	jmp	50$			|* Para esvaziar o FIFO
50$:

	call	_main			|* Executa o BOOT2

	jmp	.			|* Não deve retornar

|*
|****************************************************************
|*	Acerta o cursor (pos. em r3)				*
|****************************************************************
|*
set_cursor:
	movw	#0x03D4,r2
	movb	#14,r0
	outb	r0,r2
	inb	#IO_NOP,r0

	movw	#0x03D5,r2
	movl	r3,r0
   |***	addl	#COL+2,r0
	lsrl	#9-1,r0
	andl	#0xFF,r0
	outb	r0,r2
	inb	#IO_NOP,r0

	movw	#0x03D4,r2
	movb	#15,r0
	outb	r0,r2
	inb	#IO_NOP,r0

	movw	#0x03D5,r2
	movl	r3,r0
   |***	addl	#COL+2,r0
   |***	lsrl	#1-1,r0 ***|
	andl	#0xFF,r0
	outb	r0,r2
	inb	#IO_NOP,r0

	rts

|*
|****************************************************************
|*  Converte um longo de little em big endian (ou vice-versa)	*
|****************************************************************
|*
	.global	_long_endian_cv
_long_endian_cv:
	movl	4(sp),r0

	xchgb	r0,h0
	rorl	#16,r0
	xchgb	r0,h0

	rts

|*
|****************************************************************
|*  Converte um short de little em big endian (ou vice-versa)	*
|****************************************************************
|*
	.global	_short_endian_cv
_short_endian_cv:
	movl	4(sp),r0

	xchgb	r0,h0

	rts

|*
|****************************************************************
|*	Dá um RESET na CPU					*
|****************************************************************
|*
	.global	_reset_cpu
_reset_cpu:
	lgdtl	reset_gdt_48
1$:
	jmp	1$

|******	GDT Nulo ************************************************

	.const
	.align	4		|* Para evitar o buraco!
	.word	0
reset_gdt_48:
	.word	0
	.long	0
