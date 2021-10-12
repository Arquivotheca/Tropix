	.global	_b
	.const
	.align	4
_b:
	.double	4.000000000000000E+000
	.global	_a
	.data
	.align	4
_a:
	.double	3.000000000000000E+000
	.const
LC0:
	.isoz	"double = %e\n"
	.text
	.global	_main
_main:
	link	#0
	pushl	#1074003968
	pushl	#0
	pushl	#LC0
	call	_printf
	unlk
	rts
