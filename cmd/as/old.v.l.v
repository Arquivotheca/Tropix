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
       0000 0400        KBSZ		= 1024
       0000 000A        KBSHIFT		= 10
                        
       0010 0000        MBSZ		= 1024 * KBSZ
       0000 0014        MBSHIFT		= 20
                        
       4000 0000        GBSZ		= 1024 * MBSZ
                        
       0000 1000        PGSZ		= 4096
       0000 000C        PGSHIFT		= 12
                        
                        |*
                        |****** Definições da MMU ***************************************
                        |*
       C000 0000        SYS_ADDR	= 3 * GBSZ			|* 0xC0000000
       E000 0000        USER_ADDR	= SYS_ADDR + 512 * MBSZ		|* 0xE0000000
                        
       0000 0014        PG_DIR_SHIFT	= 20		|* Para deslocamento do dir. de pg.
                        
       0000 0003        SPROT		= 3		|* Página presente, R/W, supervisor
       0000 0007        UPROT		= 7		|* Página presente, R/W, user
                        
       0040 0000        USER_TEXT_VA	= 	     4   * MBSZ
       2000 0000        USER_DATA_VA	= 	     512 * MBSZ
       6000 0000        USER_PHYS_VA	= 1 * GBSZ + 512 * MBSZ
       6800 0000        USER_IPC_VA	= 1 * GBSZ + 640 * MBSZ
       A000 0000        USER_STACK_VA	= 2 * GBSZ + 512 * MBSZ
                        
                        |*
                        |****** Definições da CPU ***************************************
                        |*
       0000 0000        CPUID		= 0
                        
                        |*
                        |****** Definições da UPROC *************************************
                        |*
       0000 0001        USIZE		= 1		|* Tamanho (PG) da UPROC
       0000 0010        CTXT_SW_TYPE	= 16		|* Deslocamento para u.u_ctxt_sw_type
       0000 0011        NO_USER_MMU	= 17		|* Deslocamento para u.u_no_user_mmu
                        
                        	.global	_u
       E000 0000        _u		=	USER_ADDR
                        
       E000 0000        u.u_cpu		=	_u+0
       E000 0002        u.u_pri		=	_u+2
       E000 0004        u.u_dontpanic	=	_u+4
       E000 0008        u.u_proc	=	_u+8
       E000 000C        u.u_frame	=	_u+12
                        
                        |*
                        |****** Definições da PROC **************************************
                        |*
       0000 0000        proc.p_state	=	0
       0000 0004        proc.p_sig	=	4
       0000 0008        proc.p_pri	=	8
       0000 0048        proc.p_mmu_dir	=	72
                        
       0000 0005        SRUN		=	5	|* Depende do "enum" do "proc.h"
                        
                        |*
                        |****** Definições do VIDEO *************************************
                        |*
       C00B 8000        VIDEOADDR	= SYS_ADDR + 736 * KBSZ
                        
       0000 0018        LINE		= 24
       0000 0050        COL		= 80
                        
                        |*
                        |****** Seletores ***********************************************
                        |*
       0000 0008        KERNEL_CS	= { 1 * 8 + 0 }
       0000 0010        KERNEL_DS	= { 2 * 8 + 0 }
                        
       0000 001B        USER_CS		= { 3 * 8 + 3 }
       0000 0023        USER_DS		= { 4 * 8 + 3 }
                        
       0000 0028        KERNEL_TSS	= { 5 * 8 + 0 }
       0000 0033        SYS_CALL	= { 6 * 8 + 3 }
                        
                        |*
                        |****** Definição da estrutura VECDEF ***************************
                        |*
                        
                        |* Esta definição deve ser igual à de "h/intr.h"
                        
       0000 0000        VEC_PL		= 0	 	|* Prioridade da interrupção (1 a 7)
       0000 0004        VEC_UNIT	= 4		|* Unidade (informação para o driver)
       0000 0008        VEC_FUNC	= 8		|* Função de interrupção
                        
       0000 0008        VECLINESZ	= 8		|* Compartilhando até 7 unidades
                        
       0000 000C        VEC_SIZEOF	= 12		|* Tamanho de uma estrutura
                        
       0000 0060        VECLINE_SIZEOF	= VEC_SIZEOF * VECLINESZ |* Tamanho da estrutura total
                        
                        |*
                        |****** Endereços de portas *************************************
                        |*
       0000 0084        IO_NOP	= 0x84			|* Dispositivo inexistente para NOP
                        
       0000 0020        IO_ICU1	= 0x20			|* Controlador 8259A No. 1
       0000 00A0        IO_ICU2	= 0xA0			|* Controlador 8259A No. 2
                        
                        |*
                        |****** Habilita/desabilita as "lampadinhas" ********************
                        |*
       0000 0001        IDLE_PATTERN	= 1		|* Padrão do "idle"
       0000 0001        INTR_PATTERN	= 1		|* Padrão da interrupção
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
       0000 002A        CHAR		= '*'
                        
       0000 0020        SPACE		= { 0 << 8 } | ' '
       0000 042A        COLOR		= { 4 << 8 } | CHAR	|* Vermelho
       0000 0020        W		= SPACE
                        
                        .if 0	/*******************************************************/
                        W		= { 7 << 8 } | CHAR	|* Branco
                        .endif	/*******************************************************/
                        
       0000 0008        INTR_PAT_LEN	= 8			|* Em caracteres na tela
       0000 0034        INTR_PAT_LOC	= COL - 20 - INTR_PAT_LEN |* Offset na última linha
                        
       0000 0010        INTR_PAT_SZ	= 16			|* Em bytes
       0000 0004        INTR_PAT_SHIFT	= 4			|* Log (2) do acima
                        
       C00B 8F68        INTR_VIDEOADDR	= VIDEOADDR + 2 * { LINE * COL + INTR_PAT_LOC }
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
0002E4                  _vecdef:
0002E4                  	.long	0,	0,	0		|* IRQ0
0002F0                  	.long	0,	0,	0
0002FC                  	.long	0,	0,	0
000308                  	.long	0,	0,	0
000314                  	.long	0,	0,	0
000320                  	.long	0,	0,	0
00032C                  	.long	0,	0,	0
000338                  	.long	0,	0,	0
                        
000344                  	.long	0,	0,	0		|* IRQ1
000350                  	.long	0,	0,	0
00035C                  	.long	0,	0,	0
000368                  	.long	0,	0,	0
000374                  	.long	0,	0,	0
000380                  	.long	0,	0,	0
00038C                  	.long	0,	0,	0
000398                  	.long	0,	0,	0
                        
0003A4                  	.long	0,	0,	0		|* IRQ2
0003B0                  	.long	0,	0,	0
0003BC                  	.long	0,	0,	0
0003C8                  	.long	0,	0,	0
0003D4                  	.long	0,	0,	0
0003E0                  	.long	0,	0,	0
0003EC                  	.long	0,	0,	0
0003F8                  	.long	0,	0,	0
                        
000404                  	.long	0,	0,	0		|* IRQ3
000410                  	.long	0,	0,	0
00041C                  	.long	0,	0,	0
000428                  	.long	0,	0,	0
000434                  	.long	0,	0,	0
000440                  	.long	0,	0,	0
00044C                  	.long	0,	0,	0
000458                  	.long	0,	0,	0
                        
000464                  	.long	0,	0,	0		|* IRQ4
000470                  	.long	0,	0,	0
00047C                  	.long	0,	0,	0
000488                  	.long	0,	0,	0
000494                  	.long	0,	0,	0
0004A0                  	.long	0,	0,	0
0004AC                  	.long	0,	0,	0
0004B8                  	.long	0,	0,	0
                        
0004C4                  	.long	0,	0,	0		|* IRQ5
0004D0                  	.long	0,	0,	0
0004DC                  	.long	0,	0,	0
0004E8                  	.long	0,	0,	0
0004F4                  	.long	0,	0,	0
000500                  	.long	0,	0,	0
00050C                  	.long	0,	0,	0
000518                  	.long	0,	0,	0
                        
000524                  	.long	0,	0,	0		|* IRQ6
000530                  	.long	0,	0,	0
00053C                  	.long	0,	0,	0
000548                  	.long	0,	0,	0
000554                  	.long	0,	0,	0
000560                  	.long	0,	0,	0
00056C                  	.long	0,	0,	0
000578                  	.long	0,	0,	0
                        
000584                  	.long	0,	0,	0		|* IRQ7
000590                  	.long	0,	0,	0
00059C                  	.long	0,	0,	0
0005A8                  	.long	0,	0,	0
0005B4                  	.long	0,	0,	0
0005C0                  	.long	0,	0,	0
0005CC                  	.long	0,	0,	0
0005D8                  	.long	0,	0,	0
                        
0005E4                  	.long	0,	0,	0		|* IRQ8
0005F0                  	.long	0,	0,	0
0005FC                  	.long	0,	0,	0
000608                  	.long	0,	0,	0
000614                  	.long	0,	0,	0
000620                  	.long	0,	0,	0
00062C                  	.long	0,	0,	0
000638                  	.long	0,	0,	0
                        
000644                  	.long	0,	0,	0		|* IRQ9
000650                  	.long	0,	0,	0
00065C                  	.long	0,	0,	0
000668                  	.long	0,	0,	0
000674                  	.long	0,	0,	0
000680                  	.long	0,	0,	0
00068C                  	.long	0,	0,	0
000698                  	.long	0,	0,	0
                        
0006A4                  	.long	0,	0,	0		|* IRQ10
0006B0                  	.long	0,	0,	0
0006BC                  	.long	0,	0,	0
0006C8                  	.long	0,	0,	0
0006D4                  	.long	0,	0,	0
0006E0                  	.long	0,	0,	0
0006EC                  	.long	0,	0,	0
0006F8                  	.long	0,	0,	0
                        
000704                  	.long	0,	0,	0		|* IRQ11
000710                  	.long	0,	0,	0
00071C                  	.long	0,	0,	0
000728                  	.long	0,	0,	0
000734                  	.long	0,	0,	0
000740                  	.long	0,	0,	0
00074C                  	.long	0,	0,	0
000758                  	.long	0,	0,	0
                        
000764                  	.long	0,	0,	0		|* IRQ12
000770                  	.long	0,	0,	0
00077C                  	.long	0,	0,	0
000788                  	.long	0,	0,	0
000794                  	.long	0,	0,	0
0007A0                  	.long	0,	0,	0
0007AC                  	.long	0,	0,	0
0007B8                  	.long	0,	0,	0
                        
0007C4                  	.long	0,	0,	0		|* IRQ13
0007D0                  	.long	0,	0,	0
0007DC                  	.long	0,	0,	0
0007E8                  	.long	0,	0,	0
0007F4                  	.long	0,	0,	0
000800                  	.long	0,	0,	0
00080C                  	.long	0,	0,	0
000818                  	.long	0,	0,	0
                        
000824                  	.long	0,	0,	0		|* IRQ14
000830                  	.long	0,	0,	0
00083C                  	.long	0,	0,	0
000848                  	.long	0,	0,	0
000854                  	.long	0,	0,	0
000860                  	.long	0,	0,	0
00086C                  	.long	0,	0,	0
000878                  	.long	0,	0,	0
                        
000884                  	.long	0,	0,	0		|* IRQ15
000890                  	.long	0,	0,	0
00089C                  	.long	0,	0,	0
0008A8                  	.long	0,	0,	0
0008B4                  	.long	0,	0,	0
0008C0                  	.long	0,	0,	0
0008CC                  	.long	0,	0,	0
0008D8                  	.long	0,	0,	0
                        
                        |*
                        |****** Tabela dos endereços dos vetores ************************
                        |*
                        	.const
                        	.global	_vecdefaddr
0002A0                  _vecdefaddr:
0002A0 0000 0000        	.long	_irq0_vector
0002A4 0000 0008        	.long	_irq1_vector
0002A8 0000 0010        	.long	_irq2_vector
0002AC 0000 0018        	.long	_irq3_vector
0002B0 0000 0020        	.long	_irq4_vector
0002B4 0000 0028        	.long	_irq5_vector
0002B8 0000 0030        	.long	_irq6_vector
0002BC 0000 0038        	.long	_irq7_vector
0002C0 0000 0048        	.long	_irq8_vector
0002C4 0000 0050        	.long	_irq9_vector
0002C8 0000 0058        	.long	_irq10_vector
0002CC 0000 0060        	.long	_irq11_vector
0002D0 0000 0068        	.long	_irq12_vector
0002D4 0000 0070        	.long	_irq13_vector
0002D8 0000 0078        	.long	_irq14_vector
0002DC 0000 0080        	.long	_irq15_vector
                        
                        |*
                        |******	Vetores IRQ0-7 ******************************************
                        |*
                        	.text
                        	.align	4
                        	.global	_irq0_vector
000000                  _irq0_vector:
000000 60               	pushal
000001 BB 0000 02E4     	movl	#_vecdef+0*VECLINE_SIZEOF,r3	|* Entrada VECDEF IRQ0
000006 EB36             	jmp	int_common_0_7
                        
                        	.align	4
                        	.global	_irq1_vector
000008                  _irq1_vector:
000008 60               	pushal
000009 BB 0000 0344     	movl	#_vecdef+1*VECLINE_SIZEOF,r3	|* Entrada VECDEF IRQ1
00000E EB2E             	jmp	int_common_0_7
                        
                        	.align	4
                        	.global	_irq2_vector
000010                  _irq2_vector:
000010 60               	pushal
000011 BB 0000 03A4     	movl	#_vecdef+2*VECLINE_SIZEOF,r3	|* Entrada VECDEF IRQ2
000016 EB26             	jmp	int_common_0_7
                        
                        	.align	4
                        	.global	_irq3_vector
000018                  _irq3_vector:
000018 60               	pushal
000019 BB 0000 0404     	movl	#_vecdef+3*VECLINE_SIZEOF,r3	|* Entrada VECDEF IRQ3
00001E EB1E             	jmp	int_common_0_7
                        
                        	.align	4
                        	.global	_irq4_vector
000020                  _irq4_vector:
000020 60               	pushal
000021 BB 0000 0464     	movl	#_vecdef+4*VECLINE_SIZEOF,r3	|* Entrada VECDEF IRQ4
000026 EB16             	jmp	int_common_0_7
                        
                        	.align	4
                        	.global	_irq5_vector
000028                  _irq5_vector:
000028 60               	pushal
000029 BB 0000 04C4     	movl	#_vecdef+5*VECLINE_SIZEOF,r3	|* Entrada VECDEF IRQ5
00002E EB0E             	jmp	int_common_0_7
                        
                        	.align	4
                        	.global	_irq6_vector
000030                  _irq6_vector:
000030 60               	pushal
000031 BB 0000 0524     	movl	#_vecdef+6*VECLINE_SIZEOF,r3	|* Entrada VECDEF IRQ6
000036 EB06             	jmp	int_common_0_7
                        
                        	.align	4
                        	.global	_irq7_vector
000038                  _irq7_vector:
000038 60               	pushal
000039 BB 0000 0584     	movl	#_vecdef+7*VECLINE_SIZEOF,r3	|* Entrada VECDEF IRQ7
                        |*	jmp	int_common_0_7
                        
                        |*
                        |******	Código comum para os IRQs de 0-7 ************************
                        |*
00003E                  int_common_0_7:
                        |***	inb	#IO_NOP,r0
                        
00003E B0 20            	movb	#0x20,r0		|* Dá ACK da interrupção unidade #1
000040 E6 20            	outb	r0,#IO_ICU1
000042 E4 84            	inb	#IO_NOP,r0
                        
000044 EB4C             	jmp	int_common_0_15
                        
                        |*
                        |******	Vetores IRQ8-15 *****************************************
                        |*
                        	.text
                        	.align	4
                        	.global	_irq8_vector
000048                  _irq8_vector:
000048 60               	pushal
000049 BB 0000 05E4     	movl	#_vecdef+8*VECLINE_SIZEOF,r3	|* Entrada VECDEF IRQ8
00004E EB36             	jmp	int_common_8_15
                        
                        	.align	4
                        	.global	_irq9_vector
000050                  _irq9_vector:
000050 60               	pushal
000051 BB 0000 0644     	movl	#_vecdef+9*VECLINE_SIZEOF,r3	|* Entrada VECDEF IRQ9
000056 EB2E             	jmp	int_common_8_15
                        
                        	.align	4
                        	.global	_irq10_vector
000058                  _irq10_vector:
000058 60               	pushal
000059 BB 0000 06A4     	movl	#_vecdef+10*VECLINE_SIZEOF,r3	|* Entrada VECDEF IRQ10
00005E EB26             	jmp	int_common_8_15
                        
                        	.align	4
                        	.global	_irq11_vector
000060                  _irq11_vector:
000060 60               	pushal
000061 BB 0000 0704     	movl	#_vecdef+11*VECLINE_SIZEOF,r3	|* Entrada VECDEF IRQ11
000066 EB1E             	jmp	int_common_8_15
                        
                        	.align	4
                        	.global	_irq12_vector
000068                  _irq12_vector:
000068 60               	pushal
000069 BB 0000 0764     	movl	#_vecdef+12*VECLINE_SIZEOF,r3	|* Entrada VECDEF IRQ12
00006E EB16             	jmp	int_common_8_15
                        
                        	.align	4
                        	.global	_irq13_vector
000070                  _irq13_vector:
000070 60               	pushal
000071 BB 0000 07C4     	movl	#_vecdef+13*VECLINE_SIZEOF,r3	|* Entrada VECDEF IRQ13
000076 EB0E             	jmp	int_common_8_15
                        
                        	.align	4
                        	.global	_irq14_vector
000078                  _irq14_vector:
000078 60               	pushal
000079 BB 0000 0824     	movl	#_vecdef+14*VECLINE_SIZEOF,r3	|* Entrada VECDEF IRQ14
00007E EB06             	jmp	int_common_8_15
                        
                        	.align	4
                        	.global	_irq15_vector
000080                  _irq15_vector:
000080 60               	pushal
000081 BB 0000 0884     	movl	#_vecdef+15*VECLINE_SIZEOF,r3	|* Entrada VECDEF IRQ15
                        |*	jmp	int_common_8_15
                        
                        |*
                        |******	Código comum para os IRQs de 8-15 ***********************
                        |*
000086                  int_common_8_15:
                        |***	inb	#IO_NOP,r0
                        
000086 B0 20            	movb	#0x20,r0		|* Dá ACK da interrupção unidade #1
000088 E6 20            	outb	r0,#IO_ICU1
00008A E4 84            	inb	#IO_NOP,r0
                        
00008C B0 20            	movb	#0x20,r0		|* Dá ACK da interrupção unidade #2
00008E E6 A0            	outb	r0,#IO_ICU2
000090 E4 84            	inb	#IO_NOP,r0
                        
                        |*	jmp	int_common_0_15
                        
                        |*
                        |******	Código comum para todos os IRQs *************************
                        |*
                        |*	r3 = entrada em VECDEFLINE correspondente à IRQ sendo atendida 
                        |*
000092                  int_common_0_15:
000092 1E               	pushl	ds
000093 06               	pushl	es
                        
000094 66 B8 0010       	movw	#KERNEL_DS,r0
000098 8E D8            	movw	r0,ds
00009A 8E C0            	movw	r0,es
                        
                        |***	inb	#IO_NOP,r0
                        
                        |***	movl	.....,r3
00009C 8B 03            	movl	VEC_PL(r3),r0			|* r0 = Prioridade nova
                        
                        .if	INTR_PATTERN
00009E 8B 3485 0000 0000	movl	_intr_pattern_addr(*,r0@l),r4	|* Padrão de interrupção
0000A5 8B 3D 0000 0000  	movl	_intr_videoaddr,r5
0000AB B9 0000 0004     	movl	#INTR_PAT_SZ/4,r1
                        
0000B0 FC               	up
0000B1 F3               	rep
0000B2 A5               	movsl
                        .endif	INTR_PATTERN
                        
0000B3 FF 35 0000 0000  	pushl	_cpl_level		|* PL corrente
0000B9 FF 73 04         	pushl	VEC_UNIT(r3)		|* Unidade
                        
0000BC A3 0000 0000     	movl	r0,_cpl_level		|* Atribui a prioridada nova
0000C1 A3 0000 0000     	movl	r0,_cpl_mask		|* Atribui a máscara nova
0000C6 8B 0485 0000 0000	movl	_pl_mask(*,r0@l),r0	|* r0 = máscara do nível novo
                        
0000CD E6 21               	outb	r0,#IO_ICU1+1		|* Atualiza ICU #1
0000CF E4 84            	inb	#IO_NOP,r0
                        |***	inb	#IO_NOP,r0
                        
0000D1 88 E0            	movb	h0,r0			|* Atualiza ICU #2
0000D3 E6 A1            	outb	r0,#IO_ICU2+1
                        |***	inb	#IO_NOP,r0
                        |***	inb	#IO_NOP,r0
                        
0000D5 6A 01            	pushl	#1			|* Código da interrupção
                        
0000D7 F6 4424 38 03    	testb	#3,14*4(sp)		|* Guarda o "frame" se for de usuário
0000DC 7406             	jeq	8$
0000DE 89 25 E000 000C  	movl	sp,u.u_frame
0000E4                  8$:
0000E4 FB               	enint				|* Habilita as interruções
0000E5                  9$:
0000E5 FF 53 08         	call	VEC_FUNC(r3)		|* Atende a interrupção
                        
0000E8 83 C3 0C         	addl	#VEC_SIZEOF,r3		|* Aponta para a próxima unidade compartilhada
0000EB 83 7B 08 00      	cmpl	#0,VEC_FUNC(r3)
0000EF 7409             	jeq	int_coda
                        
0000F1 8B 43 04         	movl	VEC_UNIT(r3),r0		|* coloca a nova unidade
0000F4 89 4424 04       	movl	r0,4(sp)
                        
0000F8 EBEB             	jmp	9$
                        
                        |*
                        |****** Epílogo da interrupção **********************************
                        |*
0000FA                  int_coda:
0000FA FA               	disint				|* Desabilita as interrupções
                        
                        |*	popl	r3			|* Remove o código
                        |*	popl	r3			|* Remove a unidade
                        |*	popl	r3			|* Obtém a prioridade anterior
0000FB 8B 5C24 08       	movl	8(sp),r3		|* Obtém a prioridade anterior
                        
0000FF 8B 049D 0000 0000	movl	_pl_mask(*,r3@l),r0	|* r0 = máscara do nível anterior
                        
000106 E6 21            	outb	r0,#IO_ICU1+1		|* Atualiza ICU #1
000108 E4 84            	inb	#IO_NOP,r0
                        
00010A 88 E0            	movb	h0,r0			|* Atualiza ICU #2
00010C E6 A1            	outb	r0,#IO_ICU2+1
                        |***	inb	#IO_NOP,r0
                        
00010E 89 1D 0000 0000  	movl	r3,_cpl_level		|* Atribui a prioridada nova
000114 89 1D 0000 0000  	movl	r3,_cpl_mask		|* Atribui a máscara nova
                        
                        .if	INTR_PATTERN
00011A 8B 349D 0000 0000	movl	_intr_pattern_addr(*,r3@l),r4	|* Padrão de interrupção
000121 8B 3D 0000 0000  	movl	_intr_videoaddr,r5
000127 B9 0000 0004     	movl	#INTR_PAT_SZ/4,r1
                        
00012C FC               	up
00012D F3               	rep
00012E A5               	movsl
                        .endif	INTR_PATTERN
                        
00012F 85 DB            	tstl	r3			|* Não troca de processo se SPL > 0
000131 755E             	jne	9$
                        
000133 A1 E000 0008     	movl	u.u_proc,r0		|* Pega endereço da tabela PROC
000138 66 8B 48 08      	movw	proc.p_pri(r0),r1	|* Pega curpri
00013C 66 3B 0D 0000 0000	cmpw	_bestpri,r1		|* Retorna se curpri >= bestpri
000143 7D4C             	jge	9$
                        
                        |*	testb	#3,44(sp)		|* testa se é modo USER
000145 F6 4424 38 03    	testb	#3,14*4(sp)		|* testa se é modo USER
00014A 7526             	jne	1$			|* se for USER troca de processo logo
                        
                        |*	Interrupção foi em modo supervisor
                        
                        |*	movl	#_preemption_flag,r0	|* Se flag == 0 não troca
                        |*	movb	CPUID(r0),r1		|* de processo neste momento
                        |*	movl	#_preemption_mask,r0	|* Se mask == 0 não troca
                        |*	testb	r1,CPUID(r0)		|* de processo em modo supervisor
                        |*	jeq	9$			|* Preemption só quando (flag & mask)
                        
                        	|* Falta o CPUID
                        
00014C 80 3D 0000 0000  	cmpb	#0,_preemption_flag	|* Se flag == 0 não troca
       00
000153 743C             	jeq	9$			|* de processo neste momento
                        
000155 A1 E000 0008     	movl	u.u_proc,r0		|* Não troca para um processo que
00015A 83 38 05         	cmpl	#SRUN,proc.p_state(r0)	|* já não vai mais rodar mesmo
00015D 7532             	jne	9$			|* (estado dele já não é mais SRUN)
                        
00015F 80 3D 0000 0000  	cmpb	#26,_CSW
       1A
000166 750A             	jne	1$
                        
000168 6A 40            	pushl	#'@'			|* Imprime "@" se houve preemtion
00016A E8 FFFF FE91     	call	_putchar		|* em modo supervisor
00016F 83 C4 04         	addl	#4,sp
                        
                        |*	Efetiva a troca do processo corrente
                        
000172                  1$:
000172 FB               	enint				|* Habilita as interrupções
000173 E8 FFFF FE88     	call	_swrunproc		|* Troca de processo
                        
000178 A1 E000 0008     	movl	u.u_proc,r0		|* Pega de novo endereço da tabela PROC
00017D 83 78 04 00      	cmpl	#0,proc.p_sig(r0)	|* Verifica se há sinais pendentes
000181 740E             	jeq	9$			|* Retorna se não há sinais pendentes
                        
                        |*	pushl	40(sp)			|* Põe PC do usuário na stack
000183 FF 7424 34       	pushl	52(sp)			|* Põe PC do usuário na stack
000187 6A 00            	pushl	#0			|* Põe fault address = 0 na stack
000189 E8 FFFF FE72     	call	_sigexec		|* Processa os sinais
00018E 83 C4 08         	addl	#8,sp
                        
000191                  9$:
                        .if 0	|*******************************************************|
                        	pushfl
                        	andl	#0x00004000,(sp)	|* Apara o bit NT
                        	jeq	10$
                        	call	_nt_bit_on		|* Imprime a mensagem
                        10$:
                        	addl	#4,sp			|* Tira o Eflags
                        .endif	|*******************************************************|
                        
000191 80 3D 0000 0000  	cmpb	#0,_mmu_dirty+CPUID	|* Carrega a gerência de memória
       00
000198 7417             	jeq	30$
                        
00019A A1 E000 0008     	movl	u.u_proc,r0
00019F 8B 40 48         	movl	proc.p_mmu_dir(r0),r0
0001A2 2D C000 0000     	subl	#SYS_ADDR,r0
0001A7 0F22 D8          	movl	r0,cr3	
                        
0001AA C6 05 0000 0000  	clrb	_mmu_dirty+CPUID
       00
0001B1                  30$:
0001B1 83 C4 0C         	addl	#12,sp			|* Retira código, máscara e prioridade
0001B4 07               	popl	es
0001B5 1F               	popl	ds
                        
0001B6 61               	popal
                        
0001B7 9C               	pushfl
0001B8 81 2424 FFFF BFFF	andl	#~0x00004000,(sp)	|* Apara o bit NT
0001BF 9D               	popfl
                        
0001C0 CF               	rte
                        
                        |*
                        |******	Vetor da interrupção espúria 32 *************************
                        |*
                        	.global	_spurious_32_vector
                        	.align	4
0001C4                  _spurious_32_vector:
0001C4 6A 10            	pushl	#32-16			|* No. do IRQ
0001C6 EB43             	jmp	_spurious_common
                        
                        |*
                        |******	Vetor da interrupção espúria 33 *************************
                        |*
                        	.global	_spurious_33_vector
                        	.align	4
0001C8                  _spurious_33_vector:
0001C8 6A 11            	pushl	#33-16			|* No. do IRQ
0001CA EB3F             	jmp	_spurious_common
                        
                        |*
                        |******	Vetor da interrupção espúria 34 *************************
                        |*
                        	.global	_spurious_34_vector
                        	.align	4
0001CC                  _spurious_34_vector:
0001CC 6A 12            	pushl	#34-16			|* No. do IRQ
0001CE EB3B             	jmp	_spurious_common
                        
                        |*
                        |******	Vetor da interrupção espúria 35 *************************
                        |*
                        	.global	_spurious_35_vector
                        	.align	4
0001D0                  _spurious_35_vector:
0001D0 6A 13            	pushl	#35-16			|* No. do IRQ
0001D2 EB37             	jmp	_spurious_common
                        
                        |*
                        |******	Vetor da interrupção espúria 36 *************************
                        |*
                        	.global	_spurious_36_vector
                        	.align	4
0001D4                  _spurious_36_vector:
0001D4 6A 14            	pushl	#36-16			|* No. do IRQ
0001D6 EB33             	jmp	_spurious_common
                        
                        |*
                        |******	Vetor da interrupção espúria 37 *************************
                        |*
                        	.global	_spurious_37_vector
                        	.align	4
0001D8                  _spurious_37_vector:
0001D8 6A 15            	pushl	#37-16			|* No. do IRQ
0001DA EB2F             	jmp	_spurious_common
                        
                        |*
                        |******	Vetor da interrupção espúria 38 *************************
                        |*
                        	.global	_spurious_38_vector
                        	.align	4
0001DC                  _spurious_38_vector:
0001DC 6A 16            	pushl	#38-16			|* No. do IRQ
0001DE EB2B             	jmp	_spurious_common
                        
                        |*
                        |******	Vetor da interrupção espúria 39 *************************
                        |*
                        	.global	_spurious_39_vector
                        	.align	4
0001E0                  _spurious_39_vector:
0001E0 6A 17            	pushl	#39-16			|* No. do IRQ
0001E2 EB27             	jmp	_spurious_common
                        
                        |*
                        |******	Vetor da interrupção espúria 40 *************************
                        |*
                        	.global	_spurious_40_vector
                        	.align	4
0001E4                  _spurious_40_vector:
0001E4 6A 18            	pushl	#40-16			|* No. do IRQ
0001E6 EB23             	jmp	_spurious_common
                        
                        |*
                        |******	Vetor da interrupção espúria 41 *************************
                        |*
                        	.global	_spurious_41_vector
                        	.align	4
0001E8                  _spurious_41_vector:
0001E8 6A 19            	pushl	#41-16			|* No. do IRQ
0001EA EB1F             	jmp	_spurious_common
                        
                        |*
                        |******	Vetor da interrupção espúria 42 *************************
                        |*
                        	.global	_spurious_42_vector
                        	.align	4
0001EC                  _spurious_42_vector:
0001EC 6A 1A            	pushl	#42-16			|* No. do IRQ
0001EE EB1B             	jmp	_spurious_common
                        
                        |*
                        |******	Vetor da interrupção espúria 43 *************************
                        |*
                        	.global	_spurious_43_vector
                        	.align	4
0001F0                  _spurious_43_vector:
0001F0 6A 1B            	pushl	#43-16			|* No. do IRQ
0001F2 EB17             	jmp	_spurious_common
                        
                        |*
                        |******	Vetor da interrupção espúria 44 *************************
                        |*
                        	.global	_spurious_44_vector
                        	.align	4
0001F4                  _spurious_44_vector:
0001F4 6A 1C            	pushl	#44-16			|* No. do IRQ
0001F6 EB13             	jmp	_spurious_common
                        
                        |*
                        |******	Vetor da interrupção espúria 45 *************************
                        |*
                        	.global	_spurious_45_vector
                        	.align	4
0001F8                  _spurious_45_vector:
0001F8 6A 1D            	pushl	#45-16			|* No. do IRQ
0001FA EB0F             	jmp	_spurious_common
                        
                        |*
                        |******	Vetor da interrupção espúria 46 *************************
                        |*
                        	.global	_spurious_46_vector
                        	.align	4
0001FC                  _spurious_46_vector:
0001FC 6A 1E            	pushl	#46-16			|* No. do IRQ
0001FE EB0B             	jmp	_spurious_common
                        
                        |*
                        |******	Vetor da interrupção espúria 47 *************************
                        |*
                        	.global	_spurious_47_vector
                        	.align	4
000200                  _spurious_47_vector:
000200 6A 1F            	pushl	#47-16			|* No. do IRQ
000202 EB07             	jmp	_spurious_common
                        
                        |*
                        |******	Vetor das interrupções espúrias 48-255 ******************
                        |*
                        	.global	_spurious_255_vector
                        	.align	4
000204                  _spurious_255_vector:
000204 68 0000 00EF     	pushl	#255-16			|* No. do IRQ
000209 EB00             	jmp	_spurious_common
                        
                        |*
                        |****** Parte comum das	interrupções espúrias *************************
                        |*
       0000 0007        SPURIOUS_PL	= 7
                        
                        	.text
00020B                  _spurious_common:
00020B 8F 05 0000 02E0  	popl	_spurious_unit		|* Guarda o no. do IRQ
                        
000211 60               	pushal
000212 E4 84            	inb	#IO_NOP,r0
                        
000214 B0 03            	movb	#0x03,r0		|* Examina o ISR #1
000216 E6 20            	outb	r0,#IO_ICU1
000218 E4 84            	inb	#IO_NOP,r0
                        
00021A B0 03            	movb	#0x03,r0		|* Examina o ISR #2
00021C E6 A0            	outb	r0,#IO_ICU2
00021E E4 84            	inb	#IO_NOP,r0
                        
000220 E4 20            	inb	#IO_ICU1,r0		|* Obtém o ISR #1
000222 88 C2            	movb	r0,r2
000224 E4 84            	inb	#IO_NOP,r0
                        
000226 B0 02            	movb	#0x02,r0		|* Examina o IRR #1
000228 E6 20            	outb	r0,#IO_ICU1
00022A E4 84            	inb	#IO_NOP,r0
                        
00022C B0 02            	movb	#0x02,r0		|* Examina o IRR #2
00022E E6 A0            	outb	r0,#IO_ICU2
000230 E4 84            	inb	#IO_NOP,r0
                        
000232 E4 A0            	inb	#IO_ICU2,r0		|* Obtém o ISR #2
000234 88 C6            	movb	r0,h2
000236 E4 84            	inb	#IO_NOP,r0
                        
000238 B0 20            	movb	#0x20,r0		|* Dá ACK da interrupção unidade #1
00023A E6 20            	outb	r0,#IO_ICU1
00023C E4 84            	inb	#IO_NOP,r0
                        
00023E B0 20            	movb	#0x20,r0		|* Dá ACK da interrupção unidade #2
000240 E6 A0            	outb	r0,#IO_ICU2
000242 E4 84            	inb	#IO_NOP,r0
                        
000244 1E               	pushl	ds
000245 06               	pushl	es
                        
000246 66 B8 0010       	movw	#KERNEL_DS,r0
00024A 8E D8            	movw	r0,ds
00024C 8E C0            	movw	r0,es
                        
00024E FF 35 0000 0000  	pushl	_cpl_level		|* PL corrente
000254 89 D0            	movl	r2,r0
000256 C1 E0 10         	lsll	#16,r0
000259 66 A1 0000 02E0  	movw	_spurious_unit,r0
00025F 50               	pushl	r0			|* Unidade
                        
000260 C7 05 0000 0000  	movl	#SPURIOUS_PL,_cpl_level	|* Atribui a prioridada nova
       0000 0007
00026A C7 05 0000 0000  	movl	#SPURIOUS_PL,_cpl_mask	|* Atribui a máscara nova
       0000 0007
000274 A1 0000 001C     	movl	_pl_mask+SPURIOUS_PL*4,r0 |* r0 = máscara do nível novo
                        
000279 E6 21            	outb	r0,#IO_ICU1+1		|* Atualiza ICU #1
00027B E4 84            	inb	#IO_NOP,r0
                        
00027D 88 E0            	movb	h0,r0			|* Atualiza ICU #2
00027F E6 A1            	outb	r0,#IO_ICU2+1
000281 E4 84            	inb	#IO_NOP,r0
                        
000283 6A 01            	pushl	#1			|* Código da interrupção
                        
000285 F6 4424 38 03    	testb	#3,14*4(sp)		|* Guarda o "frame" se for de usuário
00028A 7406             	jeq	1$
00028C 89 25 E000 000C  	movl	sp,u.u_frame
000292                  1$:
000292 FB               	enint				|* Habilita as interruções
                        
000293 E8 FFFF FD68     	call	_spurious_int
                        
000298 E9 FFFF FE5D     	jmp	int_coda
                        
                        |*
                        |****** Variáveis ***********************************************
                        |*
                        	.data
                        	.global	_spurious_unit
0002E0                  _spurious_unit:
0002E0 0000 0000        	.long	0



TIPO  ------------SÍMBOLO------------  LOCAL  ROT EXT  --VALOR--

ABS   KBSZ                                             0000 0400
ABS   KBSHIFT                                          0000 000A
ABS   MBSZ                                             0010 0000
ABS   MBSHIFT                                          0000 0014
ABS   GBSZ                                             4000 0000
ABS   PGSZ                                             0000 1000
ABS   PGSHIFT                                          0000 000C
ABS   SYS_ADDR                                         C000 0000
ABS   USER_ADDR                                        E000 0000
ABS   PG_DIR_SHIFT                                     0000 0014
ABS   SPROT                                            0000 0003
ABS   UPROT                                            0000 0007
ABS   USER_TEXT_VA                                     0040 0000
ABS   USER_DATA_VA                                     2000 0000
ABS   USER_PHYS_VA                                     6000 0000
ABS   USER_IPC_VA                                      6800 0000
ABS   USER_STACK_VA                                    A000 0000
ABS   CPUID                                            0000 0000
ABS   USIZE                                            0000 0001
ABS   CTXT_SW_TYPE                                     0000 0010
ABS   NO_USER_MMU                                      0000 0011
ABS   _u                                           E   E000 0000
ABS   u.u_cpu                                          E000 0000
ABS   u.u_pri                                          E000 0002
ABS   u.u_dontpanic                                    E000 0004
ABS   u.u_proc                                         E000 0008
ABS   u.u_frame                                        E000 000C
ABS   proc.p_state                                     0000 0000
ABS   proc.p_sig                                       0000 0004
ABS   proc.p_pri                                       0000 0008
ABS   proc.p_mmu_dir                                   0000 0048
ABS   SRUN                                             0000 0005
ABS   VIDEOADDR                                        C00B 8000
ABS   LINE                                             0000 0018
ABS   COL                                              0000 0050
ABS   KERNEL_CS                                        0000 0008
ABS   KERNEL_DS                                        0000 0010
ABS   USER_CS                                          0000 001B
ABS   USER_DS                                          0000 0023
ABS   KERNEL_TSS                                       0000 0028
ABS   SYS_CALL                                         0000 0033
ABS   VEC_PL                                           0000 0000
ABS   VEC_UNIT                                         0000 0004
ABS   VEC_FUNC                                         0000 0008
ABS   VECLINESZ                                        0000 0008
ABS   VEC_SIZEOF                                       0000 000C
ABS   VECLINE_SIZEOF                                   0000 0060
ABS   IO_NOP                                           0000 0084
ABS   IO_ICU1                                          0000 0020
ABS   IO_ICU2                                          0000 00A0
ABS   IDLE_PATTERN                                     0000 0001
ABS   INTR_PATTERN                                     0000 0001
ABS   CHAR                                             0000 002A
ABS   SPACE                                            0000 0020
ABS   COLOR                                            0000 042A
ABS   W                                                0000 0020
ABS   INTR_PAT_LEN                                     0000 0008
ABS   INTR_PAT_LOC                                     0000 0034
ABS   INTR_PAT_SZ                                      0000 0010
ABS   INTR_PAT_SHIFT                                   0000 0004
ABS   INTR_VIDEOADDR                                   C00B 8F68
BSS   _vecdef                                  R   E   0000 02E4
TEXT  _vecdefaddr                              R   E   0000 02A0
TEXT  _irq0_vector                             R   E   0000 0000
TEXT  _irq1_vector                             R   E   0000 0008
TEXT  _irq2_vector                             R   E   0000 0010
TEXT  _irq3_vector                             R   E   0000 0018
TEXT  _irq4_vector                             R   E   0000 0020
TEXT  _irq5_vector                             R   E   0000 0028
TEXT  _irq6_vector                             R   E   0000 0030
TEXT  _irq7_vector                             R   E   0000 0038
TEXT  _irq8_vector                             R   E   0000 0048
TEXT  _irq9_vector                             R   E   0000 0050
TEXT  _irq10_vector                            R   E   0000 0058
TEXT  _irq11_vector                            R   E   0000 0060
TEXT  _irq12_vector                            R   E   0000 0068
TEXT  _irq13_vector                            R   E   0000 0070
TEXT  _irq14_vector                            R   E   0000 0078
TEXT  _irq15_vector                            R   E   0000 0080
TEXT  int_common_0_7                           R       0000 003E
TEXT  int_common_0_15                          R       0000 0092
TEXT  int_common_8_15                          R       0000 0086
UND   _intr_pattern_addr                           E   0000 0000
UND   _intr_videoaddr                              E   0000 0000
UND   _cpl_level                                   E   0000 0000
UND   _cpl_mask                                    E   0000 0000
UND   _pl_mask                                     E   0000 0000
TEXT  (025),$8                           L     R       0000 00E4
TEXT  (025),$9                           L     R       0000 00E5
TEXT  int_coda                                 R       0000 00FA
TEXT  (026),$9                           L     R       0000 0191
UND   _bestpri                                     E   0000 0000
TEXT  (026),$1                           L     R       0000 0172
UND   _preemption_flag                             E   0000 0000
UND   _CSW                                         E   0000 0000
UND   _putchar                                     E   0000 0000
UND   _swrunproc                                   E   0000 0000
UND   _sigexec                                     E   0000 0000
UND   _mmu_dirty                                   E   0000 0000
TEXT  (026),$30                          L     R       0000 01B1
TEXT  _spurious_32_vector                      R   E   0000 01C4
TEXT  _spurious_common                         R       0000 020B
TEXT  _spurious_33_vector                      R   E   0000 01C8
TEXT  _spurious_34_vector                      R   E   0000 01CC
TEXT  _spurious_35_vector                      R   E   0000 01D0
TEXT  _spurious_36_vector                      R   E   0000 01D4
TEXT  _spurious_37_vector                      R   E   0000 01D8
TEXT  _spurious_38_vector                      R   E   0000 01DC
TEXT  _spurious_39_vector                      R   E   0000 01E0
TEXT  _spurious_40_vector                      R   E   0000 01E4
TEXT  _spurious_41_vector                      R   E   0000 01E8
TEXT  _spurious_42_vector                      R   E   0000 01EC
TEXT  _spurious_43_vector                      R   E   0000 01F0
TEXT  _spurious_44_vector                      R   E   0000 01F4
TEXT  _spurious_45_vector                      R   E   0000 01F8
TEXT  _spurious_46_vector                      R   E   0000 01FC
TEXT  _spurious_47_vector                      R   E   0000 0200
TEXT  _spurious_255_vector                     R   E   0000 0204
ABS   SPURIOUS_PL                                      0000 0007
DATA  _spurious_unit                           R   E   0000 02E0
TEXT  (045),$1                           L     R       0000 0292
UND   _spurious_int                                E   0000 0000
