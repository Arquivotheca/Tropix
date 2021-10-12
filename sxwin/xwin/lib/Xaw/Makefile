#*
#****************************************************************
#*	Makefile para a biblioteca Xaw				*
#****************************************************************
#*
LIB	=	lib

DEF	=	-D CSRG_BASED			\
		-D USE_XWCHAR_STRING		\
		-D USE_XMBTOWC			\
		-D entry=_entry_		\
		-D NO_LIBC_I18N			\
		-D PROJECT_ROOT=\"/usr/xwin\"	\
		-D TROPIX

INC	=	-I .				\
		-I ../../include		\
		-I ../../include/X11		\
		-I ../../include/X11/Xmu

CC	=	cc -c -O -o $@

#*
#****************************************************************
#*	Geração da biblioteca					*
#****************************************************************
#*
OBJ	=	$LIB/Actions.o		\
		$LIB/AllWidgets.o	\
		$LIB/AsciiSink.o	\
		$LIB/AsciiSrc.o		\
		$LIB/AsciiText.o	\
		$LIB/Box.o		\
		$LIB/Command.o		\
		$LIB/Converters.o	\
		$LIB/Dialog.o		\
		$LIB/DisplayList.o	\
		$LIB/Form.o		\
		$LIB/Grip.o		\
		$LIB/Label.o		\
		$LIB/List.o		\
		$LIB/MenuButton.o	\
		$LIB/MultiScr.o		\
		$LIB/MultiSink.o	\
		$LIB/OS.o		\
		$LIB/Paned.o		\
		$LIB/Panner.o		\
		$LIB/Pixmap.o		\
		$LIB/Porthole.o		\
		$LIB/Repeater.o		\
		$LIB/Scrollbar.o	\
		$LIB/Simple.o		\
		$LIB/SimpleMenu.o	\
		$LIB/Sme.o		\
		$LIB/SmeBSB.o		\
		$LIB/SmeLine.o		\
		$LIB/StripChart.o	\
		$LIB/Text.o		\
		$LIB/TextSink.o		\
		$LIB/TextSrc.o		\
		$LIB/TextAction.o	\
		$LIB/TextPop.o		\
		$LIB/TextTr.o		\
		$LIB/Tip.o		\
		$LIB/Toggle.o		\
		$LIB/Tree.o		\
		$LIB/Vendor.o		\
		$LIB/ViewPort.o		\
		$LIB/XawIm.o		\
		$LIB/XawI18n.o		\
		$LIB/XawInit.o

$LIB/.LIBSYM:	$LIB $OBJ
		mklib $LIB

$LIB:		.exist
		mkdir -f $LIB

.virtual	links

links: 		../../include/X11/Xaw
		ln -f			\
		AllWidgets.h		\
		AsciiSink.h		\
		AsciiSinkP.h		\
		AsciiSrc.h		\
		AsciiSrcP.h		\
		AsciiText.h		\
		AsciiTextP.h		\
		Box.h			\
		BoxP.h			\
		Cardinals.h		\
		Command.h		\
		CommandP.h		\
		Dialog.h		\
		DialogP.h		\
		Form.h			\
		FormP.h			\
		Grip.h			\
		GripP.h			\
		Label.h			\
		LabelP.h		\
		List.h			\
		ListP.h			\
		MenuButtoP.h		\
		MenuButton.h		\
		MultiSink.h		\
		MultiSinkP.h		\
		MultiSrc.h		\
		MultiSrcP.h		\
		Paned.h			\
		PanedP.h		\
		Panner.h		\
		PannerP.h		\
		Porthole.h		\
		PortholeP.h		\
		Private.h		\
		Repeater.h		\
		RepeaterP.h		\
		Reports.h		\
		Scrollbar.h		\
		ScrollbarP.h		\
		Simple.h		\
		SimpleMenP.h		\
		SimpleMenu.h		\
		SimpleP.h		\
		Sme.h			\
		SmeBSB.h		\
		SmeBSBP.h		\
		SmeLine.h		\
		SmeLineP.h		\
		SmeP.h			\
		StripCharP.h		\
		StripChart.h		\
		Template.h		\
		TemplateP.h		\
		Text.h			\
		TextP.h			\
		TextSink.h		\
		TextSinkP.h		\
		TextSrc.h		\
		TextSrcP.h		\
		Tip.h			\
		TipP.h			\
		Toggle.h		\
		ToggleP.h		\
		Tree.h			\
		TreeP.h			\
		VendorEP.h		\
		Viewport.h		\
		ViewportP.h		\
		XawI18n.h		\
		XawImP.h		\
		XawInit.h		\
		../../include/X11/Xaw

../../include/X11/Xaw:	.exist
			mkdir -f ../../include/X11/Xaw

#*
#****************************************************************
#*	Compilação dos Diversos Módulos				*
#****************************************************************
#*
$LIB/Actions.o:		Actions.c
			$CC $INC $DEF Actions.c

$LIB/AllWidgets.o:	AllWidgets.c
			$CC $INC $DEF AllWidgets.c

$LIB/AsciiSink.o:	AsciiSink.c
			$CC $INC $DEF AsciiSink.c

$LIB/AsciiSrc.o:	AsciiSrc.c
			$CC $INC $DEF AsciiSrc.c

$LIB/AsciiText.o:	AsciiText.c
			$CC $INC $DEF AsciiText.c

$LIB/Box.o:		Box.c
			$CC $INC $DEF Box.c

$LIB/Command.o:		Command.c
			$CC $INC $DEF Command.c

$LIB/Converters.o:	Converters.c
			$CC $INC $DEF Converters.c

$LIB/Dialog.o:		Dialog.c
			$CC $INC $DEF Dialog.c

$LIB/DisplayList.o:	DisplayList.c
			$CC $INC $DEF DisplayList.c

$LIB/Form.o:		Form.c
			$CC $INC $DEF Form.c

$LIB/Grip.o:		Grip.c
			$CC $INC $DEF Grip.c

$LIB/Label.o:		Label.c
			$CC $INC $DEF Label.c

$LIB/List.o:		List.c
			$CC $INC $DEF List.c

$LIB/MenuButton.o:	MenuButton.c
			$CC $INC $DEF MenuButton.c

$LIB/MultiScr.o:	MultiSrc.c
			$CC $INC $DEF MultiSrc.c

$LIB/MultiSink.o:	MultiSink.c
			$CC $INC $DEF MultiSink.c

$LIB/OS.o:		OS.c
			$CC $INC $DEF -U HAS_GETPAGESIZE OS.c

$LIB/Paned.o:		Paned.c
			$CC $INC $DEF Paned.c

$LIB/Panner.o:		Panner.c
			$CC $INC $DEF Panner.c

$LIB/Pixmap.o:		Pixmap.c
			$CC $INC $DEF Pixmap.c

$LIB/Porthole.o:	Porthole.c
			$CC $INC $DEF Porthole.c

$LIB/Repeater.o:	Repeater.c
			$CC $INC $DEF Repeater.c

$LIB/Scrollbar.o:	Scrollbar.c
			$CC $INC $DEF Scrollbar.c

$LIB/Simple.o:		Simple.c
			$CC $INC $DEF Simple.c

$LIB/SimpleMenu.o:	SimpleMenu.c
			$CC $INC $DEF SimpleMenu.c

$LIB/Sme.o:		Sme.c
			$CC $INC $DEF Sme.c

$LIB/SmeBSB.o:		SmeBSB.c
			$CC $INC $DEF SmeBSB.c

$LIB/SmeLine.o:		SmeLine.c
			$CC $INC $DEF SmeLine.c

$LIB/StripChart.o:	StripChart.c
			$CC $INC $DEF StripChart.c

$LIB/Text.o:		Text.c
			$CC $INC $DEF Text.c

$LIB/TextSink.o:	TextSink.c
			$CC $INC $DEF TextSink.c

$LIB/TextSrc.o:		TextSrc.c
			$CC $INC $DEF TextSrc.c

$LIB/TextAction.o:	TextAction.c
			$CC $INC $DEF TextAction.c

$LIB/TextPop.o:		TextPop.c
			$CC $INC $DEF TextPop.c

$LIB/TextTr.o:		TextTr.c
			$CC $INC $DEF TextTr.c

$LIB/Tip.o:		Tip.c
			$CC $INC $DEF Tip.c

$LIB/Toggle.o:		Toggle.c
			$CC $INC $DEF Toggle.c

$LIB/Tree.o:		Tree.c
			$CC $INC $DEF Tree.c

$LIB/Vendor.o:		Vendor.c
			$CC $INC $DEF Vendor.c

$LIB/ViewPort.o:	Viewport.c
			$CC $INC $DEF Viewport.c

$LIB/XawIm.o:		XawIm.c
			$CC $INC $DEF XawIm.c

$LIB/XawI18n.o:		XawI18n.c
			$CC $INC $DEF XawI18n.c

$LIB/XawInit.o:		XawInit.c
			$CC $INC $DEF XawInit.c
