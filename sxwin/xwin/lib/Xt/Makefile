#*
#****************************************************************
#	Makefile para a biblioteca "Xt"				*
#****************************************************************
#*
LIB	=	lib

DEFS	=	-D entry=_entry_		\
		-D CSRG_BASED			\
		-D USE_SNPRINTF			\
		-D XT_NO_SM			\
		-D LIBXT 			\
		-D TROPIX

INC	=	-I .				\
		-I ../../include		\
		-I ../../include/X11

CC	=	cc -c -O -o $@

#*
#****************************************************************
#*	Geração da biblioteca					*
#****************************************************************
#*
OBJ	=	$LIB/ActionHook.o	\
	 	$LIB/Alloc.o		\
		$LIB/ArgList.o 		\
		$LIB/Callback.o		\
		$LIB/ClickTime.o	\
		$LIB/Composite.o	\
		$LIB/Constraint.o	\
		$LIB/Convert.o 		\
		$LIB/Converters.o	\
		$LIB/Core.o		\
		$LIB/Create.o		\
		$LIB/Destroy.o		\
		$LIB/Display.o		\
		$LIB/Error.o 		\
		$LIB/Event.o 		\
		$LIB/EventUtil.o	\
	 	$LIB/Functions.o	\
	 	$LIB/GCManager.o	\
	 	$LIB/Geometry.o		\
	 	$LIB/GetActKey.o 	\
		$LIB/GetResList.o	\
	 	$LIB/GetValues.o	\
	 	$LIB/HookObj.o		\
	 	$LIB/Hooks.o		\
	 	$LIB/Initialize.o 	\
		$LIB/Intrinsic.o	\
	 	$LIB/Keyboard.o		\
	 	$LIB/Manage.o		\
	 	$LIB/NextEvent.o	\
	 	$LIB/Object.o		\
	 	$LIB/PassivGrab.o 	\
		$LIB/Pointer.o		\
	 	$LIB/Popup.o		\
	 	$LIB/PopupCB.o		\
	 	$LIB/RectObj.o		\
	 	$LIB/ResConfig.o	\
	 	$LIB/Resources.o	\
	 	$LIB/Selection.o 	\
		$LIB/SetSens.o		\
	 	$LIB/SetValues.o	\
	 	$LIB/SetWMCW.o		\
	 	$LIB/Shell.o		\
	 	$LIB/StringDefs.o	\
	 	$LIB/Threads.o 		\
		$LIB/TMaction.o 	\
		$LIB/TMgrab.o		\
	 	$LIB/TMkey.o		\
	 	$LIB/TMparse.o		\
	 	$LIB/TMprint.o		\
	 	$LIB/TMstate.o 		\
		$LIB/VarCreate.o 	\
		$LIB/VarGet.o		\
	 	$LIB/Varargs.o
#	 	$LIB/Vendor.o

$LIB/.LIBSYM:	$LIB $OBJ
		mklib $LIB

$LIB:		.exist
		mkdir -f $LIB

.virtual	links

links:
		ln -f			\
			CallbackI.h	\
			Composite.h	\
			CompositeP.h	\
			ConstrainP.h	\
			Constraint.h	\
			ConvertI.h	\
			Core.h		\
			CoreP.h		\
			CreateI.h	\
			EventI.h	\
			HookObjI.h	\
			InitialI.h	\
			Intrinsic.h	\
			IntrinsicI.h	\
			IntrinsicP.h	\
			Object.h	\
			ObjectP.h	\
			PassivGraI.h	\
			RectObj.h	\
			RectObjP.h	\
			ResConfigP.h	\
			ResourceI.h	\
			SelectionI.h	\
			Shell.h		\
			ShellI.h	\
			ShellP.h	\
			StringDefs.h	\
			ThreadsI.h	\
			TranslateI.h	\
			VarargsI.h	\
			Vendor.h	\
			VendorP.h	\
			Xtos.h		\
		../../include/X11
#*
#****************************************************************
#*	Compilação dos Diversos Módulos				*
#****************************************************************
#*
$LIB/ActionHook.o:	ActionHook.c
			$CC $INC $DEFS ActionHook.c

$LIB/Alloc.o:		Alloc.c
			$CC $INC $DEFS -D XTMALLOC_BC Alloc.c

$LIB/ArgList.o:		ArgList.c
			$CC $INC $DEFS ArgList.c

$LIB/Callback.o:	Callback.c
			$CC $INC $DEFS Callback.c

$LIB/ClickTime.o:	ClickTime.c
			$CC $INC $DEFS ClickTime.c

$LIB/Composite.o:	Composite.c
			$CC $INC $DEFS Composite.c

$LIB/Constraint.o:	Constraint.c
			$CC $INC $DEFS Constraint.c

$LIB/Convert.o:		Convert.c
			$CC $INC $DEFS Convert.c

$LIB/Converters.o:	Converters.c
			$CC $INC $DEFS Converters.c

$LIB/Core.o:		Core.c
			$CC $INC $DEFS Core.c

$LIB/Create.o:		Create.c
			$CC $INC $DEFS Create.c

$LIB/Destroy.o:		Destroy.c
			$CC $INC $DEFS Destroy.c

$LIB/Display.o:		Display.c
			$CC $INC $DEFS Display.c

$LIB/Error.o:		Error.c
			$CC $INC $DEFS						\
				-D ERRORDB=\"/usr/xwin/conf/XtErrorDB\"	\
				-D XTERROR_PREFIX=\"\"			\
		 		-D XTWARNING_PREFIX=\"\" Error.c

$LIB/Event.o:		Event.c
			$CC $INC $DEFS Event.c

$LIB/EventUtil.o:	EventUtil.c
			$CC $INC $DEFS EventUtil.c

$LIB/Functions.o:	Functions.c
			$CC $INC $DEFS Functions.c

$LIB/GCManager.o:	GCManager.c
			$CC $INC $DEFS GCManager.c

$LIB/Geometry.o:	Geometry.c
			$CC $INC $DEFS Geometry.c

$LIB/GetActKey.o:	GetActKey.c
			$CC $INC $DEFS GetActKey.c

$LIB/GetResList.o:	GetResList.c
			$CC $INC $DEFS GetResList.c

$LIB/GetValues.o:	GetValues.c
			$CC $INC $DEFS GetValues.c

$LIB/HookObj.o:		HookObj.c
			$CC $INC $DEFS HookObj.c

$LIB/Hooks.o:		Hooks.c
			$CC $INC $DEFS Hooks.c

$LIB/Initialize.o:	Initialize.c
			$CC $INC $DEFS Initialize.c

$LIB/Intrinsic.o:	Intrinsic.c
			$CC $INC $DEFS					     \
			-D XFILESEARCHPATHDEFAULT=\"/usr/xwin/conf/Xt/%N%S\" \
			Intrinsic.c

$LIB/Keyboard.o:	Keyboard.c
			$CC $INC $DEFS Keyboard.c

$LIB/Manage.o:		Manage.c
			$CC $INC $DEFS Manage.c

$LIB/NextEvent.o:	NextEvent.c
			$CC $INC $DEFS NextEvent.c

$LIB/Object.o:		Object.c
			$CC $INC $DEFS Object.c

$LIB/PassivGrab.o:	PassivGrab.c
			$CC $INC $DEFS PassivGrab.c

$LIB/Pointer.o:		Pointer.c
			$CC $INC $DEFS Pointer.c

$LIB/Popup.o:		Popup.c
			$CC $INC $DEFS Popup.c

$LIB/PopupCB.o:		PopupCB.c
			$CC $INC $DEFS PopupCB.c

$LIB/RectObj.o:		RectObj.c
			$CC $INC $DEFS RectObj.c

$LIB/ResConfig.o:	ResConfig.c
			$CC $INC $DEFS ResConfig.c

$LIB/Resources.o:	Resources.c
			$CC $INC $DEFS Resources.c

$LIB/Selection.o:	Selection.c
			$CC $INC $DEFS Selection.c

$LIB/SetSens.o:		SetSens.c
			$CC $INC $DEFS SetSens.c

$LIB/SetValues.o:	SetValues.c
			$CC $INC $DEFS SetValues.c

$LIB/SetWMCW.o:		SetWMCW.c
			$CC $INC $DEFS SetWMCW.c

$LIB/Shell.o:		Shell.c
			$CC $INC $DEFS Shell.c

$LIB/StringDefs.o:	StringDefs.c
			$CC $INC $DEFS StringDefs.c

$LIB/Threads.o:		Threads.c
			$CC $INC $DEFS Threads.c

$LIB/TMaction.o:	TMaction.c
			$CC $INC $DEFS TMaction.c

$LIB/TMgrab.o:		TMgrab.c
			$CC $INC $DEFS TMgrab.c

$LIB/TMkey.o:		TMkey.c
			$CC $INC $DEFS TMkey.c

$LIB/TMparse.o:		TMparse.c
			$CC $INC $DEFS TMparse.c

$LIB/TMprint.o:		TMprint.c
			$CC $INC $DEFS TMprint.c

$LIB/TMstate.o:		TMstate.c
			$CC $INC $DEFS TMstate.c

$LIB/VarCreate.o:	VarCreate.c
			$CC $INC $DEFS VarCreate.c

$LIB/VarGet.o:		VarGet.c
			$CC $INC $DEFS VarGet.c

$LIB/Varargs.o:		Varargs.c
			$CC $INC $DEFS Varargs.c

#$LIB/Vendor.o:		Vendor.c
#			$CC $INC $DEFS Vendor.c

StringDefs.c:		util/string.list.v	\
			makestrs
			makestrs  < util/string.list.v > StringDefs.c

makestrs:		util/makestrs.c
			cc -D TROPIX $INC -O -o $@ util/makestrs.c
