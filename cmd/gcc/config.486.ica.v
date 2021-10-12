GEN=i486;			export GEN;
COMP=gnu.cc.486.ica;		export COMP;

DST_CC=cc;			export DST_CC;
SRC_CC=cc;			export SRC_CC;

DST_OBJ=i486.ica.obj;		export DST_OBJ
SRC_OBJ=i486.ica.obj;		export SRC_OBJ

LD=ld;				export LD

#CC=gcc.486;			export CC;
#OBJ=i486.pc.obj;		export OBJ
#src=src/varasm.c;		export src;
#obj=m68k.obj/varasm.o;		export obj;
#out=m68k.gen/insn-output.c;	export out;

gen="$GEN.gen/config.h			\
	 $GEN.gen/tm.h			\
	 $GEN.gen/tm-$GEN.h		\
	 $GEN.gen/md			\
	 $GEN.gen/aux-output.c		\
	 $GEN.gen/tropix.final.c";
	 export gen;
