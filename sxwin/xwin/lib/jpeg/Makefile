#*
#****************************************************************
#	Makefile para a biblioteca "lib/jpeg"			*
#****************************************************************
#*
INC	=	-I ../../include

DEF	=	-D entry=_entry_		\
		-D TROPIX

CC	=	cc
COPTS	=	-c -O -o $@
LIB	=	lib

#*
#****************************************************************
#*	Geração da biblioteca					*
#****************************************************************
#*
$LIB/.LIBSYM:	$LIB			\
		$LIB/cdjpeg.o		\
		$LIB/jcapimin.o		\
		$LIB/jcapistd.o		\
		$LIB/jccoefct.o		\
		$LIB/jccolor.o		\
		$LIB/jcdctmgr.o		\
		$LIB/jchuff.o		\
		$LIB/jcinit.o		\
		$LIB/jcmainct.o		\
		$LIB/jcmarker.o		\
		$LIB/jcmaster.o		\
		$LIB/jcomapi.o		\
		$LIB/jcparam.o		\
		$LIB/jcphuff.o		\
		$LIB/jcprepct.o		\
		$LIB/jcsample.o		\
		$LIB/jctrans.o		\
		$LIB/jdapimin.o		\
		$LIB/jdapistd.o		\
		$LIB/jdatadst.o		\
		$LIB/jdatasrc.o		\
		$LIB/jdcoefct.o		\
		$LIB/jdcolor.o		\
		$LIB/jddctmgr.o		\
		$LIB/jdhuff.o		\
		$LIB/jdinput.o		\
		$LIB/jdmainct.o		\
		$LIB/jdmarker.o		\
		$LIB/jdmaster.o		\
		$LIB/jdmerge.o		\
		$LIB/jdphuff.o		\
		$LIB/jdpostct.o		\
		$LIB/jdsample.o		\
		$LIB/jdtrans.o		\
		$LIB/jerror.o		\
		$LIB/jfdctflt.o		\
		$LIB/jfdctfst.o		\
		$LIB/jfdctint.o		\
		$LIB/jidctflt.o		\
		$LIB/jidctfst.o		\
		$LIB/jidctint.o		\
		$LIB/jidctred.o		\
		$LIB/jmemansi.o		\
		$LIB/jmemmgr.o		\
		$LIB/jquant1.o		\
		$LIB/jquant2.o		\
		$LIB/jutils.o		\
		$LIB/rdbmp.o		\
		$LIB/rdcolmap.o		\
		$LIB/rdgif.o		\
		$LIB/rdppm.o		\
		$LIB/rdswitch.o		\
		$LIB/rdtarga.o		\
		$LIB/transupp.o		\
		$LIB/wrbmp.o		\
		$LIB/wrgif.o		\
		$LIB/wrppm.o		\
		$LIB/wrtarga.o
		mklib $LIB

$LIB:		.exist
		mkdir -f $LIB

.virtual	links

links:
		ln -f jpeglib.h jconfig.h jmorecfg.h ../../include/X11

#*
#****************************************************************
#*	Compilação dos Diversos Módulos				*
#****************************************************************
#*
$LIB/cdjpeg.o:		cdjpeg.c	\
			cdjpeg.h	
			$CC $COPTS $DEF cdjpeg.c

$LIB/jcapimin.o:	jcapimin.c
			$CC $COPTS $DEF jcapimin.c

$LIB/jcapistd.o:	jcapistd.c
			$CC $COPTS $DEF jcapistd.c

$LIB/jccoefct.o:	jccoefct.c
			$CC $COPTS $DEF jccoefct.c

$LIB/jccolor.o:		jccolor.c
			$CC $COPTS $DEF jccolor.c

$LIB/jcdctmgr.o:	jcdctmgr.c
			$CC $COPTS $DEF jcdctmgr.c

$LIB/jchuff.o:		jchuff.c
			$CC $COPTS $DEF jchuff.c

$LIB/jcinit.o:		jcinit.c
			$CC $COPTS $DEF jcinit.c

$LIB/jcmainct.o:	jcmainct.c
			$CC $COPTS $DEF jcmainct.c

$LIB/jcmarker.o:	jcmarker.c
			$CC $COPTS $DEF jcmarker.c

$LIB/jcmaster.o:	jcmaster.c
			$CC $COPTS $DEF jcmaster.c

$LIB/jcomapi.o:		jcomapi.c
			$CC $COPTS $DEF jcomapi.c

$LIB/jcparam.o:		jcparam.c
			$CC $COPTS $DEF jcparam.c

$LIB/jcphuff.o:		jcphuff.c
			$CC $COPTS $DEF jcphuff.c

$LIB/jcprepct.o:	jcprepct.c
			$CC $COPTS $DEF jcprepct.c

$LIB/jcsample.o:	jcsample.c
			$CC $COPTS $DEF jcsample.c

$LIB/jctrans.o:		jctrans.c
			$CC $COPTS $DEF jctrans.c

$LIB/jdapimin.o:	jdapimin.c
			$CC $COPTS $DEF jdapimin.c

$LIB/jdapistd.o:	jdapistd.c
			$CC $COPTS $DEF jdapistd.c

$LIB/jdatadst.o:	jdatadst.c
			$CC $COPTS $DEF jdatadst.c

$LIB/jdatasrc.o:	jdatasrc.c
			$CC $COPTS $DEF jdatasrc.c

$LIB/jdcoefct.o:	jdcoefct.c
			$CC $COPTS $DEF jdcoefct.c

$LIB/jdcolor.o:		jdcolor.c
			$CC $COPTS $DEF jdcolor.c

$LIB/jddctmgr.o:	jddctmgr.c
			$CC $COPTS $DEF jddctmgr.c

$LIB/jdhuff.o:		jdhuff.c
			$CC $COPTS $DEF jdhuff.c

$LIB/jdinput.o:		jdinput.c
			$CC $COPTS $DEF jdinput.c

$LIB/jdmainct.o:	jdmainct.c
			$CC $COPTS $DEF jdmainct.c

$LIB/jdmarker.o:	jdmarker.c
			$CC $COPTS $DEF jdmarker.c

$LIB/jdmaster.o:	jdmaster.c
			$CC $COPTS $DEF jdmaster.c

$LIB/jdmerge.o:		jdmerge.c
			$CC $COPTS $DEF jdmerge.c

$LIB/jdphuff.o:		jdphuff.c
			$CC $COPTS $DEF jdphuff.c

$LIB/jdpostct.o:	jdpostct.c
			$CC $COPTS $DEF jdpostct.c

$LIB/jdsample.o:	jdsample.c
			$CC $COPTS $DEF jdsample.c

$LIB/jdtrans.o:		jdtrans.c
			$CC $COPTS $DEF jdtrans.c

$LIB/jerror.o:		jerror.c
			$CC $COPTS $DEF jerror.c

$LIB/jfdctflt.o:	jfdctflt.c
			$CC $COPTS $DEF jfdctflt.c

$LIB/jfdctfst.o:	jfdctfst.c
			$CC $COPTS $DEF jfdctfst.c

$LIB/jfdctint.o:	jfdctint.c
			$CC $COPTS $DEF jfdctint.c

$LIB/jidctflt.o:	jidctflt.c
			$CC $COPTS $DEF jidctflt.c

$LIB/jidctfst.o:	jidctfst.c
			$CC $COPTS $DEF jidctfst.c

$LIB/jidctint.o:	jidctint.c
			$CC $COPTS $DEF jidctint.c

$LIB/jidctred.o:	jidctred.c
			$CC $COPTS $DEF jidctred.c

$LIB/jmemansi.o:	jmemansi.c
			$CC $COPTS $DEF jmemansi.c

$LIB/jmemmgr.o:		jmemmgr.c
			$CC $COPTS $DEF jmemmgr.c

$LIB/jquant1.o:		jquant1.c
			$CC $COPTS $DEF jquant1.c

$LIB/jquant2.o:		jquant2.c
			$CC $COPTS $DEF jquant2.c

$LIB/jutils.o:		jutils.c
			$CC $COPTS $DEF jutils.c

$LIB/rdbmp.o:		rdbmp.c
			$CC $COPTS $DEF rdbmp.c

$LIB/rdcolmap.o:	rdcolmap.c
			$CC $COPTS $DEF rdcolmap.c

$LIB/rdgif.o:		rdgif.c
			$CC $COPTS $DEF rdgif.c

$LIB/rdppm.o:		rdppm.c
			$CC $COPTS $DEF rdppm.c

$LIB/rdswitch.o:	rdswitch.c
			$CC $COPTS $DEF rdswitch.c

$LIB/rdtarga.o:		rdtarga.c
			$CC $COPTS $DEF rdtarga.c

$LIB/transupp.o:	transupp.c
			$CC $COPTS $DEF transupp.c

$LIB/wrbmp.o:		wrbmp.c
			$CC $COPTS $DEF wrbmp.c

$LIB/wrgif.o:		wrgif.c
			$CC $COPTS $DEF wrgif.c

$LIB/wrppm.o:		wrppm.c
			$CC $COPTS $DEF wrppm.c

$LIB/wrtarga.o:		wrtarga.c	\
			cdjpeg.h	
			$CC $COPTS $DEF wrtarga.c
