GEN=m68k;			export GEN;
COMP=gnu.cc.m68k.pc;		export COMP;

DST_CC=cc;			export DST_CC;
SRC_CC=cc;			export SRC_CC;

DST_OBJ=m68k.pc.obj;		export DST_OBJ
SRC_OBJ=m68k.pc.obj;		export SRC_OBJ

LD=ld;			export LD

gen="$GEN.gen/config.h			\
	 $GEN.gen/tm.h			\
	 $GEN.gen/tm-$GEN.h		\
	 $GEN.gen/md			\
	 $GEN.gen/aux-output.c		\
	 $GEN.gen/tropix.final.c";
	 export gen;
