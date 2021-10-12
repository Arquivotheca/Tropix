|*
|****************************************************************
|*								*
|*			start.s					*
|*								*
|*	Inicialização do núcleo do sistema operacional		*
|*								*
|*	Versão	3.0.0, de 30.04.94				*
|*		4.6.0, de 12.08.04				*
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
|****** Variáveis globais ***************************************
|*
	.data
	.global	_end_base_bss	|* Final da parte usada abaixo de 640 KB
_end_base_bss:
	.long	_end

	.global	_end_ext_bss	|* Final da parte usada acima de 1 MB
_end_ext_bss:
	.long	SYS_ADDR + 1024 * KBSZ

	.global	_phys_mem	 |* Memória total em KB
_phys_mem:
	.long	0

	.global	_video_phys_area |* Endereço físico linear do SVGA
_video_phys_area:
	.long	0

|*
|******	Diretório das tabelas de páginas ************************
|*
	.global	_kernel_page_directory
_kernel_page_directory:
	.long	0

|*
|******	Tabelas de páginas do NÚCLEO ****************************
|*
	.global	_kernel_page_table
_kernel_page_table:
	.long	0

	.global	_extra_page_table
_extra_page_table:
	.long	0

	.global	_svga_page_table
_svga_page_table:
	.long	0

|*
|******	Endereço da PPDA do processo 0 **************************
|*
	.global _uproc_0
_uproc_0:
	.long	0

|*
|****************************************************************
|*	Início do TROPIX para PC				*
|****************************************************************
|*
|*	O KERNEL já inicia em 3 Gb!
|*
|*	É chamado na forma:	(*entry) (&bcb);
|*
	.text
	.global	start
start:
	addl	#4,sp			|* Retira o retorno ao boot
	call	_get_bcb		|* get_bcb (&bcb);

|*
|******	Carrega a tabela de segmentação *************************
|*
	lgdtl	gdt_48

	farjmpl	#KERNEL_CS,1$
1$:
	movw	#KERNEL_DS,r0
	movw	r0,ds
	movw	r0,es
	movw	r0,fs
	movw	r0,gs
	movw	r0,ss

   |***	movl	#end_stack,sp	|* Não pode usa-la até nova gerência

|*
|****** Prepara a nova paginação ********************************
|*
	movl	_end_ext_bss,r5		|* Aloca espaço para o PDT acima de 1 MB
   |***	addl	#PGSZ-1,r5		
   |***	andl	#~[PGSZ-1],r5		|* Já está arredondado para páginas por "get_bcb"
	movl	r5,_kernel_page_directory

|*	Zera a "kernel_page_directory"

	movl	#PGSZ/4,r1
   |***	movl	_kernel_page_directory,r5
	clrl	r0
   	up
	rep
	stosl

|*	Prepara as tabelas de páginas para a memória física

	movl	r5,_kernel_page_table

	movl	_phys_mem,r1		|* r1 = No. de bytes
	lsll	#KBSHIFT,r1

	addl	#PGSZ-1,r1		|* Arredonda para páginas
   |***	andl	#~[PGSZ-1],r1
	lsrl	#PGSHIFT,r1		|* r1 = No. de páginas (== PTEs)

	movl	#0|SRW,r0		|* R/W, Supervisor, Present
   |***	up
2$:
	stosl
	addl	#PGSZ,r0
	loop	2$

	clrl	r0
3$:
	testl	#PGSZ-1,r5		|* Zera o restante da Page Table
	jz	4$
	stosl
	jmp	3$

|*	Prepara as tabelas de páginas para a PPDA do processo 0
4$:
   	movl	r5,_extra_page_table

   	movl	r5,r0			|* PT "extra" tem apenas 1 página
   	addl	#PGSZ,r0
   	movl	r0,_uproc_0

	movl	#USIZE,r1		|* r1 = No. de páginas (== PTEs)
	andl	#~SYS_ADDR,r0
	orl	#SRW,r0			|* R/W, Supervisor, Present
   |***	up
5$:
	stosl
	addl	#PGSZ,r0
	loop	5$

	clrl	r0
6$:
	testl	#PGSZ-1,r5		|* Zera o restante da Page Table
	jz	7$
	stosl
	jmp	6$

|*	Zera a PPDA do processo 0
7$:
	movl	#USIZE*PGSZ/4,r1
   |*** movl	_uproc_0,r5
   |***	clrl	r0
   |***	up
	rep
	stosl

	movl	r5,_end_ext_bss		|* Atualiza o "final" do BSS (>= 1MB)

|*	Prepara as entradas do diretório de páginas para memória física

	movl	_extra_page_table,r1	|* r1 = No. de Page Tables
	subl	_kernel_page_table,r1
	lsrl	#PGSHIFT,r1

	movl	_kernel_page_table,r0
	andl	#~SYS_ADDR,r0		|* Endereço físico
	orl	#SRW,r0			|* R/W, Supervisor, Present
   |***	up

	movl	_kernel_page_directory,r5
	addl	#SYS_ADDR>>PG_DIR_SHIFT,r5
8$:
	stosl
	addl	#PGSZ,r0
	loop	8$

|*	Prepara as entradas do diretório de páginas para a PPDA do processo 0

	movl	#1,r1			|* r1 = No. de Page Tables

	movl	_extra_page_table,r0
	andl	#~SYS_ADDR,r0		|* Endereço físico
	orl	#SRW,r0			|* R/W, Supervisor, Present
   |***	up

	movl	_kernel_page_directory,r5
	addl	#_u>>PG_DIR_SHIFT,r5
9$:
	stosl
	addl	#PGSZ,r0
	loop	9$

|*
|****** Prepara a paginação para o área do SVGA *****************
|*
svga_begin:
	movl	_video_phys_area,r0
	tstl	r0
	jz	svga_end

	movl	_end_ext_bss,r5
	movl	r5,_svga_page_table

	movl	#PGSZ/4,r1
	orl	#SRW,r0			|* R/W, Supervisor, Present
   |***	up
15$:
	stosl
	addl	#PGSZ,r0
	loop	15$

	movl	r5,_end_ext_bss		|* Atualiza o "final" do BSS (>= 1MB)

	movl	#1,r1			|* r1 = No. de Page Tables

	movl	_svga_page_table,r0
	andl	#~SYS_ADDR,r0		|* Endereço físico
	orl	#SRW,r0			|* R/W, Supervisor, Present
   |***	up

	movl	_kernel_page_directory,r5
	addl	#SVGA_ADDR>>PG_DIR_SHIFT,r5
19$:
	stosl
	addl	#PGSZ,r0
	loop	19$

svga_end:

|*
|******	Liga a nova paginação ***********************************
|*
	movl	_kernel_page_directory,r0
	andl	#~SYS_ADDR,r0
	movl	r0,cr3	

	jmp	20$
20$:

|*
|******	Zera toda a memória *************************************
|*
.if 0	|*******************************************************|
	movl	_end_base_bss,r5	|* A parte abaixo de 640 Kb
	movl	#SYS_ADDR+640*KBSZ,r1
	subl	r5,r1
	lsrl	#2,r1
	clrl	r0
   |*** up
	rep
	stosl

	movl	_end_ext_bss,r5		|* A parte acima de 1 MB
	movl	_phys_mem,r1
	subl	#4096,r1		|* Não zera o RAMD de 4 MB !
	lsll	#KBSHIFT,r1
	addl	#SYS_ADDR,r1
	subl	r5,r1
	lsrl	#2,r1
   |***	clrl	r0
   |*** up
	rep
	stosl
.endif	|*******************************************************|

|*
|******	Usa a nova PILHA (já da PPDA do processo 0) *************
|*
   	movl	#_u+USIZE*PGSZ,sp

|*
|****** Inicializa IDT ******************************************
|*
	call	_init_idt

|*
|****** Inicializa as 8259s *************************************
|*
	call	_init_irq

|*
|******	Carrega a tabela de interrupção *************************
|*
	lidtl	_idt_48

|*
|******	Prepara o TSS e SYSCALL *********************************
|*
	movl	#_tss,r0		|* Coloca o endereço de "tss"
	movl	#_gdt+[KERNEL_TSS & ~7],r1

	movw	r0,2(r1)
	lsrl	#16,r0
	movb	r0,4(r1)
	movb	h0,7(r1)

	call	_init_syscall

	movl	#KERNEL_TSS,r0
	ltr	r0

|*
|****** Inicia as tabelas ***************************************
|*
	call	_cpusetup

	call	_scbsetup

	call	_main

|*
|****** Carrega a gerência de memória ***************************
|*
	movl	u.u_mmu_dir,r0
	andl	#~SYS_ADDR,r0
	movl	r0,cr3	

	clrb	_mmu_dirty+CPUID
|*
|****** Desvia para modo usuário ********************************
|*
	pushl	#USER_DS		|* ss
	pushl	#USER_STACK_VA		|* usp
	pushl	#USER_CS		|* cs
	pushl	#USER_DATA_VA		|* pc

	movw	#USER_DS,r0
	movw	r0,ds
	movw	r0,es
	movw	r0,fs
	movw	r0,gs

	farrts

|*
|****** Tabela de segmentação ***********************************
|*
	.const
|*
|*	Descritor para a carga de GDT
|*
	.align	4		|* Para evitar o buraco!
	.word	0
gdt_48:
	.word	end_gdt-_gdt-1	|* Limite
	.long	_gdt		|* Endereço

|*
|*	A Tabela GDT propriamente dita
|*
	.data
	.global	_gdt
_gdt:

|*	Entrada No. 0: Não usada

	.long	0, 0		|* Não usado

|*	Entrada No. 1: Código do KERNEL (Seletor = 1 * 8 + 0 = 8)

	.word	0xFFFF		|* Limite = 4 Gb
	.word	0		|* Base   = 0
	.word	0x9A00		|* Código leitura/execução, dpl = 0
	.word	0x00CF		|* Grão = 4 Kb, 32 bits

|*	Entrada No. 2: Dados do KERNEL (Seletor = 2 * 8 + 0 = 16)

	.word	0xFFFF		|* Limite = 4 Gb
	.word	0		|* Base   = 0
	.word	0x9200		|* Dados leitura/escrita, dpl = 0
	.word	0x00CF		|* Grão = 4 Kb, 32 bits

|*	Entrada No. 3: Código do USUÁRIO (Seletor = 3 * 8 + 3 = 27)

	.word	0xFFFF		|* Limite = 4 Gb
	.word	0		|* Base   = 0
	.word	0xFA00		|* Código leitura/execução, dpl = 3
	.word	0x00CF		|* Grão = 4 Kb, 32 bits

|*	Entrada No. 4: Dados do USUÁRIO (Seletor = 4 * 8 + 3 = 35)

	.word	0xFFFF		|* Limite = 4 Gb
	.word	0		|* Base   = 0
	.word	0xF200		|* Dados leitura/escrita, dpl = 3
	.word	0x00CF		|* Grão = 4 Kb, 32 bits

|*	Entrada No. 5: TSS (Seletor = 5 * 8 + 0 = 40)

	.word	103		|* Limite = 103 (sizeof (tss) - 1)
	.word	0		|* Base   = 0 (vai ser colocado)
	.word	0x8900		|* TSS, dpl = 0
	.word	0x0000		|*

|*	Entrada No. 6: SYS_CALL (Seletor = 6 * 8 + 3 = 51)

	.word	0		|* Offset (baixo): "init_syscall"
	.word	KERNEL_CS	|* Seletor
	.word	0xEC00		|* CALLGATE, dpl = 3
	.word	0		|* Offset (alto): "init_syscall"

end_gdt:

|*
|****************************************************************
|*  Converte um longo de little em big endian (ou vice-versa)	*
|****************************************************************
|*
	.text
	.global	_long_endian_cv
_long_endian_cv:
	movl	4(sp),r0

	bswap	r0

|*	xchgb	r0,h0
|*	rorl	#16,r0
|*	xchgb	r0,h0

	rts

|*
|****************************************************************
|*  Converte um short de little em big endian (ou vice-versa)	*
|****************************************************************
|*
	.global	_short_endian_cv
_short_endian_cv:
	clrl	r0
	movw	4(sp),r0

	xchgb	r0,h0

	rts

|*
|****************************************************************
|*	Obtém o valor de "cr0"					*
|****************************************************************
|*
	.global	_get_cr0
_get_cr0:
	movl	cr0,r0
	rts

|*
|****************************************************************
|*	Atribui o valor de "cr0"				*
|****************************************************************
|*
	.global	_put_cr0
_put_cr0:
	movl	4(sp),r0
	movl	r0,cr0
	rts

|*
|****************************************************************
|*	Obtém o valor de "cr2"					*
|****************************************************************
|*
	.global	_get_cr2
_get_cr2:
	movl	cr2,r0
	rts

|*
|****************************************************************
|*	Dá um "flush" na MMU, se necessário			*
|****************************************************************
|*
	.global	_load_cr3
_load_cr3:
	cmpb	#0,_mmu_dirty+CPUID
	jeq	1$

	movl	u.u_mmu_dir,r0
	andl	#~SYS_ADDR,r0
	movl	r0,cr3	

	clrb	_mmu_dirty+CPUID
1$:
	rts

|*
|****************************************************************
|*	Invalida entradas da TLB				*
|****************************************************************
|*
	.global	_inval_TLB
_inval_TLB:
	movl	4(sp),r0
	lsll	#PGSHIFT,r0		|* r0 = endereço virtual (em bytes)
	movl	8(sp),r1		|* r1 = nr. de páginas

1$:
	invlpg	(r0)
	addl	#PGSZ,r0
	loop	1$

	rts

|*
|****************************************************************
|*	Obtém o valor de "fp"					*
|****************************************************************
|*
	.global	_get_fp
_get_fp:
	movl	fp,r0
	rts

|*
|****************************************************************
|*	Obtém o valor de "sr"					*
|****************************************************************
|*
	.global	_get_sr
_get_sr:
	pushfl
	popl	r0
	rts

|*
|****************************************************************
|*	Dá um RESET na CPU					*
|****************************************************************
|*
	.global	_reset_cpu
_reset_cpu:
	lgdtl	reset_gdt_48

	jmp	.
|*	rts

|******	GDT Nulo ************************************************

	.const
	.align	4		|* Para evitar o buraco!
	.word	0
reset_gdt_48:
	.word	0
	.long	0
