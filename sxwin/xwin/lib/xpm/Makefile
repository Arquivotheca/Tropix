#*
#****************************************************************
#	Makefile para a biblioteca "lib/xpm"			*
#****************************************************************
#*
INC	=	-I ../../include

DEF	=	-D TROPIX

CC	=	cc
COPTS	=	-c -O -o $@
LIB	=	lib

#*
#****************************************************************
#	Geração da Biblioteca					*
#****************************************************************
#*
$LIB/.LIBSYM:	$LIB			\
		$LIB/Attrib.o		\
		$LIB/CrBufFrI.o		\
		$LIB/CrBufFrP.o		\
		$LIB/CrDatFrI.o		\
		$LIB/CrDatFrP.o		\
		$LIB/CrIFrBuf.o		\
		$LIB/CrIFrDat.o		\
		$LIB/CrIFrP.o		\
		$LIB/CrPFrBuf.o		\
		$LIB/CrPFrDat.o		\
		$LIB/CrPFrI.o		\
		$LIB/Image.o		\
		$LIB/Info.o		\
		$LIB/RdFToBuf.o		\
		$LIB/RdFToDat.o		\
		$LIB/RdFToI.o		\
		$LIB/RdFToP.o		\
		$LIB/WrFFrBuf.o		\
		$LIB/WrFFrDat.o		\
		$LIB/WrFFrI.o		\
		$LIB/WrFFrP.o		\
		$LIB/create.o		\
		$LIB/data.o		\
		$LIB/hashtab.o		\
		$LIB/misc.o		\
		$LIB/parse.o		\
		$LIB/rgb.o		\
		$LIB/scan.o
		mklib $LIB

$LIB:		.exist
		mkdir -f $LIB

.virtual	links

links:
		ln -f xpm.h ../../include/X11

#*
#****************************************************************
#	Compilação dos módulos					*
#****************************************************************
#*
$LIB/Attrib.o:		Attrib.c XpmI.h
			$CC $COPTS $INC $DEF Attrib.c

$LIB/CrBufFrI.o:	CrBufFrI.c XpmI.h
			$CC $COPTS $INC $DEF CrBufFrI.c

$LIB/CrBufFrP.o:	CrBufFrP.c XpmI.h
			$CC $COPTS $INC $DEF CrBufFrP.c

$LIB/CrDatFrI.o:	CrDatFrI.c XpmI.h
			$CC $COPTS $INC $DEF CrDatFrI.c

$LIB/CrDatFrP.o:	CrDatFrP.c XpmI.h
			$CC $COPTS $INC $DEF CrDatFrP.c

$LIB/CrIFrBuf.o:	CrIFrBuf.c XpmI.h
			$CC $COPTS $INC $DEF CrIFrBuf.c

$LIB/CrIFrDat.o:	CrIFrDat.c XpmI.h
			$CC $COPTS $INC $DEF CrIFrDat.c

$LIB/CrIFrP.o:		CrIFrP.c XpmI.h
			$CC $COPTS $INC $DEF CrIFrP.c

$LIB/CrPFrBuf.o:	CrPFrBuf.c XpmI.h
			$CC $COPTS $INC $DEF CrPFrBuf.c

$LIB/CrPFrDat.o:	CrPFrDat.c XpmI.h
			$CC $COPTS $INC $DEF CrPFrDat.c

$LIB/CrPFrI.o:		CrPFrI.c XpmI.h
			$CC $COPTS $INC $DEF CrPFrI.c

$LIB/Image.o:		Image.c XpmI.h
			$CC $COPTS $INC $DEF Image.c

$LIB/Info.o:		Info.c XpmI.h
			$CC $COPTS $INC $DEF Info.c

$LIB/RdFToBuf.o:	RdFToBuf.c XpmI.h
			$CC $COPTS $INC $DEF RdFToBuf.c

$LIB/RdFToDat.o:	RdFToDat.c XpmI.h
			$CC $COPTS $INC $DEF RdFToDat.c

$LIB/RdFToI.o:		RdFToI.c XpmI.h
			$CC $COPTS $INC $DEF RdFToI.c

$LIB/RdFToP.o:		RdFToP.c XpmI.h
			$CC $COPTS $INC $DEF RdFToP.c

$LIB/WrFFrBuf.o:	WrFFrBuf.c XpmI.h
			$CC $COPTS $INC $DEF WrFFrBuf.c

$LIB/WrFFrDat.o:	WrFFrDat.c XpmI.h
			$CC $COPTS $INC $DEF WrFFrDat.c

$LIB/WrFFrI.o:		WrFFrI.c XpmI.h
			$CC $COPTS $INC $DEF WrFFrI.c

$LIB/WrFFrP.o:		WrFFrP.c XpmI.h
			$CC $COPTS $INC $DEF WrFFrP.c

$LIB/create.o:		create.c XpmI.h
			$CC $COPTS $INC $DEF create.c

$LIB/data.o:		data.c XpmI.h
			$CC $COPTS $INC $DEF data.c

$LIB/hashtab.o:		hashtab.c XpmI.h
			$CC $COPTS $INC $DEF hashtab.c

$LIB/misc.o:		misc.c XpmI.h
			$CC $COPTS $INC $DEF misc.c

$LIB/parse.o:		parse.c XpmI.h
			$CC $COPTS $INC $DEF parse.c

$LIB/rgb.o:		rgb.c XpmI.h rgbtab.h
			$CC $COPTS $INC $DEF rgb.c

$LIB/scan.o:		scan.c XpmI.h
			$CC $COPTS $INC $DEF scan.c
