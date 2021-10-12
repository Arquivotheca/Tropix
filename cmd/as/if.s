	alpha	= 123
	beta	= 0
	delta	= -100

|****************************************************************

.if	alpha
	pushl	#1
.else
	.warn	"ERRO 1"
.endif

|****************************************************************

.if	beta == 0
	pushl	#2
.else
	.warn	"ERRO 2"
.endif

|****************************************************************

.ifdef	alpha
	pushl	#3
.else
	.warn	"ERRO 3"
.endif

|****************************************************************

.ifndef	gamma
	pushl	#4
.else
	.warn	"ERRO 4"
.endif

|****************************************************************

.if	alpha == 0
	.warn	"ERRO 5"
.elif	beta != 0
	.warn	"ERRO 6"
.elndef	alpha
	.warn	"ERRO 7"
.eldef	gamma
	.warn	"ERRO 8"
.else
	pushl	#5
.endif

|****************************************************************

.if	alpha != 0
.if	beta == 0
.ifdef	alpha
.ifdef	beta
.ifndef	gamma
	pushl	#6
.endif
.endif
.endif
.endif
.endif

|****************************************************************

.if	alpha != 0
.if	beta == 0
.ifndef	alpha
.ifdef	beta
.ifndef	gamma
	.warn	"ERRO 9"
.endif
.endif
.endif
.endif
.endif

|****************************************************************

.if	alpha == 123
	pushl	#7
.else
	.warn	"ERRO 10"
.endif

|****************************************************************

.if	alpha != 345
	pushl	#8
.else
	.warn	"ERRO 11"
.endif

|****************************************************************

.if	alpha > 100
	pushl	#9
.else
	.warn	"ERRO 12"
.endif

|****************************************************************

.if	alpha >= -10
	pushl	#10
.else
	.warn	"ERRO 13"
.endif

|****************************************************************

.if	alpha < 200
	pushl	#11
.else
	.warn	"ERRO 14"
.endif

|****************************************************************

.if	alpha < 1000
	pushl	#12
.else
	.warn	"ERRO 15"
.endif

|****************************************************************

.if	alpha != 123
	.warn	"ERRO 16"
.else
	pushl	#13
.endif

|****************************************************************

.if	alpha == 345
	.warn	"ERRO 17"
.else
	pushl	#14
.endif

|****************************************************************

.if	alpha <= 100
	.warn	"ERRO 18"
.else
	pushl	#15
.endif

|****************************************************************

.if	alpha < -10
	.warn	"ERRO 19"
.else
	pushl	#16
.endif

|****************************************************************

.if	alpha >= 200
	.warn	"ERRO 20"
.else
	pushl	#17
.endif

|****************************************************************

.if	alpha > 1000
	.warn	"ERRO 21"
.else
	pushl	#18
.endif

|****************************************************************

.if	alpha > 120 && beta == 0
	pushl	#19
.else
	.warn	"ERRO 22"
.endif

.if	alpha > 120 && beta == 1
	.warn	"ERRO 23"
.else
	pushl	#20
.endif

.if	alpha > 1000 && beta == 0
	.warn	"ERRO 24"
.else
	pushl	#21
.endif

.if	alpha > 1000 && beta == 3
	.warn	"ERRO 25"
.else
	pushl	#22
.endif

|****************************************************************

.if	alpha > 120 || beta == 0
	pushl	#23
.else
	.warn	"ERRO 26"
.endif

.if	alpha > 120 || beta == 1
	pushl	#24
.else
	.warn	"ERRO 27"
.endif

.if	alpha > 1000 || beta == 0
	pushl	#25
.else
	.warn	"ERRO 28"
.endif

.if	alpha > 1000 || beta == 3
	.warn	"ERRO 29"
.else
	pushl	#26
.endif

|****************************************************************

.if	alpha && beta
	.warn	"ERRO 30"
.else
	pushl	#27
.endif

.if	alpha && !beta
	pushl	#28
.else
	.warn	"ERRO 31"
.endif

|****************************************************************

a0	= 1 | 3
a1	= 1 & 3
a2	= 1 ^ 3
a3	= 0x0000000F << 12
a4	= 0xF0000000 >> 12
a5	= 5 + 2
a6	= 5 - 2
a7	= 5 * 2
a8	= 5 / 2
a9	= 5 % 2
a10	= ~5
a11	= !5
a12	= !0
a13	= -5

|****************************************************************

rot:			|* Teste de defasagem
	pushl	#29
