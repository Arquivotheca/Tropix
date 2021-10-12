#****************************************************************
#*	"makefile" para o programa SuperProbe			*
#****************************************************************
#
PGM	=	superprobe
DIR	=	/usr/xwin/bin
MAN	=	xwin
PROT	=	a=srx,u+w

CC	=	cc
CFLAGS	=	-O -c -o obj
DEF	=	-D NeedFunctionPrototypes
INC	=	-I h

SRC	=	src/Main.o		\
		src/Utils.c		\
		src/os.c		\
		hw/8514.c		\
		hw/AL.c			\
		hw/ARK.c		\
		hw/ATI.c		\
		hw/ATIMach.c		\
		hw/Ahead.c		\
		hw/Alliance.c		\
		hw/CGA.c		\
		hw/ChipsTech.c		\
		hw/Cirrus.c		\
		hw/Compaq.c		\
		hw/Epson.c		\
		hw/GLINT.c		\
		hw/Genoa.c		\
		hw/HMC.c		\
		hw/Herc.c		\
		hw/I128.c		\
		hw/Intergra.c		\
		hw/MDA.c		\
		hw/MX.c			\
		hw/Matrox.c		\
		hw/MemProbe.c		\
		hw/NCR.c		\
		hw/Oak.c		\
		hw/PCI.c		\
		hw/Primus.c		\
		hw/Print.c		\
		hw/RamDac.c		\
		hw/RealTek.c		\
		hw/S3.c			\
		hw/SiS.c		\
		hw/SigmaDesigns.c	\
		hw/Trident.c		\
		hw/Tseng.c		\
		hw/UMC.c		\
		hw/VGA.c		\
		hw/Video7.c		\
		hw/WD.c			\
		hw/Weitek.c		\
		hw/Yamaha.c

OBJ	=	obj/Main.o		\
		obj/8514.o		\
		obj/AL.o		\
		obj/ARK.o		\
		obj/ATI.o		\
		obj/ATIMach.o		\
		obj/Ahead.o		\
		obj/Alliance.o		\
		obj/CGA.o		\
		obj/ChipsTech.o		\
		obj/Cirrus.o		\
		obj/Compaq.o		\
		obj/Epson.o		\
		obj/GLINT.o		\
		obj/Genoa.o		\
		obj/HMC.o		\
		obj/Herc.o		\
		obj/I128.o		\
		obj/Intergra.o		\
		obj/MDA.o		\
		obj/MX.o		\
		obj/Matrox.o		\
		obj/MemProbe.o		\
		obj/NCR.o		\
		obj/Oak.o		\
		obj/PCI.o		\
		obj/Primus.o		\
		obj/Print.o		\
		obj/RamDac.o		\
		obj/RealTek.o		\
		obj/S3.o		\
		obj/SiS.o		\
		obj/SigmaDesigns.o	\
		obj/Trident.o		\
		obj/Tseng.o		\
		obj/UMC.o		\
		obj/VGA.o		\
		obj/Video7.o		\
		obj/WD.o		\
		obj/Weitek.o		\
		obj/Yamaha.o		\
		obj/Utils.o		\
		obj/os.o		\
		obj/inout.o

.virtual	cp cmp cmpq cmpqcp unld lint man modif clean rmobj

$PGM:		obj $OBJ
		cc -s -o $PGM $OBJ
		size $PGM

obj:		.exist
		mkdir -f obj
#
#****************************************************************
#*	Compilação dos Diversos Módulos				*
#****************************************************************
#
obj/8514.o:		hw/8514.c h/Probe.h
			$CC $CFLAGS $DEF $INC hw/8514.c

obj/AL.o:		hw/AL.c h/Probe.h
			$CC $CFLAGS $DEF $INC hw/AL.c

obj/ARK.o:		hw/ARK.c h/Probe.h
			$CC $CFLAGS $DEF $INC hw/ARK.c

obj/ATI.o:		hw/ATI.c h/Probe.h
			$CC $CFLAGS $DEF $INC hw/ATI.c

obj/ATIMach.o:		hw/ATIMach.c h/Probe.h
			$CC $CFLAGS $DEF $INC hw/ATIMach.c

obj/Ahead.o:		hw/Ahead.c h/Probe.h
			$CC $CFLAGS $DEF $INC hw/Ahead.c

obj/Alliance.o:		hw/Alliance.c h/Probe.h
			$CC $CFLAGS $DEF $INC hw/Alliance.c

obj/CGA.o:		hw/CGA.c h/Probe.h
			$CC $CFLAGS $DEF $INC hw/CGA.c

obj/ChipsTech.o:	hw/ChipsTech.c h/Probe.h
			$CC $CFLAGS $DEF $INC hw/ChipsTech.c

obj/Cirrus.o:		hw/Cirrus.c h/Probe.h
			$CC $CFLAGS $DEF $INC hw/Cirrus.c

obj/Compaq.o:		hw/Compaq.c h/Probe.h
			$CC $CFLAGS $DEF $INC hw/Compaq.c

obj/Epson.o:		hw/Epson.c h/Probe.h
			$CC $CFLAGS $DEF $INC hw/Epson.c

obj/GLINT.o:		hw/GLINT.c h/Probe.h
			$CC $CFLAGS $DEF $INC hw/GLINT.c

obj/Genoa.o:		hw/Genoa.c h/Probe.h
			$CC $CFLAGS $DEF $INC hw/Genoa.c

obj/HMC.o:		hw/HMC.c h/Probe.h
			$CC $CFLAGS $DEF $INC hw/HMC.c

obj/Herc.o:		hw/Herc.c h/Probe.h
			$CC $CFLAGS $DEF $INC hw/Herc.c

obj/I128.o:		hw/I128.c h/Probe.h
			$CC $CFLAGS $DEF $INC hw/I128.c

obj/Intergra.o:		hw/Intergra.c h/Probe.h
			$CC $CFLAGS $DEF $INC hw/Intergra.c

obj/MDA.o:		hw/MDA.c h/Probe.h
			$CC $CFLAGS $DEF $INC hw/MDA.c

obj/MX.o:		hw/MX.c h/Probe.h
			$CC $CFLAGS $DEF $INC hw/MX.c

obj/Matrox.o:		hw/Matrox.c h/Probe.h
			$CC $CFLAGS $DEF $INC hw/Matrox.c

obj/MemProbe.o:		hw/MemProbe.c h/Probe.h
			$CC $CFLAGS $DEF $INC hw/MemProbe.c

obj/NCR.o:		hw/NCR.c h/Probe.h
			$CC $CFLAGS $DEF $INC hw/NCR.c

obj/Oak.o:		hw/Oak.c h/Probe.h
			$CC $CFLAGS $DEF $INC hw/Oak.c

obj/PCI.o:		hw/PCI.c h/Probe.h
			$CC $CFLAGS $DEF $INC hw/PCI.c

obj/Primus.o:		hw/Primus.c h/Probe.h
			$CC $CFLAGS $DEF $INC hw/Primus.c

obj/RamDac.o:		hw/RamDac.c h/Probe.h
			$CC $CFLAGS $DEF $INC hw/RamDac.c

obj/RealTek.o:		hw/RealTek.c h/Probe.h
			$CC $CFLAGS $DEF $INC hw/RealTek.c

obj/S3.o:		hw/S3.c h/Probe.h
			$CC $CFLAGS $DEF $INC hw/S3.c

obj/SiS.o:		hw/SiS.c h/Probe.h
			$CC $CFLAGS $DEF $INC hw/SiS.c

obj/SigmaDesigns.o:	hw/SigmaDesigns.c h/Probe.h
			$CC $CFLAGS $DEF $INC hw/SigmaDesigns.c

obj/Trident.o:		hw/Trident.c h/Probe.h
			$CC $CFLAGS $DEF $INC hw/Trident.c

obj/Tseng.o:		hw/Tseng.c h/Probe.h
			$CC $CFLAGS $DEF $INC hw/Tseng.c

obj/UMC.o:		hw/UMC.c h/Probe.h
			$CC $CFLAGS $DEF $INC hw/UMC.c

obj/VGA.o:		hw/VGA.c h/Probe.h
			$CC $CFLAGS $DEF $INC hw/VGA.c

obj/Video7.o:		hw/Video7.c h/Probe.h
			$CC $CFLAGS $DEF $INC hw/Video7.c

obj/WD.o:		hw/WD.c h/Probe.h
			$CC $CFLAGS $DEF $INC hw/WD.c

obj/Weitek.o:		hw/Weitek.c h/Probe.h
			$CC $CFLAGS $DEF $INC hw/Weitek.c

obj/Yamaha.o:		hw/Yamaha.c h/Probe.h
			$CC $CFLAGS $DEF $INC hw/Yamaha.c

obj/Main.o:		src/Main.c h/Probe.h
			$CC $CFLAGS $DEF $INC src/Main.c

obj/Print.o:		src/Print.c h/Probe.h
			$CC $CFLAGS $DEF $INC src/Print.c

obj/Utils.o:		src/Utils.c h/Probe.h
			$CC $CFLAGS $DEF $INC src/Utils.c

obj/os.o:		src/os.c h/Probe.h
			$CC $CFLAGS $DEF $INC src/os.c

obj/inout.o:		src/inout.s
			as -o $@ src/inout.s

#
#****************************************************************
#*	Cópia/Comparação/Manuais				*
#****************************************************************
#
cp:		$PGM $DIR
		cp -f $PGM $DIR/$PGM
		chmod $PROT $DIR/$PGM
		ls -L $DIR/$PGM

$DIR:		.exist
		mkdir -f $DIR

cmp:
		-cmpobj $PGM $DIR/$PGM

cmpq:
		-cmpobj -q $PGM $DIR/$PGM

cmpqcp:
		-cmpobj -q $PGM $DIR/$PGM  ||  make cp

unld:
		-rm -f $PGM

lint:		$SRC
		cc -T $INC $DEF $SRC

man:		man.r
		sroff man.r >$mandir/$MAN/$PGM
		man $PGM

modif:		modif.r
		sroff modif.r >$mandir/modif/$MAN/$PGM
		modif $PGM

clean rmobj:
		walk -. -p "*.o" -p "*/.LIBSYM" . | rm -f
