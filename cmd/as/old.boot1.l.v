                        |*
                        |****************************************************************
                        |*								*
                        |*			t1.boot1.s				*
                        |*								*
                        |*	BOOT para o PC, estágio 1				*
                        |*								*
                        |*	Esta etapa do BOOT reside no bloco 0 do disco,		*
                        |*	e é carregado pela ROM em 31 Kb				*
                        |*								*
                        |*	Versão	1.0.0, de 27.07.87				*
                        |*		4.3.0, de 27.08.02				*
                        |*								*
                        |*	Módulo: Boot1						*
                        |*		NÚCLEO do TROPIX para PC			*
                        |*								*
                        |*	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
                        |*		Copyright © 2002 NCE/UFRJ - tecle "man licença"	*
                        |*								*
                        |****************************************************************
                        |*
                        
                        	.seg16
                        
                        |*
                        |******	Definições globais **************************************
                        |*
       0000 0000        DEBUG	= 0			|* Liga ou não a depuração
                        
       0000 0001        FD	= 1			|* Disquete 1.44 MB
       0000 0002        GEO	= 2			|* Disco IDE ou SCSI usando INT13 normal
       0000 0003        LINEAR	= 3			|* Disco IDE ou SCSI usando INT13 Extensions
                        
                        |*
                        |******	Definições dos dispositivos *****************************
                        |*
                        .ifndef	DEV
                        
                        	.error	"Faltou definir o dispositivo"
                        
                        .elif	DEV == 1440		|* Disquete de 1.44 Mb
                        
                        	TYPE	= FD
                        	UNIT	= 0
                        	SECT	= 18
                        	HEAD	= 2
                        
                        .elif	DEV == 80		|* Disco IDE ou SCSI usando INT13 normal
                        
                        	TYPE	= GEO
                        
                        .elif	DEV == 81		|* Disco IDE ou SCSI usando INT13 Extensions
                        
       0000 0003        	TYPE	= LINEAR
                        
                        .else
                        
                        	.error	"Valor inválido de dispositivo: %X", DEV
                        
                        .endif
                        
                        |*
                        |******	Definições Diversas *************************************
                        |*
       0000 0060        ROOTINO		= 3 << 5
                        
       0000 0020        LIDSZ		= 32
                        
       0000 0050        HDRSZ		= 80
                        
       0000 0009        BLSHIFT		= 9
       0000 0200        BLSZ		= 1 << BLSHIFT
       0000 01FF        BLMASK		= BLSZ - 1
                        
       0000 0400        KBSZ		= 1024
       0000 1000        BL4SZ		= 4096
                        
                        |*
                        |****** Entrada do Diretório do T1 ******************************
                        |*
       0000 0000        d_ino		= 0
       0000 0004        d_ent_sz	= 4
       0000 0006        d_nm_len	= 6
       0000 0008        d_name		= 8
                        
                        |*
                        |****** Entrada do INODE do T1 **********************************
                        |*
       0000 0024        n_addr		= 36
                        
                        |*
                        |****** Entrada do bloco de E/S linear **************************
                        |*
       0000 0002        l_cnt		= 2
       0000 0004        l_area		= 4
       0000 0006        l_seg		= 6
       0000 0008        l_bno		= 8
                        
                        |*
                        |******	Definições das regiões da memória ***********************
                        |*
       0000 C000        BOOT0ADDR	= 48 * KBSZ		|* Endereço de carga de "boot0"
       0000 7C00        BOOT1ADDR	= 31 * KBSZ		|* Endereço de carga de "boot1"
       0001 0000        BOOT2ADDR	= 64 * KBSZ		|* Endereço de carga de "boot2"
                        
       0000 FC00        STACKADDR	= 63 * KBSZ		|* Endereço virtual do início da pilha
                        
       0000 0005        HSEG		= 80 / 16		|* No. de segmentos do cabeçalho
                        
       0000 1000        BOOT2LOADSEG	= BOOT2ADDR / 16	|* Segmento de carga do "boot2"
       0000 1005        BOOT2EXECSEG	= BOOT2LOADSEG+HSEG	|* Segmento de execução do "boot2"
                        
                        |*
                        |******	Deslocamento das variáveis na pilha *********************
                        |*
       0000 0010        STDBOOT	= 16				|* Byte informando para não esperar
       0000 0008        OFFSET	= 8				|* Início da partição
       0000 0000        DEVCODE	= 0				|* Código do Dispositivo
       FFFF FFFC        LBNO	= -4				|* Bloco lógico do disco
       FFFF FFF8        BA	= -8				|* Endereço na memoria da leitura
       FFFF FFF4        INO	= -12				|* No. do INODE a ler
       FFFF FFF0        DIRPTR	= -16				|* Ponteiro para as entradas do diretório
       FFFF FFEC        NSECT	= -20				|* No. de setores por trilha
       FFFF FFE8        NHEAD	= -24				|* No. de cabeças
                        
       0000 0018        LINK_SZ	= 24				|* Tamanho total alocado na pilha
                        
                        |*
                        |****************************************************************
                        |******	Comeco do Boot1	*****************************************
                        |****************************************************************
                        |*
                        	.text
                        	.global	start
000000                  start:
000000 31 C0            	clrw	r0
000002 8E D8            	movw	r0,ds
000004 8E C0            	movw	r0,es
000006 8E D0            	movw	r0,ss
                        
                        |*
                        |******	Prepara a pilha *****************************************
                        |*
000008                  again:
                        .if	TYPE == FD
                        	movw	#STACKADDR,fp
                        .else	TYPE == GEO || TYPE == LINEAR
                           |***	movw	#...,fp 		|* HD: recebe em fp a partição
                        .endif	TYPE
                        
000008 BC FBE8          	movw	#STACKADDR-LINK_SZ,sp
                        
                        |*
                        |******	Põe o cabeçalho + prompt ********************************
                        |*
00000B B8 00B4          	movw	#pgversion,r0
00000E E8 0092          	call	putstr
                        
                        |*
                        |******	Inicializa o número de cabeças e setores ****************
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
                        |****** Carrega os blocos seguintes de "boot1" ******************
                        |*
000011 BE 01EC          	movw	#linear_cb,r4
000014 C7 44 02 0001    	movw	#[[ETEXT-start+BLMASK]>>BLSHIFT]-1,l_cnt(r4)
000019 C7 44 04 0200    	movw	#start+BLSZ,l_area(r4)
00001E 8C 44 06         	movw	es,l_seg(r4)
000021 66 C7 44 08 0000 	movl	#1,l_bno(r4)
       0001
                        
000029 E8 0046          	call	phys_read
                        
                        |*
                        |******	Zera o "bss" ********************************************
                        |*
00002C 30 C0            	clrb	r0
00002E BF 031C          	movw	#EDATA,r5
000031 B9 2140          	movw	#END-EDATA,r1
000034 FC               	up
000035 F3               	rep
000036 AA               	stosb
                        
                        |*
                        |******	Le o caminho ********************************************
                        |*
                        .if	TYPE == GEO || TYPE == LINEAR
000037 80 7E 10 00      	cmpb	#0,STDBOOT(fp)		|* Verifica se é o "std_boot"
00003B 0F85 01C4        	jne	decode_2
                        .endif	TYPE
                        
00003F BF 135C          	movw	#NAMES,r5		|* r5: módulos de LIDSZ bytes
000042                  3$:
000042 89 FE            	movw	r5,r4			|* r4: byte do momento
000044                  4$:
000044 E8 001B          	call	getc
                        
000047 3C 0A            	cmpb	#'\n',r0
000049 0F84 01B3        	jeq	decode_1
                        
00004D 3C 2F            	cmpb	#'/', r0
00004F 7405             	jeq	6$
                        
000051 88 04            	movb	r0,(r4)
000053 46               	incw	r4
000054 EBEE             	jmp	4$
                        
                        |*
                        |******	Achou uma Barra - Separa em Pedacos de LIDSZ caracteres *
                        |*
000056                  6$:
000056 39 FE            	cmpw	r5,r4			|* Ignora o '/' em "/boot"
000058 74EA             	jeq	4$
                        
00005A C6 04 00         	movb	#'\0',(r4)		|* Coloca um '\0' no final
                        
00005D 83 C7 20         	addw	#LIDSZ,r5
000060 EBE0             	jmp	3$
                        
                        |*
                        |******	Le e ecoa um caractere da console ***********************
                        |*
000062                  getc:
000062                  1$:
000062 30 E4            	clrb	h0			|* Le um caractere (em r0)
000064 CD 16            	int	#0x16
                        
000066 3C 0D            	cmpb	#'\r',r0		|* Transforma '\r' em '\n'
000068 7502             	jne	2$
                        
00006A B0 0A            	movb	#'\n',r0
00006C                  2$:
00006C 50               	pushw	r0			|* Salva o caractere lido
                        
00006D E8 001F          	call	putc			|* Ecoa o caractere
                        
000070 58               	popw	r0			|* Restaura o caractere lido
                        
000071 C3               	rts
                        
                        |*
                        |******	Le um Bloco Fisico do Disco *****************************
                        |*
                        |*	TODOS os argumentos já em "linear_cb"
                        |*
                        |*	r4   => Endereço do "linear_cb"
                        |*
                        .if	TYPE == LINEAR	|****************************************
                        
000072                  phys_read:
                        
                        .if	DEBUG
                        	movb	#'P',r0
                        	call	putc
                        	movl	l_bno(r4),r0
                        	call	puthex
                        .endif	DEBUG
                        
000072 66 8B 46 08      	movl	OFFSET(fp),r0		|* Acrescenta o deslocamento da partição
000076 66 01 44 08      	addl	r0,l_bno(r4)
00007A                  1$:
00007A B4 42               	movb	#0x42,h0		|* Leitura
00007C 8A 56 00         	movb	DEVCODE(fp),r2		|* Dispositivo
                        |*	movw	#linear_cb,r4		|* Endereço do bloco
00007F CD 13            	int	#0x13
000081 730B             	jcc	9$
                        
000083 B0 45            	movb	#'E',r0			|* Em caso de erro, ...
000085 E8 0007          	call	putc
                        
000088 31 C0            	clrw	r0
00008A CD 13            	int	#0x13
                        
00008C EBEC             	jmp	1$
00008E                  9$:
00008E C3               	rts
                        
                        .else	TYPE == FD || TYPE == GEO |******************************
                        
                        phys_read:
                        	pushw	r3
                        
                        .if	DEBUG
                        	movb	#'P',r0
                        	call	putc
                        	movl	l_bno(r4),r0
                        	call	puthex
                        .endif	DEBUG
                        
                        1$:
                        	movl	l_bno(r4),r0		|* Dividendo == No. do bloco dado
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
                        	movw	#0x0201,r0		|* 2 = ler, 1 bloco == BLSZ
                        .if	TYPE == GEO || TYPE == LINEAR
                        	movb	DEVCODE(fp),r2		|* Dispositivo
                        .else	TYPE == FD
                        	movb	#UNIT,r2		|* Dispositivo
                        .endif	TYPE
                        	movw	l_area(r4),r3		|* Área
                        	int	#0x13
                        
                        	jcc	9$
                        
                        	movb	#'E',r0			|* Em caso de erro, ...
                        	call	putc
                        
                        	clrw	r0
                        	int	#0x13
                        
                        	jmp	1$
                        9$:					|* Leu corretamente
                        	decb	l_cnt(r4)
                        	jle	20$
                        
                        	addw	#BLSZ,l_area(r4)
                        	incl	l_bno(r4)
                        
                        	jmp	1$
                        20$:
                        	popw	r3
                        	rts
                        
                        .endif	TYPE
                        
                        |*
                        |******	Imprime um caractere (em r0) na console	*****************
                        |*
00008F                  putc:
00008F 53               	pushw	r3			|* Salva r3
                        
000090 3C 0A            	cmpb	#'\n',r0		|* Verifica se é '\n'
000092 7507             	jne	1$
                        
000094 B0 0D            	movb	#'\r',r0		|* Imprime também o '\r'
000096 E8 FFF6          	call	putc
                        
000099 B0 0A            	movb	#'\n',r0
00009B                  1$:
00009B 30 FF            	clrb	h3			|* Escreve o caractere (em r0)
00009D B4 0E            	movb	#14,h0
00009F CD 10            	int	#0x10
                        
0000A1 5B               	popw	r3			|* Restaura r3
0000A2 C3               	rts
                        
                        |*
                        |******	Escreve uma cadeia (endereço em r0) *********************
                        |*
0000A3                  putstr:
0000A3 56               	pushw	r4			|* Salva r4
0000A4 89 C6            	movw	r0,r4
0000A6                  1$:
0000A6 8A 04            	movb	(r4),r0			|* Obtém um caractere
0000A8 46               	incw	r4
                        
0000A9 3C 00            	cmpb	#'\0',r0		|* Verifica se chegou no final
0000AB 7405             	jeq	2$
                        
0000AD E8 FFDF          	call	putc			|* Imprime um caractere
0000B0 EBF4             	jmp	1$
0000B2                  2$:
0000B2 5E               	popw	r4			|* Restaura r4
0000B3 C3               	rts
                        
                        .if	DEBUG
                        |*
                        |******	Escreve um valor hexadecimal (em r0) ********************
                        |*
                        puthex:
                        	pushl	r3
                        	pushw	r2
                        
                        	movl	r0,r3
                        	movb	#8,r2
                        1$:
                        	movl	r3,r0
                        	lsll	#4,r3
                        
                        	lsrl	#32-4,r0
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
                        	popl	r3
                        	rts
                        .endif	DEBUG
                        
                        |*
                        |******	Mensagens ***********************************************
                        |*
0000B4                  pgversion:
0000B4 0A 54 52 4F 50   	.iso	"\nTROPIX T1"
       49 58 20 54 31
                        .if	TYPE == LINEAR
0000BE 20 6C 69 6E 65   	.iso	" linear"
       61 72
                        .elif	TYPE == GEO
                        	.iso	" geo"
                        .elif	TYPE == FD
                        	.iso	" 1440"
                        .endif	TYPE
0000C5 20 62 6F 6F 74   	.iso	" boot1, Versao: 4.3.0, de 27.08.02\n"
       31 2C 20 56 65
       72 73 61 6F 3A
       20 34 2E 33 2E
       30 2C 20 64 65
       20 32 37 2E 30
       38 2E 30 32 0A
0000E8 0A 3E 20 00      	.isoz	"\n> "
                        
                        |*
                        |****** Área para leitura linear ********************************
                        |*
                        	.org	start+BLSZ-2-16-2
0001EC                  linear_cb:
0001EC 10               	.byte	16			|* Para a INT13 estendida
0001ED                  	.dsb	15
                        
                        |*
                        |******	Número mágico *******************************************
                        |*
                        	.org	start+BLSZ-2
                        
0001FE 55 AA            	.byte	0x55, 0xAA
                        
                        |*
                        |****************************************************************
                        |******	Aqui inicia o segundo bloco de "boot1" ******************
                        |****************************************************************
                        |*
                        
                        |*
                        |******	O caminho terminou: decodifica **************************
                        |*
000200                  decode_1:
000200 C6 04 00         	movb	#'\0',(r4)		|* Coloca um '\0' no final
000203                  decode_2:
000203 66 C7 46 F4 0000 	movl	#ROOTINO,INO(fp) 	|* Começa da raiz do sistema de arq.
       0060
                        
00020B BB 135C          	movw	#NAMES,r3		|* r3: Início de cada ID
                        
00020E 80 3F 00         	cmpb	#'\0',(r3)		|* Verifica se a linha está vazia
000211 7507             	jne	7$
                        
000213 66 C7 07 746F 6F62	movl	#0x746F6F62,(r3) 	|* Default = "boot" (little-endian)
00021A                  7$:
00021A C7 46 FC 0000    	clrw	LBNO(fp)		|* Começa do bloco 0 do diretório
                        
00021F E8 0067          	call	iget			|* Lê um INODE
                        
000222 80 3F 00         	cmpb	#'\0',(r3)		|* desvia se foi o fim do caminho
000225 7448             	jeq	read_boot2
000227                  9$:
000227 C7 46 F8 145C    	movw	#AREA,BA(fp)		|* Le um bloco do diretório ou arq.
00022C E8 00B4          	call	log_read
                        
00022F 0F82 FDD5        	jcs	again			|* Desvia se EOF
                        
000233 C7 46 F0 145C    	movw	#AREA,DIRPTR(fp) 	|* Ponteiro para a primeira entrada
                        
                        |*
                        |******	Verifica se esta entrada está vazia (d_ino == 0) ********
                        |*
000238                  10$:
000238 8B 7E F0         	movw	DIRPTR(fp),r5		|* r5 => entrada do diretório
                        
00023B 66 83 3D 00      	cmpl	#0,d_ino(r5)
00023F 740D             	jeq	25$
                        
                        |*
                        |******	Compara um identificador ********************************
                        |*
000241 8B 4D 06         	movw	d_nm_len(r5),r1		|* r1 => Comprimento + 1
000244 41               	incw	r1
000245 83 C7 08         	addw	#d_name,r5		|* r5 => ID do diretório
000248 89 DE            	movw	r3,r4			|* r4 => ID lido
                           |***	up
00024A F3               	repe
00024B A6               	cmpsb
                        
00024C 7412             	jeq	30$
                        
                        |*
                        |******	A entrada está vazia ou o nome não confere **************
                        |*
00024E                  25$:
00024E 8B 7E F0         	movw	DIRPTR(fp),r5		|* r5 => entrada do diretório
000251 8B 45 04         	movw	d_ent_sz(r5),r0
000254 01 46 F0         	addw	r0,DIRPTR(fp)		|* Aponta para a entrada seguinte
                        
000257 81 7E F0 245C    	cmpw	#AREA+BL4SZ,DIRPTR(fp)	|* Testa o nome seguinte
00025C 7CDA             	jlt	10$
                        
00025E EBC7             	jmp	9$			|* Se é o fim de bloco, le o seguinte
                        
                        |*
                        |******	Achou o nome ********************************************
                        |*
000260                  30$:
000260 8B 7E F0         	movw	DIRPTR(fp),r5		|* Novo INODE
000263 66 8B 05         	movl	d_ino(r5),r0
000266 66 89 46 F4      	movl	r0,INO(fp)
                        
00026A 83 C3 20         	addw	#LIDSZ,r3		|* Pula este nome
00026D EBAB             	jmp	7$
                        
                        |*
                        |******	Acabou o caminho. Le o BOOT2 ****************************
                        |*
00026F                  read_boot2:
00026F BB 1000          	movw	#BOOT2LOADSEG,r3 	|* Endereco de Leitura (relativo a "es")
000272 C7 46 F8 0000    	clrw	BA(fp)
000277                  38$:
000277 8E C3            	movw	r3,es			|* O segmento "es" é que vai sendo incrementado
                        
000279 E8 0067          	call	log_read		|* Se EOF, desvia para o boot2
00027C 7206             	jcs	callout
                        
00027E 81 C3 0100       	addw	#BL4SZ/16,r3		|* Incrementa o segmento de BL4SZ
000282 EBF3             	jmp	38$
                        
                        |*
                        |******	Acabou de Ler o BOOT2. Desvia para Ele ******************
                        |*
000284                  callout:
000284 EA 0000 1005     	farjmpw	#BOOT2EXECSEG,0		|* Desvia para o inicio do BOOT2
                        
                        |*
                        |******	Função para Ler um INODE ********************************
                        |*
                        |*	INO(fp) => Número do INODE
                        |*
000289                  iget:
000289 53               	pushw	r3			|* Salva r3
00028A 56               	pushw	r4			|* Salva r4
                        
                        .if	DEBUG
                        	movb	#'I',r0
                        	call	putc
                        	movl	INO(fp),r0
                        	call	puthex
                        .endif	DEBUG
                        
00028B BE 01EC          	movw	#linear_cb,r4
00028E C6 44 02 08      	movb	#8,l_cnt(r4)
000292 C7 44 04 145C    	movw	#AREA,l_area(r4)
                           |***	movw	es,l_seg(r4)	***|
000297 66 8B 46 F4      	movl	INO(fp),r0
00029B 66 C1 E8 05      	lsrl	#5,r0			|* Calcula o número do bloco (BL4)
00029F 66 C1 E0 03      	lsll	#3,r0			|* Transforma BL4 em BL
0002A3 66 89 44 08      	movl	r0,l_bno(r4)
                        
0002A7 E8 FDC8          	call	phys_read
                        
0002AA 8B 76 F4         	movw	INO(fp),r4		|* Calcula o deslocamento
0002AD 83 E6 1F         	andw	#31,r4
0002B0 C1 E6 07         	lslw	#7,r4
0002B3 81 C6 1480       	addw	#AREA+n_addr,r4
                        
                        |*	Copia os 16 endereços diretos + indireto simples
                        
                           |***	movw	#...,r4			|* r4 => endereços no INODE
0002B7 BF 031C          	movw	#BLADDR,r5  		|* r5 => endereços diretos
0002BA B9 0011          	movw	#16+1,r1
                           |***	up
0002BD F3               	repe
0002BE 66 A5            	movsl
                        
                        |*	Ve se tem bloco indireto
                        
0002C0                  7$:
0002C0 66 A1 035C       	movl	IBADDR,r0
0002C4 66 85 C0         	tstl	r0
0002C7 7417             	jeq	9$
                        
0002C9 BE 01EC          	movw	#linear_cb,r4
0002CC C6 44 02 08      	movb	#8,l_cnt(r4)
0002D0 C7 44 04 035C    	movw	#IBADDR,l_area(r4)
                           |***	movw	es,l_seg(r4)	***|
0002D5 66 C1 E0 03      	lsll	#3,r0			|* Transforma BL4 em BL
0002D9 66 89 44 08      	movl	r0,l_bno(r4)
                        
0002DD E8 FD92          	call	phys_read
                        
0002E0                  9$:
                        .if	DEBUG
                        	movb	#'a',r0
                        	call	putc
                        	movl	BLADDR,r0
                        	call	puthex
                        .endif	DEBUG
                        
0002E0 5E               	popw	r4			|* Restaura r4
0002E1 5B               	popw	r3			|* Restaura r3
                        
0002E2 C3               	rts
                        
                        |*
                        |******	Função para ler um Bloco Logico do Disco ****************
                        |*
                        |*	O No. do Bloco Logico é dado em LBNO, que é incrementado
                        |*	a cada chamada. No caso de leitura normal, a rotina retorna
                        |*	com o valor 1 em r0, e 0 em caso de EOF
                        |*
                        |*	LBNO => No. do Bloco Fisico
                        |*	BA   => Endereco do Buffer
                        |*
0002E3                  log_read:
0002E3 56               	pushw	r4			|* Salva r4
                        
                        .if	DEBUG
                        	movb	#'L',r0
                        	call	putc
                        	movl	LBNO(fp),r0
                        	call	puthex
                        .endif	DEBUG
                        
0002E4 8B 76 FC         	movw	LBNO(fp),r4		|* Obtem o no. lógico e incrementa
0002E7 83 46 FC 01      	addw	#1,LBNO(fp)
                        
0002EB C1 E6 02         	lslw	#2,r4			|* (4 bytes por endereco)
                        
0002EE 66 8B 84 031C    	movl	BLADDR(r4),r0		|* Carrega o Endereco Fisico
                        
0002F3 66 85 C0         	tstl	r0			|* Desvia se NÃO EOF
0002F6 7503             	jne	2$
                        
0002F8 F9               	stc				|* Carry on => EOF
0002F9 EB1C             	jmp	3$
0002FB                  2$:
0002FB BE 01EC          	movw	#linear_cb,r4
0002FE C6 44 02 08      	movb	#8,l_cnt(r4)
000302 8B 4E F8         	movw	BA(fp),r1
000305 89 4C 04         	movw	r1,l_area(r4)
000308 8C 44 06         	movw	es,l_seg(r4)
00030B 66 C1 E0 03      	lsll	#3,r0			|* Transforma BL4 em BL
00030F 66 89 44 08      	movl	r0,l_bno(r4)
                        
000313 E8 FD5C             	call	phys_read 		|* Lê o bloco e retorna com 1
                        
000316 F8               	clc				|* Carry off => leu o bloco
000317                  3$:
000317 5E               	popw	r4			|* Restaura r3
                        
000318 C3               	rts
                        
                        |*
                        |******	Áreas e Váriaveis ***************************************
                        |*
                        	.text
000319                  ETEXT:					|* Para marcar o final do TEXT
                        
                        	.bss
00031C                  EDATA:
                        
00031C                  BLADDR: .dsl	16			|*   16 Blocos Diretos
00035C                  IBADDR: .dsl	1024			|* 1024 Blocos Indiretos
                        
00135C                  NAMES: 	.dsb	256			|* Caminho do boot2
                        
00145C                  AREA:	.dsb	BL4SZ			|* Área para ler os diretórios e inodes
                        
00245C                  END:



TIPO  ------------SÍMBOLO------------  LOCAL  ROT EXT  --VALOR--

ABS   DEV                                              0000 0051
ABS   DEBUG                                            0000 0000
ABS   FD                                               0000 0001
ABS   GEO                                              0000 0002
ABS   LINEAR                                           0000 0003
ABS   TYPE                                             0000 0003
ABS   ROOTINO                                          0000 0060
ABS   LIDSZ                                            0000 0020
ABS   HDRSZ                                            0000 0050
ABS   BLSHIFT                                          0000 0009
ABS   BLSZ                                             0000 0200
ABS   BLMASK                                           0000 01FF
ABS   KBSZ                                             0000 0400
ABS   BL4SZ                                            0000 1000
ABS   d_ino                                            0000 0000
ABS   d_ent_sz                                         0000 0004
ABS   d_nm_len                                         0000 0006
ABS   d_name                                           0000 0008
ABS   n_addr                                           0000 0024
ABS   l_cnt                                            0000 0002
ABS   l_area                                           0000 0004
ABS   l_seg                                            0000 0006
ABS   l_bno                                            0000 0008
ABS   BOOT0ADDR                                        0000 C000
ABS   BOOT1ADDR                                        0000 7C00
ABS   BOOT2ADDR                                        0001 0000
ABS   STACKADDR                                        0000 FC00
ABS   HSEG                                             0000 0005
ABS   BOOT2LOADSEG                                     0000 1000
ABS   BOOT2EXECSEG                                     0000 1005
ABS   STDBOOT                                          0000 0010
ABS   OFFSET                                           0000 0008
ABS   DEVCODE                                          0000 0000
ABS   LBNO                                             FFFF FFFC
ABS   BA                                               FFFF FFF8
ABS   INO                                              FFFF FFF4
ABS   DIRPTR                                           FFFF FFF0
ABS   NSECT                                            FFFF FFEC
ABS   NHEAD                                            FFFF FFE8
ABS   LINK_SZ                                          0000 0018
TEXT  start                                    R   E   0000 0000
TEXT  again                                    R       0000 0008
TEXT  pgversion                                R       0000 00B4
TEXT  putstr                                   R       0000 00A3
TEXT  linear_cb                                R       0000 01EC
TEXT  ETEXT                                    R       0000 0319
TEXT  phys_read                                R       0000 0072
BSS   EDATA                                    R       0000 031C
BSS   END                                      R       0000 245C
TEXT  decode_2                                 R       0000 0203
BSS   NAMES                                    R       0000 135C
TEXT  (003),$3                           L     R       0000 0042
TEXT  (003),$4                           L     R       0000 0044
TEXT  getc                                     R       0000 0062
TEXT  decode_1                                 R       0000 0200
TEXT  (003),$6                           L     R       0000 0056
TEXT  (004),$1                           L     R       0000 0062
TEXT  (004),$2                           L     R       0000 006C
TEXT  putc                                     R       0000 008F
TEXT  (005),$1                           L     R       0000 007A
TEXT  (005),$9                           L     R       0000 008E
TEXT  (006),$1                           L     R       0000 009B
TEXT  (007),$1                           L     R       0000 00A6
TEXT  (007),$2                           L     R       0000 00B2
TEXT  (011),$7                           L     R       0000 021A
TEXT  iget                                     R       0000 0289
TEXT  read_boot2                               R       0000 026F
TEXT  (011),$9                           L     R       0000 0227
BSS   AREA                                     R       0000 145C
TEXT  log_read                                 R       0000 02E3
TEXT  (011),$10                          L     R       0000 0238
TEXT  (011),$25                          L     R       0000 024E
TEXT  (011),$30                          L     R       0000 0260
TEXT  (012),$38                          L     R       0000 0277
TEXT  callout                                  R       0000 0284
BSS   BLADDR                                   R       0000 031C
TEXT  (014),$7                           L     R       0000 02C0
BSS   IBADDR                                   R       0000 035C
TEXT  (014),$9                           L     R       0000 02E0
TEXT  (015),$2                           L     R       0000 02FB
TEXT  (015),$3                           L     R       0000 0317
