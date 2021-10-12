#*
#****************************************************************
#	makefile para a biblioteca "fvwm/lib"			*
#****************************************************************
#*
LIB	=		../lib
CC	=		cc
COPTS	=		-c -O -o $@

OBJ	=		$LIB/CatString3.o		\
			$LIB/CopyString.o		\
			$LIB/GetFdWidth.o		\
			$LIB/GetFont.o			\
			$LIB/GetLine.o			\
			$LIB/Picture.o			\
			$LIB/ReadPacket.o		\
			$LIB/SendInfo.o			\
			$LIB/SetMask.o			\
			$LIB/envvar.o			\
			$LIB/findIconFile.o		\
			$LIB/mystrcasecmp.o		\
			$LIB/safemalloc.o		\
			$LIB/strncmp.o			\
			$LIB/wild.o

$LIB/.LIBSYM:		$LIB $OBJ
			mklib $LIB

$LIB:			.exist
			mkdir -f $LIB

$LIB/CatString3.o:	CatString3.c
			$CC $COPTS CatString3.c

$LIB/CopyString.o:	CopyString.c			\
			fvwmlib.h	
			$CC $COPTS CopyString.c

$LIB/GetFdWidth.o:	GetFdWidth.c
			$CC $COPTS GetFdWidth.c

$LIB/GetFont.o:		GetFont.c			\
			../global.h			\
			fvwmlib.h	
			$CC $COPTS GetFont.c

$LIB/GetLine.o:		GetLine.c			\
			fvwmlib.h			\
			../fvwm/h/module.h	
			$CC $COPTS GetLine.c

$LIB/Picture.o:		Picture.c			\
			../global.h			\
			fvwmlib.h	
			$CC $COPTS Picture.c

$LIB/ReadPacket.o:	ReadPacket.c			\
			fvwmlib.h			\
			../fvwm/h/module.h	
			$CC $COPTS ReadPacket.c

$LIB/SendInfo.o:	SendInfo.c
			$CC $COPTS SendInfo.c

$LIB/SetMask.o:		SetMask.c			\
			fvwmlib.h			\
			../fvwm/h/module.h	
			$CC $COPTS SetMask.c

$LIB/envvar.o:		envvar.c			\
			fvwmlib.h	
			$CC $COPTS envvar.c

$LIB/findIconFile.o:	findIconFile.c			\
			fvwmlib.h	
			$CC $COPTS findIconFile.c

$LIB/mystrcasecmp.o:	mystrcasecmp.c
			$CC $COPTS mystrcasecmp.c

$LIB/safemalloc.o:	safemalloc.c
			$CC $COPTS safemalloc.c

$LIB/strncmp.o:		strncmp.c
			$CC $COPTS strncmp.c

$LIB/wild.o:		wild.c
			$CC $COPTS wild.c
