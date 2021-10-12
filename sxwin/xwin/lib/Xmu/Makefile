#*
#****************************************************************
#*	Makefile para a biblioteca Xmu				*				
#****************************************************************
#*
LIB	=	lib

DEF	=	-D CSRG_BASED		\
		-D HAS_SNPRINTF		\
		-D entry=_entry_	\
		-D TROPIX

INC	=	-I .			\
		-I ../../include	\
		-I ../../include/X11	\
		-I ../../include/X11/Xmu

CC	=	cc -c -O -o $@

#*
#****************************************************************
#*	Geração da biblioteca					*
#****************************************************************
#*
OBJ	=	$LIB/AllCmap.o		\
		$LIB/Atoms.o		\
		$LIB/ClientWin.o	\
		$LIB/Clip.o		\
		$LIB/CloseHook.o	\
		$LIB/CmapAlloc.o	\
		$LIB/CrCmap.o		\
		$LIB/CrPixFBit.o	\
		$LIB/CursorName.o	\
		$LIB/CvtCache.o		\
		$LIB/CvtStdSel.o	\
		$LIB/DefErrMsg.o	\
		$LIB/DelCmap.o		\
		$LIB/DisplayQue.o	\
		$LIB/Distinct.o		\
		$LIB/DrRndRect.o	\
		$LIB/DrawLogo.o		\
		$LIB/ExtAgent.o		\
		$LIB/FtoCback.o		\
		$LIB/EditresCom.o	\
		$LIB/ExtAgent.o		\
		$LIB/FtoCback.o		\
		$LIB/GetHost.o		\
		$LIB/GrayPixmap.o	\
		$LIB/Initer.o		\
		$LIB/GrayPixmap.o	\
		$LIB/Initer.o		\
		$LIB/LocBitmap.o	\
		$LIB/Lookup.o		\
		$LIB/LookupCmap.o	\
		$LIB/Lower.o		\
		$LIB/RdBitF.o		\
		$LIB/ScrOfWin.o		\
		$LIB/ShapeWidg.o	\
		$LIB/ShapeWidg.o	\
		$LIB/StdCmap.o		\
		$LIB/StrToBS.o		\
		$LIB/StrToBmap.o	\
		$LIB/StrToCurs.o	\
		$LIB/StrToGrav.o	\
		$LIB/StrToJust.o	\
		$LIB/StrToLong.o	\
		$LIB/StrToOrnt.o	\
		$LIB/StrToShap.o	\
		$LIB/StrToWidg.o	\
		$LIB/UpdMapHint.o	\
		$LIB/VisCmap.o		\
		$LIB/WidgetNode.o	\
		$LIB/Xct.o

$LIB/.LIBSYM:	$LIB $OBJ
		mklib $LIB

$LIB:		.exist
		mkdir -f $LIB

.virtual	links

links: 		../../include/X11/Xmu
		ln -f			\
			Atoms.h		\
			CharSet.h	\
			CloseHook.h	\
			Converters.h	\
			CurUtil.h	\
			CvtCache.h	\
			DisplayQue.h	\
			Drawing.h	\
			Editres.h	\
			EditresP.h	\
			Error.h		\
			ExtAgent.h	\
			Initer.h	\
			Lookup.h	\
			Misc.h		\
			StdCmap.h	\
			StdSel.h	\
			SysUtil.h	\
			WhitePoint.h	\
			WidgetNode.h	\
			WinUtil.h	\
			Xct.h		\
			Xmu.h		\
		../../include/X11/Xmu

../../include/X11/Xmu:	.exist
			mkdir -f ../../include/X11/Xmu

#*
#****************************************************************
#*	Compilação dos diversos módulos				*
#****************************************************************
#*
$LIB/AllCmap.o:		AllCmap.c
			$CC $INC $DEF AllCmap.c

$LIB/Atoms.o:		Atoms.c
			$CC $INC $DEF Atoms.c

$LIB/ClientWin.o:	ClientWin.c
			$CC $INC $DEF ClientWin.c

$LIB/Clip.o:		Clip.c
			$CC $INC $DEF Clip.c

$LIB/CloseHook.o:	CloseHook.c
			$CC $INC $DEF CloseHook.c

$LIB/CmapAlloc.o:	CmapAlloc.c
			$CC $INC $DEF CmapAlloc.c

$LIB/CrCmap.o:		CrCmap.c
			$CC $INC $DEF CrCmap.c

$LIB/CrPixFBit.o:	CrPixFBit.c
			$CC $INC $DEF CrPixFBit.c

$LIB/CursorName.o:	CursorName.c
			$CC $INC $DEF CursorName.c

$LIB/CvtCache.o:	CvtCache.c
			$CC $INC $DEF CvtCache.c

$LIB/CvtStdSel.o:	CvtStdSel.c
			$CC $INC $DEF CvtStdSel.c

$LIB/DefErrMsg.o:	DefErrMsg.c
			$CC $INC $DEF DefErrMsg.c

$LIB/DelCmap.o:		DelCmap.c
			$CC $INC $DEF DelCmap.c

$LIB/DisplayQue.o:	DisplayQue.c
			$CC $INC $DEF DisplayQue.c

$LIB/Distinct.o:	Distinct.c
			$CC $INC $DEF Distinct.c

$LIB/DrRndRect.o:	DrRndRect.c
			$CC $INC $DEF DrRndRect.c

$LIB/DrawLogo.o:	DrawLogo.c
			$CC $INC $DEF DrawLogo.c

$LIB/EditresCom.o:	EditresCom.c
			$CC $INC $DEF EditresCom.c

$LIB/ExtAgent.o:	ExtAgent.c
			$CC $INC $DEF ExtAgent.c

$LIB/FtoCback.o:	FToCback.c
			$CC $INC $DEF FToCback.c

$LIB/GetHost.o:		GetHost.c
			$CC $INC $DEF GetHost.c

$LIB/GrayPixmap.o:	GrayPixmap.c
			$CC $INC $DEF GrayPixmap.c

$LIB/Initer.o:		Initer.c
			$CC $INC $DEF Initer.c

$LIB/LocBitmap.o:	LocBitmap.c
			$CC $INC $DEF	\
				-D BITMAPDIR=\"/usr/xwin/bitmaps\" LocBitmap.c

$LIB/Lookup.o:		Lookup.c
			$CC $INC $DEF Lookup.c

$LIB/LookupCmap.o:	LookupCmap.c
			$CC $INC $DEF LookupCmap.c

$LIB/Lower.o:		Lower.c
			$CC $INC $DEF Lower.c

$LIB/RdBitF.o:		RdBitF.c
			$CC $INC $DEF RdBitF.c

$LIB/ScrOfWin.o:	ScrOfWin.c
			$CC $INC $DEF ScrOfWin.c

$LIB/ShapeWidg.o:	ShapeWidg.c
			$CC $INC $DEF ShapeWidg.c

$LIB/StdCmap.o:		StdCmap.c
			$CC $INC $DEF StdCmap.c

$LIB/StrToBS.o:		StrToBS.c
			$CC $INC $DEF StrToBS.c

$LIB/StrToBmap.o:	StrToBmap.c
			$CC $INC $DEF StrToBmap.c

$LIB/StrToCurs.o:	StrToCurs.c
			$CC $INC $DEF StrToCurs.c

$LIB/StrToGrav.o:	StrToGrav.c
			$CC $INC $DEF StrToGrav.c

$LIB/StrToJust.o:	StrToJust.c
			$CC $INC $DEF StrToJust.c

$LIB/StrToLong.o:	StrToLong.c
			$CC $INC $DEF StrToLong.c

$LIB/StrToOrnt.o:	StrToOrnt.c
			$CC $INC $DEF StrToOrnt.c

$LIB/StrToShap.o:	StrToShap.c
			$CC $INC $DEF StrToShap.c

$LIB/StrToWidg.o:	StrToWidg.c
			$CC $INC $DEF StrToWidg.c

$LIB/UpdMapHint.o:	UpdMapHint.c
			$CC $INC $DEF UpdMapHint.c

$LIB/VisCmap.o:		VisCmap.c
			$CC $INC $DEF VisCmap.c

$LIB/WidgetNode.o:	WidgetNode.c
			$CC $INC $DEF WidgetNode.c

$LIB/Xct.o:		Xct.c
			$CC $INC $DEF Xct.c
