|*
|****************************************************************
|*								*
|*			v7.boot1.s				*
|*								*
|*	BOOT para o PC, estágio 1				*
|*								*
|*	Esta etapa do BOOT reside no bloco 0 do disco,		*
|*	e é carregado pela ROM em 31 Kb				*
|*								*
|*	Versão	1.0.0, de 27.07.87				*
|*		4.0.0, de 13.07.00				*
|*								*
|*	Módulo: Boot1						*
|*		NÚCLEO do TROPIX para PC			*
|*								*
|*	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
|*		Copyright © 2000 NCE/UFRJ - tecle "man licença"	*
|*								*
|****************************************************************
|*

	.seg16

|*
|******	Definições globais **************************************
|*
DEBUG	= 0			|* Liga ou não a depuração
ADR2II	= 1			|* Blocos duplamente indiretos

FD	= 1			|* Disquetes
GEO	= 2			|* Disco IDE ou SCSI usando INT13 normal
LINEAR	= 3			|* Disco IDE ou SCSI usando INT13 Extensions

|*
|******	Definições dos dispositivos *****************************
|*
.ifndef	DEV

	.error	"Faltou definir o dispositivo"

.elif	DEV == 1722		|* Disquete de 1.72 MB

	TYPE	= FD
	UNIT	= 0
	SECT	= 21
	HEAD	= 2

.elif	DEV == 1440		|* Disquete de 1.44 MB

	TYPE	= FD
	UNIT	= 0
	SECT	= 18
	HEAD	= 2

.elif	DEV == 1200		|* Disquete de 1.2 MB

	TYPE	= FD
	UNIT	= 0
	SECT	= 15
	HEAD	= 2

.elif	DEV == 360		|* Disquete de 360 Kb

	TYPE	= FD
	UNIT	= 0
	SECT	= 9
	HEAD	= 2

.elif	DEV == 80		|* Disco IDE ou SCSI usando INT13 normal

	TYPE	= GEO

.elif	DEV == 81		|* Disco IDE ou SCSI usando INT13 Extensions

	TYPE	= LINEAR

.else

	.error	"Valor inválido de dispositivo: %X", DEV

.endif

|*
|******	Definições Diversas *************************************
|*
ROOTINO		= 2
IDSZ		= 14
ENTSZ		= 16
HDRSZ		= 80
BLSZ		= 512
KBSZ		= 1024

|*
|******	Definições das regiões da memória ***********************
|*
BOOT0ADDR	= 48 * KBSZ		|* Endereço de carga de "boot0"
BOOT1ADDR	= 31 * KBSZ		|* Endereço de carga de "boot1"
BOOT2ADDR	= 64 * KBSZ		|* Endereço de carga de "boot2"

DIFF		= BOOT0ADDR-BOOT1ADDR	|* Para as funções compartilhadas

STACKADDR	= 63 * KBSZ		|* Endereço virtual do início da pilha

HSEG		= 80 / 16		|* No. de segmentos do cabeçalho

BOOT2LOADSEG	= BOOT2ADDR / 16	|* Segmento de carga do "boot2"
BOOT2EXECSEG	= BOOT2LOADSEG+HSEG	|* Segmento de execução do "boot2"

|*
|******	Deslocamento das variáveis na pilha *********************
|*
STDBOOT	= 16				|* Byte informando para não esperar
OFFSET	= 8				|* Início da partição
DEVCODE	= 0				|* Código do Dispositivo
LBNO	= -4				|* Bloco lógico do disco
PBNO	= -8				|* Bloco físico (longo) do disco
BA	= -12				|* Endereço na memoria da leitura
INO	= -16				|* No. do INODE a ler
DIRPTR	= -20				|* Ponteiro para as entradas do diretório
NSECT	= -24				|* No. de setores por trilha
NHEAD	= -28				|* No. de cabeças

LINK_SZ	= 28				|* Tamanho total alocado na pilha

|*
|****************************************************************
|*	Comeco do Boot1						*
|****************************************************************
|*
	.text
	.global	start
start:
	clrw	r0
	movw	r0,ds
	movw	r0,es
	movw	r0,ss

|*
|*	Prepara a pilha
|*
again:
.if	TYPE == FD
	movw	#STACKADDR,fp
.else	TYPE == GEO || TYPE == LINEAR
   |***	movw	#...,fp 		|* HD: recebe em fp a partição
.endif	TYPE

	movw	#STACKADDR-LINK_SZ,sp

|*
|*	Zera o "bss"
|*
	clrb	r0
	movw	#EDATA,r5
	movw	#END-EDATA,r1
	up
	rep
	stosb

|*
|*	Põe o cabeçalho + prompt
|*
	movw	#pgversion,r0
	call	putstr

|*
|*	Inicializa o número de cabeças e setores
|*
.if	TYPE == FD

	movl	#SECT,NSECT(fp)
	movl	#HEAD,NHEAD(fp)

.elif 	TYPE == GEO

	movb	#0x08,h0		|* 8 = obter características
	movb	DEVCODE(fp),r2		|* Dispositivo
	int	#0x13

	andb	#0x3F,r1		|* Guarda o No. de setores/trilha
	movzbl	r1,r0
	movl	r0,NSECT(fp)

	movzbl	h2,r0			|* Guarda o No. de cabeças
	incl	r0
	movl	r0,NHEAD(fp)

.endif	TYPE

|*
|*	Le o caminho
|*
.if	TYPE == GEO || TYPE == LINEAR
	cmpb	#0,STDBOOT(fp)		|* Verifica se é o "std_boot"
	jne	decode
.endif	TYPE

	movw	#NAMES,r5		|* r5: módulos de IDSZ bytes
3$:
	movw	r5,r4			|* r4: byte do momento
4$:
	call	getc

	cmpb	#'\n',r0
	jeq	decode

	cmpb	#'/', r0
	jeq	6$

	movb	r0,(r4)
	incw	r4
	jmp	4$

|*
|*	Achou uma Barra - Separa em Pedacos de IDSZ caracteres
|*
6$:
	cmpw	r5,r4			|* Ignora o '/' em "/boot"
	jeq	4$

	addw	#IDSZ,r5
	jmp	3$

|*
|*	O caminho terminou: decodifica
|*
decode:
	movw	#ROOTINO,INO(fp) 	|* Começa da raiz do sistema de arq.

	movw	#NAMES,r3		|* r3: Início de cada ID

	cmpb	#0,(r3)			|* Verifica se a linha está vazia
	jne	7$
	movl	#0x746F6F62,(r3) 	|* Default = "boot" (little-endian)
7$:
	clrw	LBNO(fp)		|* Começa do bloco 0 do diretório

	call	iget			|* Lê um INODE

	cmpb	#0,(r3)			|* desvia se foi o fim do caminho
	jeq	read_boot2
9$:
	movw	#AREA,BA(fp)		|* Le um bloco do diretório ou arq.
	call	log_read

	jcs	again			|* Desvia se EOF

	movw	#AREA,DIRPTR(fp) 	|* Ponteiro para a primeira entrada

|*
|*	Verifica se esta entrada está vazia
|*
10$:
	movw	DIRPTR(fp),r5
	cmpw	#0,(r5)
	jeq	11$

	addw	#2,r5			|* Aponta para o ID

|*
|*	Compara um identificador
|*
	movw	r3,r4			|* Id dado
   |***	movw	DIRPTR(fp),r5		|* Id do diretório
   |***	addw	#2,r5
	movw	#IDSZ,r1		|* Comprimento
   |***	up
	repe
	cmpsb

	jeq	13$

|*
|*	A entrada está vazia ou o nome não confere
|*
11$:
	addw	#ENTSZ,DIRPTR(fp)	|* Aponta para a entrada seguinte

	cmpw	#AREA+BLSZ,DIRPTR(fp)	|* Testa o nome seguinte
	jlt	10$

	jmp	9$			|* Se é o fim de bloco, le o seguinte

|*
|*	Achou o nome
|*
13$:
	movw	DIRPTR(fp),r5		|* Novo inode (big-endian)
	movw	(r5),r0
	xchgb	r0,h0
	movw	r0,INO(fp)

	addw	#IDSZ,r3		|* Pula este nome
	jmp	7$

|*
|*	Acabou o caminho. Le o BOOT2
|*
read_boot2:
	movw	#BOOT2LOADSEG,r3 	|* Endereco de Leitura (relativo a "es")
	clrw	BA(fp)
14$:
	movw	r3,es

	call	log_read		|* Se EOF, desvia para o boot2
	jcs	callout

	addw	#BLSZ/16,r3		|* Incrementa o segmento de BLSZ
	jmp	14$

|*
|*	Acabou de Ler o BOOT2. Desvia para Ele
|*
callout:
	farjmpw	#BOOT2EXECSEG,0		|* Desvia para o inicio do BOOT2

|*
|****************************************************************
|*	Rotina para Ler um INODE. o INO é dado em INO(fp)	*
|****************************************************************
|*
iget:
	pushw	r3			|* Salva r3

.if	DEBUG
	movb	#'i',r0
	call	putc
	movw	INO(fp),r0
	call	puthex
.endif	DEBUG

	clrl	r0			|* Obtém o no. do bloco (em BIG_ENDIAN)
	movw	INO(fp),r0
	addw	#15,r0
	lsrw	#3,r0
	call	long_endian_cv

	movw	#AREA,BA(fp)		|* Le o bloco (dado em r0)
	call	phys_read

	movw	INO(fp),r5
	addw	#15,r5
	andw	#0x07,r5		|* Nr. do inode do bloco (0 a 7)
	aslw	#6,r5

|*
|*	Converte os enderecos de 3 para 4 bytes (continua em big-endian)
|*
	addw	#AREA+12+12*3,r5 	|* r5 = final dos end. de 3 bytes (12 end)
	movw	#BLADDR+12*4,r4  	|* r4 = end. de 4 bytes
1$:
	subw	#3,r5
	subw	#4,r4

	movl	(r5),r0
	lsll	#8,r0
	movl	r0,(r4)

	cmpw	#BLADDR,r4		|* ve se chegou no fim
	jhi	1$

.if	ADR2II
|*
|*	Ve se tem bloco duplamente indireto
|*
	movl	IBADDR+4,r0
	tstl	r0
	jeq	7$

	movw	#IIADDR,BA(fp)		|* Le o bloco (dado em r0)
	call	phys_read

	movl	IIADDR,r0

   |***	movw	#IIADDR,BA(fp)		|* Le o bloco (dado em r0)
	call	phys_read
.endif	ADR2II

|*
|*	Ve se tem bloco indireto
|*
7$:
	movl	IBADDR,r0
	tstl	r0
	jeq	9$

	movw	#IBADDR,BA(fp)		|* Le o bloco (dado em r0)
	call	phys_read
9$:
	popw	r3			|* Restaura r3

	rts

|*
|****************************************************************
|*	Conversão little-big ENDIAN				*
|****************************************************************
|*
long_endian_cv:
	xchgb	r0,h0
	rorl	#16,r0
	xchgb	r0,h0

	rts

|*
|****************************************************************
|*	Rotina para ler um Bloco Logico do Disco		*
|****************************************************************
|*
|*	O No. do Bloco Logico é dado em LBNO, que é incrementado
|*	a cada chamada. No caso de leitura normal, a rotina retorna
|*	com o valor 1 em r0, e 0 em caso de EOF
|*
|*	LBNO => No. do Bloco Fisico
|*	BA   => Endereco do Buffer
|*
log_read:
	pushw	r3			|* Salva r3

.if	DEBUG
	movb	#'l',r0
	call	putc
	movw	LBNO(fp),r0
	call	puthex
.endif	DEBUG

	movw	LBNO(fp),r3		|* Obtem o no. lógico e incrementa
	addw	#1,LBNO(fp)

	aslw	#2,r3			|* (4 bytes por endereco)

	movl	BLADDR(r3),r0		|* Carrega o Endereco Fisico

	tstl	r0			|* Desvia se NÃO EOF
	jne	2$

	stc				|* Carry on => EOF
	jmp	3$
2$:
   	call	phys_read 		|* Lê o bloco e retorna com 1
	clc				|* Carry off => leu o bloco
3$:
	popw	r3			|* Restaura r3

	rts

|*
|****************************************************************
|*	Le um Bloco Fisico do Disco				*
|****************************************************************
|*
|*	r0   => No. do Bloco Fisico (32 bits, em BIG_ENDIAN!)
|*	BA   => Endereco do Buffer
|*
phys_read:
	call	long_endian_cv		|* Transforma em little-endian
	movl	r0,PBNO(fp)		|* Guarda o número do bloco

.if	TYPE == LINEAR

	pushw	r4
1$:
	movw	#linear_cb,r4		|* Endereço do bloco

	movb	#16,(r4)		|* 16 bytes
	movb	#1,2(r4)		|* Um bloco

	movw	BA(fp),r0		|* Área
	movw	r0,4(r4)

	movw	es,6(r4)		|* Segmento "es"

	movl	PBNO(fp),r0
	addl	OFFSET(fp),r0		|* Acrescenta o deslocamento da partição
	movl	r0,8(r4)
|*	clrl	12(r4)

   	movb	#0x42,h0		|* Leitura
	movb	DEVCODE(fp),r2		|* Dispositivo
|*	movw	#linear_cb,r4		|* Endereço do bloco
	int	#0x13
	jcc	9$

	movb	#'E',r0			|* Em caso de erro, ...
	call	putc

	clrw	r0
	int	#0x13

	jmp	1$
9$:
	popw	r4
	rts

.else	TYPE == FD || TYPE == GEO

1$:
	movl	PBNO(fp),r0		|* Dividendo == No. do bloco dado
.if	TYPE == GEO
	addl	OFFSET(fp),r0		|* Acrescenta o deslocamento da partição
.endif	TYPE
	clrl	r2			|* Zera a parte alta do dividendo

	divul	NSECT(fp),r0		|* Quociente em r0 (l) (usa 32 bits)
					|* Resto == (setor - 1) em r2 (l)

	movb	r2,r1			|* r1 == setor
	incb	r1

	clrl	r2			|* Zera a parte alta do dividendo

	divul	NHEAD(fp),r0		|* Quociente == cilindro em r0 (w)
					|* Resto == cabeça em r2 (w)

	movb	r0,h1			|* Cilindro
	lsrw	#2,r0
	andb	#0xC0,r0
	orb	r0,r1

   |***	movb	...,r1			|* Setor
	movb	r2,h2			|* Cabeça
	movw	#0x0201,r0		|* 2 = ler, 1 bloco
.if	TYPE == GEO || TYPE == LINEAR
	movb	DEVCODE(fp),r2		|* Dispositivo
.else	TYPE == FD
	movb	#UNIT,r2		|* Dispositivo
.endif	TYPE
	movw	BA(fp),r3		|* Área
	int	#0x13

	jcc	9$

	movb	#'E',r0			|* Em caso de erro, ...
	call	putc

	clrw	r0
	int	#0x13

	jmp	1$
9$:
	rts

.endif	TYPE

.if	TYPE == GEO
|*
|****************************************************************
|*	Define os objetos compartilhadas com "boot0"		*
|****************************************************************
|*
putstr		= start + DIFF + 0x0189
putc		= start + DIFF + 0x019A
getc		= start + DIFF + 0x01AE

.elif	TYPE == FD || TYPE == LINEAR
|*
|****************************************************************
|*	Le e ecoa um caractere da console			*
|****************************************************************
|*
getc:
1$:
	clrb	h0			|* Le um caractere (em r0)
	int	#0x16

	cmpb	#'\r',r0		|* Transforma '\r' em '\n'
	jne	2$

	movb	#'\n',r0
2$:
	pushw	r0			|* Salva o caractere lido

	call	putc			|* Ecoa o caractere

	popw	r0			|* Restaura o caractere lido

	rts

|*
|****************************************************************
|*	Imprime um caractere (em r0) na console			*
|****************************************************************
|*
putc:
	pushw	r3			|* Salva r3

	cmpb	#'\n',r0		|* Verifica se é '\n'
	jne	1$

	movb	#'\r',r0		|* Imprime também o '\r'
	call	putc

	movb	#'\n',r0
1$:
	clrb	h3			|* Escreve o caractere (em r0)
	movb	#14,h0
	int	#0x10

	popw	r3			|* Restaura r3
	rts

|*
|****************************************************************
|*	Escreve uma cadeia (endereço em r0)			*
|****************************************************************
|*
putstr:
	pushw	r4			|* Salva r4
	movw	r0,r4
1$:
	movb	(r4),r0			|* Obtém um caractere
	incw	r4

	cmpb	#'\0',r0		|* Verifica se chegou no final
	jeq	2$

	call	putc			|* Imprime um caractere
	jmp	1$
2$:
	popw	r4			|* Restaura r4
	rts
.endif	TYPE

.if	DEBUG
|*
|****************************************************************
|*	Escreve um valor hexadecimal (em r0)			*
|****************************************************************
|*
puthex:
	pushw	r3
	pushw	r2

	movw	r0,r3
	movb	#4,r2
1$:
	movw	r3,r0
	lslw	#4,r3

	lsrw	#12,r0
	andb	#0x0F,r0

	cmpb	#10,r0
	jge	2$

	addb	#'0',r0
	jmp	4$
2$:
	addb	#'A'-10,r0
4$:
	call	putc

	decb	r2
	jgt	1$

	movb	#'\n',r0
	call	putc

	popw	r2
	popw	r3
	rts
.endif	DEBUG

|*
|****************************************************************
|*	Mensagens 						*
|****************************************************************
|*
pgversion:
	.iso	"\nTROPIX boot1"
.if	TYPE == LINEAR
	.iso	" (linear)"
.elif	TYPE == GEO
	.iso	" (geo)"
.endif	TYPE
	.iso	", Versao: 4.0.0, de 13.07.00\n"
	.isoz	"\n> "

|*
|****************************************************************
|*	Número mágico						*
|****************************************************************
|*
	.org	start+BLSZ-2

	.byte	0x55, 0xAA

|*
|****************************************************************
|*	Áreas e Váriaveis					*
|****************************************************************
|*
	.bss
EDATA:

.if	TYPE == LINEAR
linear_cb:
	.dsl	4			|* Para a INT13 estendida
.endif	TYPE == LINEAR

BLADDR: .dsl	10			|*  10 Blocos Diretos
IBADDR: .dsl	128			|* 128 Blocos Indiretos
IIADDR: .dsl	128			|* 128 Blocos duplamente Indiretos

NAMES: 	.dsb	80			|* Caminho do boot2

AREA:	.dsb	BLSZ			|* Área para ler os diretórios e inodes

END:
