#*
#****************************************************************
#	Makefile para o diretório "hw/xf1bpp"			*
#****************************************************************
#*
xc	=		../../..

INC	=		-I .					\
			-I $xc/sys/mfb				\
			-I $xc/sys/mi				\
			-I $xc/sys/include			\
			-I $xc/sys/hw				\
			-I $xc/sys/hw/common			\
			-I $xc/include				\
			-I $xc/include/X11			\
			-I $xc/include/X11/fonts

DEF	=		$STDDEF					\
			-D XF86MONO
#			-D IN_MODULE				\
#			-D XFree86Module

CC	=		cc
COPTS	=		-c -O -o $@
LIB	=		$xc/sys/lib/xf1bpp
MFB	=		$xc/sys/mfb

$LIB/.LIBSYM:		$LIB					\
			$LIB/mfbgc.o				\
			$LIB/mfbwindow.o			\
			$LIB/mfbfont.o				\
			$LIB/mfbfillrct.o			\
			$LIB/mfbpntwin.o			\
			$LIB/maskbits.o				\
			$LIB/mfbpixmap.o			\
			$LIB/mfbimage.o				\
			$LIB/mfbline.o				\
			$LIB/mfbbres.o				\
			$LIB/mfbhrzvert.o			\
			$LIB/mfbbresd.o				\
			$LIB/mfbseg.o				\
			$LIB/mfbpushpxl.o			\
			$LIB/mfbzerarc.o			\
			$LIB/mfbfillarc.o			\
			$LIB/mfbfillsp.o			\
			$LIB/mfbsetsp.o				\
			$LIB/mfbscrinit.o			\
			$LIB/mfbscrclse.o			\
			$LIB/mfbclip.o				\
			$LIB/mfbbitblt.o			\
			$LIB/mfbgetsp.o				\
			$LIB/mfbpolypnt.o			\
			$LIB/mfbbltC.o				\
			$LIB/mfbbltX.o				\
			$LIB/mfbbltCI.o				\
			$LIB/mfbbltO.o				\
			$LIB/mfbbltG.o				\
			$LIB/mfbpgbwht.o			\
			$LIB/mfbpgbblak.o			\
			$LIB/mfbpgbinv.o			\
			$LIB/mfbigbwht.o			\
			$LIB/mfbigbblak.o			\
			$LIB/mfbcmap.o				\
			$LIB/mfbpawhite.o			\
			$LIB/mfbpablack.o			\
			$LIB/mfbpainv.o				\
			$LIB/mfbtileC.o				\
			$LIB/mfbtileG.o				\
			$LIB/mfbtewhite.o			\
			$LIB/mfbteblack.o			\
			$LIB/mfbmisc.o				\
			$LIB/mfbbstore.o			\
			$LIB/mfbplywhite.o			\
			$LIB/mfbplyblack.o			\
			$LIB/mfbplyinv.o
			mklib $LIB

$LIB:			.exist
			mkdir -f $LIB

$LIB/mfbgc.o:		$MFB/mfbgc.c
			cat mfbmap.h $MFB/mfbgc.c > x.c
			$CC $COPTS $INC $DEF x.c
			rm -f x.c

$LIB/mfbwindow.o:	$MFB/mfbwindow.c
			cat mfbmap.h $MFB/mfbwindow.c > x.c
			$CC $COPTS $INC $DEF x.c
			rm -f x.c

$LIB/mfbfont.o:		$MFB/mfbfont.c
			cat mfbmap.h $MFB/mfbfont.c > x.c
			$CC $COPTS $INC $DEF x.c
			rm -f x.c

$LIB/mfbfillrct.o:	$MFB/mfbfillrct.c
			cat mfbmap.h $MFB/mfbfillrct.c > x.c
			$CC $COPTS $INC $DEF x.c
			rm -f x.c

$LIB/mfbpntwin.o:	$MFB/mfbpntwin.c
			cat mfbmap.h $MFB/mfbpntwin.c > x.c
			$CC $COPTS $INC $DEF x.c
			rm -f x.c

$LIB/maskbits.o:	$MFB/maskbits.c
			cat mfbmap.h $MFB/maskbits.c > x.c
			$CC $COPTS $INC $DEF x.c
			rm -f x.c

$LIB/mfbpixmap.o:	$MFB/mfbpixmap.c
			cat mfbmap.h $MFB/mfbpixmap.c > x.c
			$CC $COPTS $INC $DEF x.c
			rm -f x.c

$LIB/mfbimage.o:	$MFB/mfbimage.c
			cat mfbmap.h $MFB/mfbimage.c > x.c
			$CC $COPTS $INC $DEF x.c
			rm -f x.c

$LIB/mfbline.o:		$MFB/mfbline.c
			cat mfbmap.h $MFB/mfbline.c > x.c
			$CC $COPTS $INC $DEF x.c
			rm -f x.c

$LIB/mfbbres.o:		$MFB/mfbbres.c
			cat mfbmap.h $MFB/mfbbres.c > x.c
			$CC $COPTS $INC $DEF x.c
			rm -f x.c

$LIB/mfbhrzvert.o:	$MFB/mfbhrzvert.c
			cat mfbmap.h $MFB/mfbhrzvert.c > x.c
			$CC $COPTS $INC $DEF x.c
			rm -f x.c

$LIB/mfbbresd.o:	$MFB/mfbbresd.c
			cat mfbmap.h $MFB/mfbbresd.c > x.c
			$CC $COPTS $INC $DEF x.c
			rm -f x.c

$LIB/mfbseg.o:		$MFB/mfbline.c
			cat mfbmap.h $MFB/mfbline.c > x.c
			$CC $COPTS $INC $DEF -D POLYSEGMENT x.c
			rm -f x.c

$LIB/mfbpushpxl.o:	$MFB/mfbpushpxl.c
			cat mfbmap.h $MFB/mfbpushpxl.c > x.c
			$CC $COPTS $INC $DEF x.c
			rm -f x.c

$LIB/mfbzerarc.o:	$MFB/mfbzerarc.c
			cat mfbmap.h $MFB/mfbzerarc.c > x.c
			$CC $COPTS $INC $DEF x.c
			rm -f x.c

$LIB/mfbfillarc.o:	$MFB/mfbfillarc.c
			cat mfbmap.h $MFB/mfbfillarc.c > x.c
			$CC $COPTS $INC $DEF x.c
			rm -f x.c

$LIB/mfbfillsp.o:	$MFB/mfbfillsp.c
			cat mfbmap.h $MFB/mfbfillsp.c > x.c
			$CC $COPTS $INC $DEF x.c
			rm -f x.c

$LIB/mfbsetsp.o:	$MFB/mfbsetsp.c
			cat mfbmap.h $MFB/mfbsetsp.c > x.c
			$CC $COPTS $INC $DEF x.c
			rm -f x.c

$LIB/mfbscrinit.o:	$MFB/mfbscrinit.c
			cat mfbmap.h $MFB/mfbscrinit.c > x.c
			$CC $COPTS $INC $DEF x.c
			rm -f x.c

$LIB/mfbscrclse.o:	$MFB/mfbscrclse.c
			cat mfbmap.h $MFB/mfbscrclse.c > x.c
			$CC $COPTS $INC $DEF x.c
			rm -f x.c

$LIB/mfbclip.o:		$MFB/mfbclip.c
			cat mfbmap.h $MFB/mfbclip.c > x.c
			$CC $COPTS $INC $DEF x.c
			rm -f x.c

$LIB/mfbbitblt.o:	$MFB/mfbbitblt.c
			cat mfbmap.h $MFB/mfbbitblt.c > x.c
			$CC $COPTS $INC $DEF x.c
			rm -f x.c

$LIB/mfbgetsp.o:	$MFB/mfbgetsp.c
			cat mfbmap.h $MFB/mfbgetsp.c > x.c
			$CC $COPTS $INC $DEF x.c
			rm -f x.c

$LIB/mfbpolypnt.o:	$MFB/mfbpolypnt.c
			cat mfbmap.h $MFB/mfbpolypnt.c > x.c
			$CC $COPTS $INC $DEF x.c
			rm -f x.c

$LIB/mfbbltC.o:		$MFB/mfbblt.c
			cat mfbmap.h $MFB/mfbblt.c > x.c
			$CC $COPTS $INC $DEF -D MROP=Mcopy x.c
			rm -f x.c

$LIB/mfbbltX.o:		$MFB/mfbblt.c
			cat mfbmap.h $MFB/mfbblt.c > x.c
			$CC $COPTS $INC $DEF -D MROP=Mxor x.c
			rm -f x.c

$LIB/mfbbltCI.o:	$MFB/mfbblt.c
			cat mfbmap.h $MFB/mfbblt.c > x.c
			$CC $COPTS $INC $DEF -D MROP=McopyInverted x.c
			rm -f x.c

$LIB/mfbbltO.o:		$MFB/mfbblt.c
			cat mfbmap.h $MFB/mfbblt.c > x.c
			$CC $COPTS $INC $DEF -D MROP=Mor x.c
			rm -f x.c

$LIB/mfbbltG.o:		$MFB/mfbblt.c
			cat mfbmap.h $MFB/mfbblt.c > x.c
			$CC $COPTS $INC $DEF -D MROP=0 x.c
			rm -f x.c

$LIB/mfbpgbwht.o:	$MFB/mfbplygblt.c
			cat mfbmap.h $MFB/mfbplygblt.c > x.c
			$CC $COPTS $INC $DEF				\
			-D MFBPOLYGLYPHBLT=mfbPolyGlyphBltWhite		\
			-D OPEQ=\|= x.c
			rm -f x.c

$LIB/mfbpgbblak.o: 	$MFB/mfbplygblt.c
			cat mfbmap.h $MFB/mfbplygblt.c > x.c
			$CC $COPTS $INC $DEF				\
			-D MFBPOLYGLYPHBLT=mfbPolyGlyphBltBlack		\
			-D OPEQ=\&=\~ x.c
			rm -f x.c

$LIB/mfbpgbinv.o:	$MFB/mfbplygblt.c
			cat mfbmap.h $MFB/mfbplygblt.c > x.c
			$CC $COPTS $INC $DEF				\
			-D MFBPOLYGLYPHBLT=mfbPolyGlyphBltInvert	\
			-D OPEQ=\^= x.c
			rm -f x.c

$LIB/mfbigbwht.o:	$MFB/mfbimggblt.c
			cat mfbmap.h $MFB/mfbimggblt.c > x.c
			$CC $COPTS $INC $DEF				\
			-D MFBIMAGEGLYPHBLT=mfbImageGlyphBltWhite	\
			-D OPEQ=\|= x.c
			rm -f x.c

$LIB/mfbigbblak.o:	$MFB/mfbimggblt.c
			cat mfbmap.h $MFB/mfbimggblt.c > x.c
			$CC $COPTS $INC $DEF				\
			-D MFBIMAGEGLYPHBLT=mfbImageGlyphBltBlack	\
			-D OPEQ=\&=\~ x.c
			rm -f x.c

$LIB/mfbcmap.o:		$MFB/mfbcmap.c
			cat mfbmap.h $MFB/mfbcmap.c > x.c
			$CC $COPTS $INC $DEF x.c
			rm -f x.c

$LIB/mfbpawhite.o:	$MFB/mfbpntarea.c
			cat mfbmap.h $MFB/mfbpntarea.c > x.c
			$CC $COPTS $INC $DEF				\
			-D MFBSOLIDFILLAREA=mfbSolidWhiteArea		\
			-D MFBSTIPPLEFILLAREA=mfbStippleWhiteArea	\
			-D OPEQ=\|= -D EQWHOLEWORD=\=\~0 x.c
			rm -f x.c

$LIB/mfbpablack.o:	$MFB/mfbpntarea.c
			cat mfbmap.h $MFB/mfbpntarea.c > x.c
			$CC $COPTS $INC $DEF				\
			-D MFBSOLIDFILLAREA=mfbSolidBlackArea		\
			-D MFBSTIPPLEFILLAREA=mfbStippleBlackArea	\
			-D OPEQ=\&=\~ -D EQWHOLEWORD=\=0 x.c
			rm -f x.c

$LIB/mfbpainv.o:	$MFB/mfbpntarea.c
			cat mfbmap.h $MFB/mfbpntarea.c > x.c
			$CC $COPTS $INC $DEF				\
			-D MFBSOLIDFILLAREA=mfbSolidInvertArea		\
			-D MFBSTIPPLEFILLAREA=mfbStippleInvertArea	\
			-D OPEQ=\^= -D EQWHOLEWORD=\^=\~0 x.c
			rm -f x.c

$LIB/mfbtileC.o:	$MFB/mfbtile.c
			cat mfbmap.h $MFB/mfbtile.c > x.c
			$CC $COPTS $INC $DEF -D MROP=Mcopy x.c
			rm -f x.c

$LIB/mfbtileG.o:	$MFB/mfbtile.c
			cat mfbmap.h $MFB/mfbtile.c > x.c
			$CC $COPTS $INC $DEF -D MROP=0 x.c
			rm -f x.c

$LIB/mfbtewhite.o:	$MFB/mfbtegblt.c
			cat mfbmap.h $MFB/mfbtegblt.c > x.c
			$CC $COPTS $INC $DEF				\
			-D MFBTEGLYPHBLT=mfbTEGlyphBltWhite		\
			-D OP= -D CLIPTETEXT=mfbImageGlyphBltWhite x.c
			rm -f x.c

$LIB/mfbteblack.o:	$MFB/mfbtegblt.c
			cat mfbmap.h $MFB/mfbtegblt.c > x.c
			$CC $COPTS $INC $DEF				\
			-D MFBTEGLYPHBLT=mfbTEGlyphBltBlack		\
			-D OP=\~ -D CLIPTETEXT=mfbImageGlyphBltBlack x.c
			rm -f x.c

$LIB/mfbmisc.o:		$MFB/mfbmisc.c
			cat mfbmap.h $MFB/mfbmisc.c > x.c
			$CC $COPTS $INC $DEF x.c
			rm -f x.c

$LIB/mfbbstore.o:	$MFB/mfbbstore.c
			cat mfbmap.h $MFB/mfbbstore.c > x.c
			$CC $COPTS $INC $DEF x.c
			rm -f x.c

$LIB/mfbplywhite.o:	$MFB/mfbply1rct.c
			cat mfbmap.h $MFB/mfbply1rct.c > x.c
			$CC $COPTS $INC $DEF				\
			-D MFBFILLPOLY1RECT=mfbFillPolyWhite		\
			-D OPEQ=\|= -D EQWHOLEWORD=\=\~0 x.c
			rm -f x.c

$LIB/mfbplyblack.o:	$MFB/mfbply1rct.c
			cat mfbmap.h $MFB/mfbply1rct.c > x.c
			$CC $COPTS $INC $DEF				\
			-D MFBFILLPOLY1RECT=mfbFillPolyBlack		\
			-D OPEQ=\&=\~ -D EQWHOLEWORD=\=0 x.c
			rm -f x.c

$LIB/mfbplyinv.o:	$MFB/mfbply1rct.c
			cat mfbmap.h $MFB/mfbply1rct.c > x.c
			$CC $COPTS $INC $DEF				\
			-D MFBFILLPOLY1RECT=mfbFillPolyInvert		\
			-D OPEQ=\^= -D EQWHOLEWORD=\^=\~0 x.c
			rm -f x.c
