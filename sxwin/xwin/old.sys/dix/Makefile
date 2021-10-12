#*
#****************************************************************
#	Makefile para o diretório "dix"				*
#****************************************************************
#*
xc	=		../..

INC	=		-I $xc/sys/include			\
			-I $xc/sys/Xext				\
			-I $xc/sys/Xi				\
			-I $xc					\
			-I $xc/include				\
			-I $xc/include/X11			\
			-I $xc/include/X11/fonts		\
			-I $xc/include/X11/extensions

LIB	=		$xc/sys/lib/dix
LIBXP	=		$xc/sys/lib/xpstubs

CC	=		cc
COPTS	=		-c -O -o $@

.virtual		all

all:			$LIB/.LIBSYM				\
			$LIBXP/.LIBSYM

$LIB/.LIBSYM:		$LIB					\
			$LIB/atom.o				\
			$LIB/colormap.o				\
			$LIB/cursor.o				\
			$LIB/devices.o				\
			$LIB/dispatch.o				\
			$LIB/dixutils.o				\
			$LIB/events.o				\
			$LIB/extension.o			\
			$LIB/gc.o				\
			$LIB/globals.o				\
			$LIB/glyphcurs.o			\
			$LIB/grabs.o				\
			$LIB/main.o				\
			$LIB/property.o				\
			$LIB/resource.o				\
			$LIB/swaprep.o				\
			$LIB/swapreq.o				\
			$LIB/tables.o				\
			$LIB/window.o				\
			$LIB/initatoms.o			\
			$LIB/dixfonts.o				\
			$LIB/privates.o				\
			$LIB/pixmap.o
			mklib $LIB

$LIBXP/.LIBSYM:		$LIBXP					\
			$LIBXP/xpstubs.o
			mklib $LIBXP

$LIB:			.exist
			mkdir -f $LIB

$LIBXP:			.exist
			mkdir -f $LIBXP

$LIB/atom.o:		atom.c			
			$CC $COPTS $INC $STDDEF atom.c

$LIB/colormap.o:	colormap.c		
			$CC $COPTS $INC $STDDEF colormap.c

$LIB/cursor.o:		cursor.c		
			$CC $COPTS $INC $STDDEF cursor.c

$LIB/devices.o:		devices.c		
			$CC $COPTS $INC $STDDEF devices.c

$LIB/dispatch.o:	dispatch.c		
			$CC $COPTS $INC $STDDEF dispatch.c

$LIB/dixutils.o:	dixutils.c		
			$CC $COPTS $INC $STDDEF dixutils.c

$LIB/events.o:		events.c		
			$CC $COPTS $INC $STDDEF events.c

$LIB/extension.o:	extension.c		
			$CC $COPTS $INC $STDDEF extension.c

$LIB/gc.o:		gc.c			
			$CC $COPTS $INC $STDDEF gc.c

$LIB/globals.o:		globals.c		
			cat fontpath.h globals.c > x.c
			$CC $COPTS $INC $STDDEF -D RGB_DB=\"/usr/xwin/conf/rgb\" x.c
			rm -f x.c

$LIB/glyphcurs.o:	glyphcurs.c		
			$CC $COPTS $INC $STDDEF glyphcurs.c

$LIB/grabs.o:		grabs.c			
			$CC $COPTS $INC $STDDEF grabs.c

$LIB/main.o:		main.c			
			$CC $COPTS $INC $STDDEF			\
				-D VENDOR_STRING=\""The XFree86 Project, Inc"\"	\
				-D VENDOR_RELEASE=3310  main.c

$LIB/property.o:	property.c		
			$CC $COPTS $INC $STDDEF property.c

$LIB/resource.o:	resource.c		
			$CC $COPTS $INC $STDDEF resource.c

$LIB/swaprep.o:		swaprep.c		
			$CC $COPTS $INC $STDDEF swaprep.c

$LIB/swapreq.o:		swapreq.c		
			$CC $COPTS $INC $STDDEF swapreq.c

$LIB/tables.o:		tables.c		
			$CC $COPTS $INC $STDDEF tables.c

$LIB/window.o:		window.c		
			$CC $COPTS $INC $STDDEF window.c

$LIB/initatoms.o:	initatoms.c		
			$CC $COPTS $INC $STDDEF initatoms.c

$LIB/dixfonts.o:	dixfonts.c		
			$CC $COPTS $INC $STDDEF dixfonts.c

$LIB/privates.o:	privates.c		
			$CC $COPTS $INC $STDDEF privates.c

$LIB/pixmap.o:		pixmap.c		
			$CC $COPTS $INC $STDDEF pixmap.c

$LIBXP/xpstubs.o:	xpstubs.c		
			$CC $COPTS $INC $STDDEF xpstubs.c
