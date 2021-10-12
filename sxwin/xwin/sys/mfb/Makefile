#*
#****************************************************************
#	Makefile para o diretório "mfb"				*
#****************************************************************
#*
xc	=		../..

INC	=		-I .					\
			-I $xc/sys/mi				\
			-I $xc/sys/include			\
			-I $xc/sys/hw/common			\
			-I $xc/sys/hw				\
			-I $xc/include				\
			-I $xc/include/X11			\
			-I $xc/include/X11/fonts

DEF	=		$STDDEF
#			-D IN_MODULE
#			-D XFree86Module

CC	=		cc
COPTS	=		-c -O -o $@
LIB	=		$xc/sys/lib/mfb

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

$LIB/mfbgc.o:		mfbgc.c
			$CC $COPTS $INC $DEF mfbgc.c

$LIB/mfbwindow.o:	mfbwindow.c
			$CC $COPTS $INC $DEF mfbwindow.c

$LIB/mfbfont.o:		mfbfont.c
			$CC $COPTS $INC $DEF mfbfont.c

$LIB/mfbfillrct.o:	mfbfillrct.c
			$CC $COPTS $INC $DEF mfbfillrct.c

$LIB/mfbpntwin.o:	mfbpntwin.c
			$CC $COPTS $INC $DEF mfbpntwin.c

$LIB/maskbits.o:	maskbits.c
			$CC $COPTS $INC $DEF maskbits.c

$LIB/mfbpixmap.o:	mfbpixmap.c
			$CC $COPTS $INC $DEF mfbpixmap.c

$LIB/mfbimage.o:	mfbimage.c
			$CC $COPTS $INC $DEF mfbimage.c

$LIB/mfbline.o:		mfbline.c
			$CC $COPTS $INC $DEF mfbline.c

$LIB/mfbbres.o:		mfbbres.c
			$CC $COPTS $INC $DEF mfbbres.c

$LIB/mfbhrzvert.o:	mfbhrzvert.c
			$CC $COPTS $INC $DEF mfbhrzvert.c

$LIB/mfbbresd.o:	mfbbresd.c
			$CC $COPTS $INC $DEF mfbbresd.c

$LIB/mfbseg.o:		mfbline.c
			$CC $COPTS $INC $DEF -D POLYSEGMENT mfbline.c

$LIB/mfbpushpxl.o:	mfbpushpxl.c
			$CC $COPTS $INC $DEF mfbpushpxl.c

$LIB/mfbzerarc.o:	mfbzerarc.c
			$CC $COPTS $INC $DEF mfbzerarc.c

$LIB/mfbfillarc.o:	mfbfillarc.c
			$CC $COPTS $INC $DEF mfbfillarc.c

$LIB/mfbfillsp.o:	mfbfillsp.c
			$CC $COPTS $INC $DEF mfbfillsp.c

$LIB/mfbsetsp.o:	mfbsetsp.c
			$CC $COPTS $INC $DEF mfbsetsp.c

$LIB/mfbscrinit.o:	mfbscrinit.c
			$CC $COPTS $INC $DEF mfbscrinit.c

$LIB/mfbscrclse.o:	mfbscrclse.c
			$CC $COPTS $INC $DEF mfbscrclse.c

$LIB/mfbclip.o:		mfbclip.c
			$CC $COPTS $INC $DEF mfbclip.c

$LIB/mfbbitblt.o:	mfbbitblt.c
			$CC $COPTS $INC $DEF mfbbitblt.c

$LIB/mfbgetsp.o:	mfbgetsp.c
			$CC $COPTS $INC $DEF mfbgetsp.c

$LIB/mfbpolypnt.o:	mfbpolypnt.c
			$CC $COPTS $INC $DEF mfbpolypnt.c

$LIB/mfbbltC.o:		mfbblt.c
			$CC $COPTS $INC $DEF -D MROP=Mcopy mfbblt.c

$LIB/mfbbltX.o:		mfbblt.c
			$CC $COPTS $INC $DEF -D MROP=Mxor mfbblt.c

$LIB/mfbbltCI.o:	mfbblt.c
			$CC $COPTS $INC $DEF -D MROP=McopyInverted mfbblt.c

$LIB/mfbbltO.o:		mfbblt.c
			$CC $COPTS $INC $DEF -D MROP=Mor mfbblt.c

$LIB/mfbbltG.o:		mfbblt.c
			$CC $COPTS $INC $DEF -D MROP=0 mfbblt.c

$LIB/mfbpgbwht.o:	mfbplygblt.c
			$CC $COPTS $INC $DEF				\
				-D MFBPOLYGLYPHBLT=mfbPolyGlyphBltWhite	\
				-D OPEQ=MFB_OPEQ_WHITE mfbplygblt.c

$LIB/mfbpgbblak.o:	mfbplygblt.c
			$CC $COPTS $INC $DEF				\
				-D MFBPOLYGLYPHBLT=mfbPolyGlyphBltBlack \
				-D OPEQ=MFB_OPEQ_BLACK mfbplygblt.c

$LIB/mfbpgbinv.o:	mfbplygblt.c
			$CC $COPTS $INC $DEF				\
				-D MFBPOLYGLYPHBLT=mfbPolyGlyphBltInvert \
				-D OPEQ=MFB_OPEQ_INVERT mfbplygblt.c

$LIB/mfbigbwht.o:	mfbimggblt.c
			$CC $COPTS $INC $DEF				\
				-D MFBIMAGEGLYPHBLT=mfbImageGlyphBltWhite \
				-D OPEQ=MFB_OPEQ_WHITE mfbimggblt.c

$LIB/mfbigbblak.o:	mfbimggblt.c
			$CC $COPTS $INC $DEF				\
				-D MFBIMAGEGLYPHBLT=mfbImageGlyphBltBlack \
				-D OPEQ=MFB_OPEQ_BLACK mfbimggblt.c

$LIB/mfbcmap.o:		mfbcmap.c
			$CC $COPTS $INC $DEF mfbcmap.c

$LIB/mfbpawhite.o:	mfbpntarea.c
			$CC $COPTS $INC $DEF				\
				-D MFBSOLIDFILLAREA=mfbSolidWhiteArea 	\
				-D MFBSTIPPLEFILLAREA=mfbStippleWhiteArea\
				-D OPEQ=MFB_OPEQ_WHITE 			\
				-D EQWHOLEWORD=MFB_EQWHOLEWORD_WHITE mfbpntarea.c

$LIB/mfbpablack.o:	mfbpntarea.c
			$CC $COPTS $INC $DEF				\
				-D MFBSOLIDFILLAREA=mfbSolidBlackArea 	\
				-D MFBSTIPPLEFILLAREA=mfbStippleBlackArea\
				-D OPEQ=MFB_OPEQ_BLACK 			\
				-D EQWHOLEWORD=MFB_EQWHOLEWORD_BLACK mfbpntarea.c

$LIB/mfbpainv.o:	mfbpntarea.c
			$CC $COPTS $INC $DEF				\
				-D MFBSOLIDFILLAREA=mfbSolidInvertArea 	\
				-D MFBSTIPPLEFILLAREA=mfbStippleInvertArea\
				-D OPEQ=MFB_OPEQ_INVERT 		\
				-D EQWHOLEWORD=MFB_EQWHOLEWORD_INVERT mfbpntarea.c

$LIB/mfbtileC.o:	mfbtile.c
			$CC $COPTS $INC $DEF -D MROP=Mcopy mfbtile.c

$LIB/mfbtileG.o:	mfbtile.c
			$CC $COPTS $INC $DEF -D MROP=0 mfbtile.c

$LIB/mfbtewhite.o:	mfbtegblt.c
			$CC $COPTS $INC $DEF				\
				-D MFBTEGLYPHBLT=mfbTEGlyphBltWhite 	\
				-D OP=MFB_OP_WHITE			\
				-D CLIPTETEXT=mfbImageGlyphBltWhite mfbtegblt.c

$LIB/mfbteblack.o:	mfbtegblt.c
			$CC $COPTS $INC $DEF				\
				-D MFBTEGLYPHBLT=mfbTEGlyphBltBlack 	\
				-D OP=MFB_OP_BLACK			\
				-D CLIPTETEXT=mfbImageGlyphBltBlack mfbtegblt.c

$LIB/mfbmisc.o:		mfbmisc.c
			$CC $COPTS $INC $DEF mfbmisc.c

$LIB/mfbbstore.o:		mfbbstore.c
			$CC $COPTS $INC $DEF mfbbstore.c

$LIB/mfbplywhite.o:	mfbply1rct.c
			$CC $COPTS $INC $DEF				\
				-D MFBFILLPOLY1RECT=mfbFillPolyWhite 	\
				-D OPEQ=MFB_OPEQ_WHITE			\
				-D EQWHOLEWORD=MFB_EQWHOLEWORD_WHITE mfbply1rct.c

$LIB/mfbplyblack.o:	mfbply1rct.c
			$CC $COPTS $INC $DEF				\
				-D MFBFILLPOLY1RECT=mfbFillPolyBlack 	\
				-D OPEQ=MFB_OPEQ_BLACK			\
				-D EQWHOLEWORD=MFB_EQWHOLEWORD_BLACK mfbply1rct.c

$LIB/mfbplyinv.o:	mfbply1rct.c
			$CC $COPTS $INC $DEF				\
				-D MFBFILLPOLY1RECT=mfbFillPolyInvert 	\
				-D OPEQ=MFB_OPEQ_INVERT			\
				-D EQWHOLEWORD=MFB_EQWHOLEWORD_INVERT mfbply1rct.c
