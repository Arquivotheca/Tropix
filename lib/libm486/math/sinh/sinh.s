	.const
	.align	4
LC0:
	.double	1.000000000000000E+000
	.align	4
LC1:
	.double	2.000000000000000E+000
	.text
	.global	_sinh
_sinh:
	link	#16
	pushl	12(fp)
	pushl	8(fp)
	call	_exp
	fstpd	-16(fp)
	fldd	LC0
	fdivd	-16(fp)
	fstpd	-8(fp)
	fldd	-16(fp)
	fsubd	-8(fp)
	fdivd	LC1
	unlk
	rts
	.const
	.align	4
LC2:
	.double	1.000000000000000E+000
	.align	4
LC3:
	.double	2.000000000000000E+000
	.text
	.global	_cosh
_cosh:
	link	#16
	pushl	12(fp)
	pushl	8(fp)
	call	_exp
	fstpd	-16(fp)
	fldd	LC2
	fdivd	-16(fp)
	fstpd	-8(fp)
	fldd	-16(fp)
	faddd	-8(fp)
	fdivd	LC3
	unlk
	rts
	.const
	.align	4
LC4:
	.double	2.000000000000000E+000
	.align	4
LC5:
	.double	1.000000000000000E+000
	.text
	.global	_tanh
_tanh:
	link	#0
	fldd	LC4
	fmuld	8(fp)
	subl	#8,sp
	fstpd	(sp)
	call	_exp
	faddd	LC5
	fdivrd	LC4
	fsubrd	LC5
	unlk
	rts
