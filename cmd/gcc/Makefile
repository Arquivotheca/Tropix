#*
#****************************************************************
#*	Makefile para o programa "gnu"				*
#****************************************************************
#*
ROOT	=
DIR	= $ROOT/lib
PGM	= gnu
PGM2	= gnuc
PROT	= a+x,u+m
MAN	= cmd
ATUAL	= toplev.c

#*
#******	Define o compilador para PC *****************************
#*
GEN=i486
COMP=gnu.cc.486.pc

DST_CC=cc
SRC_CC=cc

DST_OBJ=i486.pc.obj
SRC_OBJ=i486.pc.obj

LD=ld

#gen="$GEN.gen/config.h			\
#	 $GEN.gen/tm.h			\
#	 $GEN.gen/tm-$GEN.h		\
#	 $GEN.gen/md			\
#	 $GEN.gen/aux-output.c		\
#	 $GEN.gen/tropix.final.c"

.putenv	GEN

#*
#******	Fontes **************************************************
#*
SRC	=	src/version.c		\
		src/c-parse.y		\
		lex/scan.c		\
		lex/etc.c		\
		tree/tree-alloc.c	\
		tree/tree-build.c	\
		tree/tree-node.c	\
		tree/tree-type.c	\
		tchk/tchk-build0.c	\
		tchk/tchk-build1.c	\
		tchk/tchk-conv.c	\
		tchk/tchk-type.c	\
		tchk/tchk-expand.c	\
		dcl/dcl-data.c		\
		dcl/dcl-level.c		\
		dcl/dcl-tag.c		\
		dcl/dcl-dcl.c		\
		dcl/dcl-grok.c		\
		dcl/dcl-parm.c		\
		dcl/dcl-enum.c		\
		top/top-data.c		\
		top/top-error.c		\
		top/top-file.c		\
		top/top-main.c		\
		top/top-mem.c		\
		top/top-rest.c		\
		src/print-tree.c	\
		src/c-convert.c		\
		src/stor-layout.c	\
		src/fold-const.c	\
		src/rtl.c		\
		src/rtlanal.c		\
		src/expr.c		\
		src/stmt.c		\
		src/expmed.c		\
		src/explow.c		\
		src/optabs.c		\
		src/varasm.c		\
		src/symout.c		\
		src/dbxout.c		\
		src/emit-rtl.c		\
		src/integrate.c		\
		src/jump.c		\
		src/cse.c		\
		src/loop.c		\
		src/flow.c		\
		src/stupid.c		\
		src/combine.c		\
		src/regclass.c		\
		src/local-alloc.c	\
		src/global-alloc.c	\
		src/reload.c		\
		src/reload1.c		\
		$GEN.gen/insn-peep.c	\
		src/recog.c		\
		src/final.c		\
		src/genrecog.c		\
		src/genextract.c	\
		src/genoutput.c		\
		src/caller-save.c
#		src/bcopy.c

#*
#******	Objetos *************************************************
#*
OBJECTS	= 	$DST_OBJ/version.o	\
		$DST_OBJ/c-parse.tab.o	\
		$DST_OBJ/scan.o		\
		$DST_OBJ/etc.o		\
		$DST_OBJ/tree-alloc.o	\
		$DST_OBJ/tree-build.o	\
		$DST_OBJ/tree-node.o	\
		$DST_OBJ/tree-type.o	\
		$DST_OBJ/tchk-build0.o	\
		$DST_OBJ/tchk-build1.o	\
		$DST_OBJ/tchk-conv.o	\
		$DST_OBJ/tchk-type.o	\
		$DST_OBJ/tchk-expand.o	\
		$DST_OBJ/dcl-data.o	\
		$DST_OBJ/dcl-level.o	\
		$DST_OBJ/dcl-tag.o	\
		$DST_OBJ/dcl-dcl.o	\
		$DST_OBJ/dcl-grok.o	\
		$DST_OBJ/dcl-parm.o	\
		$DST_OBJ/dcl-enum.o	\
		$DST_OBJ/top-data.o	\
		$DST_OBJ/top-error.o	\
		$DST_OBJ/top-file.o	\
		$DST_OBJ/top-main.o	\
		$DST_OBJ/top-mem.o	\
		$DST_OBJ/top-rest.o	\
		$DST_OBJ/print-tree.o	\
		$DST_OBJ/c-convert.o	\
		$DST_OBJ/stor-layout.o	\
		$DST_OBJ/fold-const.o	\
		$DST_OBJ/dst_rtl.o	\
		$DST_OBJ/rtlanal.o	\
		$DST_OBJ/expr.o		\
		$DST_OBJ/stmt.o		\
		$DST_OBJ/expmed.o	\
		$DST_OBJ/explow.o	\
		$DST_OBJ/optabs.o	\
		$DST_OBJ/varasm.o	\
		$DST_OBJ/symout.o	\
		$DST_OBJ/dbxout.o	\
		$DST_OBJ/emit-rtl.o	\
		$DST_OBJ/insn-emit.o	\
		$DST_OBJ/integrate.o	\
		$DST_OBJ/jump.o		\
		$DST_OBJ/cse.o		\
		$DST_OBJ/loop.o		\
		$DST_OBJ/flow.o		\
		$DST_OBJ/stupid.o	\
		$DST_OBJ/combine.o	\
		$DST_OBJ/regclass.o	\
		$DST_OBJ/local-alloc.o	\
		$DST_OBJ/global-alloc.o	\
		$DST_OBJ/reload.o	\
		$DST_OBJ/reload1.o	\
		$DST_OBJ/insn-peep.o	\
		$DST_OBJ/recog.o	\
		$DST_OBJ/final.o	\
		$DST_OBJ/tropix.final.o	\
		$DST_OBJ/insn-recog.o	\
		$DST_OBJ/insn-extract.o	\
		$DST_OBJ/insn-output.o	\
		$DST_OBJ/caller-save.o
#		$DST_OBJ/bcopy.o

.virtual	NM c cmp cp cmpq cmpqcp gar1 gar11 gar12 gar13 gar14 gar15 gar2
.virtual	unld lint lintsz man modif nm prman prsz sz tst

#*
#******	Comandos ************************************************
#*
$COMP:		$DST_OBJ $OBJECTS $DST_OBJ/dst_obstack.o
		$DST_CC -us -o $COMP $OBJECTS $DST_OBJ/dst_obstack.o
#		$LD -un -o $COMP /lib/crt0.o $OBJECTS $DST_OBJ/dst_obstack.o -lc -lm
		chmod +m $COMP
		size $COMP
		ls -L $COMP
#		$COMP -M &

unld:
		rm -f $COMP

c:		$DST_OBJ $OBJECTS

$DST_OBJ:	.exist
		mkdir $DST_OBJ
#		if test -d $DST_OBJ; then touch $DST_OBJ; else mkdir $DST_OBJ; fi

tst:
		-rm -f cp.s
		$COMP -ftropix -O cp.c
#		-time $COMP -ftropix -O cp.c
		diff -c save.cp.s cp.s | show

#*
#******	Criação dos arquivos gerados ****************************
#*
$GEN.gen/insn-flags.h:	$GEN.gen/md.v $GEN.gen/genflags
		$GEN.gen/genflags $GEN.gen/md.v >$GEN.gen/insn-flags.h

$GEN.gen/insn-codes.h:	$GEN.gen/md.v $GEN.gen/gencodes
		$GEN.gen/gencodes $GEN.gen/md.v >$GEN.gen/insn-codes.h

$GEN.gen/insn-config.h: $GEN.gen/md.v $GEN.gen/genconfig
		$GEN.gen/genconfig $GEN.gen/md.v >$GEN.gen/insn-config.h

$GEN.gen/insn-emit.c: $GEN.gen/md.v $GEN.gen/genemit
		$GEN.gen/genemit $GEN.gen/md.v >$GEN.gen/insn-emit.c

$GEN.gen/insn-peep.c: $GEN.gen/md.v $GEN.gen/genpeep
		$GEN.gen/genpeep $GEN.gen/md.v >$GEN.gen/insn-peep.c

$GEN.gen/insn-recog.c: $GEN.gen/md.v $GEN.gen/genrecog
		$GEN.gen/genrecog $GEN.gen/md.v >$GEN.gen/insn-recog.c

$GEN.gen/insn-extract.c: $GEN.gen/md.v $GEN.gen/genextract
		$GEN.gen/genextract $GEN.gen/md.v >$GEN.gen/insn-extract.c

$GEN.gen/insn-output.c: $GEN.gen/md.v $GEN.gen/genoutput
		$GEN.gen/genoutput $GEN.gen/md.v >$GEN.gen/insn-output.c

#*
#******	Criação dos programas geradores *************************
#*
$GEN.gen/genflags:	$SRC_OBJ/genflags.o $SRC_OBJ/src_rtl.o $SRC_OBJ/src_obstack.o
		$SRC_CC -u -o $GEN.gen/genflags $SRC_OBJ/genflags.o $SRC_OBJ/src_rtl.o $SRC_OBJ/src_obstack.o

$GEN.gen/gencodes:	$SRC_OBJ/gencodes.o $SRC_OBJ/src_rtl.o $SRC_OBJ/src_obstack.o
		$SRC_CC -u -o $GEN.gen/gencodes $SRC_OBJ/gencodes.o $SRC_OBJ/src_rtl.o $SRC_OBJ/src_obstack.o

$GEN.gen/genconfig:	$SRC_OBJ/genconfig.o $SRC_OBJ/src_rtl.o $SRC_OBJ/src_obstack.o
		$SRC_CC -u -o $GEN.gen/genconfig $SRC_OBJ/genconfig.o $SRC_OBJ/src_rtl.o $SRC_OBJ/src_obstack.o

$GEN.gen/genemit:	$SRC_OBJ/genemit.o $SRC_OBJ/src_rtl.o $SRC_OBJ/src_obstack.o
		$SRC_CC -u -o $GEN.gen/genemit $SRC_OBJ/genemit.o $SRC_OBJ/src_rtl.o $SRC_OBJ/src_obstack.o

$GEN.gen/genpeep:	$SRC_OBJ/genpeep.o $SRC_OBJ/src_rtl.o $SRC_OBJ/src_obstack.o
		$SRC_CC -u -o $GEN.gen/genpeep $SRC_OBJ/genpeep.o $SRC_OBJ/src_rtl.o $SRC_OBJ/src_obstack.o

$GEN.gen/genrecog:	$SRC_OBJ/genrecog.o $SRC_OBJ/src_rtl.o $SRC_OBJ/src_obstack.o
		$SRC_CC -u -o $GEN.gen/genrecog $SRC_OBJ/genrecog.o $SRC_OBJ/src_rtl.o $SRC_OBJ/src_obstack.o

$GEN.gen/genextract:	$SRC_OBJ/genextract.o $SRC_OBJ/src_rtl.o $SRC_OBJ/src_obstack.o
		$SRC_CC -u -o $GEN.gen/genextract $SRC_OBJ/genextract.o $SRC_OBJ/src_rtl.o $SRC_OBJ/src_obstack.o

$GEN.gen/genoutput:	$SRC_OBJ/genoutput.o $SRC_OBJ/src_rtl.o $SRC_OBJ/src_obstack.o
		$SRC_CC -u -o $GEN.gen/genoutput $SRC_OBJ/genoutput.o $SRC_OBJ/src_rtl.o $SRC_OBJ/src_obstack.o

#*
#******	Compilação de cada módulo *******************************
#*
$DST_OBJ/toplev.o: src/toplev.c			\
		$GEN.gen/config.h		\
			$GEN.gen/tm.h		\
			$GEN.gen/tm-$GEN.h	\
		h/input.h			\
		h/tree.h			\
			h/tree.def.v		\
			h/machmode.def.v	\
			h/real.h		\
		h/c-tree.h			\
		h/rtl.h				\
			h/rtl.def.v		\
			h/machmode.def.v	\
		h/flags.h
		$DST_CC -c -O -o $DST_OBJ src/toplev.c

$DST_OBJ/top-data.o: top/top-data.c		\
		$GEN.gen/config.h		\
			$GEN.gen/tm.h		\
			$GEN.gen/tm-$GEN.h	\
		h/input.h			\
		h/tree.h			\
			h/tree.def.v		\
			h/machmode.def.v	\
			h/real.h		\
		h/c-tree.h			\
		h/rtl.h				\
			h/rtl.def.v		\
			h/machmode.def.v	\
		h/flags.h
		$DST_CC -c -O -o $DST_OBJ top/top-data.c

$DST_OBJ/top-error.o: top/top-error.c		\
		$GEN.gen/config.h		\
			$GEN.gen/tm.h		\
			$GEN.gen/tm-$GEN.h	\
		h/input.h			\
		h/tree.h			\
			h/tree.def.v		\
			h/machmode.def.v	\
			h/real.h		\
		h/c-tree.h			\
		h/rtl.h				\
			h/rtl.def.v		\
			h/machmode.def.v	\
		h/flags.h
		$DST_CC -c -O -o $DST_OBJ top/top-error.c

$DST_OBJ/top-file.o: top/top-file.c		\
		$GEN.gen/config.h		\
			$GEN.gen/tm.h		\
			$GEN.gen/tm-$GEN.h	\
		h/input.h			\
		h/tree.h			\
			h/tree.def.v		\
			h/machmode.def.v	\
			h/real.h		\
		h/c-tree.h			\
		h/rtl.h				\
			h/rtl.def.v		\
			h/machmode.def.v	\
		h/flags.h
		$DST_CC -c -O -o $DST_OBJ top/top-file.c

$DST_OBJ/top-main.o: top/top-main.c		\
		$GEN.gen/config.h		\
			$GEN.gen/tm.h		\
			$GEN.gen/tm-$GEN.h	\
		h/input.h			\
		h/tree.h			\
			h/tree.def.v		\
			h/machmode.def.v	\
			h/real.h		\
		h/c-tree.h			\
		h/rtl.h				\
			h/rtl.def.v		\
			h/machmode.def.v	\
		h/flags.h
		$DST_CC -c -O -o $DST_OBJ top/top-main.c

$DST_OBJ/top-mem.o: top/top-mem.c		\
		$GEN.gen/config.h		\
			$GEN.gen/tm.h		\
			$GEN.gen/tm-$GEN.h	\
		h/input.h			\
		h/tree.h			\
			h/tree.def.v		\
			h/machmode.def.v	\
			h/real.h		\
		h/c-tree.h			\
		h/rtl.h				\
			h/rtl.def.v		\
			h/machmode.def.v	\
		h/flags.h
		$DST_CC -c -O -o $DST_OBJ top/top-mem.c

$DST_OBJ/top-rest.o: top/top-rest.c		\
		$GEN.gen/config.h		\
			$GEN.gen/tm.h		\
			$GEN.gen/tm-$GEN.h	\
		h/input.h			\
		h/tree.h			\
			h/tree.def.v		\
			h/machmode.def.v	\
			h/real.h		\
		h/c-tree.h			\
		h/rtl.h				\
			h/rtl.def.v		\
			h/machmode.def.v	\
		h/flags.h
		$DST_CC -c -O -o $DST_OBJ top/top-rest.c

$DST_OBJ/version.o:	src/version.c
		$DST_CC -c -O -o $DST_OBJ src/version.c

$DST_OBJ/c-parse.tab.o: src/c-parse.tab.c 	\
		$GEN.gen/config.h		\
			$GEN.gen/tm.h		\
			$GEN.gen/tm-$GEN.h	\
		h/tree.h			\
			h/tree.def.v		\
			h/machmode.def.v	\
			h/real.h		\
		h/input.h			\
		h/c-parse.h			\
		h/c-tree.h
		$DST_CC -c -O -o $DST_OBJ src/c-parse.tab.c

src/c-parse.tab.h				\
src/c-parse.tab.c: src/c-parse.y
		bison -d src/c-parse.y

$DST_OBJ/scan.o: lex/scan.c			\
		$GEN.gen/config.h		\
			$GEN.gen/tm.h		\
			$GEN.gen/tm-$GEN.h	\
		h/tree.h			\
			h/tree.def.v		\
			h/machmode.def.v	\
			h/real.h		\
		h/input.h			\
		h/c-parse.h			\
		h/c-tree.h			\
		src/c-parse.tab.h
		$DST_CC -c -O -o $DST_OBJ lex/scan.c

$DST_OBJ/etc.o: lex/etc.c			\
		$GEN.gen/config.h		\
			$GEN.gen/tm.h		\
			$GEN.gen/tm-$GEN.h	\
		h/tree.h			\
			h/tree.def.v		\
			h/machmode.def.v	\
			h/real.h		\
		h/c-tree.h
		$DST_CC -c -O -o $DST_OBJ lex/etc.c

$DST_OBJ/tree-alloc.o: tree/tree-alloc.c	\
		h/tree.h			\
			h/tree.def.v		\
			h/machmode.def.v	\
			h/real.h		\
		h/obstack.h
		$DST_CC -c -O -o $DST_OBJ tree/tree-alloc.c

$DST_OBJ/tree-build.o: tree/tree-build.c	\
		$GEN.gen/config.h		\
			$GEN.gen/tm.h		\
			$GEN.gen/tm-$GEN.h	\
		h/tree.h			\
			h/tree.def.v		\
			h/machmode.def.v	\
			h/real.h		\
		h/obstack.h
		$DST_CC -c -O -o $DST_OBJ tree/tree-build.c

$DST_OBJ/tree-node.o: tree/tree-node.c		\
		$GEN.gen/config.h		\
			$GEN.gen/tm.h		\
			$GEN.gen/tm-$GEN.h	\
		h/tree.h			\
			h/tree.def.v		\
			h/machmode.def.v	\
			h/real.h		\
		h/obstack.h
		$DST_CC -c -O -o $DST_OBJ tree/tree-node.c

$DST_OBJ/tree-type.o: tree/tree-type.c		\
		$GEN.gen/config.h		\
			$GEN.gen/tm.h		\
			$GEN.gen/tm-$GEN.h	\
		h/tree.h			\
			h/tree.def.v		\
			h/machmode.def.v	\
			h/real.h		\
		h/obstack.h
		$DST_CC -c -O -o $DST_OBJ tree/tree-type.c

$DST_OBJ/print-tree.o: src/print-tree.c 	\
		$GEN.gen/config.h		\
			$GEN.gen/tm.h		\
			$GEN.gen/tm-$GEN.h	\
		h/tree.h			\
			h/tree.def.v		\
			h/machmode.def.v	\
			h/real.h		\
		h/tree.def.v
		$DST_CC -c -O -o $DST_OBJ src/print-tree.c

$DST_OBJ/dcl-data.o:	dcl/dcl-data.c		\
		h/tree.h			\
			h/tree.def.v		\
			h/machmode.def.v	\
			h/real.h		\
		dcl/dcl.h
		$DST_CC -c -O -o $DST_OBJ dcl/dcl-data.c

$DST_OBJ/dcl-level.o:	dcl/dcl-level.c		\
		h/tree.h			\
			h/tree.def.v		\
			h/machmode.def.v	\
			h/real.h		\
		h/flags.h			\
		dcl/dcl.h
		$DST_CC -c -O -o $DST_OBJ dcl/dcl-level.c

$DST_OBJ/dcl-tag.o:	dcl/dcl-tag.c		\
		h/tree.h			\
			h/tree.def.v		\
			h/machmode.def.v	\
			h/real.h		\
		h/flags.h			\
		h/c-tree.h			\
		tree/tree.h			\
		tchk/tchk.h			\
		dcl/dcl.h
		$DST_CC -c -O -o $DST_OBJ dcl/dcl-tag.c

$DST_OBJ/dcl-dcl.o:	dcl/dcl-dcl.c		\
		h/tree.h			\
			h/tree.def.v		\
			h/machmode.def.v	\
			h/real.h		\
		h/c-tree.h			\
		h/flags.h			\
		h/c-parse.h			\
		tree/tree.h			\
		tchk/tchk.h			\
		dcl/dcl.h
		$DST_CC -c -O -o $DST_OBJ dcl/dcl-dcl.c

$DST_OBJ/dcl-grok.o:	dcl/dcl-grok.c		\
		h/tree.h			\
			h/tree.def.v		\
			h/machmode.def.v	\
			h/real.h		\
		h/c-tree.h			\
		h/c-parse.h			\
		tree/tree.h			\
		tchk/tchk.h			\
		dcl/dcl.h
		$DST_CC -c -O -o $DST_OBJ dcl/dcl-grok.c

$DST_OBJ/dcl-parm.o:	dcl/dcl-parm.c		\
		h/tree.h			\
			h/tree.def.v		\
			h/machmode.def.v	\
			h/real.h		\
		h/c-tree.h			\
		tree/tree.h			\
		dcl/dcl.h
		$DST_CC -c -O -o $DST_OBJ dcl/dcl-parm.c

$DST_OBJ/dcl-enum.o:	dcl/dcl-enum.c		\
		h/tree.h			\
			h/tree.def.v		\
			h/machmode.def.v	\
			h/real.h		\
		h/flags.h			\
		tree/tree.h			\
		tchk/tchk.h			\
		dcl/dcl.h
		$DST_CC -c -O -o $DST_OBJ dcl/dcl-enum.c

$DST_OBJ/tchk-build0.o:	tchk/tchk-build0.c	\
		$GEN.gen/config.h		\
			$GEN.gen/tm.h		\
			$GEN.gen/tm-$GEN.h	\
		h/tree.h			\
			h/tree.def.v		\
			h/machmode.def.v	\
			h/real.h		\
		h/c-tree.h			\
		h/flags.h
		$DST_CC -c -O -o $DST_OBJ tchk/tchk-build0.c

$DST_OBJ/tchk-build1.o:	tchk/tchk-build1.c	\
		$GEN.gen/config.h		\
			$GEN.gen/tm.h		\
			$GEN.gen/tm-$GEN.h	\
		h/tree.h			\
			h/tree.def.v		\
			h/machmode.def.v	\
			h/real.h		\
		h/c-tree.h			\
		h/flags.h
		$DST_CC -c -O -o $DST_OBJ tchk/tchk-build1.c

$DST_OBJ/tchk-conv.o:	tchk/tchk-conv.c	\
		$GEN.gen/config.h		\
			$GEN.gen/tm.h		\
			$GEN.gen/tm-$GEN.h	\
		h/tree.h			\
			h/tree.def.v		\
			h/machmode.def.v	\
			h/real.h		\
		h/c-tree.h			\
		h/flags.h
		$DST_CC -c -O -o $DST_OBJ tchk/tchk-conv.c

$DST_OBJ/tchk-type.o:	tchk/tchk-type.c	\
		$GEN.gen/config.h		\
			$GEN.gen/tm.h		\
			$GEN.gen/tm-$GEN.h	\
		h/tree.h			\
			h/tree.def.v		\
			h/machmode.def.v	\
			h/real.h		\
		h/flags.h			\
		h/c-tree.h			\
		h/flags.h
		$DST_CC -c -O -o $DST_OBJ tchk/tchk-type.c

$DST_OBJ/tchk-expand.o:	tchk/tchk-expand.c	\
		$GEN.gen/config.h		\
			$GEN.gen/tm.h		\
			$GEN.gen/tm-$GEN.h	\
		h/tree.h			\
			h/tree.def.v		\
			h/machmode.def.v	\
			h/real.h		\
		h/c-tree.h			\
		h/flags.h			\
		h/rtl.h				\
			h/rtl.def.v		\
			h/machmode.def.v
		$DST_CC -c -O -o $DST_OBJ tchk/tchk-expand.c

$DST_OBJ/c-convert.o: src/c-convert.c		\
		$GEN.gen/config.h		\
			$GEN.gen/tm.h		\
			$GEN.gen/tm-$GEN.h	\
		h/tree.h			\
			h/tree.def.v		\
			h/machmode.def.v	\
			h/real.h
		$DST_CC -c -O -o $DST_OBJ src/c-convert.c

$DST_OBJ/stor-layout.o: src/stor-layout.c 	\
		$GEN.gen/config.h		\
			$GEN.gen/tm.h		\
			$GEN.gen/tm-$GEN.h	\
		h/tree.h			\
			h/tree.def.v		\
			h/machmode.def.v	\
			h/real.h		\
		h/rtl.h				\
			h/rtl.def.v		\
			h/machmode.def.v
		$DST_CC -c -O -o $DST_OBJ src/stor-layout.c

$DST_OBJ/fold-const.o: src/fold-const.c 	\
		$GEN.gen/config.h		\
			$GEN.gen/tm.h		\
			$GEN.gen/tm-$GEN.h	\
		h/tree.h			\
			h/tree.def.v		\
			h/machmode.def.v	\
			h/real.h
		$DST_CC -c -O -o $DST_OBJ src/fold-const.c

$DST_OBJ/dst_rtl.o:	src/rtl.c		\
		$GEN.gen/config.h		\
			$GEN.gen/tm.h		\
			$GEN.gen/tm-$GEN.h	\
		h/rtl.h				\
			h/rtl.def.v		\
			h/machmode.def.v	\
		h/obstack.h			\
		h/rtl.def.v			\
		h/machmode.def.v
		$DST_CC -c -O -o $DST_OBJ/dst_rtl.o src/rtl.c
		ln -f $DST_OBJ/dst_rtl.o $DST_OBJ/src_rtl.o 

$SRC_OBJ/src_rtl.o:	src/rtl.c		\
		$GEN.gen/config.h		\
			$GEN.gen/tm.h		\
			$GEN.gen/tm-$GEN.h	\
		h/rtl.h				\
			h/rtl.def.v		\
			h/machmode.def.v	\
		h/obstack.h			\
		h/rtl.def.v			\
		h/machmode.def.v
		$SRC_CC -c -O -o $SRC_OBJ/src_rtl.o src/rtl.c
		ln -f $SRC_OBJ/src_rtl.o $SRC_OBJ/dst_rtl.o

$DST_OBJ/rtlanal.o:	src/rtlanal.c		\
		$GEN.gen/config.h		\
		h/rtl.h				\
			h/rtl.def.v		\
			h/machmode.def.v
		$DST_CC -c -O -o $DST_OBJ src/rtlanal.c

$DST_OBJ/expr.o:	src/expr.c		\
		$GEN.gen/config.h		\
			$GEN.gen/tm.h		\
			$GEN.gen/tm-$GEN.h	\
		h/rtl.h				\
			h/rtl.def.v		\
			h/machmode.def.v	\
		h/tree.h			\
			h/tree.def.v		\
			h/machmode.def.v	\
			h/real.h		\
		h/flags.h			\
		$GEN.gen/insn-flags.h		\
		$GEN.gen/insn-codes.h		\
		h/expr.h			\
			$GEN.gen/insn-codes.h	\
		$GEN.gen/insn-config.h		\
		h/recog.h			\
		h/varargs.h			\
		h/typeclass.h			\
		h/tree.def.v
		$DST_CC -c -O -o $DST_OBJ src/expr.c

$DST_OBJ/stmt.o:	src/stmt.c		\
		$GEN.gen/config.h		\
			$GEN.gen/tm.h		\
			$GEN.gen/tm-$GEN.h	\
		h/rtl.h				\
			h/rtl.def.v		\
			h/machmode.def.v	\
		h/tree.h			\
			h/tree.def.v		\
			h/machmode.def.v	\
			h/real.h		\
		h/flags.h			\
		$GEN.gen/insn-flags.h		\
		$GEN.gen/insn-config.h		\
		$GEN.gen/insn-codes.h		\
		h/expr.h			\
			$GEN.gen/insn-codes.h	\
		h/regs.h			\
		h/hard-reg-set.h		\
		h/recog.h
		$DST_CC -c -O -o $DST_OBJ src/stmt.c

$DST_OBJ/expmed.o:	src/expmed.c		\
		$GEN.gen/config.h		\
			$GEN.gen/tm.h		\
			$GEN.gen/tm-$GEN.h	\
		h/rtl.h				\
			h/rtl.def.v		\
			h/machmode.def.v	\
		h/tree.h			\
			h/tree.def.v		\
			h/machmode.def.v	\
			h/real.h		\
		h/flags.h			\
		$GEN.gen/insn-flags.h		\
		$GEN.gen/insn-codes.h		\
		$GEN.gen/insn-config.h		\
		h/expr.h			\
			$GEN.gen/insn-codes.h	\
		h/recog.h
		$DST_CC -c -O -o $DST_OBJ src/expmed.c

$DST_OBJ/explow.o:	src/explow.c		\
		$GEN.gen/config.h		\
			$GEN.gen/tm.h		\
			$GEN.gen/tm-$GEN.h	\
		h/rtl.h				\
			h/rtl.def.v		\
			h/machmode.def.v	\
		h/tree.h			\
			h/tree.def.v		\
			h/machmode.def.v	\
			h/real.h		\
		h/flags.h			\
		h/expr.h			\
			$GEN.gen/insn-codes.h
		$DST_CC -c -O -o $DST_OBJ src/explow.c

$DST_OBJ/optabs.o:	src/optabs.c		\
		$GEN.gen/config.h		\
			$GEN.gen/tm.h		\
			$GEN.gen/tm-$GEN.h	\
		h/rtl.h				\
			h/rtl.def.v		\
			h/machmode.def.v	\
		h/tree.h			\
			h/tree.def.v		\
			h/machmode.def.v	\
			h/real.h		\
		h/flags.h			\
		$GEN.gen/insn-flags.h		\
		$GEN.gen/insn-codes.h		\
		h/expr.h			\
			$GEN.gen/insn-codes.h	\
		$GEN.gen/insn-config.h		\
		h/recog.h
		$DST_CC -c -O -o $DST_OBJ src/optabs.c

$DST_OBJ/varasm.o:	src/varasm.c		\
		$GEN.gen/config.h		\
			$GEN.gen/tm.h		\
			$GEN.gen/tm-$GEN.h	\
		h/rtl.h				\
			h/rtl.def.v		\
			h/machmode.def.v	\
		h/tree.h			\
			h/tree.def.v		\
			h/machmode.def.v	\
			h/real.h		\
		h/flags.h			\
		h/expr.h			\
			$GEN.gen/insn-codes.h	\
		h/hard-reg-set.h		\
		h/obstack.h
		$DST_CC -c -O -o $DST_OBJ src/varasm.c

$DST_OBJ/symout.o:	src/symout.c		\
		$GEN.gen/config.h		\
			$GEN.gen/tm.h		\
			$GEN.gen/tm-$GEN.h	\
		h/tree.h			\
			h/tree.def.v		\
			h/machmode.def.v	\
			h/real.h		\
		h/symseg.h			\
		h/rtl.h				\
			h/rtl.def.v		\
			h/machmode.def.v	\
		h/gdbfiles.h			\
		h/stab.h
		$DST_CC -c -O -o $DST_OBJ src/symout.c

$DST_OBJ/dbxout.o:	src/dbxout.c		\
		$GEN.gen/config.h		\
			$GEN.gen/tm.h		\
			$GEN.gen/tm-$GEN.h	\
		h/tree.h			\
			h/tree.def.v		\
			h/machmode.def.v	\
			h/real.h		\
		h/rtl.h				\
			h/rtl.def.v		\
			h/machmode.def.v	\
		h/flags.h			\
		h/stab.h
		$DST_CC -c -O -o $DST_OBJ src/dbxout.c

$DST_OBJ/emit-rtl.o:	src/emit-rtl.c		\
		$GEN.gen/config.h		\
			$GEN.gen/tm.h		\
			$GEN.gen/tm-$GEN.h	\
		h/varargs.h			\
		h/rtl.h				\
			h/rtl.def.v		\
			h/machmode.def.v	\
		h/regs.h			\
		$GEN.gen/insn-config.h		\
		h/real.h
		$DST_CC -c -O -o $DST_OBJ src/emit-rtl.c

$DST_OBJ/insn-emit.o: $GEN.gen/insn-emit.c	\
		$GEN.gen/config.h		\
			$GEN.gen/tm.h		\
			$GEN.gen/tm-$GEN.h	\
		h/rtl.h				\
			h/rtl.def.v		\
			h/machmode.def.v	\
		h/expr.h			\
			$GEN.gen/insn-codes.h	\
		h/real.h			\
		$GEN.gen/insn-config.h		\
		$GEN.gen/insn-flags.h
		$DST_CC -c -O -o $DST_OBJ $GEN.gen/insn-emit.c

$DST_OBJ/integrate.o: src/integrate.c		\
		$GEN.gen/config.h		\
			$GEN.gen/tm.h		\
			$GEN.gen/tm-$GEN.h	\
		h/rtl.h				\
			h/rtl.def.v		\
			h/machmode.def.v	\
		h/tree.h			\
			h/tree.def.v		\
			h/machmode.def.v	\
			h/real.h		\
		h/flags.h			\
		$GEN.gen/insn-flags.h		\
		h/expr.h			\
			$GEN.gen/insn-codes.h	\
		h/obstack.h
		$DST_CC -c -O -o $DST_OBJ src/integrate.c

$DST_OBJ/jump.o:	src/jump.c		\
		$GEN.gen/config.h		\
			$GEN.gen/tm.h		\
			$GEN.gen/tm-$GEN.h	\
		h/rtl.h				\
			h/rtl.def.v		\
			h/machmode.def.v	\
		h/flags.h			\
		h/regs.h
		$DST_CC -c -O -o $DST_OBJ src/jump.c

$DST_OBJ/cse.o:	src/cse.c			\
		$GEN.gen/config.h		\
			$GEN.gen/tm.h		\
			$GEN.gen/tm-$GEN.h	\
		h/rtl.h				\
			h/rtl.def.v		\
			h/machmode.def.v	\
		h/regs.h			\
		h/hard-reg-set.h		\
		h/flags.h			\
		h/real.h
		$DST_CC -c -O -o $DST_OBJ src/cse.c

$DST_OBJ/loop.o:	src/loop.c		\
		$GEN.gen/config.h		\
			$GEN.gen/tm.h		\
			$GEN.gen/tm-$GEN.h	\
		h/rtl.h				\
			h/rtl.def.v		\
			h/machmode.def.v	\
		h/expr.h			\
			$GEN.gen/insn-codes.h	\
		$GEN.gen/insn-config.h		\
		h/regs.h			\
		h/hard-reg-set.h		\
		h/recog.h			\
		h/flags.h
		$DST_CC -c -O -o $DST_OBJ src/loop.c

$DST_OBJ/flow.o:	src/flow.c		\
		$GEN.gen/config.h		\
			$GEN.gen/tm.h		\
			$GEN.gen/tm-$GEN.h	\
		h/rtl.h				\
			h/rtl.def.v		\
			h/machmode.def.v	\
		h/basic-block.h			\
		h/regs.h			\
		h/hard-reg-set.h		\
		h/flags.h			\
		h/obstack.h
		$DST_CC -c -O -o $DST_OBJ src/flow.c

$DST_OBJ/stupid.o:	src/stupid.c		\
		$GEN.gen/config.h		\
			$GEN.gen/tm.h		\
			$GEN.gen/tm-$GEN.h	\
		h/rtl.h				\
			h/rtl.def.v		\
			h/machmode.def.v	\
		h/hard-reg-set.h		\
		h/regs.h
		$DST_CC -c -O -o $DST_OBJ src/stupid.c

$DST_OBJ/combine.o:	src/combine.c		\
		$GEN.gen/config.h		\
			$GEN.gen/tm.h		\
			$GEN.gen/tm-$GEN.h	\
		h/rtl.h				\
			h/rtl.def.v		\
			h/machmode.def.v	\
		h/flags.h			\
		h/regs.h			\
		h/basic-block.h			\
		$GEN.gen/insn-config.h		\
		h/recog.h
		$DST_CC -c -O -o $DST_OBJ src/combine.c

$DST_OBJ/regclass.o:	src/regclass.c		\
		$GEN.gen/config.h		\
			$GEN.gen/tm.h		\
			$GEN.gen/tm-$GEN.h	\
		h/rtl.h				\
			h/rtl.def.v		\
			h/machmode.def.v	\
		h/hard-reg-set.h		\
		h/flags.h			\
		h/basic-block.h			\
		h/regs.h			\
		$GEN.gen/insn-config.h		\
		h/recog.h
		$DST_CC -c -O -o $DST_OBJ src/regclass.c

$DST_OBJ/local-alloc.o: src/local-alloc.c 	\
		$GEN.gen/config.h		\
			$GEN.gen/tm.h		\
			$GEN.gen/tm-$GEN.h	\
		h/rtl.h				\
			h/rtl.def.v		\
			h/machmode.def.v	\
		h/flags.h			\
		h/basic-block.h			\
		h/regs.h			\
		h/hard-reg-set.h		\
		$GEN.gen/insn-config.h		\
		h/recog.h
		$DST_CC -c -O -o $DST_OBJ src/local-alloc.c

$DST_OBJ/global-alloc.o: src/global-alloc.c 	\
		$GEN.gen/config.h		\
			$GEN.gen/tm.h		\
			$GEN.gen/tm-$GEN.h	\
		h/rtl.h				\
			h/rtl.def.v		\
			h/machmode.def.v	\
		h/flags.h			\
		h/basic-block.h			\
		h/hard-reg-set.h		\
		h/regs.h			\
		$GEN.gen/insn-config.h
		$DST_CC -c -O -o $DST_OBJ src/global-alloc.c

$DST_OBJ/reload.o:	src/reload.c		\
		$GEN.gen/config.h		\
			$GEN.gen/tm.h		\
			$GEN.gen/tm-$GEN.h	\
		h/rtl.h				\
			h/rtl.def.v		\
			h/machmode.def.v	\
		$GEN.gen/insn-config.h		\
		h/recog.h			\
		h/reload.h			\
		h/regs.h			\
		h/hard-reg-set.h		\
		h/flags.h			\
		h/real.h
		$DST_CC -c -O -o $DST_OBJ src/reload.c

$DST_OBJ/reload1.o:	src/reload1.c		\
		$GEN.gen/config.h		\
			$GEN.gen/tm.h		\
			$GEN.gen/tm-$GEN.h	\
		h/rtl.h				\
			h/rtl.def.v		\
			h/machmode.def.v	\
		$GEN.gen/insn-config.h		\
		h/flags.h			\
		h/regs.h			\
		h/hard-reg-set.h		\
		h/reload.h			\
		h/recog.h			\
		h/basic-block.h
		$DST_CC -c -O -o $DST_OBJ src/reload1.c

$DST_OBJ/insn-peep.o: $GEN.gen/insn-peep.c 	\
		$GEN.gen/config.h		\
			$GEN.gen/tm.h		\
			$GEN.gen/tm-$GEN.h	\
		h/rtl.h				\
			h/rtl.def.v		\
			h/machmode.def.v	\
		h/regs.h			\
		h/real.h
		$DST_CC -c -O -o $DST_OBJ $GEN.gen/insn-peep.c

$DST_OBJ/recog.o:	src/recog.c		\
		$GEN.gen/config.h		\
			$GEN.gen/tm.h		\
			$GEN.gen/tm-$GEN.h	\
		h/rtl.h				\
			h/rtl.def.v		\
			h/machmode.def.v	\
		$GEN.gen/insn-config.h		\
		h/recog.h			\
		h/regs.h			\
		h/hard-reg-set.h		\
		h/real.h
		$DST_CC -c -O -o $DST_OBJ src/recog.c

$DST_OBJ/final.o:	src/final.c		\
		$GEN.gen/config.h		\
			$GEN.gen/tm.h		\
			$GEN.gen/tm-$GEN.h	\
		h/rtl.h				\
			h/rtl.def.v		\
			h/machmode.def.v	\
		h/regs.h			\
		$GEN.gen/insn-config.h		\
		h/recog.h			\
		h/conditions.h			\
		h/gdbfiles.h			\
		h/flags.h			\
		h/real.h			\
		h/output.h			\
		h/stab.h
		$DST_CC -c -O -o $DST_OBJ src/final.c

$DST_OBJ/tropix.final.o: $GEN.gen/tropix.final.c \
		$GEN.gen/config.h		\
			$GEN.gen/tm.h		\
			$GEN.gen/tm-$GEN.h	\
		h/rtl.h				\
			h/rtl.def.v		\
			h/machmode.def.v	\
		h/regs.h			\
		$GEN.gen/insn-config.h		\
		h/recog.h			\
		h/conditions.h			\
		h/gdbfiles.h			\
		h/flags.h			\
		h/real.h			\
		h/output.h			\
		h/stab.h
		$DST_CC -c -O -o $DST_OBJ $GEN.gen/tropix.final.c

$DST_OBJ/insn-recog.o: $GEN.gen/insn-recog.c 	\
		$GEN.gen/config.h		\
			$GEN.gen/tm.h		\
			$GEN.gen/tm-$GEN.h	\
		h/rtl.h				\
			h/rtl.def.v		\
			h/machmode.def.v	\
		$GEN.gen/insn-config.h		\
		h/recog.h			\
		h/real.h
		$DST_CC -c -O -o $DST_OBJ $GEN.gen/insn-recog.c

$DST_OBJ/insn-extract.o: $GEN.gen/insn-extract.c \
		$GEN.gen/config.h		\
			$GEN.gen/tm.h		\
			$GEN.gen/tm-$GEN.h	\
		h/rtl.h				\
			h/rtl.def.v		\
			h/machmode.def.v
		$DST_CC -c -O -o $DST_OBJ $GEN.gen/insn-extract.c

$DST_OBJ/insn-output.o: $GEN.gen/insn-output.c	\
		$GEN.gen/config.h		\
			$GEN.gen/tm.h		\
			$GEN.gen/tm-$GEN.h	\
		h/rtl.h				\
			h/rtl.def.v		\
			h/machmode.def.v	\
		h/regs.h			\
		h/hard-reg-set.h		\
		h/real.h			\
		h/conditions.h			\
		$GEN.gen/insn-flags.h		\
		$GEN.gen/insn-config.h		\
		h/output.h			\
		$GEN.gen/aux-output.c
#		$DST_CC -c -o $DST_OBJ $GEN.gen/insn-output.c
		$DST_CC -c -O -o $DST_OBJ $GEN.gen/insn-output.c

$SRC_OBJ/genflags.o:	src/genflags.c		\
		$GEN.gen/config.h		\
			$GEN.gen/tm.h		\
			$GEN.gen/tm-$GEN.h	\
		h/rtl.h				\
			h/rtl.def.v		\
			h/machmode.def.v	\
		h/obstack.h
		$SRC_CC -c -O -o $SRC_OBJ src/genflags.c

$SRC_OBJ/gencodes.o:	src/gencodes.c		\
		$GEN.gen/config.h		\
			$GEN.gen/tm.h		\
			$GEN.gen/tm-$GEN.h	\
		h/rtl.h				\
			h/rtl.def.v		\
			h/machmode.def.v	\
		h/obstack.h
		$SRC_CC -c -O -o $SRC_OBJ src/gencodes.c

$SRC_OBJ/genconfig.o: src/genconfig.c		\
		$GEN.gen/config.h		\
			$GEN.gen/tm.h		\
			$GEN.gen/tm-$GEN.h	\
		h/rtl.h				\
			h/rtl.def.v		\
			h/machmode.def.v	\
		h/obstack.h
		$SRC_CC -c -O -o $SRC_OBJ src/genconfig.c

$DST_OBJ/dst_obstack.o:	src/obstack.c		\
		h/obstack.h
		$DST_CC -c -O -o $DST_OBJ/dst_obstack.o src/obstack.c
		ln -f $DST_OBJ/dst_obstack.o $DST_OBJ/src_obstack.o

$SRC_OBJ/src_obstack.o:	src/obstack.c		\
		h/obstack.h
		$SRC_CC -c -O -o $SRC_OBJ/src_obstack.o src/obstack.c
		ln -f $SRC_OBJ/src_obstack.o $SRC_OBJ/dst_obstack.o

$SRC_OBJ/genemit.o:	src/genemit.c		\
		$GEN.gen/config.h		\
			$GEN.gen/tm.h		\
			$GEN.gen/tm-$GEN.h	\
		h/rtl.h				\
			h/rtl.def.v		\
			h/machmode.def.v	\
		h/obstack.h
		$SRC_CC -c -O -o $SRC_OBJ src/genemit.c

$SRC_OBJ/genpeep.o:	src/genpeep.c		\
		$GEN.gen/config.h		\
			$GEN.gen/tm.h		\
			$GEN.gen/tm-$GEN.h	\
		h/rtl.h				\
			h/rtl.def.v		\
			h/machmode.def.v	\
		h/obstack.h
		$SRC_CC -c -O -o $SRC_OBJ src/genpeep.c

$SRC_OBJ/genrecog.o:	src/genrecog.c		\
		$GEN.gen/config.h		\
			$GEN.gen/tm.h		\
			$GEN.gen/tm-$GEN.h	\
		h/rtl.h				\
			h/rtl.def.v		\
			h/machmode.def.v	\
		h/obstack.h
		$SRC_CC -c -O -o $SRC_OBJ src/genrecog.c

$SRC_OBJ/genextract.o: src/genextract.c 	\
		$GEN.gen/config.h		\
			$GEN.gen/tm.h		\
			$GEN.gen/tm-$GEN.h	\
		h/rtl.h				\
			h/rtl.def.v		\
			h/machmode.def.v	\
		h/obstack.h
		$SRC_CC -c -O -o $SRC_OBJ src/genextract.c

$SRC_OBJ/genoutput.o: src/genoutput.c		\
		$GEN.gen/config.h		\
			$GEN.gen/tm.h		\
			$GEN.gen/tm-$GEN.h	\
		h/rtl.h				\
			h/rtl.def.v		\
			h/machmode.def.v	\
		h/obstack.h
		$SRC_CC -c -O -o $SRC_OBJ src/genoutput.c

$DST_OBJ/caller-save.o: src/caller-save.c 	\
		$GEN.gen/config.h		\
			$GEN.gen/tm.h		\
			$GEN.gen/tm-$GEN.h	\
		h/rtl.h				\
			h/rtl.def.v		\
			h/machmode.def.v	\
		$GEN.gen/insn-config.h		\
		h/flags.h			\
		h/regs.h			\
		h/hard-reg-set.h		\
		h/reload.h			\
		h/recog.h			\
		h/basic-block.h
		$DST_CC -c -O -o $DST_OBJ src/caller-save.c

$DST_OBJ/bcopy.o:	src/bcopy.c
		$DST_CC -c -O -o $DST_OBJ src/bcopy.c

#*
#******	Comados diversos ****************************************
#*
lint:		$ATUAL
		$DST_CC -T $ATUAL

cp:		$COMP
#		untext $COMP
		cp -f $COMP $DIR
		chmod $PROT $DIR
		ls -L $DIR
#		strip $DIR
		$DIR/$COMP -M

cmp:
		-cmpobj $COMP $DIR

cmpq:
		-cmpobj -q $COMP $DIR

cmpqcp:
		-cmpobj -q $COMP $DIR || make cp

sz:		prsz
		prsz

prsz:		prsz.c
		$DST_CC -f -O -s -o prsz prsz.c

lintsz:
		$DST_CC -T prsz.c

nm:
		nm -otn	$DST_OBJ/*.o >nm_list

NM:
		nm -o	$DST_OBJ/*.o >NM_list

man:		man.r
		sroff man.r >$mandir/$MAN/$PGM
		man $PGM

modif:		modif.r
		sroff modif.r >$mandir/modif/$MAN/$PGM
		modif $PGM

prman:
		sroff man.r | lpr

#*
#******	Criação do "back up" ************************************
#*
gar1:
	gar -ovs8	src
	gar -icv8

gar2:
	gar -ov8	Makefile		\
			h 			\
			tree			\
			tchk			\
			dcl			\
			top			\
			lex/etc.c		\
			lex/scan.c		\
						\
			config.*		\
						\
			m68k.gen/config.h	\
			m68k.gen/tm.h		\
			m68k.gen/tm-m68k.h	\
			m68k.gen/md.v		\
			m68k.gen/aux-output.c	\
			m68k.gen/tropix.final.c	\
						\
			i486.gen/config.h	\
			i486.gen/tm.h		\
			i486.gen/tm-i486.h	\
			i486.gen/md.v		\
			i486.gen/aux-output.c	\
			i486.gen/tropix.final.c	\
						\
			cp.c save.cp.s
	gar -icv8

gar11:
	gar -ov8	h/basic-block.h		\
			h/c-parse.h		\
			h/c-tree.h		\
			h/conditions.h		\
			h/expr.h		\
			h/flags.h		\
			h/gdbfiles.h		\
			h/hard-reg-set.h	\
			h/input.h		\
			h/machmode.def.v		\
			h/obstack.h		\
			h/output.h		\
			h/real.h		\
			h/recog.h		\
			h/regs.h		\
			h/reload.h		\
			h/rtl.def.v		\
			h/rtl.h			\
			h/stab.h		\
			h/stddef.h		\
			h/symseg.h		\
			h/tree.def.v		\
			h/tree.h		\
			h/typeclass.h		\
			h/varargs.h		\
			h/machmode.def.v		\
			h/rtl.def.v		\
			h/tree.def.v		\
			src/bcopy.c		\
			src/c-convert.c		\
			src/c-parse.tab.c	\
			src/c-parse.tab.h	\
			src/c-parse.y		\
			src/caller-save.c	\
			src/combine.c		\
			src/cse.c		\
			src/dbxout.c		\
			src/emit-rtl.c		\
			src/explow.c		\
			src/expmed.c	

gar12:
	gar -ovs9	src/expr.c		\
			src/final.c		\
			src/flow.c		\
			src/fold-const.c	\
			src/gencodes.c		\
			src/genconfig.c		\
			src/genemit.c		\
			src/genextract.c	\
			src/genflags.c		\
			src/genoutput.c		\
			src/genpeep.c		\
			src/genrecog.c		\
			src/global-alloc.c	\
			src/integrate.c		\
			src/jump.c		\
			src/local-alloc.c	\
			src/loop.c		\
			src/obstack.c	

gar13:
	gar -ovs8	src/optabs.c		\
			src/print-tree.c	\
			src/recog.c		\
			src/regclass.c		\
			src/reload.c		\
			src/reload1.c		\
			src/rtl.c		\
			src/rtlanal.c		\
			src/stmt.c		\
			src/stor-layout.c	\
			src/stupid.c		\
			src/symout.c		\
			src/varasm.c		\
			src/version.c	

gar14:
		gar -ov8			\
			Makefile		\
			config.*		\
						\
			m68k.gen/config.h	\
			m68k.gen/tm.h		\
			m68k.gen/tm-m68k.h	\
			m68k.gen/md.v		\
			m68k.gen/aux-output.c	\
			m68k.gen/tropix.final.c	\
						\
			i486.gen/config.h	\
			i486.gen/tm.h		\
			i486.gen/tm-i486.h	\
			i486.gen/md.v		\
			i486.gen/aux-output.c	\
			i486.gen/tropix.final.c	\

		gar -icv8

gar15:
		gar -ov9			\
			Makefile		\
			tree			\
			tchk			\
			dcl			\
			top			\
			lex/etc.c		\
			lex/scan.c
		gar -icv9
