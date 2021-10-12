#*
#****************************************************************
#	Makefile para o diretório "Xi"				*
#****************************************************************
#*
xc	=		../..

INC	=		-I .					\
			-I $xc/sys/include			\
			-I $xc/include				\
			-I $xc/include/X11			\
			-I $xc/include/X11/extensions

CC	=		cc
COPTS	=		-c -O -o $@
LIB	=		$xc/sys/lib/Xi

$LIB/.LIBSYM:		$LIB					\
			$LIB/allowev.o				\
			$LIB/chgkbd.o				\
			$LIB/chgdctl.o				\
			$LIB/chgfctl.o				\
			$LIB/chgkmap.o				\
			$LIB/chgprop.o				\
			$LIB/chgptr.o				\
			$LIB/closedev.o				\
			$LIB/devbell.o				\
			$LIB/exevents.o				\
			$LIB/extinit.o				\
			$LIB/getdctl.o				\
			$LIB/getfctl.o				\
			$LIB/getfocus.o				\
			$LIB/getbmap.o				\
			$LIB/getkmap.o				\
			$LIB/getmmap.o				\
			$LIB/getprop.o				\
			$LIB/getselev.o				\
			$LIB/getvers.o				\
			$LIB/gtmotion.o				\
			$LIB/grabdev.o				\
			$LIB/grabdevb.o				\
			$LIB/grabdevk.o				\
			$LIB/listdev.o				\
			$LIB/opendev.o				\
			$LIB/queryst.o				\
			$LIB/selectev.o				\
			$LIB/sendexev.o				\
			$LIB/setdval.o				\
			$LIB/setfocus.o				\
			$LIB/setmode.o				\
			$LIB/setbmap.o				\
			$LIB/setmmap.o				\
			$LIB/ungrdev.o				\
			$LIB/ungrdevb.o				\
			$LIB/ungrdevk.o
			mklib $LIB

$LIB:			.exist
			mkdir -f $LIB

$LIB/allowev.o:		allowev.c
			$CC $COPTS $STDDEF $INC allowev.c

$LIB/chgkbd.o:		chgkbd.c
			$CC $COPTS $STDDEF $INC chgkbd.c

$LIB/chgdctl.o:		chgdctl.c
			$CC $COPTS $STDDEF $INC chgdctl.c

$LIB/chgfctl.o:		chgfctl.c
			$CC $COPTS $STDDEF $INC chgfctl.c

$LIB/chgkmap.o:		chgkmap.c
			$CC $COPTS $STDDEF $INC chgkmap.c

$LIB/chgprop.o:		chgprop.c
			$CC $COPTS $STDDEF $INC chgprop.c

$LIB/chgptr.o:		chgptr.c
			$CC $COPTS $STDDEF $INC chgptr.c

$LIB/closedev.o:	closedev.c
			$CC $COPTS $STDDEF $INC closedev.c

$LIB/devbell.o:		devbell.c
			$CC $COPTS $STDDEF $INC devbell.c

$LIB/exevents.o:	exevents.c
			$CC $COPTS $STDDEF $INC exevents.c

$LIB/extinit.o:		extinit.c
			$CC $COPTS $STDDEF $INC extinit.c

$LIB/getdctl.o:		getdctl.c
			$CC $COPTS $STDDEF $INC getdctl.c

$LIB/getfctl.o:		getfctl.c
			$CC $COPTS $STDDEF $INC getfctl.c

$LIB/getfocus.o:	getfocus.c
			$CC $COPTS $STDDEF $INC getfocus.c

$LIB/getbmap.o:		getbmap.c
			$CC $COPTS $STDDEF $INC getbmap.c

$LIB/getkmap.o:		getkmap.c
			$CC $COPTS $STDDEF $INC getkmap.c

$LIB/getmmap.o:		getmmap.c
			$CC $COPTS $STDDEF $INC getmmap.c

$LIB/getprop.o:		getprop.c
			$CC $COPTS $STDDEF $INC getprop.c

$LIB/getselev.o:	getselev.c
			$CC $COPTS $STDDEF $INC getselev.c

$LIB/getvers.o:		getvers.c
			$CC $COPTS $STDDEF $INC getvers.c

$LIB/gtmotion.o:	gtmotion.c
			$CC $COPTS $STDDEF $INC gtmotion.c

$LIB/grabdev.o:		grabdev.c
			$CC $COPTS $STDDEF $INC grabdev.c

$LIB/grabdevb.o:	grabdevb.c
			$CC $COPTS $STDDEF $INC grabdevb.c

$LIB/grabdevk.o:	grabdevk.c
			$CC $COPTS $STDDEF $INC grabdevk.c

$LIB/listdev.o:		listdev.c
			$CC $COPTS $STDDEF $INC listdev.c

$LIB/opendev.o:		opendev.c
			$CC $COPTS $STDDEF $INC opendev.c

$LIB/queryst.o:		queryst.c
			$CC $COPTS $STDDEF $INC queryst.c

$LIB/selectev.o:	selectev.c
			$CC $COPTS $STDDEF $INC selectev.c

$LIB/sendexev.o:	sendexev.c
			$CC $COPTS $STDDEF $INC sendexev.c

$LIB/setdval.o:		setdval.c
			$CC $COPTS $STDDEF $INC setdval.c

$LIB/setfocus.o:	setfocus.c
			$CC $COPTS $STDDEF $INC setfocus.c

$LIB/setmode.o:		setmode.c
			$CC $COPTS $STDDEF $INC setmode.c

$LIB/setbmap.o:		setbmap.c
			$CC $COPTS $STDDEF $INC setbmap.c

$LIB/setmmap.o:		setmmap.c
			$CC $COPTS $STDDEF $INC setmmap.c

$LIB/ungrdev.o:		ungrdev.c
			$CC $COPTS $STDDEF $INC ungrdev.c

$LIB/ungrdevb.o:	ungrdevb.c
			$CC $COPTS $STDDEF $INC ungrdevb.c

$LIB/ungrdevk.o:	ungrdevk.c
			$CC $COPTS $STDDEF $INC ungrdevk.c


