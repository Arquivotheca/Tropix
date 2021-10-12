	.global	_pgversion
	.const
_pgversion:
	.isoz	"Versão:\t3.0.0, de 13.12.93"
	.global	_exitcode
	.data
	.align	4
_exitcode:
	.long	0
	.global	_many_files
	.align	4
_many_files:
	.long	0
	.global	_dirptr
	.align	4
_dirptr:
	.long	0
	.const
LC0:
	.isoz	"rdbcfstaRNMH"
LC1:
	.isoz	"%s: Os argumentos supérfluos serão ignorados\n"
LC2:
	.isoz	"."
LC3:
	.isoz	"\n"
LC4:
	.isoz	"\"%s\":\n"
LC5:
	.isoz	"Total: %D\n"
	.text
	.global	_main
_main:
	link	#512
	pushl	r5
	pushl	r4
	pushl	r3
	movl	8(fp),r5
	movl	12(fp),r3
	clrl	r4
	movl	(r3),r2
	movl	r2,_pgname
	jmp	L2
L19:
	addl	#-72,r0
	cmpl	#44,r0
	jhi	L17
	jmp	L18(*,r0@l)
	.const
L18:
	.long	L16
	.long	L17
	.long	L17
	.long	L17
	.long	L17
	.long	L15
	.long	L14
	.long	L17
	.long	L17
	.long	L17
	.long	L13
	.long	L17
	.long	L17
	.long	L17
	.long	L17
	.long	L17
	.long	L17
	.long	L17
	.long	L17
	.long	L17
	.long	L17
	.long	L17
	.long	L17
	.long	L17
	.long	L17
	.long	L12
	.long	L7
	.long	L8
	.long	L6
	.long	L17
	.long	L9
	.long	L17
	.long	L17
	.long	L17
	.long	L17
	.long	L17
	.long	L17
	.long	L17
	.long	L17
	.long	L17
	.long	L17
	.long	L17
	.long	L5
	.long	L10
	.long	L11
	.text
L5:
	incl	_rflag
	jmp	L2
L6:
	incl	_dflag
	jmp	L2
L7:
	incl	_bflag
	jmp	L2
L8:
	incl	_cflag
	jmp	L2
L9:
	incl	_fflag
	jmp	L2
L10:
	incl	_sflag
	jmp	L2
L11:
	incl	_tflag
	jmp	L2
L12:
	incl	_aflag
	jmp	L2
L13:
	incl	_Rflag
	jmp	L2
L14:
	incl	_Nflag
	jmp	L2
L15:
	pushl	#0
	call	_exit
	addl	#4,sp
L16:
	call	_help
L17:
	pushl	#__ioblk+48
	pushl	#10
	call	_fputc
	call	_help
	addl	#8,sp
L2:
	pushl	#LC0
	pushl	r3
	pushl	r5
	call	_getopt
	addl	#12,sp
	cmpl	#-1,r0
	jne	L19
	movl	_optind,r0
	asll	#2,r0
	leal	(r0,r3),r3
	subl	_optind,r5
	movl	_rflag,r0
	addl	_dflag,r0
	addl	_bflag,r0
	addl	_cflag,r0
	addl	_fflag,r0
	jne	L20
	incl	_rflag
	incl	_dflag
	incl	_bflag
	incl	_cflag
	incl	_fflag
L20:
	movl	_aflag,r0
	addl	_Rflag,r0
	cmpl	#1,r0
	jle	L21
	call	_help
L21:
	cmpl	#0,_Nflag
	jeq	L22
	cmpl	#0,(r3)
	jeq	L22
	pushl	_pgname
	pushl	#LC1
	pushl	#__ioblk+48
	call	_fprintf
	addl	#12,sp
L22:
	cmpl	#0,_Nflag
	jeq	L23
	leal	-512(fp),r3
	jmp	L24
L26:
	pushl	r3
	call	_strdup
	pushl	r0
	call	_proc_file
	addl	#8,sp
L24:
	pushl	r3
	call	_gets
	addl	#4,sp
	cmpl	r0,r3
	jeq	L26
	jmp	L27
L23:
	cmpl	#0,(r3)
	jne	L28
	pushl	#LC2
	call	_proc_file
	addl	#4,sp
	jmp	L27
L28:
	cmpl	#1,r5
	jle	L46
	incl	_many_files
	jmp	L46
L34:
	pushl	(r3)
	call	_proc_file
	addl	#4,sp
	addl	#4,r3
L46:
	cmpl	#0,(r3)
	jne	L34
L27:
	cmpl	#1,r5
	jgt	L36
	cmpl	#0,_Nflag
	jne	L36
	cmpl	#0,_Rflag
	jeq	L35
L36:
	movl	#1,r4
L35:
	movl	_dirlist+44,r2
	jmp	L47
L43:
	tstl	r4
	jeq	L40
	movl	r4,r0
	incl	r4
	cmpl	#1,r0
	jle	L41
	pushl	#LC3
	call	_printf
	addl	#4,sp
L41:
	movl	_dirptr,r0
	pushl	(r0)
	pushl	#LC4
	call	_printf
	addl	#8,sp
L40:
	pushl	_dirptr
	call	_dir_read
	addl	#4,sp
	cmpl	#0,_sflag
	jeq	L42
	pushl	r0
	pushl	#LC5
	call	_printf
	addl	#8,sp
L42:
	pushl	r4
	pushl	#_filelist
	call	_show_list
	addl	#8,sp
	movl	_dirptr,r0
	movl	44(r0),r2
L47:
	movl	r2,_dirptr
	tstl	r2
	jne	L43
	pushl	_exitcode
	call	_exit
	leal	-524(fp),sp
	popl	r3
	popl	r4
	popl	r5
	unlk
	rts
	.const
LC6:
	.isoz	"%s: Memória esgotada\n"
LC7:
	.isoz	"%s: Não consegui obter o estado de \"%s\" (%s)\n"
LC8:
	.isoz	"%s: \"%s\" não é um diretório\n"
	.text
	.global	_proc_file
_proc_file:
	link	#0
	pushl	r4
	pushl	r3
	movl	8(fp),r4
	pushl	#48
	call	_malloc
	movl	r0,r3
	addl	#4,sp
	tstl	r3
	jne	L49
	pushl	_pgname
	pushl	#LC6
	pushl	#__ioblk+48
	call	_fprintf
	pushl	#1
	call	_exit
	addl	#16,sp
L49:
	leal	4(r3),r0
	pushl	r0
	pushl	r4
	call	_stat
	addl	#8,sp
	tstl	r0
	jge	L50
	pushl	_errno
	call	_strerror
	pushl	r0
	pushl	r4
	pushl	_pgname
	pushl	#LC7
	pushl	#__ioblk+48
	call	_fprintf
	jmp	L53
L50:
	movl	r4,(r3)
	movl	8(r3),r0
	andl	#61440,r0
	cmpl	#16384,r0
	jne	L51
	pushl	r3
	pushl	#_dirlist
	call	_list_insert
	jmp	L48
L51:
	cmpl	#0,_many_files
	jne	L53
	pushl	r4
	pushl	_pgname
	pushl	#LC8
	pushl	#__ioblk+48
	call	_fprintf
	addl	#16,sp
L53:
	pushl	r3
	call	_free
	incl	_exitcode
L48:
	leal	-8(fp),sp
	popl	r3
	popl	r4
	unlk
	rts
	.global	_list_insert
_list_insert:
	link	#0
	pushl	r5
	pushl	r4
	pushl	r3
	movl	8(fp),r0
	movl	12(fp),r5
	cmpl	#_dirlist,r0
	jne	L55
	movl	_dirptr,r3
	tstl	r3
	jne	L56
	movl	r0,r3
L56:
	movl	(r5),r4
	cmpl	#0,44(r3)
	jeq	L62
L61:
	movl	44(r3),r0
	pushl	(r0)
	pushl	r4
	call	_strcmp
	addl	#8,sp
	tstl	r0
	jle	L62
	movl	44(r3),r3
	cmpl	#0,44(r3)
	jne	L61
	jmp	L62
L55:
	cmpl	#0,_tflag
	jeq	L63
	movl	28(r5),r2
	movl	r0,r3
	cmpl	#0,44(r3)
	jeq	L62
L68:
	movl	44(r3),r0
	cmpl	28(r0),r2
	jge	L62
	movl	r0,r3
	cmpl	#0,44(r3)
	jne	L68
	jmp	L62
L63:
	movl	(r5),r4
	movl	r0,r3
	cmpl	#0,44(r3)
	jeq	L62
L74:
	movl	44(r3),r0
	pushl	(r0)
	pushl	r4
	call	_strcmp
	addl	#8,sp
	tstl	r0
	jle	L62
	movl	44(r3),r3
	cmpl	#0,44(r3)
	jne	L74
L62:
	movl	44(r3),r1
	movl	r1,44(r5)
	movl	r5,44(r3)
	leal	-12(fp),sp
	popl	r3
	popl	r4
	popl	r5
	unlk
	rts
	.const
LC9:
	.isoz	"  "
LC10:
	.isoz	""
LC11:
	.isoz	"%sDiretórios:"
LC12:
	.isoz	"\n%s "
LC13:
	.isoz	" %-14.14s"
LC14:
	.isoz	"%sArquivos regulares:"
LC15:
	.isoz	"%sEspecial de blocos:"
LC16:
	.isoz	"%sEspecial de caracteres:"
LC17:
	.isoz	"%sFifos:"
	.text
	.global	_show_list
_show_list:
	link	#4
	pushl	r5
	pushl	r4
	pushl	r3
	clrl	r3
	movl	#0,-4(fp)
	cmpl	#0,12(fp)
	jeq	L79
	movl	#LC9,r5
	jmp	L80
L79:
	movl	#LC10,r5
L80:
	cmpl	#0,_dflag
	jeq	L81
	movl	8(fp),r1
	movl	44(r1),r4
	tstl	r4
	jeq	L81
L88:
	movl	8(r4),r0
	andl	#61440,r0
	cmpl	#16384,r0
	jne	L84
	movl	r3,r0
	movl	#5,r1
	extlq
	divsl	r1
	tstl	r2
	jne	L86
	tstl	r3
	jne	L87
	pushl	r5
	pushl	#LC11
	call	_printf
	addl	#8,sp
L87:
	pushl	r5
	pushl	#LC12
	call	_printf
	addl	#8,sp
L86:
	pushl	(r4)
	call	_last_id
	pushl	r0
	pushl	#LC13
	call	_printf
	incl	r3
	addl	#12,sp
L84:
	movl	44(r4),r4
	tstl	r4
	jne	L88
L81:
	tstl	r3
	jle	L89
	pushl	#LC3
	call	_printf
	incl	-4(fp)
	clrl	r3
	addl	#4,sp
L89:
	cmpl	#0,_rflag
	jeq	L90
	movl	8(fp),r1
	movl	44(r1),r4
	tstl	r4
	jeq	L90
L98:
	movl	8(r4),r0
	andl	#61440,r0
	cmpl	#32768,r0
	jne	L93
	movl	r3,r0
	movl	#5,r1
	extlq
	divsl	r1
	tstl	r2
	jne	L95
	tstl	r3
	jne	L96
	cmpl	#0,-4(fp)
	jeq	L97
	pushl	#LC3
	call	_printf
	addl	#4,sp
L97:
	pushl	r5
	pushl	#LC14
	call	_printf
	addl	#8,sp
L96:
	pushl	r5
	pushl	#LC12
	call	_printf
	addl	#8,sp
L95:
	pushl	(r4)
	call	_last_id
	pushl	r0
	pushl	#LC13
	call	_printf
	incl	r3
	addl	#12,sp
L93:
	movl	44(r4),r4
	tstl	r4
	jne	L98
L90:
	tstl	r3
	jle	L99
	pushl	#LC3
	call	_printf
	incl	-4(fp)
	clrl	r3
	addl	#4,sp
L99:
	cmpl	#0,_bflag
	jeq	L100
	movl	8(fp),r1
	movl	44(r1),r4
	tstl	r4
	jeq	L100
L108:
	movl	8(r4),r0
	andl	#61440,r0
	cmpl	#24576,r0
	jne	L103
	movl	r3,r0
	movl	#5,r1
	extlq
	divsl	r1
	tstl	r2
	jne	L105
	tstl	r3
	jne	L106
	cmpl	#0,-4(fp)
	jeq	L107
	pushl	#LC3
	call	_printf
	addl	#4,sp
L107:
	pushl	r5
	pushl	#LC15
	call	_printf
	addl	#8,sp
L106:
	pushl	r5
	pushl	#LC12
	call	_printf
	addl	#8,sp
L105:
	pushl	(r4)
	call	_last_id
	pushl	r0
	pushl	#LC13
	call	_printf
	incl	r3
	addl	#12,sp
L103:
	movl	44(r4),r4
	tstl	r4
	jne	L108
L100:
	tstl	r3
	jle	L109
	pushl	#LC3
	call	_printf
	incl	-4(fp)
	clrl	r3
	addl	#4,sp
L109:
	cmpl	#0,_cflag
	jeq	L110
	movl	8(fp),r1
	movl	44(r1),r4
	tstl	r4
	jeq	L110
L118:
	movl	8(r4),r0
	andl	#61440,r0
	cmpl	#8192,r0
	jne	L113
	movl	r3,r0
	movl	#5,r1
	extlq
	divsl	r1
	tstl	r2
	jne	L115
	tstl	r3
	jne	L116
	cmpl	#0,-4(fp)
	jeq	L117
	pushl	#LC3
	call	_printf
	addl	#4,sp
L117:
	pushl	r5
	pushl	#LC16
	call	_printf
	addl	#8,sp
L116:
	pushl	r5
	pushl	#LC12
	call	_printf
	addl	#8,sp
L115:
	pushl	(r4)
	call	_last_id
	pushl	r0
	pushl	#LC13
	call	_printf
	incl	r3
	addl	#12,sp
L113:
	movl	44(r4),r4
	tstl	r4
	jne	L118
L110:
	tstl	r3
	jle	L119
	pushl	#LC3
	call	_printf
	incl	-4(fp)
	clrl	r3
	addl	#4,sp
L119:
	cmpl	#0,_fflag
	jeq	L120
	movl	8(fp),r1
	movl	44(r1),r4
	tstl	r4
	jeq	L120
L128:
	movl	8(r4),r0
	andl	#61440,r0
	cmpl	#4096,r0
	jne	L123
	movl	r3,r0
	movl	#5,r1
	extlq
	divsl	r1
	tstl	r2
	jne	L125
	tstl	r3
	jne	L126
	cmpl	#0,-4(fp)
	jeq	L127
	pushl	#LC3
	call	_printf
	addl	#4,sp
L127:
	pushl	r5
	pushl	#LC17
	call	_printf
	addl	#8,sp
L126:
	pushl	r5
	pushl	#LC12
	call	_printf
	addl	#8,sp
L125:
	pushl	(r4)
	call	_last_id
	pushl	r0
	pushl	#LC13
	call	_printf
	incl	r3
	addl	#12,sp
L123:
	movl	44(r4),r4
	tstl	r4
	jne	L128
L120:
	tstl	r3
	jle	L129
	pushl	#LC3
	call	_printf
	addl	#4,sp
L129:
	movl	8(fp),r1
	movl	44(r1),r4
	tstl	r4
	jeq	L134
L133:
	movl	44(r4),r3
	pushl	r4
	call	_free
	addl	#4,sp
	movl	r3,r4
	tstl	r4
	jne	L133
L134:
	movl	8(fp),r1
	movl	#0,44(r1)
	leal	-16(fp),sp
	popl	r3
	popl	r4
	popl	r5
	unlk
	rts
	.const
LC18:
	.isoz	"%s: Não consegui abrir \"%s\" (%s)\n"
LC19:
	.isoz	"%s: Erro na leitura do diretório \"%s\" (%s)\n"
LC20:
	.isoz	".."
	.text
	.global	_dir_read
_dir_read:
	link	#544
	pushl	r5
	pushl	r4
	pushl	r3
	movl	#0,-540(fp)
	movl	8(fp),r2
	movzwl	6(r2),r0
	pushl	r0
	movswl	4(r2),r0
	pushl	r0
	call	_inopen
	movl	r0,-536(fp)
	addl	#8,sp
	tstl	r0
	jge	L141
	pushl	_errno
	call	_strerror
	pushl	r0
	movl	8(fp),r2
	pushl	(r2)
	pushl	_pgname
	pushl	#LC18
	pushl	#__ioblk+48
	call	_fprintf
	clrl	r0
	jmp	L140
L141:
	movl	#0,-532(fp)
	clrl	r4
	leal	-16(fp),r2
	movl	r2,-544(fp)
L142:
	cmpl	-532(fp),r4
	jlo	L145
	pushl	#512
	leal	-528(fp),r0
	pushl	r0
	pushl	-536(fp)
	call	_read
	movl	r0,r3
	addl	#12,sp
	tstl	r3
	jge	L146
	pushl	_errno
	call	_strerror
	pushl	r0
	movl	8(fp),r2
	pushl	(r2)
	pushl	_pgname
	pushl	#LC19
	pushl	#__ioblk+48
	call	_fprintf
	addl	#24,sp
L146:
	tstl	r3
	jle	L143
	leal	-528(fp),r4
	leal	(r3,r4),r2
	movl	r2,-532(fp)
L145:
	cmpw	#0,(r4)
	jeq	L144
	leal	2(r4),r0
	pushl	r0
	pushl	-544(fp)
	call	_idcan
	addl	#8,sp
	cmpl	#0,_aflag
	jne	L149
	pushl	#LC2
	pushl	-544(fp)
	call	_streq
	addl	#8,sp
	tstl	r0
	jne	L144
	pushl	#LC20
	pushl	-544(fp)
	call	_streq
	addl	#8,sp
	tstl	r0
	jne	L144
L149:
	pushl	#48
	call	_malloc
	movl	r0,r3
	addl	#4,sp
	tstl	r3
	jne	L151
	pushl	_pgname
	pushl	#LC6
	pushl	#__ioblk+48
	call	_fprintf
	pushl	#1
	call	_exit
	addl	#16,sp
L151:
	pushl	-544(fp)
	movl	8(fp),r2
	pushl	(r2)
	call	_compose_name
	movl	r0,r5
	leal	4(r3),r0
	pushl	r0
	movzwl	(r4),r0
	pushl	r0
	movl	8(fp),r2
	movswl	4(r2),r0
	pushl	r0
	call	_instat
	addl	#20,sp
	tstl	r0
	jge	L152
	pushl	_errno
	call	_strerror
	pushl	r0
	pushl	r5
	pushl	_pgname
	pushl	#LC7
	pushl	#__ioblk+48
	call	_fprintf
	pushl	r3
	call	_free
	incl	_exitcode
	addl	#28,sp
	jmp	L144
L152:
	movl	r5,(r3)
	pushl	r3
	pushl	#_filelist
	call	_list_insert
	addl	#8,sp
	cmpl	#0,_Rflag
	jeq	L153
	movl	8(r3),r0
	andl	#61440,r0
	cmpl	#16384,r0
	jne	L153
	pushl	r3
	call	_infodup
	pushl	r0
	pushl	#_dirlist
	call	_list_insert
	addl	#12,sp
L153:
	movl	20(r3),r0
	addl	#511,r0
	asrl	#9,r0
	movl	-540(fp),r2
	leal	(r0,r2),r1
	movl	r1,-540(fp)
L144:
	addl	#16,r4
	jmp	L142
L143:
	pushl	-536(fp)
	call	_close
	movl	-540(fp),r0
L140:
	leal	-556(fp),sp
	popl	r3
	popl	r4
	popl	r5
	unlk
	rts
	.global	_infodup
_infodup:
	link	#0
	pushl	r4
	pushl	r3
	movl	8(fp),r4
	pushl	#48
	call	_malloc
	movl	r0,r3
	addl	#4,sp
	tstl	r3
	jne	L155
	pushl	_pgname
	pushl	#LC6
	pushl	#__ioblk+48
	call	_fprintf
	pushl	#1
	call	_exit
L155:
	movl	(r4),r2
	movl	r2,(r3)
	movl	4(r4),r2
	movl	r2,4(r3)
	movl	8(r4),r2
	movl	r2,8(r3)
	movl	12(r4),r2
	movl	r2,12(r3)
	movl	16(r4),r2
	movl	r2,16(r3)
	movl	20(r4),r2
	movl	r2,20(r3)
	movl	24(r4),r2
	movl	r2,24(r3)
	movl	28(r4),r2
	movl	r2,28(r3)
	movl	32(r4),r2
	movl	r2,32(r3)
	movl	36(r4),r2
	movl	r2,36(r3)
	movl	40(r4),r2
	movl	r2,40(r3)
	movl	44(r4),r2
	movl	r2,44(r3)
	movl	r3,r0
	leal	-8(fp),sp
	popl	r3
	popl	r4
	unlk
	rts
	.const
LC21:
	.isoz	"/"
LC22:
	.isoz	"./"
	.text
	.global	_compose_name
_compose_name:
	link	#0
	pushl	r5
	pushl	r4
	pushl	r3
	movl	8(fp),r4
	movl	12(fp),r5
	pushl	r4
	call	_strlen
	movl	r0,r3
	pushl	r5
	call	_strlen
	leal	2(r0,r3),r2
	pushl	r2
	call	_malloc
	movl	r0,r3
	addl	#12,sp
	tstl	r3
	jne	L157
	pushl	_pgname
	pushl	#LC6
	pushl	#__ioblk+48
	call	_fprintf
	pushl	#1
	call	_exit
	addl	#16,sp
L157:
	pushl	r4
	pushl	r3
	call	_strcpy
	pushl	#LC21
	pushl	r3
	call	_strcat
	pushl	r5
	pushl	r3
	call	_strcat
	pushl	#2
	pushl	#LC22
	pushl	r3
	call	_strncmp
	tstl	r0
	jeq	L158
	movl	r3,r0
	jmp	L156
L158:
	leal	2(r3),r0
L156:
	leal	-12(fp),sp
	popl	r3
	popl	r4
	popl	r5
	unlk
	rts
	.global	_last_id
_last_id:
	link	#0
	pushl	r3
	movl	8(fp),r3
	pushl	#47
	pushl	r3
	call	_strrchr
	tstl	r0
	jeq	L161
	incl	r0
	jmp	L160
L161:
	movl	r3,r0
L160:
	leal	-4(fp),sp
	popl	r3
	unlk
	rts
	.const
LC23:
	.isoz	"%s - lista o conteúdo de diretórios\n\n%s\n\nSintaxe:\n\t%s [-rdbcfstaRN] [<diretório> ...]\n"
LC24:
	.isoz	"\nOpções:\t-r: Arquivos regulares\n\t-d: Diretórios\n\t-b: Especiais de blocos\n\t-c: Especiais de caracteres\n\t-f: Fifos\n\t-s: Tamanho, em blocos (de 512 bytes)\n"
LC25:
	.isoz	"\t-t: Ordena os arquivos pela data da última modificação\n\t-a: Lista também \".\" e \"..\"\n\t-R: Lista subdiretórios recursivamente\n\t-N: Lê os <diretório>s de \"stdin\"\n\nObs.:\tSe não for dado nenhum <diretório> nem a opção \"-N\",\n\tserá considerado o diretório corrente\n"
	.text
	.global	_help
_help:
	link	#0
	pushl	r3
	pushl	_pgname
	pushl	#_pgversion
	pushl	_pgname
	pushl	#LC23
	movl	#__ioblk+48,r3
	pushl	r3
	call	_fprintf
	pushl	#LC24
	pushl	r3
	call	_fprintf
	pushl	#LC25
	pushl	r3
	call	_fprintf
	addl	#36,sp
	pushl	#2
	call	_exit
	leal	-4(fp),sp
	popl	r3
	unlk
	rts
	.bss
	.global	_dirlist
	.align	4
_dirlist:
	.dsb	48
	.global	_filelist
	.align	4
_filelist:
	.dsb	48
	.global	_Nflag
_Nflag:
	.dsl	1
	.global	_Rflag
_Rflag:
	.dsl	1
	.global	_aflag
_aflag:
	.dsl	1
	.global	_tflag
_tflag:
	.dsl	1
	.global	_sflag
_sflag:
	.dsl	1
	.global	_fflag
_fflag:
	.dsl	1
	.global	_cflag
_cflag:
	.dsl	1
	.global	_bflag
_bflag:
	.dsl	1
	.global	_dflag
_dflag:
	.dsl	1
	.global	_rflag
_rflag:
	.dsl	1
	.global	_pgname
_pgname:
	.dsl	1
