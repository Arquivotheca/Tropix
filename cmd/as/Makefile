#*
#****************************************************************
#*	Makefile para o programa "as.486"			*
#****************************************************************
#*

#*
#******	Definições de uso geral *********************************
#*
ROOT	=
DIR	= $ROOT/bin
PGM	= as
PROT	= a+x,u+m
MAN	= cmd	
CC	= cc
SRC_CC	= cc
CFLAGS	= -c -O
LDFLAGS	= -s -u
DST_AS	= /bin/as
DIR_PGM = as.486
DEF	=

#*
#****** Nome dos objetos virtuais *******************************
#*
.virtual	c lint tstall tst tsts tstif tstf diff.f tst32 tst16 tstq tstv tstboot1
.virtual	tstlc tstpseudo tst_op tst_id nm unld
.virtual	cp cmp cmpq cmpqcp man modif prman zipto zipfrom
.virtual	pento penfrom pensave

#*
#******	Arquivos ************************************************
#*
SRC	= 	src/as.c	 	src/code0.c		\
		src/msg.c	 	src/expr.c		\
		src/if.c 		src/lst.c		\
		src/opnd.c					\
		src/pass1.c 		src/pass2.c		\
		src/pseudo.c		src/regular1.c		\
		src/regular1.c		src/scan.c		\
		src/sdi.c 		src/symtb.c		\
		src/tab.c

TBL_SRC	= 	tbl/mk_id_table.c	tbl/mk_op_table.c	\
		tbl/scan_tb.c	 	tbl/src_op_table.c	\
		tbl/tst_id_table.c 	tbl/tst_op_table.c

TBL	=	obj/scan_tb.o	obj/id_sym_tb.o		obj/op_code_tb.o

OBJ	= 	obj/as.o		\
		obj/pass1.o		\
		obj/regular1.o		\
		obj/regular2.o		\
		obj/pass2.o		\
		obj/msg.o		\
		obj/scan.o		\
		obj/symtb.o		\
		obj/opnd.o		\
		obj/pseudo.o		\
		obj/if.o		\
		obj/code0.o		\
		obj/lst.o		\
		obj/tab.o		\
		obj/sdi.o		\
		obj/expr.o

#*
#******	Link-edição final ***************************************
#*
$PGM:		obj $OBJ $TBL
		$CC $LDFLAGS -o $PGM $OBJ $TBL
		size $PGM

c:		obj $OBJ $TBL

lint:
		$CC $DEF -T $TBL_SRC $SRC

obj:		.exist
		mkdir obj

unld:
		-rm -f $PGM

#*
#******	Testes **************************************************
#*
tstall:		tst tsts tstf tstif tst32 tst16 tstq tstboot1 tstv tstlc tstpseudo
		rm -f t.l t.o s.l if.l if.o f.l f.o 32.l 32.o 16.l 16.o q.l q.o boot1.l boot1.o v.l v.o

tst:
		./$PGM -l t.l -o t.o t.s
		-cmpobj old.t.o.v t.o
		-cmp -l old.t.l.v t.l

tsts:
		./$PGM -l s.l -o /dev/null s.s
		-cmp -l old.s.l.v s.l

tstif:
		./$PGM -l if.l -o if.o if.s
		-cmpobj old.if.o.v if.o
		-cmp -l old.if.l.v if.l

tstf:
		./$PGM -l f.l -o f.o f.s
		-cmpobj old.f.o.v f.o
		-cmp -l old.f.l.v f.l

diff.f:
		diff -c old.f.l.v f.l | show
		mv -i f.l old.f.l.v

tst32:
		./$PGM -l 32.l -o 32.o 32.s
		-cmpobj old.32.o.v 32.o
		-cmp -l old.32.l.v 32.l

tst16:
		./$PGM -w -l 16.l -o 16.o 16.s
		-cmpobj old.16.o.v 16.o
		-cmp -l old.16.l.v 16.l

tstq:
		./$PGM -l q.l -o q.o q.s
		-cmpobj old.q.o.v q.o
		-cmp -l old.q.l.v q.l

tstboot1:
		./$PGM -D DEV=81 -l boot1.l -o boot1.o boot1.s
		-cmpobj old.boot1.o.v boot1.o
		-cmp -l old.boot1.l.v boot1.l

tstv:
		./$PGM -l v.l -o v.o v.s
		-cmpobj old.v.o.v v.o
		-cmp -l old.v.l.v v.l

tstlc:
		./$PGM lc.s
		-cmpobj old.lc.o.v a.out
		./$PGM cp.s
		-cmpobj old.cp.o.v a.out
		-rm -f a.out

tstpseudo:
		$CC $DEF -SO src/pseudo.c
		./$PGM pseudo.s
		-cmpobj a.out obj/pseudo.o
		rm -f pseudo.s a.out

tst_op:		tst_op_table
		tst_op_table -v | show
		rm -i tst_op_table obj/tst_op_table.o

tst_op_table:	obj/tst_op_table.o 		\
		obj/op_code_tb.o
		$CC -u -s -o tst_op_table obj/tst_op_table.o obj/op_code_tb.o
		size tst_op_table

obj/tst_op_table.o: tbl/tst_op_table.c 		\
		h/common.h			\
		h/op_code.h			\
		h/op_hash.h
		$CC $DEF -c -O -o obj tbl/tst_op_table.c

tst_id:		tst_id_table
		tst_id_table
		rm -i tst_id_table

#*
#******	Dependências e compilação dos módulos *******************
#*
obj/anomalous.o: src/anomalous.c	\
		h/common.h		\
		h/scan.h		\
		h/symtb.h		\
		h/expr.h		\
		h/opnd.h		\
		h/instr.h		\
		h/msg.h			\
		h/as.h
		$CC $DEF $CFLAGS -o obj src/anomalous.c

obj/as.o:	src/as.c		\
		h/common.h		\
		h/tokens.h		\
		h/scan.h		\
		h/id_sym_tb.h		\
		h/expr.h		\
		h/msg.h			\
		h/as.h
		$CC $DEF $CFLAGS -o obj src/as.c

obj/code0.o:	src/code0.c		\
		h/common.h		\
		h/tokens.h		\
		h/scan.h		\
		h/id_sym_tb.h		\
		h/expr.h		\
		h/opnd.h		\
		h/msg.h			\
		h/as.h
		$CC $DEF $CFLAGS -o obj src/code0.c

obj/msg.o:	src/msg.c		\
		h/common.h		\
		h/tokens.h		\
		h/scan.h		\
		h/id_sym_tb.h		\
		h/expr.h		\
		h/opnd.h		\
		h/msg.h			\
		h/as.h
		$CC $DEF $CFLAGS -o obj src/msg.c

obj/expr.o:	src/expr.c		\
		h/common.h		\
		h/tokens.h		\
		h/scan.h		\
		h/msg.h			\
		h/id_sym_tb.h		\
		h/expr.h		\
		h/as.h
		$CC $DEF $CFLAGS -o obj src/expr.c

obj/lst.o:	src/lst.c		\
		h/common.h		\
		h/tokens.h		\
		h/scan.h		\
		h/id_sym_tb.h		\
		h/msg.h			\
		h/as.h
		$CC $DEF $CFLAGS -o obj src/lst.c

obj/opnd.o:	src/opnd.c		\
		h/common.h		\
		h/as.h			\
		h/tokens.h		\
		h/scan.h		\
		h/lex.h			\
		h/id_sym_tb.h		\
		h/expr.h		\
		h/opnd.h		\
		h/msg.h
		$CC $DEF $CFLAGS -o obj src/opnd.c

obj/pass1.o:	src/pass1.c		\
		h/common.h		\
		h/tokens.h		\
		h/scan.h		\
		h/id_sym_tb.h		\
		h/expr.h		\
		h/opnd.h		\
		h/msg.h			\
		h/as.h			\
		h/op_code.h
		$CC $DEF $CFLAGS -o obj src/pass1.c

obj/regular1.o:	src/regular1.c		\
		h/common.h		\
		h/tokens.h		\
		h/scan.h		\
		h/id_sym_tb.h		\
		h/expr.h		\
		h/opnd.h		\
		h/msg.h			\
		h/as.h			\
		h/op_code.h
		$CC $DEF $CFLAGS -o obj src/regular1.c

obj/regular2.o:	src/regular2.c		\
		h/common.h		\
		h/tokens.h		\
		h/scan.h		\
		h/id_sym_tb.h		\
		h/expr.h		\
		h/opnd.h		\
		h/msg.h			\
		h/as.h			\
		h/op_code.h
		$CC $DEF $CFLAGS -o obj src/regular2.c

obj/pass2.o:	src/pass2.c		\
		h/common.h		\
		h/tokens.h		\
		h/scan.h		\
		h/id_sym_tb.h		\
		h/expr.h		\
		h/opnd.h		\
		h/msg.h			\
		h/as.h			\
		h/op_code.h
		$CC $DEF $CFLAGS -o obj src/pass2.c

obj/pseudo.o:	src/pseudo.c		\
		h/common.h		\
		h/tokens.h		\
		h/scan.h		\
		h/id_sym_tb.h		\
		h/tabs.h		\
		h/expr.h		\
		h/op_code.h		\
		h/opnd.h		\
		h/msg.h			\
		h/as.h
		$CC $DEF $CFLAGS -o obj src/pseudo.c

obj/if.o:	src/if.c		\
		h/common.h		\
		h/tokens.h		\
		h/scan.h		\
		h/id_sym_tb.h		\
		h/tabs.h		\
		h/expr.h		\
		h/op_code.h		\
		h/opnd.h		\
		h/msg.h			\
		h/as.h
		$CC $DEF $CFLAGS -o obj src/if.c

obj/scan.o:	src/scan.c		\
		h/common.h		\
		h/tokens.h		\
		h/scan.h		\
		h/lex.h			\
		h/msg.h			\
		h/id_sym_tb.h		\
		h/as.h
		$CC $DEF $CFLAGS -o obj src/scan.c

obj/sdi.o:	src/sdi.c		\
		h/common.h		\
		h/tokens.h		\
		h/scan.h		\
		h/id_sym_tb.h		\
		h/msg.h			\
		h/as.h
		$CC $DEF $CFLAGS -o obj src/sdi.c

obj/symtb.o:	src/symtb.c		\
		h/common.h		\
		h/tokens.h		\
		h/scan.h		\
		h/id_sym_tb.h		\
		h/as.h			\
		h/msg.h			\
		h/op_code.h		\
		h/op_hash.h		\
		h/id_hash.h
		$CC $DEF $CFLAGS -o obj src/symtb.c

obj/tab.o:	src/tab.c		\
		h/common.h		\
		h/tokens.h		\
		h/scan.h		\
		h/id_sym_tb.h		\
		h/tabs.h		\
		h/as.h
		$CC $DEF $CFLAGS -o obj src/tab.c

obj/fpetome.o:	src/fpetome.s
		$DIR/as -o obj/fpetome.o src/fpetome.s

obj/scan_tb.o:	tbl/scan_tb.c		\
		h/common.h		\
		h/tokens.h		\
		h/scan.h		\
		h/lex.h
		$CC $DEF $CFLAGS -o obj tbl/scan_tb.c

#*
#******	Geração das tabela de identificadores *******************
#*
ID_ASM	= .id_hash_tb.s .id_sym_tb.s .reg_val_vec.s

obj/id_sym_tb.o: tbl/mk_id_table.c	\
		h/id_sym_tb.h		\
		h/id_insert.h		\
		h/id_hash.h
		$SRC_CC $DEF -u -o mk_id_table tbl/mk_id_table.c
		./mk_id_table -v
		$DST_AS -o obj/id_sym_tb.o $ID_ASM
		rm -f $ID_ASM mk_id_table

tst_id_table:	tbl/tst_id_table.c	\
		obj/id_sym_tb.o		\
		h/id_sym_tb.h		\
		h/id_hash.h
		$CC $DEF -u -O -o tst_id_table tbl/tst_id_table.c obj/id_sym_tb.o

#*
#******	Geração das tabela de operadores ************************
#*
OP_ASM	= .op_code_tb.s .op_addr_tb.s .op_hash_tb.s .op_hash_sz.s

obj/op_code_tb.o:  obj/mk_op_table.o obj/src_op_table.o
		$SRC_CC -u -s -o mk_op_table obj/mk_op_table.o obj/src_op_table.o
		./mk_op_table
#		mk_op_table -v $OP_HASH_SZ
		$DST_AS -o obj/op_code_tb.o $OP_ASM
		rm -f $OP_ASM mk_op_table

obj/mk_op_table.o: tbl/mk_op_table.c		\
		h/common.h			\
		h/op_code.h			\
		h/src_op_code.h			\
		h/op_hash.h
		$SRC_CC $DEF -c -O -o obj tbl/mk_op_table.c

obj/src_op_table.o:  tbl/src_op_table.c		\
		h/common.h			\
		h/id_sym_tb.h			\
		h/expr.h			\
		h/opnd.h			\
		h/op_code.h			\
		h/src_op_code.h
		$SRC_CC $DEF -c -o obj tbl/src_op_table.c

#*
#******	Tabela de símbolos **************************************
#*
nm:
		nm -o obj/*.o >nm_list

#*
#******	Cópia para o diretório definitivo ***********************
#*
cp:		$PGM
		-untext $PGM
		cp -f $PGM $DIR
		chmod -v $PROT $DIR/$PGM
#		$DIR/$PGM -M

#*
#******	Comparação **********************************************
#*
cmp:
		-cmpobj $PGM $DIR

#*
#******	Comparação rápida ***************************************
#*
cmpq:
		-cmpobj -q $PGM $DIR

cmpqcp:
		-cmpobj -q $PGM $DIR || make cp


#*
#******	Manual **************************************************
#*
man:
		sroff man.r >$mandir/cmd/as
		man as

prman:
		sroff man.r | lpr

modif:
		sroff modif.r >$mandir/modif/$MAN/$PGM
		modif $PGM

#*
#****** Atualizações ********************************************
#*
zipto:
		-cmptree -sg . /zip$cmd/$PGM
		-forldiff /zip$cmd/$PGM
		-rm garfield

zipfrom:
		cd /zip$cmd/$PGM &&		\
		cmptree -sg . $cmd/$PGM;	\
		forldiff $cmd/$PGM;		\
		rm /zip$cmd/$PGM/garfield

pento:
		-mount /pen
		-cmptree -sg . /pen$cmd/$PGM
		-forldiff /pen$cmd/$PGM
		-rm -f garfield
		-umount /pen

penfrom:
		-mount /pen
		cd /pen$cmd/$PGM &&		\
		cmptree -sg . $cmd/$PGM;	\
		forldiff $cmd/$PGM;		\
		rm -f /pen$cmd/$PGM/garfield
		-umount /pen

pensave:
		-mount /pen
		-pento $cmd/$PGM
		-umount /pen
