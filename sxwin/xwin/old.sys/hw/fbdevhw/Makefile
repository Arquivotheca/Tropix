#*
#****************************************************************
#	Makefile para o diretório "hw/fbdevhw"			*
#****************************************************************
#*
xc	=		../../..

INC	=		-I .					\
			-I $xc/sys/hw				\
			-I $xc/sys/hw/common			\
			-I $xc/sys/hw/ddc			\
			-I $xc/sys/hw/i2c			\
			-I $xc/sys/hw/os-support		\
			-I $xc/sys/hw/os-support/bus		\
			-I $xc/sys/Xext				\
			-I $xc/sys/cfb				\
			-I $xc/sys/mfb				\
			-I $xc/sys/mi				\
			-I $xc/sys/include			\
			-I $xc/include				\
			-I $xc/include/X11

DEF	=		$STDDEF
#			-D IN_MODULE				\
#			-D XFree86Module

CC	=		cc
COPTS	=		-c -O -o $@
LIB	=		$xc/sys/lib/fbdevhw

$LIB/.LIBSYM:		$LIB					\
			$LIB/fbdevhwstub.o
			mklib $LIB

$LIB:			.exist
			mkdir -f $LIB

$LIB/fbdevhwstub.o:	fbdevhwstub.c
			$CC $COPTS $INC $DEF fbdevhwstub.c

# rm -f fbdevhw.man
# => /usr/bin/cpp -undef -traditional  -D__filemansuffix__=5 
# -D__miscmansuffix__=7 -D__drivermansuffix__=4 
# -D__projectroot__=/usr/X11R6  <fbdevhw.cpp | sed -e '/^#  *[0-9][0-9]*  
# *.*$/d' -e '/^XCOMM$/s//#/' -e '/^XCOMM[^a-zA-Z0-9_]/s/^XCOMM/#/' 
# >fbdevhw.man
# => rm -f fbdevhw.4.html fbdevhw.4-html
# => ../../../../../config/util/rman -f HTML < fbdevhw.man > fbdevhw.4-html 
# &&  mv -f fbdevhw.4-html fbdevhw.4.html
