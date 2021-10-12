|*
|****************************************************************
|*								*
|*			seg.16.s				*
|*								*
|*	Prepara o PC para funcionar em 32 bits			*
|*								*
|*	Versão	3.0.0, de 20.04.94				*
|*		4.9.0, de 17.06.06				*
|*								*
|*	Módulo: Boot2						*
|*		NÚCLEO do TROPIX para PC			*
|*								*
|*	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
|*		Copyright © 2006 NCE/UFRJ - tecle "man licença"	*
|*								*
|****************************************************************
|*

|*
|*	Este módulo (código de 16 bits) deve ficar no começo
|*	do BOOT2 (e naturalmente não ocupar mais de 64 KB).
|*	Além disto, ele NÃO pode usar as áreas "data" nem "bss".
|*	A seqüência de operações é a seguinte:
|*
|*		1. BOOT1 carrega o BOOT2 em 64 KB
|*		2. BOOT2 passa para 32 bits
|*		3. BOOT2 autocopia-se para 1 MB
|*		4. BOOT2 desvia para 1 MB
|*
|*	Este módulo é "link-editado" para 1 MB e executado
|*	em 64 KB - portanto CUIDADO com relocáveis LONGOs!
|*
	.seg16		|* Fundamental !

|*
|****************************************************************
|*	Definições globais					*
|****************************************************************
|*
DEBUG		= 0			|* Com/sem depuração

KBSZ		= 1024
MBSZ		= 1024 * 1024
GBSZ		= 1024 * 1024 * 1024

BOOT2LOAD	= 64 * KBSZ + 80	|* Deve ser múltiplo de 16

BOOT2EXEC	= 1  * MBSZ
DIFF		= BOOT2EXEC - BOOT2LOAD

BOOT2SEG	= BOOT2LOAD / 16
STACKSEG	= [BOOT2LOAD - [64 * KBSZ]] / 16

COL		= 80

BIOS_MEM_SIZEOF	= 20
BIOS_MEM_MAGIC	= 0x534D4150

|*
|******	Estrutura de informação dos discos **********************
|*
D_PRESENT	= 0			|* Disco presente
D_INT13EXT	= 1			|* Aceita as extensões da INT13
D_NSECT		= 2			|* Número de setores
D_NHEAD		= 3			|* Número de cabeças

D_SIZEOF	= 4			|* Tamanho da estrutura

|*
|****** Estrutura PARTTB ****************************************
|*
PT_DEVCODE	= 0			|* Dispositivo
PT_OFFSET	= 8			|* Início da partição
PT_VIDEOMODE	= 17			|* Modo de vídeo

|*
|****** Variáveis da pilha **************************************
|*
UNIT		= -4

|*
|****************************************************************
|*	Comeco do Boot2						*
|****************************************************************
|*
	.text
boot2_start:
	movb	PT_DEVCODE(fp),r1	|* Guarda o dispositivo e início do "boot1"
	movl	PT_OFFSET(fp),r2	|* Guarda o início do "boot1"
	movb	PT_VIDEOMODE(fp),r3	|* Guarda o modo de vídeo

	movw	#BOOT2SEG,r0		|* Prepara os segmentos
	movw	r0,ds
	movw	r0,es

	movw	#STACKSEG,r0
	movw	r0,ss

	movb	r1,_boot1_devcode	|* Armazena o dispositivo e início do "boot1"
	movl	r2,_boot1_offset

	movw	#[64 * KBSZ - 4],fp	|* 1 KB para variáveis locais
	movw	#[63 * KBSZ - 4],sp

|*
|******	Desliga os motores dos disquettes ***********************
|*
	movw	#0x03F2,r2
	clrb	r0
	outb	r0,r2

|*
|****** Verifica se deve entrar no modo SVGA (1024x768) *********
|*
	cmpb	#'V',r3			|* Verifica se recebe do "boot1"
	jeq	no_svga

	cmpb	#'S',r3
	jeq	try_svga

	cmpb	#0,_video_mode
	jz	no_svga
try_svga:
	movw	#0x4F01,r0		|* Get mode information
	movw	#0x4105,r1		|* 1024x768, 256 cores, linear
	movw	#_supervga_info,r5
	int	#0x10

	movb	#1,r1
	cmpw	#0x004F,r0
	jne	supervga_failed

	movw    #_supervga_info,r5
	movl	40(r5),r0 
	movb	#2,r1
	tstl	r0
	jz	supervga_failed

	movw	#0x4F02,r0		|* Set video mode
	movw	#0x4105,r3		|* 1024x768, 256 cores, linear
	int	#0x10

	movb	#3,r1
	cmpw	#0x004F,r0
	jeq	end_of_video

|*	Não conseguiu entrar no modo SVGA

supervga_failed:
	movb	r1,_supervga_code
   |***	jmp	no_svga

|*
|****** NÃO entra no modo SVGA **********************************
|*
no_svga:
	clrb	_video_mode

|*	movb	#'\n',r0		|* Escreve um "\n"
|*	clrb	h3
|*	movb	#14,h0
|*	int	#0x10

|*
|****** Altera a tabela de geração de caracteres ****************
|*
	clrb	r3			|* Tabela # 0
	movb	#16,h3
	clrw	r2
	movw	#256,r1
	movw	#_video_8x16_tb,fp
	clrb	r0			|* Serviço 0: copia a tabela
	movb	#0x11,h0
	int	#0x10

	clrb	r3			|* Tabela # 0
	movb	#3,r0			|* Serviço 3: ativa a tabela
	movb	#0x11,h0
	int	#0x10

	movw	#[64 * KBSZ - 4],fp	|* Restaura o "fp"
|*	movw	#[63 * KBSZ - 4],sp

end_of_video:

|*
|******	Muda as cores *******************************************
|*
	movw	#0x1010,r0		|* Muda a cor
	movw	#1,r3			|* Cor 1 (azul)
	movb	#8,h2			|* R
	movb	#36,h1			|* G	Dodgerblue (30, 144, 255)
	movb	#63,r1			|* B
	int	#0x10

	movw	#0x1010,r0		|* Muda a cor
	movw	#2,r3			|* Cor 2 (verde)
	movb	#0,h2			|* R
	movb	#52,h1			|* G	Green3 (0, 205, 0)
	movb	#0,r1			|* B
	int	#0x10

	movw	#0x1010,r0		|* Muda a cor
	movw	#4,r3			|* Cor 4 (vermelho)
	movb	#63,h2			|* R
	movb	#18,h1			|* G	Orangered (255, 69, 0)
	movb	#0,r1			|* B
	int	#0x10

|*	movw	#0x1010,r0		|* Muda a cor
|*	movw	#4,r3			|* Cor 4 (vermelho)
|*	movb	#55,h2			|* R
|*	movb	#28,h1			|* G	Palevioletred (219, 112, 147)
|*	movb	#37,r1			|* B
|*	int	#0x10

|*
|******	Obtém o endereço final da memória utilizável ************
|*
get_mem_info:
	movw	#int15_area,r5		|* r5 = Área para int 0x15
	clrl	r3
1$:
	movl	#BIOS_MEM_SIZEOF,r1	|* r1 = Tamanho da entrada
	movl	#BIOS_MEM_MAGIC,r2	|* r2 = No. mágico
	movl	#0xE820,r0

	int	#0x15			|* Obtém a entrada corrente
	jcs	9$			|* Houve erro ?

	cmpl	#BIOS_MEM_MAGIC,r0	|* Confere o no. mágico
	jne	9$

	movl	16(r5),r0		|* Considera apenas RAM (1) e ACPI Data (3)
	cmpl	#1,r0
	jeq	2$
	cmpl	#3,r0
	jne	5$
2$:
	movl	 (r5),r0
	addl	8(r5),r0		|* r0 = endereço final da área

	cmpl	_end_mem_addr,r0	|* if (r0 > end_mem_addr)
	jls	5$

	movl	r0,_end_mem_addr	|*	end_mem_addr = r0
5$:
	tstl	r3			|* Fim da tabela ?
	jne	1$
9$:

|*
|******	Obtém as características dos discos *********************
|*
get_disk_info:
	movw	#_disk_info_table,r4	|* Começo da tabela
	movb	#0x80,UNIT(fp)		|* Disco inicial
1$:
	call	get_param
	jcs	seg_and_jmp

	addw	#D_SIZEOF,r4		|* Entrada seguinte
	incb	UNIT(fp)		|* Disco seguinte

	jmp	1$

|*
|****** Guarda o endereço do vetor de interrupções **************
|*
VESA =	0
.if	VESA

	sidt	_intr_addr

.endif	VESA

|*
|******	Liga a segmentação **************************************
|*
seg_and_jmp:
	pushfl				|* Para ver se a BIOS usa interrupções
	popl	r0
	movl	r0,_cpu_flags

	disint				|* Importante, pois algumas BIOS usam interrupções

	lgdtl	gdt_48

	movl	cr0,r0
	orw	#1,r0
	movl	r0,cr0

	farjmpl	#1*8,begin_of_segmentation-DIFF		|* Descritor no. 1

|*
|****** Função para obter características de um disco ***********
|*
|*	Argumento:	UNIT(fp)	|* Unidade
|*			r4		|* Tabela da unidade
|*
|*	Atualiza:	|* Informações da tabela:
|*
|*			D_PRESENT	|* Disco presente
|*			D_INT13EXT	|* Aceita as extensões da INT13
|*			D_NSECT		|* Número de setores
|*			D_NHEAD		|* Número de cabeças
|*
|*	Devolve o CF ligado se a unidade não existe
|*
|*	NÃO Conserva o r3!
|*
get_param:
	clrb	D_PRESENT(r4)		|* Por enquanto, não há Disco presente
	clrb	D_INT13EXT(r4)		|* Nem tem as extensões

	movb	#0x15,h0		|* 15 = verificar existência
	movb	UNIT(fp),r2		|* Dispositivo
	int	#0x13
	cmpb	#3,h0			|* Se != 3 NÃO existe
	beq	2$
1$:
	stc				|* Retorna com o CARRY ligado
	rts
2$:
	pushw	r4			|* Precauções com a INT 13/8
	pushw	r5
	movw	#disk_param_table,r5

	movb	#0x08,h0		|* 8 = obter características
	movb	UNIT(fp),r2		|* Dispositivo
	int	#0x13

	popw	r5
	popw	r4

	bcs	1$			|* Testa o CARRY

	tstb	h0			|* Testa também o "h0"
	bne	1$

	addb	#0x80,r2		|* Verifica se não passou do total
	cmpb	UNIT(fp),r2
	bls	1$

|* 	O disco existe

	incb	D_PRESENT(r4)		|* Há Disco presente

	andb	#0x3F,r1		|* Guarda o No. de setores/trilha
	movb	r1,D_NSECT(r4)

	incb	h2
	movb	h2,D_NHEAD(r4)		|* Guarda o No. de cabeças

|*	Verifica se tem as extensões da INT 13

   	movb	#0x41,h0		|* Installation check
	movb	UNIT(fp),r2		|* Dispositivo
	movw	#0x55AA,r3		|* Número Mágico
	int	#0x13

	bcs	9$			|* Desvia se retornou com o carry

	cmpw	#0xAA55,r3		|* Confere o 0xAA55
	bne	9$

	testb	#1,r1			|* O bit 0 deve estar ligado
	beq	9$

	incb	D_INT13EXT(r4)		|* Tem as extensões da INT 13
9$:
	clc
	rts

|*	Tabela (quem sabe quando?) preenchida pela INT 13/8

	.const
disk_param_table:
	.dsl	3
	.global	_cpu_flags
_cpu_flags:
	.long	0

.if	VESA

	.global	_intr_addr_limit, _intr_addr_base
	.word	0
_intr_addr:
_intr_addr_limit:
	.word	0xAAAA
_intr_addr_base:
	.long	0xBBBBCCCC

.endif	VESA

|*
|****************************************************************
|*	Tabela de segmentação					*
|****************************************************************
|*
	.const
	.align	4		|* Para evitar o buraco!
	.word	0
gdt_48:
	.word	end_gdt-_gdt-1	|* Limite
	.long	_gdt-DIFF	|* Endereço

|*
|******	A Tabela GDT propriamente dita **************************
|*
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

end_gdt:

|*
|****************************************************************
|*	Variáveis no ".const"					*
|****************************************************************
|*
	.const
	.global	_boot1_devcode, _boot1_offset, _end_mem_addr
.if 0	|*******************************************************|
	.global	_video_pos, _boot1_devcode, _boot1_offset, _end_mem_addr
_video_pos:
	.long	0			|* Posição do video
.endif	|*******************************************************|
_boot1_devcode:
	.long	0			|* Herdado do "boot1"
_boot1_offset:
	.long	0
_end_mem_addr:
	.long	0			|* Endereço final da memória utilizável

	.global	_disk_info_table
_disk_info_table:
	.dsb	33 * D_SIZEOF		|* Tamanho da tabela (32 discos)

	.global	int15_area
int15_area:
	.dsb	BIOS_MEM_SIZEOF		|* Área para a int 0x15, 0xE820

	.global	_video_mode, _supervga_code
_video_mode:
	.byte	1
_supervga_code:
	.byte	0

	.global	_supervga_info
_supervga_info:
	.dsb	256

.if	DEBUG
|*
|****************************************************************
|*	Escreve uma cadeia (endereço em r0)			*
|****************************************************************
|*
	.text
	.global	putstr
putstr:
	pushw	r4		|* Salva r4
	movw	r0,r4
1$:
	movb	(r4),r0		|* Obtém um caractere
	incw	r4

	cmpb	#'\0',r0	|* Verifica se chegou no final
	jeq	2$

	call	putc		|* Imprime um caractere
	jmp	1$
2$:
	popw	r4		|* Restaura r4
	rts

|*
|****************************************************************
|*	Imprime um caractere (em r0) na console			*
|****************************************************************
|*
	.global	putc
putc:
	pushw	r3		|* Salva r3

	cmpb	#'\n',r0	|* Verifica se é '\n'
	jne	1$

	movb	#'\r',r0	|* Imprime também o '\r'
	call	putc

	movb	#'\n',r0
1$:
	clrb	h3		|* Escreve o caractere (em r0)
	movb	#14,h0
	int	#0x10

	popw	r3		|* Restaura r3
	rts
.endif	DEBUG
