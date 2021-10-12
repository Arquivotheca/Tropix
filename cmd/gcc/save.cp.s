	.global	_pgversion
	.const
_pgversion:
	.isoz	"Versão:\t2.3.1, de 26.11.89"
LC0:
	.isoz	"ifvmxNdH"
LC1:
	.isoz	"r"
LC2:
	.isoz	"/dev/tty"
LC3:
	.isoz	"%s: Não consegui abrir \"/dev/tty\"\n"
LC4:
	.isoz	"/"
LC5:
	.isoz	"%s: O arquivo \"%s\" não é um diretório\n\n"
	.text
	.global	_main
_main:
	link	#1076
	pushl	r5
	pushl	r4
	pushl	r3
	movl	8(fp),r2
	movl	r2,-1068(fp)
	movl	12(fp),r4
	movl	#0,-1072(fp)
	movl	#0,-1076(fp)
	movl	(r4),r2
	movl	r2,_pgname
	jmp	.L2
.L15:
	addl	#-72,r0
	cmpl	#48,r0
	jhi	.L13
	jmp	.L14(*,r0@l)
	.const
.L14:
	.long	.L12
	.long	.L13
	.long	.L13
	.long	.L13
	.long	.L13
	.long	.L13
	.long	.L10
	.long	.L13
	.long	.L13
	.long	.L13
	.long	.L13
	.long	.L13
	.long	.L13
	.long	.L13
	.long	.L13
	.long	.L13
	.long	.L13
	.long	.L13
	.long	.L13
	.long	.L13
	.long	.L13
	.long	.L13
	.long	.L13
	.long	.L13
	.long	.L13
	.long	.L13
	.long	.L13
	.long	.L13
	.long	.L11
	.long	.L13
	.long	.L6
	.long	.L13
	.long	.L13
	.long	.L5
	.long	.L13
	.long	.L13
	.long	.L13
	.long	.L8
	.long	.L13
	.long	.L13
	.long	.L13
	.long	.L13
	.long	.L13
	.long	.L13
	.long	.L13
	.long	.L13
	.long	.L7
	.long	.L13
	.long	.L9
	.text
.L5:
	incl	_iflag
	jmp	.L2
.L6:
	incl	_fflag
	jmp	.L2
.L7:
	incl	_vflag
	jmp	.L2
.L8:
	incl	_mflag
	jmp	.L2
.L9:
	incl	_xflag
	jmp	.L2
.L10:
	incl	_Nflag
	jmp	.L2
.L11:
	incl	_dflag
	jmp	.L2
.L12:
	call	_help
.L13:
	pushl	#__ioblk+48
	pushl	#10
	call	_fputc
	call	_help
	addl	#8,sp
.L2:
	pushl	#LC0
	pushl	r4
	pushl	-1068(fp)
	call	_getopt
	addl	#12,sp
	cmpl	#-1,r0
	jne	.L15
	movl	_optind,r0
	asll	#2,r0
	leal	(r0,r4),r4
	movl	_optind,r2
	subl	r2,-1068(fp)
	cmpl	#0,_iflag
	jne	.L17
	cmpl	#0,_fflag
	jne	.L16
.L17:
	pushl	#LC1
	pushl	#LC2
	call	_fopen
	movl	r0,_ttyfp
	addl	#8,sp
	tstl	r0
	jne	.L16
	pushl	_pgname
	pushl	#LC3
	pushl	#__ioblk+48
	call	_fprintf
	pushl	_pgname
	call	_perror
	pushl	#1
	call	_exit
	addl	#20,sp
.L16:
	cmpl	#0,_Nflag
	jeq	.L19
	cmpl	#1,-1068(fp)
	jeq	.L21
	jmp	.L42
.L19:
	cmpl	#1,-1068(fp)
	jgt	.L21
.L42:
	call	_help
.L21:
	leal	-1064(fp),r0
	pushl	r0
	movl	-1068(fp),r2
	movl	-4(r4,r2@l),r5
	pushl	r5
	call	_stat
	addl	#8,sp
	tstl	r0
	jlt	.L23
	movl	-1060(fp),r0
	andl	#61440,r0
	cmpl	#16384,r0
	jne	.L23
	pushl	r5
	leal	-1024(fp),r3
	pushl	r3
	call	_strcpy
	pushl	#LC4
	pushl	r3
	call	_strcat
	pushl	#47
	pushl	r3
	call	_strrchr
	incl	r0
	movl	r0,-1076(fp)
	addl	#24,sp
.L23:
	cmpl	#0,_Nflag
	jne	.L25
	cmpl	#2,-1068(fp)
	jle	.L24
.L25:
	cmpl	#0,-1076(fp)
	jne	.L24
	pushl	r5
	pushl	_pgname
	pushl	#LC5
	pushl	#__ioblk+48
	call	_fprintf
	call	_help
	addl	#16,sp
.L24:
	cmpl	#0,-1076(fp)
	jne	.L26
	pushl	4(r4)
	pushl	(r4)
	call	_copia
	movl	-1072(fp),r2
	leal	(r0,r2),r1
	movl	r1,-1072(fp)
	addl	#8,sp
	jmp	.L27
.L26:
	cmpl	#0,_Nflag
	jne	.L28
	movl	-1068(fp),r2
	movl	#0,-4(r4,r2@l)
	cmpl	#0,(r4)
	jeq	.L27
.L34:
	pushl	#47
	pushl	(r4)
	call	_strrchr
	addl	#8,sp
	tstl	r0
	jne	.L32
	movl	(r4),r0
	jmp	.L33
.L32:
	incl	r0
.L33:
	pushl	r0
	pushl	-1076(fp)
	call	_strcpy
	leal	-1024(fp),r0
	pushl	r0
	pushl	(r4)
	call	_copia
	movl	-1072(fp),r2
	leal	(r0,r2),r1
	movl	r1,-1072(fp)
	addl	#16,sp
	addl	#4,r4
	cmpl	#0,(r4)
	jne	.L34
	jmp	.L27
.L28:
	leal	-512(fp),r3
	jmp	.L36
.L40:
	pushl	#47
	pushl	r3
	call	_strrchr
	addl	#8,sp
	tstl	r0
	jne	.L38
	movl	r3,r0
	jmp	.L39
.L38:
	incl	r0
.L39:
	pushl	r0
	pushl	-1076(fp)
	call	_strcpy
	leal	-1024(fp),r0
	pushl	r0
	pushl	r3
	call	_copia
	movl	-1072(fp),r2
	leal	(r0,r2),r1
	movl	r1,-1072(fp)
	addl	#16,sp
.L36:
	pushl	r3
	call	_gets
	addl	#4,sp
	tstl	r0
	jne	.L40
.L27:
	pushl	-1072(fp)
	call	_exit
	leal	-1088(fp),sp
	popl	r3
	popl	r4
	popl	r5
	unlk
	rts
	.const
LC6:
	.isoz	"=> copia (%s, %s)\n"
LC7:
	.isoz	"%s: "
LC8:
	.isoz	"Não consegui obter o estado de \"%s\"\n"
LC9:
	.isoz	"%s: \"%s\" é um diretório\n"
LC10:
	.isoz	"%s: \"%s\" já existe e é um diretório\n"
LC11:
	.isoz	"%s: \"%s\" já existe e é o mesmo arquivo do que \"%s\"\n"
LC12:
	.isoz	"%s: \"%s\" já existe. Apaga/reescreve? (n): "
LC13:
	.isoz	"%s: Não consigo remover \"%s\"\n"
LC14:
	.isoz	"%s:\n"
	.text
	.global	_copia
_copia:
	link	#80
	pushl	r5
	pushl	r4
	pushl	r3
	movl	8(fp),r5
	movl	12(fp),r4
	cmpl	#0,_dflag
	jeq	.L44
	pushl	r4
	pushl	r5
	pushl	#LC6
	pushl	#__ioblk+48
	call	_fprintf
	addl	#16,sp
.L44:
	cmpl	#0,_iflag
	jeq	.L45
	pushl	r5
	pushl	#LC7
	movl	#__ioblk+48,r3
	pushl	r3
	call	_fprintf
	pushl	r3
	call	_fflush
	pushl	#0
	pushl	_ttyfp
	call	_askyesno
	addl	#24,sp
	tstl	r0
	jle	.L58
.L45:
	leal	-40(fp),r0
	pushl	r0
	pushl	r5
	call	_stat
	addl	#8,sp
	tstl	r0
	jge	.L47
	pushl	r5
	pushl	#LC8
	pushl	#__ioblk+48
	call	_fprintf
	jmp	.L59
.L47:
	movl	-36(fp),r0
	andl	#61440,r0
	cmpl	#16384,r0
	jne	.L48
	pushl	r5
	pushl	_pgname
	pushl	#LC9
	jmp	.L60
.L48:
	leal	-80(fp),r0
	pushl	r0
	pushl	r4
	call	_stat
	addl	#8,sp
	tstl	r0
	jlt	.L49
	movl	-76(fp),r0
	andl	#61440,r0
	cmpl	#16384,r0
	jne	.L50
	pushl	r4
	pushl	_pgname
	pushl	#LC10
.L60:
	pushl	#__ioblk+48
	call	_fprintf
.L64:
	movl	#1,r0
	jmp	.L43
.L50:
	movw	-80(fp),r1
	cmpw	-40(fp),r1
	jne	.L51
	movw	-78(fp),r1
	cmpw	-38(fp),r1
	jne	.L51
	pushl	r5
	pushl	r4
	pushl	_pgname
	pushl	#LC11
	pushl	#__ioblk+48
	call	_fprintf
	jmp	.L64
.L51:
	cmpl	#0,_fflag
	jne	.L52
	pushl	r4
	pushl	_pgname
	pushl	#LC12
	movl	#__ioblk+48,r3
	pushl	r3
	call	_fprintf
	pushl	r3
	call	_fflush
	pushl	#0
	pushl	_ttyfp
	call	_askyesno
	addl	#28,sp
	tstl	r0
	jle	.L64
.L52:
	movl	-36(fp),r2
	andl	#61440,r2
	movl	-76(fp),r0
	andl	#61440,r0
	cmpl	r2,r0
	jne	.L55
	cmpl	#32768,r2
	jeq	.L49
.L55:
	pushl	r4
	call	_unlink
	addl	#4,sp
	tstl	r0
	jge	.L49
	pushl	r4
	pushl	_pgname
	pushl	#LC13
	pushl	#__ioblk+48
	call	_fprintf
.L59:
	pushl	_pgname
	call	_perror
	jmp	.L64
.L49:
	leal	-80(fp),r0
	pushl	r0
	leal	-40(fp),r0
	pushl	r0
	pushl	r4
	pushl	r5
	call	_copiablocos
	addl	#16,sp
	tstl	r0
	jne	.L64
	cmpl	#0,_vflag
	jeq	.L58
	cmpl	#0,_iflag
	jne	.L58
	pushl	r5
	pushl	#LC14
	pushl	#__ioblk+24
	call	_fprintf
.L58:
	clrl	r0
.L43:
	leal	-92(fp),sp
	popl	r3
	popl	r4
	popl	r5
	unlk
	rts
	.const
LC15:
	.isoz	"=> copiablocos (%s, %s)\n"
LC16:
	.isoz	"%s: Não consegui criar o arquivo \"%s\"\n"
LC17:
	.isoz	"%s: Não consegui abrir o arquivo \"%s\"\n"
LC18:
	.isoz	"%s: Erro de escrita em \"%s\"\n"
LC19:
	.isoz	"%s: Erro de leitura em \"%s\"\n"
LC20:
	.isoz	"%s: Não consegui criar \"%s\"\n"
LC21:
	.isoz	"%s: Tipo de arquivo inválido\n"
LC22:
	.isoz	"%s: Não consegui obter o estado de \"%s\"\n"
LC23:
	.isoz	"%s: Não consegui restaurar a proteção de \"%s\"\n"
LC24:
	.isoz	"%s: Não consegui restaurar os tempos de \"%s\"\n"
LC25:
	.isoz	"%s: Não consegui restaurar o dono/grupo de \"%s\"\n"
	.text
	.global	_copiablocos
_copiablocos:
	link	#516
	pushl	r5
	pushl	r4
	pushl	r3
	cmpl	#0,_dflag
	jeq	.L66
	pushl	12(fp)
	pushl	8(fp)
	pushl	#LC15
	pushl	#__ioblk+48
	call	_fprintf
	addl	#16,sp
.L66:
	movl	16(fp),r1
	movl	4(r1),r0
	andl	#61440,r0
	cmpl	#8192,r0
	jeq	.L79
	jgt	.L84
	cmpl	#4096,r0
	jeq	.L79
	jmp	.L82
.L84:
	cmpl	#24576,r0
	jeq	.L79
	cmpl	#32768,r0
	jne	.L82
.L69:
	movl	16(fp),r1
	pushl	4(r1)
	pushl	12(fp)
	call	_creat
	movl	r0,r5
	addl	#8,sp
	tstl	r5
	jge	.L70
	pushl	12(fp)
	pushl	_pgname
	pushl	#LC16
	jmp	.L93
.L70:
	pushl	#0
	pushl	8(fp)
	call	_open
	movl	r0,-516(fp)
	addl	#8,sp
	cmpl	#0,-516(fp)
	jge	.L71
	pushl	8(fp)
	pushl	_pgname
	pushl	#LC17
	pushl	#__ioblk+48
	call	_fprintf
	pushl	_pgname
	call	_perror
	jmp	.L94
.L71:
	leal	-512(fp),r4
	jmp	.L72
.L75:
	pushl	r3
	pushl	r4
	pushl	r5
	call	_write
	addl	#12,sp
	cmpl	r0,r3
	jeq	.L72
	pushl	12(fp)
	pushl	_pgname
	pushl	#LC18
	pushl	#__ioblk+48
	call	_fprintf
	pushl	_pgname
	call	_perror
	pushl	-516(fp)
	call	_close
.L94:
	pushl	r5
	call	_close
	movl	#1,r0
	jmp	.L65
.L72:
	pushl	#512
	pushl	r4
	pushl	-516(fp)
	call	_read
	movl	r0,r3
	addl	#12,sp
	tstl	r3
	jgt	.L75
	pushl	-516(fp)
	call	_close
	pushl	r5
	call	_close
	addl	#8,sp
	tstl	r3
	jge	.L67
	pushl	8(fp)
	pushl	_pgname
	pushl	#LC19
	jmp	.L93
.L79:
	cmpl	#0,_xflag
	jeq	.L69
	movl	16(fp),r1
	movswl	14(r1),r0
	pushl	r0
	pushl	4(r1)
	pushl	12(fp)
	call	_mknod
	addl	#12,sp
	tstl	r0
	jge	.L67
	pushl	12(fp)
	pushl	_pgname
	pushl	#LC20
	jmp	.L93
.L82:
	pushl	_pgname
	pushl	#LC21
	pushl	#__ioblk+48
	call	_fprintf
	movl	#1,r0
	jmp	.L65
.L67:
	pushl	20(fp)
	pushl	12(fp)
	call	_stat
	addl	#8,sp
	tstl	r0
	jge	.L85
	pushl	12(fp)
	pushl	_pgname
	pushl	#LC22
	jmp	.L93
.L85:
	cmpl	#0,_mflag
	jeq	.L90
	movl	16(fp),r1
	movl	4(r1),r3
	andl	#-61441,r3
	movl	20(fp),r1
	movl	4(r1),r0
	andl	#-61441,r0
	cmpl	r3,r0
	jeq	.L87
	pushl	r3
	pushl	12(fp)
	call	_chmod
	addl	#8,sp
	tstl	r0
	jge	.L87
	pushl	12(fp)
	pushl	_pgname
	pushl	#LC23
	jmp	.L93
.L87:
	movl	16(fp),r1
	leal	20(r1),r0
	pushl	r0
	pushl	12(fp)
	call	_utime
	addl	#8,sp
	tstl	r0
	jge	.L89
	pushl	12(fp)
	pushl	_pgname
	pushl	#LC24
	jmp	.L93
.L89:
	movl	20(fp),r1
	movw	10(r1),r2
	movl	16(fp),r1
	cmpw	10(r1),r2
	jne	.L91
	movl	20(fp),r1
	movw	12(r1),r2
	movl	16(fp),r1
	cmpw	12(r1),r2
	jeq	.L90
.L91:
	movl	16(fp),r1
	movzwl	12(r1),r0
	pushl	r0
	movzwl	10(r1),r0
	pushl	r0
	pushl	12(fp)
	call	_chown
	addl	#12,sp
	tstl	r0
	jge	.L90
	pushl	12(fp)
	pushl	_pgname
	pushl	#LC25
.L93:
	pushl	#__ioblk+48
	call	_fprintf
	pushl	_pgname
	call	_perror
	movl	#1,r0
	jmp	.L65
.L90:
	clrl	r0
.L65:
	leal	-528(fp),sp
	popl	r3
	popl	r4
	popl	r5
	unlk
	rts
	.const
LC26:
	.isoz	"%s - copia arquivos\n"
LC27:
	.isoz	"\n%s\n"
LC28:
	.isoz	"\nSintaxe:\n\t%s [-ifvmx] <arquivo1> <arquivo2>\n\t%s [-ifvmx] <arquivo1> [<arquivo2> ...] <diretório>\n\t%s [-ifvmx] -N <diretório>\n"
LC29:
	.isoz	"\nOpções:\t-i: Pede a confirmaçãoªpara cada arquivo a copiar\n\t-f: Os arquivos destino existentes são removidos silenciosamente\n\t-v: Lista o nome dos arquivos copiados\n\t-m: Copia o dono, grupo, proteção e datas do arquivo\n\t-x: Para arquivos especiais e FIFOs, copia o nó-índice\n\t-N: Lê os nomes dos arquivos de \"stdin\"\n"
	.text
	.global	_help
_help:
	link	#0
	pushl	r3
	pushl	_pgname
	pushl	#LC26
	movl	#__ioblk+48,r3
	pushl	r3
	call	_fprintf
	pushl	#_pgversion
	pushl	#LC27
	pushl	r3
	call	_fprintf
	pushl	_pgname
	pushl	_pgname
	pushl	_pgname
	pushl	#LC28
	pushl	r3
	call	_fprintf
	addl	#44,sp
	pushl	#LC29
	pushl	r3
	call	_fprintf
	pushl	#2
	call	_exit
	leal	-4(fp),sp
	popl	r3
	unlk
	rts
	.common	_ttyfp,4
	.common	_dflag,4
	.common	_Nflag,4
	.common	_xflag,4
	.common	_mflag,4
	.common	_vflag,4
	.common	_fflag,4
	.common	_iflag,4
	.common	_pgname,4
