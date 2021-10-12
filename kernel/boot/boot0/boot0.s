|*
|****************************************************************
|*								*
|*			boot0.s					*
|*								*
|*	Estágio ZERO da carga do sistema			*
|*								*
|*	Versão	3.0.0, de 18.09.94				*
|*		4.9.0, de 02.01.08				*
|*								*
|*	Módulo: Boot0						*
|*		NÚCLEO do TROPIX para PC			*
|*								*
|*	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
|*		Copyright © 2008 NCE/UFRJ - tecle "man licença"	*
|*								*
|****************************************************************
|*

|*
|******	Definições globais **************************************
|*
SIZE_DEBUG	= 0		|* Depuração do tamanho em blocos
INDEX_DEBUG	= 0		|* Depuração do índice lido

KBSZ		= 1024
BLSZ		= 512

DOS_EXT_G	= 0x05		|* Código da partição estendida DOS (geo)
DOS_EXT_L	= 0x0F		|* Código da partição estendida DOS (linear)
TROPIX_EXT	= 0xAE		|* Código da partição estendida TROPIX
TROPIX_V7	= 0xA8		|* Código da partição TROPIX V7
TROPIX_T1	= 0xA9		|* Código da partição TROPIX T1

BOOT0LOAD	= 31 * KBSZ	|* Endereço de carga de "boot0"
BOOT1LOAD	= 31 * KBSZ	|* Endereço de carga de "boot1"

FRAMEADDR	= 63 * KBSZ	|* Endereço da pilha

|*
|******	Definições dos dispositivos *****************************
|*
.ifndef	BUNIT

	.error	"Faltou definir o dispositivo para ler o resto de boot0"

.elif	BUNIT == 0x80		|* Disco IDE

.elif	BUNIT == 0		|* Qualquer disquette

.else
	.error	"Valor inválido de dispositivo: %X", BUNIT
.endif

|*
|******	Estrutura da tabela de partições no disco ***************
|*
DT_ACTIVE	= 0	|* Ativo (0x80) ou não
DT_TYPE		= 4	|* Tipo da partição
DT_OFFSET	= 8	|* Deslocamento da partição
DT_SIZE		= 12	|* Tamanho da partição

DT_SIZEOF	= 16	|* Tamanho da estrutura

|*
|******	Estrutura da tabela de partições na memória *************
|*
CT_ACTIVE	= 0	|* Ativo (0x80) ou não
CT_TYPE		= 4	|* Tipo da partição
CT_OFFSET	= 8	|* Deslocamento da partição
CT_SIZE		= 12	|* Tamanho da partição

CT_PREFIX	= 16	|* Prefixo do nome: "[hs]"
CT_NAME		= 17	|* Nome da partição: "[ab]"
CT_REG_INDEX	= 18	|* No. da partição regular: "1" a "4"
CT_LOG_INDEX	= 19	|* No. da sub-partição lógica: "a" a "z"

CT_INT13	= 20	|* Extensões da INT13

CT_SIZEOF	= 24	|* Tamanho da estrutura

|*
|****** Variáveis na pilha **************************************
|*
UNIT		= -4	|* Dispositivo (0x00, 0x80, 0x81, 0x82, ...)
INT13		= -8	|* Aceita as extensões da INT 13
NSECT		= -12	|* No. de setores por trilha
NHEAD		= -16	|* No. de cabeças
PBNO		= -20	|* No. do Bloco Fisico (32 bits)
AREA		= -24	|* Endereco do Buffer
TRY_CNT		= -28	|* Contador de tentativas de leitura
INDEX		= -32	|* Um contador genérico
EXT_BEGIN	= -36	|* Início da partição estendida

PREFIX		= -40	|* Prefixo do nome: "[hs]"
NAME		= -39	|* Nome da partição: "[ab]"
REG_INDEX	= -38	|* No. da partição regular: 0 a 3
LOG_INDEX	= -37	|* No. da sub-partição lógica: 0 a 25

SECONDS		= -44	|* No. de segundos restantes
CMOS_SEG	= -43	|* Contador de segundos da CMOS

DIGIT		= -84	|* Área para conversão para decimal

LINE		= -160	|* Área para leitura do índice

|*
|****** Começo do programa **************************************
|*
	.seg16
	.global	start
start:
	jmp	1$

	.isoz	"TROPIX BOOT0"	|* Assinatura
1$:
	clrw	r0
	movw	r0,ds
	movw	r0,es
	movw	r0,ss

	movw	#FRAMEADDR,fp
	movw	#FRAMEADDR-KBSZ,sp

|*
|****** Autocopia-se para o endereço definitivo (48 KB) *********
|*
	movw	#BOOT0LOAD,r4
	movw	#start,r5
	movw	#BLSZ/2,r1
	up
	rep
	movsw

	farjmpw	#0,3$		|* Desvia para o novo endereço
3$:

|*
|****** Escreve a versão do programa ****************************
|*
|*	Cuidado para não utilizar trechos
|*	do programa ainda não carregados!
|*
	movw	#pgversion,r0
	call	putstr

|*
|****** Inicialização provisória para ler o resto de "boot0" ****
|*
	movb	#BUNIT,UNIT(fp)
	movb	#'G',INT13(fp)	|* Por enquanto não usa as extensões da INT 13

.if	BUNIT == 0
	movl	#18,NSECT(fp)	|* No. de setores/trilha (Qualquer disquette)
	movl	#2,NHEAD(fp)	|* No. de cabeças (Disquette)
.else
	movl	#32,NSECT(fp)	|* No. de setores/trilha (Qualquer disco)
	movl	#6,NHEAD(fp)	|* No. de cabeças (Qualquer disco)
.endif	BUNIT == 0

|*
|****** Carrega os blocos seguintes de "boot0" ******************
|*
   |***	movb	#BUNIT,UNIT(fp)
	movl	#1,PBNO(fp)
	movw	#start+BLSZ,AREA(fp)
5$:
	cmpw	#_edata,AREA(fp)
	jhs	9$

	call	phys_read

	incl	PBNO(fp)
	addw	#BLSZ,AREA(fp)
	jmp	5$
9$:

|*
|****** Le as tabelas de partições dos vários discos ************
|*
get_all_parttb:
	movb	#0x80,UNIT(fp)		|* Inicialização
	movb	#'h',PREFIX(fp)
	movb	#'a'-1,NAME(fp)
1$:
	call	get_param		|* Se retorna com CF, acabou
	jcs	get_first_TROPIX

   |***	movb	#...,UNIT(fp)		|* Carrega a tabela do MBR
	clrl	PBNO(fp)
	movw	#area,AREA(fp)
	call	phys_read

	movw	#area+BLSZ-64-2,r4	|* Copia a tabela
	movw	#part_tb,r5
	movw	#64/2+1,r1		|* Inclui o número mágico
   |***	up
	rep
	movsw

	cmpw	#0xAA55,-2(r4)
	jeq	2$

	movw	#inval_reg,r0
	call	putstr

	movl	PBNO(fp),r0		|* no. do bloco
	movw	#10,r1
	call	print10

	movb	#')',r0			|* ")"
	call	putc

	movb	#'\n',r0		|* "\n"
	call	putc
2$:
   |***	movw	#...,core_table_next
   |***	...	part_tb

	call	get_partition

	incb	UNIT(fp)		|* Unidade seguinte
	jmp	1$

|*
|****** Mensagens ***********************************************
|*
pgversion:
	.isoz	"\nTROPIX boot0, Versao: 4.9.0, de 02.01.08\n"

|*
|******	Lê um bloco físico do disco *****************************
|*
|*	UNIT  => Dispositivo (0x00, 0x01, 0x80, 0x81, 0x82, ...)
|*	INT13 => 'G' se tradicional, 'L' com extensões
|*	PBNO  => No. do Bloco Fisico (32 bits)
|*	AREA  => Endereco do Buffer
|*
phys_read:
	cmpb	#'L',INT13(fp)	|* Desvia se linear
	jeq	linear_phys_read

	pushw	r3

	movb	#3,TRY_CNT(fp)	|* No. de tentativas
1$:
	movl	PBNO(fp),r0	|* Dividendo == No. do bloco dado
	clrl	r2

	divul	NSECT(fp),r0	|* Quociente em r0 (l) (usa 32 bits)
				|* Resto == (setor - 1) em r2 (l)

	movb	r2,r1		|* r1 == setor
	incb	r1

	clrl	r2		|* Zera a parte alta do dividendo

	divul	NHEAD(fp),r0	|* Quociente == cilindro em r0 (l)
				|* Resto == cabeça em r2 (l)

	cmpl	#1024,r0	|* Verifica se deu maior do que 1023
	jhs	8$		|* Retorna com o carry ligado

3$:
	movb	r0,h1		|* Insere o número do cilindro
	lsrw	#2,r0
	andb	#0xC0,r0
	orb	r0,r1

   |***	movb	...,r1		|* Setor
	movb	r2,h2		|* Cabeça
	movw	#0x0201,r0	|* 2 = ler, 1 bloco
	movb	UNIT(fp),r2	|* Dispositivo
	movw	AREA(fp),r3	|* Área
	int	#0x13

	jcc	9$		|* Aproveita o CARRY clear para retornar

5$:
	cmpl	#0x504F5254,signature	|* "TROP" em little-endian
	jne	6$

	movw	#read_error_msg0,r0
	call	putstr

	movb	UNIT(fp),r0	|* Dispositivo (em decimal)
	movzbl	r0,r0
	clrw	r1
	call	print10

	movw	#read_error_msg1,r0
	call	putstr

	movl	PBNO(fp),r0	|* Dividendo == No. do bloco dado
	clrw	r1
	call	print10

	movb	#'\n',r0
	jmp	7$
6$:
	movb	#'E',r0		|* Em caso de erro, ...
7$:
	call	putc

	decb	TRY_CNT(fp)	|* Verifica se deve desistir
	jle	8$

	clrw	r0		|* RESET
	movb	UNIT(fp),r2
	int	#0x13

	jmp	1$		|* Tenta novamente
8$:
	stc			|* Liga o CARRY
9$:
	popw	r3
	rts

|*
|******	Escreve uma cadeia (endereço em r0) *********************
|*
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
|******	Imprime um caractere (em r0) na console *****************
|*
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

|*
|******	Le e ecoa um caractere da console ***********************
|*
getc:
	clrb	h0		|* Le um caractere (em r0)
	int	#0x16

	cmpb	#'\r',r0	|* Transforma '\r' em '\n'
	jne	2$

	movb	#'\n',r0
2$:
	pushw	r0		|* Salva o caractere lido

	call	putc		|* Ecoa o caractere

	popw	r0		|* Restaura o caractere lido

	rts

|*
|******	Tabela de partições *************************************
|*
	.org	start+BLSZ-64-2
part_tb:
	.dsb	4 * 16		|* A tabela de partições

|*
|******	Número mágico *******************************************
|*
	.byte	0x55, 0xAA


|****************************************************************
|******	Aqui inicia o segundo bloco de "boot0" ******************
|****************************************************************

signature:
	.isoz	"TROPIX BOOT0"
std_boot:
	.byte	0		|* Indica timeout no getc
std_index:
	.byte	0		|* Partição -std-

	.dsb	5		|* Reserva para novas idéias brilhantes

|*
|****** Função para obter características de um disco ***********
|*
|*	Argumento:	UNIT(fp)
|*
|*	Atualiza:	NSECT(fp)	|* No. de setores/trilha
|*			NHEAD(fp)	|* No. de cabeças
|*			PREFIX(fp)	|* "h" ou "s"
|*			NAME(fp)	|* "a", "b", ...
|*
|*	Devolve o CF ligado se a unidade não existe
|*
get_param:
	movb	#0x15,h0		|* 15 = verificar existência
	movb	UNIT(fp),r2		|* Dispositivo
	int	#0x13

	cmpb	#0,h0			|* Se != 0 => Existe
	jne	2$
1$:
	stc				|* Retorna com o CARRY ligado
	rts

|*	Tenta obter a geometria

2$:
	pushw	r4			|* Precauções com a INT 13/8
	pushw	r5
	movw	#disk_param_table,r5

	movb	#0x08,h0		|* 8 = obter características
	movb	UNIT(fp),r2		|* Dispositivo
	int	#0x13

	popw	r5
	popw	r4

	jcs	1$			|* Testa o CARRY

	tstb	h0			|* Testa também o "h0"
	jne	1$

	addb	#0x80,r2		|* Verifica se não passou do total
	cmpb	UNIT(fp),r2
	jls	1$

|*	O disco existe

	andb	#0x3F,r1		|* Guarda o No. de setores/trilha
	movzbl	r1,r0
	movl	r0,NSECT(fp)

	movzbl	h2,r0			|* Guarda o No. de cabeças
	incl	r0
	movl	r0,NHEAD(fp)

|*	Verifica se tem as extensões da INT 13

	movb	#'G',INT13(fp)		|* Por enquanto não tem as extensões da INT 13

   	movb	#0x41,h0		|* Installation check
	movb	UNIT(fp),r2		|* Dispositivo
	movw	#0x55AA,r3		|* Número Mágico
	int	#0x13

	jcs	4$			|* Desvia se retornou com o carry

	cmpw	#0xAA55,r3		|* Confere o 0xAA55
	jne	4$

	testb	#1,r1			|* O bit 0 deve estar ligado
	jeq	4$

	movb	#'L',INT13(fp)		|* Tem as extensões da INT 13
4$:

|*	Tenta descobrir se é IDE ou SCSI

ide_or_scsi:
	cmpb	#64,r0			|* Testa o esquema SCSI (64, 32)
	jne	4$

	cmpb	#32,r1
	jeq	7$
4$:
	cmpb	#255,r0			|* Testa o esquema SCSI (255, 63)
	jne	6$

	cmpb	#63,r1
	jeq	7$
6$:
   |***	movb	#'h',PREFIX(fp)		|* Discos IDE
	incb	NAME(fp)
	jmp	9$
7$:
	cmpb	#'h',PREFIX(fp)		|* Discos SCSI
	jne	8$

	movb	#'s',PREFIX(fp)		|* Primeiro disco SCSI
	movb	#'a',NAME(fp)
	jmp	9$
8$:
   |***	movb	#'s',PREFIX(fp)		|* Discos SCSI seguintes
	incb	NAME(fp)
9$:
	clc
	rts

|*	Tabela (quem sabe quando?) preenchida pela INT 13/8

	.data
disk_param_table:
	.dsl	3

	.text

|*
|******	Imprime um número decimal *******************************
|*
|*	Entrada: r0: Número a imprimir	(long)
|*		 r1: Tamanho do campo	(short)
|*
print10:
	pushw	r5
	pushw	r4
	pushl	r3

	leaw	DIGIT(fp),r5	|* Área para os dígitos gerados
	movl	#10,r3		|* Base (dividendo)
1$:
	clrl	r2
	divul	r3,r0

	addb	#'0',r2
	decw	r5
	movb	r2,(r5)

	tstl	r0
	jne	1$
2$:
	tstw	r1		|* Se não foi dada o tamanho do campo
	jeq	5$

	leaw	DIGIT(fp),r4
	subw	r1,r4
4$:
	cmpw	r5,r4		|* Coloca os brancos antes
	jhs	5$

	movb	#' ',r0
	call	putc

	incw	r4
	jmp	4$
5$:
	leaw	DIGIT(fp),r4	|* Coloca os dígitos decimais
6$:
	cmpw	r4,r5
	jhs	8$

	movb	(r5),r0
	call	putc

	incw	r5
	jmp	6$
8$:
	popl	r3
	popw	r4
	popw	r5
	rts

|*
|****** Função para obter as partições de um disco **************
|*
|*	Já supõe a tabela de partições em "part_tb"
|*
|*	Argumentos: UNIT(fp), NAME(fp)
|*
|*	Espera valores nas variáveis globais "part_tb" e "core_table_next"
|*
get_partition:
	movw	#part_tb,r4		|* "r4" tabela fonte
	clrb	REG_INDEX(fp)
1$:
	cmpw	#part_tb+4*16,r4	|* Verifica se terminou
	jlo	2$
	rts
2$:
	cmpl	#0,DT_SIZE(r4)		|* Verifica se está definida
	jeq	3$

	cmpb	#DOS_EXT_G,DT_TYPE(r4)	|* Verifica se é estendida DOS Geo
	jeq	5$
	cmpb	#DOS_EXT_L,DT_TYPE(r4)	|* Verifica se é estendida DOS Linear
	jeq	5$
	cmpb	#TROPIX_EXT,DT_TYPE(r4)	|* Verifica se é estendida TROPIX
	jeq	5$

	cmpb	#0x80,DT_ACTIVE(r4)	|* Verifica se é ativa
	jne	3$

	movw	core_table_next,r5	|* Copia a entrada do disco toda
	movw	#16/2,r1
   |***	up
	rep
	movsw

	subw	#16,r4			|* Restaura os ponteiros
	subw	#16,r5

	movb	PREFIX(fp),r0		|* Compõe o nome
	movb	r0,CT_PREFIX(r5)
	movb	NAME(fp),r0
	movb	r0,CT_NAME(r5)
	movb	REG_INDEX(fp),r0
	addb	#'1',r0
	movb	r0,CT_REG_INDEX(r5)
	movb	#' ',CT_LOG_INDEX(r5)

	movb	UNIT(fp),r0		|* Coloca o código do disco
	movb	r0,CT_ACTIVE(r5)

	movb	INT13(fp),r0		|* Coloca se tem as extensões
	movb	r0,CT_INT13(r5)

	addw	#CT_SIZEOF,core_table_next
3$:
	incb	REG_INDEX(fp)
	addw	#16,r4
	jmp	1$

|*
|****** Obtém as partições lógicas de um disco ******************
|*
5$:
	pushw	r4			|* Não esquece de guardar

	movb	#0,LOG_INDEX(fp)

	movl	DT_OFFSET(r4),r0	|* PBNO == ext_offset
	movl	r0,EXT_BEGIN(fp)
	movl	r0,PBNO(fp)
6$:
   |***	movb	...,UNIT(fp)		|* Lê o bloco lógico
   |***	movl	...,PBNO(fp)
	movw	#area,AREA(fp)
	call	phys_read

	jcs	9$			|* Se cilindro > 1023, encerra!

	movw	#area+BLSZ-2-64,r4

	cmpw	#0xAA55,64(r4)		|* É importante conferir!
	jeq	7$

	movw	#inval_log,r0
	call	putstr

	movl	PBNO(fp),r0		|* no. do bloco
	movw	#10,r1
	call	print10

	movb	#')',r0			|* ")"
	call	putc

	movb	#'\n',r0		|* "\n"
	call	putc

	jmp	9$
7$:
	cmpl	#0,DT_SIZE(r4)		|* Verifica se é a última
	jeq	9$

	cmpb	#0x80,DT_ACTIVE(r4)	|* Verifica se está ativa
	jne	8$

	movw	core_table_next,r5	|* Copia a entrada do disco toda
	movw	#16/2,r1
   |***	up
	rep
	movsw

	subw	#16,r4			|* Restaura os ponteiros
	subw	#16,r5

	movl	PBNO(fp),r0
	addl	r0,CT_OFFSET(r5)

	movb	PREFIX(fp),r0		|* Compõe o nome
	movb	r0,CT_PREFIX(r5)
	movb	NAME(fp),r0
	movb	r0,CT_NAME(r5)
	movb	REG_INDEX(fp),r0
	addb	#'1',r0
	movb	r0,CT_REG_INDEX(r5)
	movb	LOG_INDEX(fp),r0
	addb	#'a',r0
	movb	r0,CT_LOG_INDEX(r5)

	movb	UNIT(fp),r0		|* Coloca o código do disco
	movb	r0,CT_ACTIVE(r5)

	movb	INT13(fp),r0		|* Coloca se tem as extensões
	movb	r0,CT_INT13(r5)

	addw	#CT_SIZEOF,core_table_next
8$:
	incb	LOG_INDEX(fp)

	addw	#16,r4

	cmpl	#0,DT_SIZE(r4)
	jeq	9$

	movl	EXT_BEGIN(fp),r0
	addl	DT_OFFSET(r4),r0
	movl	r0,PBNO(fp)
	jmp	6$
9$:
	popw	r4			|* Continua com a próxima regular
	jmp	3$
|*
|****** Obtém a primeira partição TROPIX ************************
|*
get_first_TROPIX:
	cmpb	#0,std_index		|* Desvia se já há um "std_index"
	jne	print_table

	movw	#core_table-CT_SIZEOF,r4
3$:
	addw	#CT_SIZEOF,r4
	incb	std_index

	cmpb	#TROPIX_T1,CT_TYPE(r4)	|* Verifica se é TROPIX T1
	jeq	print_table	

	cmpb	#TROPIX_V7,CT_TYPE(r4)	|* Verifica se é TROPIX V7
	jeq	print_table	

	cmpw	core_table_next,r4
	jlo	3$

	movb	#1,std_index

|*
|******	Imprime todas as partições ******************************
|*
print_table:
	movw	#table_prefix,r0
	call	putstr

	movw	#core_table,r4
	movb	#1,INDEX(fp)
1$:
	cmpw	core_table_next,r4
	jhs	get_partition_index

	movzbl	INDEX(fp),r0		|* nn
	movw	#2,r1
	call	print10

	movw	#name_prefix,r0		|* "  :"
	call	putstr

	movb	CT_PREFIX(r4),r0	|* [hs]
	call	putc

	movb	#'d',r0			|* "d"
	call	putc

	movb	CT_NAME(r4),r0		|* [ab]
	call	putc

	movb	CT_REG_INDEX(r4),r0	|* "1" a "4"
	call	putc

	movb	CT_LOG_INDEX(r4),r0	|* " " ou "a" a "z")
	call	putc

	movb	#' ',r0
	call	putc

	movb	CT_INT13(r4),r0	
	call	putc

.if	SIZE_DEBUG
	movl	CT_OFFSET(r4),r0
	movw	#8,r1
	call	print10

	movl	CT_SIZE(r4),r0
	movw	#8,r1
	call	print10
.else	! SIZE_DEBUG
	movl	CT_SIZE(r4),r0		|* Tamanho em MB (inteiro)
	lsrl	#20-9,r0
	movw	#6,r1
	call	print10

	movb	#'.',r0
	call	putc

	movw	CT_SIZE(r4),r0		|* Tamanho em MB (décimos)
	andw	#0x07FF,r0
	movw	#10,r1
	muluw	r1,r0
	lsrw	#20-9,r0
	addb	#'0',r0
	call	putc
.endif	SIZE_DEBUG

	movw	#dois_brancos,r0
	call	putstr

	movb	CT_TYPE(r4),r0
	call	putpnm

	movb	#'\n',r0
	call	putc

	addw	#CT_SIZEOF,r4
	incb	INDEX(fp)
	jmp	1$

|*
|****** Pede o número de uma partição ***************************
|*
get_partition_index:
	movb	#'\n',r0		|* Pula uma linha
	call	putc

	movb	#15,SECONDS(fp)		|* Espera 15 segundos

	movb	#02,h0			|* Le os segundos da CMOS
	int	#0x1A
	movb	h2,CMOS_SEG(fp)
1$:
	movw	#partition_index_prefix_0,r0
	call	putstr

	movzbl	std_index,r0		|* Imprime a partição -std-
	clrw	r1
	call	print10

	movw	#partition_index_prefix_1,r0
	call	putstr

	movzbl	SECONDS(fp),r0		|* Imprime o tempo restante
	clrw	r1
	call	print10

	movw	#partition_index_prefix_2,r0
	call	putstr
2$:
	movb	#01,h0			|* Desvia se já foi teclado algo
	int	#0x16
	jne	3$

	movb	#02,h0			|* Le os segundos da CMOS
	int	#0x1A
	cmpb	h2,CMOS_SEG(fp)
	jeq	2$

	movb	h2,CMOS_SEG(fp)		|* Guarda o novo valor e
	decb	SECONDS(fp)		|* Decrementa o contador
	jgt	1$

	incb	std_boot		|* Timeout: usa -std-

	movb	#'\n',r0		|* Pula uma linha
	call	putc

	jmp	13$

|*	Foi teclado algo: Le a linha

3$:
	leaw	LINE(fp),r5		|* Área para os dígitos lidos
4$:
	call	getc
	cmpb	#'\n',r0
	jeq	11$

	cmpb	#'\b',r0		|* <bs>
	jne	8$

	movb	#' ',r0			|* Escreve um branco
	call	putc

	leaw	LINE(fp),r0		|* Verifica se pode retroceder
	cmpw	r0,r5
	jhi	5$

	movb	#'\a',r0		|* Avisa que já está no começo
	call	putc
	jmp	4$
5$:
	movb	#'\b',r0		|* Retrocede
	call	putc

	decw	r5
	jmp	4$
8$:
	movb	r0,(r5)			|* Armazena e prepara para o próximo
	incw	r5
	jmp	4$

|*	Acabou a linha: Pula os <sp> e <ht> iniciais

11$:
	clrb	(r5)			|* Marca o final da linha

	leaw	LINE(fp),r5		|* Novamente, a Área para os dígitos lidos
12$:
	movb	(r5),r0			|* Verifica se terminou
	tstb	r0
	jne	14$
13$:
	movzbw	std_index,r0		|* Só <sp>, <ht> ou <nl>
	movw	r0,INDEX(fp)
	jmp	examine_partition_index
14$:
	cmpb	#' ',r0			|* Pula <sp> e <ht>
	jeq	17$
	cmpb	#'\t',r0
	jne	21$
17$:
	incw	r5			|* Avança o ponteiro
	jmp	12$

|*	Encontrou caracteres úteis

21$:
	clrw	INDEX(fp)		|* Inicializa o valor
22$:
	movb	(r5),r0			|* Verifica se terminou
	tstb	r0
	jeq	examine_partition_index

	cmpb	#'0',r0
	jlt	invalid_index
	cmpb	#'9',r0
	jgt	invalid_index

	subb	#'0',r0
	movzbw	r0,r1

	movw	#10,r0
	muluw	INDEX(fp),r0
	addw	r1,r0
	movw	r0,INDEX(fp)

	incw	r5			|* Avança o ponteiro
	jmp	22$

|*
|****** Examina o número dado ***********************************
|*
examine_partition_index:

.if	INDEX_DEBUG
	movw	INDEX(fp),r0
	movzwl	r0,r0
	clrw	r1
	call	print10

	jmp	print_table
.endif	INDEX_DEBUG

	movw	INDEX(fp),r0
	tstw	r0
	jle	invalid_index
	decw	r0

	movw	#CT_SIZEOF,r1
	muluw	r1,r0
	movw	r0,r4
	addw	#core_table,r4
	cmpw	core_table_next,r4
	jlo	load
invalid_index:
	movw	#invalid_partition_index,r0
	call	putstr
	jmp	print_table

|*
|******	Já achou a partição adequada ****************************
|*
load:
	movb	CT_ACTIVE(r4),r0
	movb	r0,UNIT(fp)
	movl	CT_OFFSET(r4),r0
	movl	r0,PBNO(fp)
	movw	#BOOT1LOAD,AREA(fp)

	call	get_param		|* Não esquece de atualizar
	call	phys_read

	movw	#BOOT1LOAD+BLSZ-2,r5
	cmpw	#0xAA55,(r5)
	jeq	exec

	movw	#inval_boot1,r0
	call	putstr

	jmp	print_table
exec:
	movw	r4,fp		|* Entrada em "fp" e "r4"

	movb	std_boot,r0	|* Fornece o -std- no começo da entrada seguinte
	movb	r0,16(fp)

	farjmpw	#0,BOOT1LOAD

|*
|******	Escreve o tipo da partição ******************************
|*
putpnm:
	pushw	r3

	movw	#nm_table,r3
1$:
	cmpw	#0,(r3)		|* Testa o final da tabela
	jlt	10$

	cmpb	r0,(r3)
	jeq	4$

	addw	#4,r3
	jmp	1$
4$:
	movw	2(r3),r0	|* Achou o nome
	call	putstr
	jmp	20$
10$:
	movw	r0,r3		|* Escreve em hexadecimal

	movw	#unknown_type,r0
	call	putstr

	movb	#2,r2
11$:
	movw	r3,r0
	lslw	#4,r3

	lsrw	#4,r0
	andb	#0x0F,r0

	cmpb	#10,r0
	jge	12$

	addb	#'0',r0
	jmp	14$
12$:
	addb	#'A'-10,r0
14$:
	call	putc

	decb	r2
	jgt	11$

	movb	#')',r0
	call	putc
20$:
	popw	r3
	rts

|*
|****** Leitura linar de um bloco físico do disco ***************
|*
|*	UNIT  => Dispositivo (0x00, 0x01, 0x80, 0x81, 0x82, ...)
|*	PBNO  => No. do Bloco Fisico (32 bits)
|*	AREA  => Endereco do Buffer
|*
linear_phys_read:
	pushw	r4

	movb	#3,TRY_CNT(fp)		|* No. de tentativas
1$:
	movw	#linear_cb,r4		|* Endereço do bloco

	movb	#16,0(r4)		|* 16 bytes
	movw	#1,2(r4)		|* Um bloco

	movw	AREA(fp),r0		|* Endereço
	movw	r0,4(r4)

	movw	ds,6(r4)		|* Segmento

	movl	PBNO(fp),r0		|* No. do bloco
	movl	r0,8(r4)

   	movb	#0x42,h0		|* Leitura
	movb	UNIT(fp),r2		|* Unidade
|*	movw	#linear_cb,r4		|* Endereço do bloco
	int	#0x13
	jcc	9$			|* Desvia se retornou sem o carry

	movb	#'E',r0			|* Mensagem de erro
	call	putc

	decb	TRY_CNT(fp)		|* Verifica se deve desistir
	jle	8$

	clrw	r0			|* RESET
	movb	UNIT(fp),r2
	int	#0x13

	jmp	1$			|* Tenta novamente
8$:
	stc				|* Liga o CARRY
9$:
	popw	r4
	rts

	.data
linear_cb:
	.long	0
	.long	0
	.long	0
	.long	0

|*
|****** Tabela de nomes de sistemas de arquivos *****************
|*
	.data
nm_table:
	.word	0x01, name_01
	.word	0x02, name_02
	.word	0x03, name_03
	.word	0x04, name_04
	.word	0x05, name_05
	.word	0x06, name_06
	.word	0x07, name_07
	.word	0x08, name_08
	.word	0x09, name_09
	.word	0x0A, name_0A
	.word	0x0B, name_0B
	.word	0x0C, name_0C
	.word	0x0E, name_0E
	.word	0x0F, name_0F
	.word	0x40, name_40
	.word	0x4F, name_4F
	.word	0x51, name_51
	.word	0x52, name_52
	.word	0x63, name_63
	.word	0x64, name_64
	.word	0x75, name_75
	.word	0x80, name_80
	.word	0x81, name_81
	.word	0x82, name_82
	.word	0x83, name_83
	.word	0x93, name_93
	.word	0x94, name_94
	.word	0xA5, name_A5
	.word	0xA8, name_A8
	.word	0xA9, name_A9
	.word	0xAA, name_AA
	.word	0xAE, name_AE
	.word	0xAF, name_AF
	.word	0xB7, name_B7
	.word	0xB8, name_B8
	.word	0xC7, name_C7
	.word	0xDB, name_DB
	.word	0xE1, name_E1
	.word	0xE3, name_E3
	.word	0xF2, name_F2
	.word	0xFF, name_FF
	.word	-1,   0

name_01: .isoz	"DOS FAT12"
name_02: .isoz	"XENIX root"
name_03: .isoz	"XENIX usr"
name_04: .isoz	"DOS FAT16 < 32 MB"
name_05: .isoz	"DOS Extended"
name_06: .isoz	"DOS FAT16 >= 32 MB"
name_07: .isoz	"NTFS"
name_08: .isoz	"AIX"
name_09: .isoz	"AIX bootable"
name_0A: .isoz	"OPUS"
name_0B: .isoz	"DOS FAT32"
name_0C: .isoz	"DOS FAT32 (L)"
name_0E: .isoz	"DOS FAT16 (L)"
name_0F: .isoz	"DOS Extended (L)"
name_40: .isoz	"Venix 80286"
name_4F: .isoz	"QNX"
name_51: .isoz	"Novell (?)"
name_52: .isoz	"Microport"
name_63: .isoz	"GNU HURD"
name_64: .isoz	"Novell"
name_75: .isoz	"PC/IX"
name_80: .isoz	"old MINIX"
name_81: .isoz	"LINUX/MINIX"
name_82: .isoz	"LINUX swap"
name_83: .isoz	"LINUX ext"
name_93: .isoz	"Amoeba"
name_94: .isoz	"Amoeba BBT"
name_A5: .isoz	"BSD 4.2"
name_A8: .isoz	"TROPIX V7"
name_A9: .isoz	"TROPIX T1"
name_AA: .isoz	"TROPIX fs3"
name_AE: .isoz	"TROPIX Extended"
name_AF: .isoz	"TROPIX swap"
name_B7: .isoz	"BSDI fs"
name_B8: .isoz	"BSDI swap"
name_C7: .isoz	"Syrinx"
name_DB: .isoz	"CP/M"
name_E1: .isoz	"DOS access"
name_E3: .isoz	"DOS R/O"
name_F2: .isoz	"DOS Secondary"
name_FF: .isoz	"BBT"

|*
|****** Mensagens ***********************************************
|*
	.data
read_error_msg0:
	.isoz	"\nErro de Leitura: dev = "

read_error_msg1:
	.isoz	", blkno = "

table_prefix:
	.isoz	"\nPARTICOES ATIVAS:\n\n"

name_prefix:
	.isoz	":  "

dois_brancos:
	.isoz	"  "

unknown_type:
	.isoz	"Tipo desconhecido (0x"

partition_index_prefix_0:
	.isoz	"\rIndice da particao desejada ("

partition_index_prefix_1:
	.isoz	", "

partition_index_prefix_2:
	.isoz	"): "

inval_log:
	.isoz	"\n*** Tabela de particao logica com numero magico invalido ("

inval_reg:
	.isoz	"\n*** Tabela de particao regular com numero magico invalido ("

invalid_partition_index:
	.isoz	"\n*** Indice invalido de particao\n"

inval_boot1:
	.isoz	"\n*** Esta particao contem um \"boot1\" invalido\n"

|*
|****** Áreas ***************************************************
|*
	.data
core_table_next:
	.word	core_table	|* Próxima entrada vaga

	.bss
extended_entry:
	.word	0		|* Ponteiro para a partição estendida

area:
	.dsb	BLSZ		|* Área de leitura dos blocos

core_table:
	.dsl	100*CT_SIZEOF/4	|* Tabela de partições da memória
