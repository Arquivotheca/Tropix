|*
|****************************************************************
|*								*
|*			port.s					*
|*								*
|*	Interface para ler/escrever de uma porta		*
|*								*
|*	Versão	3.0.0, de 26.06.94				*
|*		4.0.0, de 15.01.01				*
|*								*
|*	Módulo: Boot2						*
|*		NÚCLEO do TROPIX para PC			*
|*								*
|*	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
|*		Copyright © 1997 NCE/UFRJ - tecle "man licença"	*
|*								*
|****************************************************************
|*

	.seg32

IO_NOP	= 0x84			|* Dispositivo inexistente para NOP

|*
|****************************************************************
|*	Lê um byte de uma porta					*
|****************************************************************
|*
|*	int
|*	read_port (int portno);
|*
	.text
	.global	_read_port

_read_port:
	inb	#IO_NOP,r0	|* Espera um pouco
	inb	#IO_NOP,r0

	movl	4(sp),r2	|* No. do port

	clrl	r0
	inb	r2,r0		|* Lê o byte

	rts

|*
|****************************************************************
|*	Escreve um byte em uma porta				*
|****************************************************************
|*
|*	void
|*	write_port (int val, int portno);
|*
	.global	_write_port

_write_port:
	inb	#IO_NOP,r0	|* Espera um pouco
	inb	#IO_NOP,r0

	movl	4(sp),r0	|* Valor
	movl	8(sp),r2	|* No. do port

	outb	r0,r2		|* Escreve o byte

	rts

|*
|****************************************************************
|*	Lê um "word" de uma porta				*
|****************************************************************
|*
|*	int
|*	read_port_short (int portno);
|*
	.global	_read_port_short
_read_port_short:
	inb	#IO_NOP,r0	|* Espera um pouco
	inb	#IO_NOP,r0

	movl	4(sp),r2	|* No. do port

	clrl	r0
	inw	r2,r0		|* Lê o short

	rts

|*
|****************************************************************
|*	Escreve um "word" em uma porta				*
|****************************************************************
|*
|*	void
|*	write_port_short (int val, int portno);
|*
	.global	_write_port_short
_write_port_short:
	inb	#IO_NOP,r0	|* Espera um pouco
	inb	#IO_NOP,r0

	movl	4(sp),r0	|* Valor
	movl	8(sp),r2	|* No. do port

	outw	r0,r2		|* Escreve o short

	rts

|*
|****************************************************************
|*	Lê um "long" de uma porta				*
|****************************************************************
|*
|*	long
|*	read_port_long (int portno);
|*
	.global	_read_port_long
_read_port_long:
|*	inb	#IO_NOP,r0	|* Espera um pouco
|*	inb	#IO_NOP,r0

	movl	4(sp),r2	|* No. do port

	inl	r2,r0		|* Lê o long

	rts

|*
|****************************************************************
|*	Escreve um "long" em uma porta				*
|****************************************************************
|*
|*	void
|*	write_port_long (long val, int portno);
|*
	.global	_write_port_long
_write_port_long:
|*	inb	#IO_NOP,r0	|* Espera um pouco
|*	inb	#IO_NOP,r0

	movl	4(sp),r0	|* Valor
	movl	8(sp),r2	|* No. do port

	outl	r0,r2		|* Escreve o long

	rts

|*
|****************************************************************
|*	Lê um byte do CMOS 					*
|****************************************************************
|*
|*	int
|*	read_cmos (int addr);
|*
	.text
	.global	_read_cmos

_read_cmos:
	movl	4(sp),r0	|* Endereço
	outb	r0,#0x70

	clrl	r0
	inb	#0x71,r0	|* Lê o byte

	rts

|*
|****************************************************************
|*	Escreve uma seqüência de bytes em uma porta		*
|****************************************************************
|*
|*	void
|*	write_port_string_byte (int portno, void *area, int byte_count);
|*
	.global	_write_port_string_byte
_write_port_string_byte:
	pushl	r4

	movl	8(sp),r2	|* No. do port
	movl	12(sp),r4	|* Area
	movl	16(sp),r1	|* Short_count

	up

	rep
	outsb

	outb	r0,#0x80	|* Delay (?!!)

	popl	r4
	rts

|*
|****************************************************************
|*	Lê uma seqüência de "words" de uma porta		*
|****************************************************************
|*
|*	void
|*	read_port_string_short (int portno, void *area, int short_count);
|*
	.global	_read_port_string_short
_read_port_string_short:
	pushl	r5

	movl	8(sp),r2	|* No. do port
	movl	12(sp),r5	|* Area
	movl	16(sp),r1	|* Short_count

	up

	rep
	insw

	outb	r0,#0x80	|* Delay (?!!)

	popl	r5
	rts

|*
|****************************************************************
|*	Escreve uma seqüência de "words" em uma porta		*
|****************************************************************
|*
|*	void
|*	write_port_string_short (int portno, void *area, int short_count);
|*
	.global	_write_port_string_short
_write_port_string_short:
	pushl	r4

	movl	8(sp),r2	|* No. do port
	movl	12(sp),r4	|* Area
	movl	16(sp),r1	|* Short_count

	up

	rep
	outsw

	outb	r0,#0x80	|* Delay (?!!)

	popl	r4
	rts

|*
|****************************************************************
|*	Lê uma seqüência de "longs" de uma porta		*
|****************************************************************
|*
|*	void
|*	read_port_string_long (int portno, void *area, int long_count);
|*
	.global	_read_port_string_long
_read_port_string_long:
	pushl	r5

	movl	8(sp),r2	|* No. do port
	movl	12(sp),r5	|* Area
	movl	16(sp),r1	|* Long_count

	up

	rep
	insl

	outb	r0,#0x80	|* Delay (?!!)

	popl	r5
	rts

.if 0	|*******************************************************|
|*
|****************************************************************
|*	Escreve uma seqüência de "longs" em uma porta		*
|****************************************************************
|*
|*	void
|*	write_port_string_long (int portno, void *area, int long_count);
|*
	.global	_write_port_string_long
_write_port_string_long:
	pushl	r4

	movl	8(sp),r2	|* No. do port
	movl	12(sp),r4	|* Area
	movl	16(sp),r1	|* Long_count

	up

	rep
	outsl

	popl	r4
	rts
.endif	|*******************************************************|
