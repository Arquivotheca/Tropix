#*
#****************************************************************
#	Makefile para a biblioteca "lib/X11" (vulgo "Xlib")	*
#****************************************************************
#*
INC	=		-I .					\
			-I ../../include			\
			-I ../../include/X11			\
			-I ../../include/X11/extensions

DEF	=		-D CSRG_BASED				\
			-D MALLOC_0_RETURNS_NULL		\
			-D HAS_SNPRINTF				\
			-D LIBX11				\
			-D TROPIX				\
			-D entry=__entry

CC	=		cc
COPTS	=		-c -O -o $@
LIB	=		lib

.virtual		links

$LIB/.LIBSYM:		$LIB			\
			$LIB/Context.o		\
			$LIB/Depths.o		\
			$LIB/ParseCmd.o		\
			$LIB/Quarks.o		\
			$LIB/AllCells.o		\
			$LIB/AllPlanes.o	\
			$LIB/AllowEv.o		\
			$LIB/AutoRep.o		\
			$LIB/Backgnd.o		\
			$LIB/BdrWidth.o		\
			$LIB/Bell.o		\
			$LIB/Border.o		\
			$LIB/ChAccCon.o		\
			$LIB/ChClMode.o		\
			$LIB/ChActPGb.o		\
			$LIB/ChCmap.o		\
			$LIB/ChGC.o		\
			$LIB/ChKeyCon.o		\
			$LIB/ChPntCon.o		\
			$LIB/ChProp.o		\
			$LIB/ChSaveSet.o	\
			$LIB/ChWAttrs.o		\
			$LIB/ChWindow.o		\
			$LIB/ChkIfEv.o		\
			$LIB/ChkMaskEv.o	\
			$LIB/ChkWinEv.o		\
			$LIB/ChkTypEv.o		\
			$LIB/ChkTypWEv.o	\
			$LIB/CirWin.o		\
			$LIB/CirWinDn.o		\
			$LIB/CirWinUp.o		\
			$LIB/ClDisplay.o	\
			$LIB/Clear.o		\
			$LIB/ClearArea.o	\
			$LIB/ConfWind.o		\
			$LIB/ConnDis.o		\
			$LIB/ConvSel.o		\
			$LIB/CopyArea.o		\
			$LIB/CopyCmap.o		\
			$LIB/CopyGC.o		\
			$LIB/CopyPlane.o	\
			$LIB/CrBFData.o		\
			$LIB/CrCmap.o		\
			$LIB/CrCursor.o		\
			$LIB/CrGC.o		\
			$LIB/CrGlCur.o		\
			$LIB/CrPFBData.o	\
			$LIB/CrPixmap.o		\
			$LIB/CrWindow.o		\
			$LIB/Cursor.o		\
			$LIB/DefCursor.o	\
			$LIB/DelProp.o		\
			$LIB/DestSubs.o		\
			$LIB/DestWind.o		\
			$LIB/DisName.o		\
			$LIB/DrArc.o		\
			$LIB/DrArcs.o		\
			$LIB/DrLine.o		\
			$LIB/DrLines.o		\
			$LIB/DrPoint.o		\
			$LIB/DrPoints.o		\
			$LIB/DrRect.o		\
			$LIB/DrRects.o		\
			$LIB/DrSegs.o		\
			$LIB/ErrDes.o		\
			$LIB/ErrHndlr.o		\
			$LIB/EvToWire.o		\
			$LIB/FetchName.o	\
			$LIB/FillArc.o		\
			$LIB/FillArcs.o		\
			$LIB/FillPoly.o		\
			$LIB/FillRct.o		\
			$LIB/FillRcts.o		\
			$LIB/Flush.o		\
			$LIB/Font.o		\
			$LIB/FontNames.o	\
			$LIB/FontInfo.o		\
			$LIB/FreeCmap.o		\
			$LIB/FreeCols.o		\
			$LIB/FreeCurs.o		\
			$LIB/FreeEData.o	\
			$LIB/FreeGC.o		\
			$LIB/FreePix.o		\
			$LIB/FSSaver.o		\
			$LIB/GCMisc.o		\
			$LIB/Geom.o		\
			$LIB/GetAtomNm.o	\
			$LIB/GetColor.o		\
			$LIB/GetDflt.o		\
			$LIB/GetFPath.o		\
			$LIB/GetFProp.o		\
			$LIB/GetGeom.o		\
			$LIB/GetHColor.o	\
			$LIB/GetHints.o		\
			$LIB/GetIFocus.o	\
			$LIB/GetImage.o		\
			$LIB/GetKCnt.o		\
			$LIB/GetMoEv.o		\
			$LIB/GetPCnt.o		\
			$LIB/GetPntMap.o	\
			$LIB/GetProp.o		\
			$LIB/GetSOwner.o	\
			$LIB/GetSSaver.o	\
			$LIB/GetStCmap.o	\
			$LIB/GetWAttrs.o	\
			$LIB/GrButton.o		\
			$LIB/GrKey.o		\
			$LIB/GrKeybd.o		\
			$LIB/GrPointer.o	\
			$LIB/GrServer.o		\
			$LIB/Host.o		\
			$LIB/IfEvent.o		\
			$LIB/ImUtil.o		\
			$LIB/ImText.o		\
			$LIB/ImText16.o		\
			$LIB/InitExt.o		\
			$LIB/InsCmap.o		\
			$LIB/IntAtom.o		\
			$LIB/KeyBind.o		\
			$LIB/KeysymStr.o	\
			$LIB/KillCl.o		\
			$LIB/LiHosts.o		\
			$LIB/LiICmaps.o		\
			$LIB/LiProps.o		\
			$LIB/ListExt.o		\
			$LIB/LoadFont.o		\
			$LIB/LookupCol.o	\
			$LIB/LowerWin.o		\
			$LIB/Macros.o		\
			$LIB/MapRaised.o	\
			$LIB/MapSubs.o		\
			$LIB/MapWindow.o	\
			$LIB/MaskEvent.o	\
			$LIB/Misc.o		\
			$LIB/ModMap.o		\
			$LIB/MoveWin.o		\
			$LIB/NextEvent.o	\
			$LIB/OpenDis.o		\
			$LIB/ParseCol.o		\
			$LIB/ParseGeom.o	\
			$LIB/PeekEvent.o	\
			$LIB/PeekIfEv.o		\
			$LIB/Pending.o		\
			$LIB/PmapBgnd.o		\
			$LIB/PmapBord.o		\
			$LIB/PolyReg.o		\
			$LIB/PolyTxt.o		\
			$LIB/PolyTxt16.o	\
			$LIB/PutBEvent.o	\
			$LIB/PutImage.o		\
			$LIB/QuBest.o		\
			$LIB/QuColor.o		\
			$LIB/QuColors.o		\
			$LIB/QuCurShp.o		\
			$LIB/QuExt.o		\
			$LIB/QuKeybd.o		\
			$LIB/QuPntr.o		\
			$LIB/QuStipShp.o	\
			$LIB/QuTextE16.o	\
			$LIB/QuTextExt.o	\
			$LIB/QuTileShp.o	\
			$LIB/QuTree.o		\
			$LIB/RaiseWin.o		\
			$LIB/RdBitF.o		\
			$LIB/RecolorC.o		\
			$LIB/ReconfWin.o	\
			$LIB/Region.o		\
			$LIB/RepWindow.o	\
			$LIB/RestackWs.o	\
			$LIB/RotProp.o		\
			$LIB/ScrResStr.o	\
			$LIB/SelInput.o		\
			$LIB/SendEvent.o	\
			$LIB/SetBack.o		\
			$LIB/SetFore.o		\
			$LIB/SetCRects.o	\
			$LIB/SetClMask.o	\
			$LIB/SetClOrig.o	\
			$LIB/SetDashes.o	\
			$LIB/SetFPath.o		\
			$LIB/SetFont.o		\
			$LIB/SetFunc.o		\
			$LIB/SetHints.o		\
			$LIB/SetIFocus.o	\
			$LIB/SetLStyle.o	\
			$LIB/SetPMask.o		\
			$LIB/SetPntMap.o	\
			$LIB/SetSOwner.o	\
			$LIB/SetSSaver.o	\
			$LIB/SetState.o		\
			$LIB/SetStip.o		\
			$LIB/SetStCmap.o	\
			$LIB/SetTile.o		\
			$LIB/SetTSOrig.o	\
			$LIB/StBytes.o		\
			$LIB/StColor.o		\
			$LIB/StColors.o		\
			$LIB/StNColor.o		\
			$LIB/StName.o		\
			$LIB/StrKeysym.o	\
			$LIB/Sync.o		\
			$LIB/Synchro.o		\
			$LIB/Text.o		\
			$LIB/Text16.o		\
			$LIB/TextExt.o		\
			$LIB/TextExt16.o	\
			$LIB/TrCoords.o		\
			$LIB/UndefCurs.o	\
			$LIB/UngrabBut.o	\
			$LIB/UngrabKbd.o	\
			$LIB/UngrabKey.o	\
			$LIB/UngrabPtr.o	\
			$LIB/UngrabSvr.o	\
			$LIB/UninsCmap.o	\
			$LIB/UnldFont.o		\
			$LIB/UnmapSubs.o	\
			$LIB/UnmapWin.o		\
			$LIB/VisUtil.o		\
			$LIB/WarpPtr.o		\
			$LIB/WinEvent.o		\
			$LIB/Window.o		\
			$LIB/WrBitF.o		\
			$LIB/XlibAsync.o	\
			$LIB/XlibInt.o		\
			$LIB/Xrm.o		\
			$LIB/evtomask.o		\
			$LIB/globals.o		\
			$LIB/GetGCVals.o	\
			$LIB/GetNrmHint.o	\
			$LIB/GetRGBCMap.o	\
			$LIB/GetTxtProp.o	\
			$LIB/GetWMCMapW.o	\
			$LIB/GetWMProto.o	\
			$LIB/Iconify.o		\
			$LIB/PixFormats.o	\
			$LIB/PropAlloc.o	\
			$LIB/ReconfWM.o		\
			$LIB/SetNrmHint.o	\
			$LIB/SetRGBCMap.o	\
			$LIB/SetTxtProp.o	\
			$LIB/SetWMCMapW.o	\
			$LIB/SetWMProto.o	\
			$LIB/StrToText.o	\
			$LIB/TextToStr.o	\
			$LIB/Withdraw.o		\
			$LIB/WMGeom.o		\
			$LIB/WMProps.o		\
#			$LIB/AuDispose.o	\
#			$LIB/AuGetBest.o	\
#			$LIB/AuFileName.o	\
#			$LIB/AuRead.o		\
			$LIB/Lab.o		\
			$LIB/LabGcC.o		\
			$LIB/LabGcL.o		\
			$LIB/LabGcLC.o		\
			$LIB/LabMnL.o		\
			$LIB/LabMxC.o		\
			$LIB/LabMxL.o		\
			$LIB/LabMxLC.o		\
			$LIB/LabWpAj.o		\
			$LIB/Luv.o		\
			$LIB/LuvGcC.o		\
			$LIB/LuvGcL.o		\
			$LIB/LuvGcLC.o		\
			$LIB/LuvMnL.o		\
			$LIB/LuvMxC.o		\
			$LIB/LuvMxL.o		\
			$LIB/LuvMxLC.o		\
			$LIB/LuvWpAj.o		\
			$LIB/XYZ.o		\
			$LIB/uvY.o		\
			$LIB/xyY.o		\
			$LIB/HVC.o		\
			$LIB/HVCGcC.o		\
			$LIB/HVCGcV.o		\
			$LIB/HVCGcVC.o		\
			$LIB/HVCMnV.o		\
			$LIB/HVCMxC.o		\
			$LIB/HVCMxV.o		\
			$LIB/HVCMxVC.o		\
			$LIB/HVCMxVs.o		\
			$LIB/HVCWpAj.o		\
			$LIB/UNDEFINED.o	\
			$LIB/AddDIC.o		\
			$LIB/AddSF.o		\
			$LIB/cmsAllNCol.o	\
			$LIB/cmsAllCol.o	\
			$LIB/CCC.o		\
			$LIB/cmsCmap.o		\
			$LIB/cmsColNm.o		\
			$LIB/CvColW.o		\
			$LIB/CvCols.o		\
			$LIB/cmsGlobls.o	\
			$LIB/IdOfPr.o		\
			$LIB/cmsInt.o		\
			$LIB/LRGB.o		\
			$LIB/cmsLkCol.o		\
			$LIB/cmsMath.o		\
			$LIB/OfCCC.o		\
			$LIB/PrOfId.o		\
			$LIB/cmsProp.o		\
			$LIB/QBlack.o		\
			$LIB/QBlue.o		\
			$LIB/QGreen.o		\
			$LIB/QRed.o		\
			$LIB/QWhite.o		\
			$LIB/QuCol.o		\
			$LIB/QuCols.o		\
			$LIB/SetCCC.o		\
			$LIB/SetGetCols.o	\
			$LIB/StCol.o		\
			$LIB/StCols.o		\
			$LIB/cmsTrig.o		\
			$LIB/XRGB.o		\
			$LIB/FSWrap.o		\
			$LIB/ICWrap.o		\
			$LIB/IMWrap.o		\
			$LIB/OCWrap.o		\
			$LIB/OMWrap.o		\
			$LIB/lcWrap.o		\
			$LIB/mbWMProps.o	\
			$LIB/utf8WMProps.o	\
			$LIB/mbWrap.o		\
			$LIB/wcWrap.o		\
			$LIB/utf8Wrap.o		\
			$LIB/FilterEv.o		\
			$LIB/RegstFlt.o		\
			$LIB/locking.o		\
			$LIB/LockDis.o		\
			$LIB/x11trans.o		\
			$LIB/ximtrans.o		\
			$LIB/XKB.o		\
#			$LIB/XKBBind.o		\
			$LIB/XKBCompat.o	\
			$LIB/XKBCtrls.o		\
			$LIB/XKBCvt.o		\
			$LIB/XKBGetMap.o	\
			$LIB/XKBGetByName.o	\
			$LIB/XKBNames.o		\
			$LIB/XKBRdBuf.o		\
			$LIB/XKBSetMap.o	\
			$LIB/XKBUse.o		\
			$LIB/XKBleds.o		\
			$LIB/XKBBell.o		\
			$LIB/XKBGeom.o		\
			$LIB/XKBSetGeom.o	\
			$LIB/XKBExtDev.o	\
			$LIB/XKBList.o		\
			$LIB/XKBMisc.o		\
			$LIB/XKBMAlloc.o	\
			$LIB/XKBGAlloc.o	\
			$LIB/XKBAlloc.o		\
			$LIB/SetLocale.o	\
			$LIB/imCallbk.o		\
			$LIB/imConv.o		\
			$LIB/imDefFlt.o		\
			$LIB/imDefIc.o		\
			$LIB/imDefIm.o		\
			$LIB/imDefLkup.o	\
			$LIB/imDispch.o		\
			$LIB/imEvToWire.o	\
			$LIB/imExten.o		\
			$LIB/imImSw.o		\
			$LIB/imInsClbk.o	\
			$LIB/imInt.o		\
			$LIB/imKStoUCS.o	\
			$LIB/imLcFlt.o		\
			$LIB/imLcGIc.o		\
			$LIB/imLcIc.o		\
			$LIB/imLcIm.o		\
			$LIB/imLcLkup.o		\
			$LIB/imLcPrs.o		\
			$LIB/imLcSIc.o		\
			$LIB/imRm.o		\
			$LIB/imRmAttr.o		\
			$LIB/imThaiFlt.o	\
			$LIB/imThaiIc.o		\
			$LIB/imThaiIm.o		\
			$LIB/imTrX.o		\
			$LIB/imTransR.o		\
			$LIB/lcCT.o		\
			$LIB/lcCharSet.o	\
			$LIB/lcConv.o		\
			$LIB/lcDB.o		\
			$LIB/lcDefConv.o	\
#			$LIB/lcEuc.o		\
			$LIB/lcFile.o		\
			$LIB/lcGenConv.o	\
			$LIB/lcGeneric.o	\
			$LIB/lcInit.o		\
#			$LIB/lcJis.o		\
			$LIB/lcPrTxt.o		\
			$LIB/lcPubWrap.o	\
			$LIB/lcPublic.o		\
			$LIB/lcRM.o		\
#			$LIB/lcSjis.o		\
			$LIB/lcStd.o		\
			$LIB/lcTxtPr.o		\
			$LIB/lcUTF8.o		\
			$LIB/lcUTF8Load.o	\
			$LIB/lcUtil.o		\
			$LIB/omDefault.o	\
			$LIB/omGeneric.o	\
			$LIB/omImText.o		\
			$LIB/omText.o		\
			$LIB/omTextEsc.o	\
			$LIB/omTextExt.o	\
			$LIB/omTextPer.o	\
			$LIB/omXChar.o
			mklib $LIB

$LIB:			.exist
			mkdir -f $LIB

links:
			ln -f			\
				Xlib.h		\
				Xlibint.h	\
				Xresource.h	\
				Xcms.h		\
				Xutil.h		\
				XKBlib.h	\
				Xlocale.h	\
				cursorfont.h	\
				../../include/X11

$LIB/Context.o:		Context.c
			$CC $COPTS $INC $DEF Context.c

$LIB/Depths.o:		Depths.c
			$CC $COPTS $INC $DEF Depths.c

$LIB/ParseCmd.o:	ParseCmd.c
			$CC $COPTS $INC $DEF ParseCmd.c

$LIB/Quarks.o:		Quarks.c
			$CC $COPTS $INC $DEF Quarks.c

$LIB/AllCells.o:	AllCells.c
			$CC $COPTS $INC $DEF AllCells.c

$LIB/AllPlanes.o:	AllPlanes.c
			$CC $COPTS $INC $DEF AllPlanes.c

$LIB/AllowEv.o:		AllowEv.c
			$CC $COPTS $INC $DEF AllowEv.c

$LIB/AutoRep.o:		AutoRep.c
			$CC $COPTS $INC $DEF AutoRep.c

$LIB/Backgnd.o:		Backgnd.c
			$CC $COPTS $INC $DEF Backgnd.c

$LIB/BdrWidth.o:	BdrWidth.c
			$CC $COPTS $INC $DEF BdrWidth.c

$LIB/Bell.o:		Bell.c
			$CC $COPTS $INC $DEF Bell.c

$LIB/Border.o:		Border.c
			$CC $COPTS $INC $DEF Border.c

$LIB/ChAccCon.o:	ChAccCon.c
			$CC $COPTS $INC $DEF ChAccCon.c

$LIB/ChClMode.o:	ChClMode.c
			$CC $COPTS $INC $DEF ChClMode.c

$LIB/ChActPGb.o:	ChActPGb.c
			$CC $COPTS $INC $DEF ChActPGb.c

$LIB/ChCmap.o:		ChCmap.c
			$CC $COPTS $INC $DEF ChCmap.c

$LIB/ChGC.o:		ChGC.c
			$CC $COPTS $INC $DEF ChGC.c

$LIB/ChKeyCon.o:	ChKeyCon.c
			$CC $COPTS $INC $DEF ChKeyCon.c

$LIB/ChPntCon.o:	ChPntCon.c
			$CC $COPTS $INC $DEF ChPntCon.c

$LIB/ChProp.o:		ChProp.c
			$CC $COPTS $INC $DEF ChProp.c

$LIB/ChSaveSet.o:	ChSaveSet.c
			$CC $COPTS $INC $DEF ChSaveSet.c

$LIB/ChWAttrs.o:	ChWAttrs.c
			$CC $COPTS $INC $DEF ChWAttrs.c

$LIB/ChWindow.o:	ChWindow.c
			$CC $COPTS $INC $DEF ChWindow.c

$LIB/ChkIfEv.o:		ChkIfEv.c
			$CC $COPTS $INC $DEF ChkIfEv.c

$LIB/ChkMaskEv.o:	ChkMaskEv.c
			$CC $COPTS $INC $DEF ChkMaskEv.c

$LIB/ChkWinEv.o:	ChkWinEv.c
			$CC $COPTS $INC $DEF ChkWinEv.c

$LIB/ChkTypEv.o:	ChkTypEv.c
			$CC $COPTS $INC $DEF ChkTypEv.c

$LIB/ChkTypWEv.o:	ChkTypWEv.c
			$CC $COPTS $INC $DEF ChkTypWEv.c

$LIB/CirWin.o:		CirWin.c
			$CC $COPTS $INC $DEF CirWin.c

$LIB/CirWinDn.o:	CirWinDn.c
			$CC $COPTS $INC $DEF CirWinDn.c

$LIB/CirWinUp.o:	CirWinUp.c
			$CC $COPTS $INC $DEF CirWinUp.c

$LIB/ClDisplay.o:	ClDisplay.c
			$CC $COPTS $INC $DEF ClDisplay.c

$LIB/Clear.o:		Clear.c
			$CC $COPTS $INC $DEF Clear.c

$LIB/ClearArea.o:	ClearArea.c
			$CC $COPTS $INC $DEF ClearArea.c

$LIB/ConfWind.o:	ConfWind.c
			$CC $COPTS $INC $DEF ConfWind.c

$LIB/ConnDis.o:		ConnDis.c
			$CC $COPTS $INC -I ../xtrans	\
			$DEF -D TRANS_CLIENT -D X11_t ConnDis.c

$LIB/ConvSel.o:		ConvSel.c
			$CC $COPTS $INC $DEF ConvSel.c

$LIB/CopyArea.o:	CopyArea.c
			$CC $COPTS $INC $DEF CopyArea.c

$LIB/CopyCmap.o:	CopyCmap.c
			$CC $COPTS $INC $DEF CopyCmap.c

$LIB/CopyGC.o:		CopyGC.c
			$CC $COPTS $INC $DEF CopyGC.c

$LIB/CopyPlane.o:	CopyPlane.c
			$CC $COPTS $INC $DEF CopyPlane.c

$LIB/CrBFData.o:	CrBFData.c
			$CC $COPTS $INC $DEF CrBFData.c

$LIB/CrCmap.o:		CrCmap.c
			$CC $COPTS $INC $DEF CrCmap.c

$LIB/CrCursor.o:	CrCursor.c
			$CC $COPTS $INC $DEF CrCursor.c

$LIB/CrGC.o:		CrGC.c
			$CC $COPTS $INC $DEF CrGC.c

$LIB/CrGlCur.o:		CrGlCur.c
			$CC $COPTS $INC $DEF CrGlCur.c

$LIB/CrPFBData.o:	CrPFBData.c
			$CC $COPTS $INC $DEF CrPFBData.c

$LIB/CrPixmap.o:	CrPixmap.c
			$CC $COPTS $INC $DEF CrPixmap.c

$LIB/CrWindow.o:	CrWindow.c
			$CC $COPTS $INC $DEF CrWindow.c

$LIB/Cursor.o:		Cursor.c
			$CC $COPTS $INC $DEF Cursor.c

$LIB/DefCursor.o:	DefCursor.c
			$CC $COPTS $INC $DEF DefCursor.c

$LIB/DelProp.o:		DelProp.c
			$CC $COPTS $INC $DEF DelProp.c

$LIB/DestSubs.o:	DestSubs.c
			$CC $COPTS $INC $DEF DestSubs.c

$LIB/DestWind.o:	DestWind.c
			$CC $COPTS $INC $DEF DestWind.c

$LIB/DisName.o:		DisName.c
			$CC $COPTS $INC $DEF DisName.c

$LIB/DrArc.o:		DrArc.c
			$CC $COPTS $INC $DEF DrArc.c

$LIB/DrArcs.o:		DrArcs.c
			$CC $COPTS $INC $DEF DrArcs.c

$LIB/DrLine.o:		DrLine.c
			$CC $COPTS $INC $DEF DrLine.c

$LIB/DrLines.o:		DrLines.c
			$CC $COPTS $INC $DEF DrLines.c

$LIB/DrPoint.o:		DrPoint.c
			$CC $COPTS $INC $DEF DrPoint.c

$LIB/DrPoints.o:	DrPoints.c
			$CC $COPTS $INC $DEF DrPoints.c

$LIB/DrRect.o:		DrRect.c
			$CC $COPTS $INC $DEF DrRect.c

$LIB/DrRects.o:		DrRects.c
			$CC $COPTS $INC $DEF DrRects.c

$LIB/DrSegs.o:		DrSegs.c
			$CC $COPTS $INC $DEF DrSegs.c

$LIB/ErrDes.o:		ErrDes.c
			$CC $COPTS $INC $DEF ErrDes.c

$LIB/ErrHndlr.o:	ErrHndlr.c
			$CC $COPTS $INC $DEF ErrHndlr.c

$LIB/EvToWire.o:	EvToWire.c
			$CC $COPTS $INC $DEF EvToWire.c

$LIB/FetchName.o:	FetchName.c
			$CC $COPTS $INC $DEF FetchName.c

$LIB/FillArc.o:		FillArc.c
			$CC $COPTS $INC $DEF FillArc.c

$LIB/FillArcs.o:	FillArcs.c
			$CC $COPTS $INC $DEF FillArcs.c

$LIB/FillPoly.o:	FillPoly.c
			$CC $COPTS $INC $DEF FillPoly.c

$LIB/FillRct.o:		FillRct.c
			$CC $COPTS $INC $DEF FillRct.c

$LIB/FillRcts.o:	FillRcts.c
			$CC $COPTS $INC $DEF FillRcts.c

$LIB/Flush.o:		Flush.c
			$CC $COPTS $INC $DEF Flush.c

$LIB/Font.o:		Font.c
			$CC $COPTS $INC $DEF -D XF86BIGFONT Font.c

$LIB/FontNames.o:	FontNames.c
			$CC $COPTS $INC $DEF FontNames.c

$LIB/FontInfo.o:	FontInfo.c
			$CC $COPTS $INC $DEF -D XF86BIGFONT FontInfo.c

$LIB/FreeCmap.o:	FreeCmap.c
			$CC $COPTS $INC $DEF FreeCmap.c

$LIB/FreeCols.o:	FreeCols.c
			$CC $COPTS $INC $DEF FreeCols.c

$LIB/FreeCurs.o:	FreeCurs.c
			$CC $COPTS $INC $DEF FreeCurs.c

$LIB/FreeEData.o:	FreeEData.c
			$CC $COPTS $INC $DEF FreeEData.c

$LIB/FreeGC.o:		FreeGC.c
			$CC $COPTS $INC $DEF FreeGC.c

$LIB/FreePix.o:		FreePix.c
			$CC $COPTS $INC $DEF FreePix.c

$LIB/FSSaver.o:		FSSaver.c
			$CC $COPTS $INC $DEF FSSaver.c

$LIB/GCMisc.o:		GCMisc.c
			$CC $COPTS $INC $DEF GCMisc.c

$LIB/Geom.o:		Geom.c
			$CC $COPTS $INC $DEF Geom.c

$LIB/GetAtomNm.o:	GetAtomNm.c
			$CC $COPTS $INC $DEF GetAtomNm.c

$LIB/GetColor.o:	GetColor.c
			$CC $COPTS $INC $DEF GetColor.c

$LIB/GetDflt.o:		GetDflt.c
			$CC $COPTS $INC $DEF GetDflt.c

$LIB/GetFPath.o:	GetFPath.c
			$CC $COPTS $INC $DEF GetFPath.c

$LIB/GetFProp.o:	GetFProp.c
			$CC $COPTS $INC $DEF GetFProp.c

$LIB/GetGeom.o:		GetGeom.c
			$CC $COPTS $INC $DEF GetGeom.c

$LIB/GetHColor.o:	GetHColor.c
			$CC $COPTS $INC $DEF GetHColor.c

$LIB/GetHints.o:	GetHints.c
			$CC $COPTS $INC $DEF GetHints.c

$LIB/GetIFocus.o:	GetIFocus.c
			$CC $COPTS $INC $DEF GetIFocus.c

$LIB/GetImage.o:	GetImage.c
			$CC $COPTS $INC $DEF GetImage.c

$LIB/GetKCnt.o:		GetKCnt.c
			$CC $COPTS $INC $DEF GetKCnt.c

$LIB/GetMoEv.o:		GetMoEv.c
			$CC $COPTS $INC $DEF GetMoEv.c

$LIB/GetPCnt.o:		GetPCnt.c
			$CC $COPTS $INC $DEF GetPCnt.c

$LIB/GetPntMap.o:	GetPntMap.c
			$CC $COPTS $INC $DEF GetPntMap.c

$LIB/GetProp.o:		GetProp.c
			$CC $COPTS $INC $DEF GetProp.c

$LIB/GetSOwner.o:	GetSOwner.c
			$CC $COPTS $INC $DEF GetSOwner.c

$LIB/GetSSaver.o:	GetSSaver.c
			$CC $COPTS $INC $DEF GetSSaver.c

$LIB/GetStCmap.o:	GetStCmap.c
			$CC $COPTS $INC $DEF GetStCmap.c

$LIB/GetWAttrs.o:	GetWAttrs.c
			$CC $COPTS $INC $DEF GetWAttrs.c

$LIB/GrButton.o:	GrButton.c
			$CC $COPTS $INC $DEF GrButton.c

$LIB/GrKey.o:		GrKey.c
			$CC $COPTS $INC $DEF GrKey.c

$LIB/GrKeybd.o:		GrKeybd.c
			$CC $COPTS $INC $DEF GrKeybd.c

$LIB/GrPointer.o:	GrPointer.c
			$CC $COPTS $INC $DEF GrPointer.c

$LIB/GrServer.o:	GrServer.c
			$CC $COPTS $INC $DEF GrServer.c

$LIB/Host.o:		Host.c
			$CC $COPTS $INC $DEF Host.c

$LIB/IfEvent.o:		IfEvent.c
			$CC $COPTS $INC $DEF IfEvent.c

$LIB/ImUtil.o:		ImUtil.c
			$CC $COPTS $INC $DEF ImUtil.c

$LIB/ImText.o:		ImText.c
			$CC $COPTS $INC $DEF ImText.c

$LIB/ImText16.o:	ImText16.c
			$CC $COPTS $INC $DEF ImText16.c

$LIB/InitExt.o:		InitExt.c
			$CC $COPTS $INC $DEF InitExt.c

$LIB/InsCmap.o:		InsCmap.c
			$CC $COPTS $INC $DEF InsCmap.c

$LIB/IntAtom.o:		IntAtom.c
			$CC $COPTS $INC $DEF IntAtom.c

$LIB/KeyBind.o:		KeyBind.c
			$CC $COPTS $INC $DEF KeyBind.c

$LIB/KeysymStr.o:	KeysymStr.c	\
			ks_tables.h
			$CC $COPTS $INC $DEF KeysymStr.c

$LIB/KillCl.o:		KillCl.c
			$CC $COPTS $INC $DEF KillCl.c

$LIB/LiHosts.o:		LiHosts.c
			$CC $COPTS $INC $DEF LiHosts.c

$LIB/LiICmaps.o:	LiICmaps.c
			$CC $COPTS $INC $DEF LiICmaps.c

$LIB/LiProps.o:		LiProps.c
			$CC $COPTS $INC $DEF LiProps.c

$LIB/ListExt.o:		ListExt.c
			$CC $COPTS $INC $DEF ListExt.c

$LIB/LoadFont.o:	LoadFont.c
			$CC $COPTS $INC $DEF LoadFont.c

$LIB/LookupCol.o:	LookupCol.c
			$CC $COPTS $INC $DEF LookupCol.c

$LIB/LowerWin.o:	LowerWin.c
			$CC $COPTS $INC $DEF LowerWin.c

$LIB/Macros.o:		Macros.c
			$CC $COPTS $INC $DEF Macros.c

$LIB/MapRaised.o:	MapRaised.c
			$CC $COPTS $INC $DEF MapRaised.c

$LIB/MapSubs.o:		MapSubs.c
			$CC $COPTS $INC $DEF MapSubs.c

$LIB/MapWindow.o:	MapWindow.c
			$CC $COPTS $INC $DEF MapWindow.c

$LIB/MaskEvent.o:	MaskEvent.c
			$CC $COPTS $INC $DEF MaskEvent.c

$LIB/Misc.o:		Misc.c
			$CC $COPTS $INC $DEF Misc.c

$LIB/ModMap.o:		ModMap.c
			$CC $COPTS $INC $DEF ModMap.c

$LIB/MoveWin.o:		MoveWin.c
			$CC $COPTS $INC $DEF MoveWin.c

$LIB/NextEvent.o:	NextEvent.c
			$CC $COPTS $INC $DEF NextEvent.c

$LIB/OpenDis.o:		OpenDis.c
			$CC $COPTS $INC $DEF OpenDis.c

$LIB/ParseCol.o:	ParseCol.c
			$CC $COPTS $INC $DEF ParseCol.c

$LIB/ParseGeom.o:	ParseGeom.c
			$CC $COPTS $INC $DEF ParseGeom.c

$LIB/PeekEvent.o:	PeekEvent.c
			$CC $COPTS $INC $DEF PeekEvent.c

$LIB/PeekIfEv.o:	PeekIfEv.c
			$CC $COPTS $INC $DEF PeekIfEv.c

$LIB/Pending.o:		Pending.c
			$CC $COPTS $INC $DEF Pending.c

$LIB/PmapBgnd.o:	PmapBgnd.c
			$CC $COPTS $INC $DEF PmapBgnd.c

$LIB/PmapBord.o:	PmapBord.c
			$CC $COPTS $INC $DEF PmapBord.c

$LIB/PolyReg.o:		PolyReg.c
			$CC $COPTS $INC $DEF PolyReg.c

$LIB/PolyTxt.o:		PolyTxt.c
			$CC $COPTS $INC $DEF PolyTxt.c

$LIB/PolyTxt16.o:	PolyTxt16.c
			$CC $COPTS $INC $DEF PolyTxt16.c

$LIB/PutBEvent.o:	PutBEvent.c
			$CC $COPTS $INC $DEF PutBEvent.c

$LIB/PutImage.o:	PutImage.c
			$CC $COPTS $INC $DEF PutImage.c

$LIB/QuBest.o:		QuBest.c
			$CC $COPTS $INC $DEF QuBest.c

$LIB/QuColor.o:		QuColor.c
			$CC $COPTS $INC $DEF QuColor.c

$LIB/QuColors.o:	QuColors.c
			$CC $COPTS $INC $DEF QuColors.c

$LIB/QuCurShp.o:	QuCurShp.c
			$CC $COPTS $INC $DEF QuCurShp.c

$LIB/QuExt.o:		QuExt.c
			$CC $COPTS $INC $DEF QuExt.c

$LIB/QuKeybd.o:		QuKeybd.c
			$CC $COPTS $INC $DEF QuKeybd.c

$LIB/QuPntr.o:		QuPntr.c
			$CC $COPTS $INC $DEF QuPntr.c

$LIB/QuStipShp.o:	QuStipShp.c
			$CC $COPTS $INC $DEF QuStipShp.c

$LIB/QuTextE16.o:	QuTextE16.c
			$CC $COPTS $INC $DEF QuTextE16.c

$LIB/QuTextExt.o:	QuTextExt.c
			$CC $COPTS $INC $DEF QuTextExt.c

$LIB/QuTileShp.o:	QuTileShp.c
			$CC $COPTS $INC $DEF QuTileShp.c

$LIB/QuTree.o:		QuTree.c
			$CC $COPTS $INC $DEF QuTree.c

$LIB/RaiseWin.o:	RaiseWin.c
			$CC $COPTS $INC $DEF RaiseWin.c

$LIB/RdBitF.o:		RdBitF.c
			$CC $COPTS $INC $DEF RdBitF.c

$LIB/RecolorC.o:	RecolorC.c
			$CC $COPTS $INC $DEF RecolorC.c

$LIB/ReconfWin.o:	ReconfWin.c
			$CC $COPTS $INC $DEF ReconfWin.c

$LIB/Region.o:		Region.c
			$CC $COPTS $INC $DEF Region.c

$LIB/RepWindow.o:	RepWindow.c
			$CC $COPTS $INC $DEF RepWindow.c

$LIB/RestackWs.o:	RestackWs.c
			$CC $COPTS $INC $DEF RestackWs.c

$LIB/RotProp.o:		RotProp.c
			$CC $COPTS $INC $DEF RotProp.c

$LIB/ScrResStr.o:	ScrResStr.c
			$CC $COPTS $INC $DEF ScrResStr.c

$LIB/SelInput.o:	SelInput.c
			$CC $COPTS $INC $DEF SelInput.c

$LIB/SendEvent.o:	SendEvent.c
			$CC $COPTS $INC $DEF SendEvent.c

$LIB/SetBack.o:		SetBack.c
			$CC $COPTS $INC $DEF SetBack.c

$LIB/SetFore.o:		SetFore.c
			$CC $COPTS $INC $DEF SetFore.c

$LIB/SetCRects.o:	SetCRects.c
			$CC $COPTS $INC $DEF SetCRects.c

$LIB/SetClMask.o:	SetClMask.c
			$CC $COPTS $INC $DEF SetClMask.c

$LIB/SetClOrig.o:	SetClOrig.c
			$CC $COPTS $INC $DEF SetClOrig.c

$LIB/SetDashes.o:	SetDashes.c
			$CC $COPTS $INC $DEF SetDashes.c

$LIB/SetFPath.o:	SetFPath.c
			$CC $COPTS $INC $DEF SetFPath.c

$LIB/SetFont.o:		SetFont.c
			$CC $COPTS $INC $DEF SetFont.c

$LIB/SetFunc.o:		SetFunc.c
			$CC $COPTS $INC $DEF SetFunc.c

$LIB/SetHints.o:	SetHints.c
			$CC $COPTS $INC $DEF SetHints.c

$LIB/SetIFocus.o:	SetIFocus.c
			$CC $COPTS $INC $DEF SetIFocus.c

$LIB/SetLStyle.o:	SetLStyle.c
			$CC $COPTS $INC $DEF SetLStyle.c

$LIB/SetPMask.o:	SetPMask.c
			$CC $COPTS $INC $DEF SetPMask.c

$LIB/SetPntMap.o:	SetPntMap.c
			$CC $COPTS $INC $DEF SetPntMap.c

$LIB/SetSOwner.o:	SetSOwner.c
			$CC $COPTS $INC $DEF SetSOwner.c

$LIB/SetSSaver.o:	SetSSaver.c
			$CC $COPTS $INC $DEF SetSSaver.c

$LIB/SetState.o:	SetState.c
			$CC $COPTS $INC $DEF SetState.c

$LIB/SetStip.o:		SetStip.c
			$CC $COPTS $INC $DEF SetStip.c

$LIB/SetStCmap.o:	SetStCmap.c
			$CC $COPTS $INC $DEF SetStCmap.c

$LIB/SetTile.o:		SetTile.c
			$CC $COPTS $INC $DEF SetTile.c

$LIB/SetTSOrig.o:	SetTSOrig.c
			$CC $COPTS $INC $DEF SetTSOrig.c

$LIB/StBytes.o:		StBytes.c
			$CC $COPTS $INC $DEF StBytes.c

$LIB/StColor.o:		StColor.c
			$CC $COPTS $INC $DEF StColor.c

$LIB/StColors.o:	StColors.c
			$CC $COPTS $INC $DEF StColors.c

$LIB/StNColor.o:	StNColor.c
			$CC $COPTS $INC $DEF StNColor.c

$LIB/StName.o:		StName.c
			$CC $COPTS $INC $DEF StName.c

$LIB/StrKeysym.o:	StrKeysym.c			\
			ks_tables.h
			$CC $COPTS $INC $DEF		\
			-D KEYSYMDB=\"/usr/xwin/lib/X11/XKeysymDB\" StrKeysym.c -o 

$LIB/Sync.o:		Sync.c
			$CC $COPTS $INC $DEF Sync.c

$LIB/Synchro.o:		Synchro.c
			$CC $COPTS $INC $DEF Synchro.c

$LIB/Text.o:		Text.c
			$CC $COPTS $INC $DEF Text.c

$LIB/Text16.o:		Text16.c
			$CC $COPTS $INC $DEF Text16.c

$LIB/TextExt.o:		TextExt.c
			$CC $COPTS $INC $DEF TextExt.c

$LIB/TextExt16.o:	TextExt16.c
			$CC $COPTS $INC $DEF TextExt16.c

$LIB/TrCoords.o:	TrCoords.c
			$CC $COPTS $INC $DEF TrCoords.c

$LIB/UndefCurs.o:	UndefCurs.c
			$CC $COPTS $INC $DEF UndefCurs.c

$LIB/UngrabBut.o:	UngrabBut.c
			$CC $COPTS $INC $DEF UngrabBut.c

$LIB/UngrabKbd.o:	UngrabKbd.c
			$CC $COPTS $INC $DEF UngrabKbd.c

$LIB/UngrabKey.o:	UngrabKey.c
			$CC $COPTS $INC $DEF UngrabKey.c

$LIB/UngrabPtr.o:	UngrabPtr.c
			$CC $COPTS $INC $DEF UngrabPtr.c

$LIB/UngrabSvr.o:	UngrabSvr.c
			$CC $COPTS $INC $DEF UngrabSvr.c

$LIB/UninsCmap.o:	UninsCmap.c
			$CC $COPTS $INC $DEF UninsCmap.c

$LIB/UnldFont.o:	UnldFont.c
			$CC $COPTS $INC $DEF UnldFont.c

$LIB/UnmapSubs.o:	UnmapSubs.c
			$CC $COPTS $INC $DEF UnmapSubs.c

$LIB/UnmapWin.o:	UnmapWin.c
			$CC $COPTS $INC $DEF UnmapWin.c

$LIB/VisUtil.o:		VisUtil.c
			$CC $COPTS $INC $DEF VisUtil.c

$LIB/WarpPtr.o:		WarpPtr.c
			$CC $COPTS $INC $DEF WarpPtr.c

$LIB/WinEvent.o:	WinEvent.c
			$CC $COPTS $INC $DEF WinEvent.c

$LIB/Window.o:		Window.c
			$CC $COPTS $INC $DEF Window.c

$LIB/WrBitF.o:		WrBitF.c
			$CC $COPTS $INC $DEF WrBitF.c

$LIB/XlibAsync.o:	XlibAsync.c
			$CC $COPTS $INC $DEF XlibAsync.c

$LIB/XlibInt.o:		XlibInt.c
			$CC $COPTS $INC $DEF XlibInt.c

$LIB/Xrm.o:		Xrm.c
			$CC $COPTS $INC $DEF Xrm.c

$LIB/evtomask.o:	evtomask.c
			$CC $COPTS $INC $DEF evtomask.c

$LIB/globals.o:		globals.c
			$CC $COPTS $INC $DEF globals.c

$LIB/GetGCVals.o:	GetGCVals.c
			$CC $COPTS $INC $DEF GetGCVals.c

$LIB/GetNrmHint.o:	GetNrmHint.c
			$CC $COPTS $INC $DEF GetNrmHint.c

$LIB/GetRGBCMap.o:	GetRGBCMap.c
			$CC $COPTS $INC $DEF GetRGBCMap.c

$LIB/GetTxtProp.o:	GetTxtProp.c
			$CC $COPTS $INC $DEF GetTxtProp.c

$LIB/GetWMCMapW.o:	GetWMCMapW.c
			$CC $COPTS $INC $DEF GetWMCMapW.c

$LIB/GetWMProto.o:	GetWMProto.c
			$CC $COPTS $INC $DEF GetWMProto.c

$LIB/Iconify.o:		Iconify.c
			$CC $COPTS $INC $DEF Iconify.c

$LIB/PixFormats.o:	PixFormats.c
			$CC $COPTS $INC $DEF PixFormats.c

$LIB/PropAlloc.o:	PropAlloc.c
			$CC $COPTS $INC $DEF PropAlloc.c

$LIB/ReconfWM.o:	ReconfWM.c
			$CC $COPTS $INC $DEF ReconfWM.c

$LIB/SetNrmHint.o:	SetNrmHint.c
			$CC $COPTS $INC $DEF SetNrmHint.c

$LIB/SetRGBCMap.o:	SetRGBCMap.c
			$CC $COPTS $INC $DEF SetRGBCMap.c

$LIB/SetTxtProp.o:	SetTxtProp.c
			$CC $COPTS $INC $DEF SetTxtProp.c

$LIB/SetWMCMapW.o:	SetWMCMapW.c
			$CC $COPTS $INC $DEF SetWMCMapW.c

$LIB/SetWMProto.o:	SetWMProto.c
			$CC $COPTS $INC $DEF SetWMProto.c

$LIB/StrToText.o:	StrToText.c
			$CC $COPTS $INC $DEF StrToText.c

$LIB/TextToStr.o:	TextToStr.c
			$CC $COPTS $INC $DEF TextToStr.c

$LIB/Withdraw.o:	Withdraw.c
			$CC $COPTS $INC $DEF Withdraw.c

$LIB/WMGeom.o:		WMGeom.c
			$CC $COPTS $INC $DEF WMGeom.c

$LIB/WMProps.o:		WMProps.c
			$CC $COPTS $INC $DEF WMProps.c

$LIB/AuDispose.o:	AuDispose.c
			$CC $COPTS $INC $DEF AuDispose.c

$LIB/AuGetBest.o:	AuGetBest.c
			$CC $COPTS $INC $DEF AuGetBest.c

$LIB/AuFileName.o:	AuFileName.c
			$CC $COPTS $INC $DEF AuFileName.c

$LIB/AuRead.o:		AuRead.c
			$CC $COPTS $INC $DEF AuRead.c

$LIB/Lab.o:		Lab.c
			$CC $COPTS $INC $DEF Lab.c

$LIB/LabGcC.o:		LabGcC.c
			$CC $COPTS $INC $DEF LabGcC.c

$LIB/LabGcL.o:		LabGcL.c
			$CC $COPTS $INC $DEF LabGcL.c

$LIB/LabGcLC.o:		LabGcLC.c
			$CC $COPTS $INC $DEF LabGcLC.c

$LIB/LabMnL.o:		LabMnL.c
			$CC $COPTS $INC $DEF LabMnL.c

$LIB/LabMxC.o:		LabMxC.c
			$CC $COPTS $INC $DEF LabMxC.c

$LIB/LabMxL.o:		LabMxL.c
			$CC $COPTS $INC $DEF LabMxL.c

$LIB/LabMxLC.o:		LabMxLC.c
			$CC $COPTS $INC $DEF LabMxLC.c

$LIB/LabWpAj.o:		LabWpAj.c
			$CC $COPTS $INC $DEF LabWpAj.c

$LIB/Luv.o:		Luv.c
			$CC $COPTS $INC $DEF Luv.c

$LIB/LuvGcC.o:		LuvGcC.c
			$CC $COPTS $INC $DEF LuvGcC.c

$LIB/LuvGcL.o:		LuvGcL.c
			$CC $COPTS $INC $DEF LuvGcL.c

$LIB/LuvGcLC.o:		LuvGcLC.c
			$CC $COPTS $INC $DEF LuvGcLC.c

$LIB/LuvMnL.o:		LuvMnL.c
			$CC $COPTS $INC $DEF LuvMnL.c

$LIB/LuvMxC.o:		LuvMxC.c
			$CC $COPTS $INC $DEF LuvMxC.c

$LIB/LuvMxL.o:		LuvMxL.c
			$CC $COPTS $INC $DEF LuvMxL.c

$LIB/LuvMxLC.o:		LuvMxLC.c
			$CC $COPTS $INC $DEF LuvMxLC.c

$LIB/LuvWpAj.o:		LuvWpAj.c
			$CC $COPTS $INC $DEF LuvWpAj.c

$LIB/XYZ.o:		XYZ.c
			$CC $COPTS $INC $DEF XYZ.c

$LIB/uvY.o:		uvY.c
			$CC $COPTS $INC $DEF uvY.c

$LIB/xyY.o:		xyY.c
			$CC $COPTS $INC $DEF xyY.c

$LIB/HVC.o:		HVC.c
			$CC $COPTS $INC $DEF HVC.c

$LIB/HVCGcC.o:		HVCGcC.c
			$CC $COPTS $INC $DEF HVCGcC.c

$LIB/HVCGcV.o:		HVCGcV.c
			$CC $COPTS $INC $DEF HVCGcV.c

$LIB/HVCGcVC.o:		HVCGcVC.c
			$CC $COPTS $INC $DEF HVCGcVC.c

$LIB/HVCMnV.o:		HVCMnV.c
			$CC $COPTS $INC $DEF HVCMnV.c

$LIB/HVCMxC.o:		HVCMxC.c
			$CC $COPTS $INC $DEF HVCMxC.c

$LIB/HVCMxV.o:		HVCMxV.c
			$CC $COPTS $INC $DEF HVCMxV.c

$LIB/HVCMxVC.o:		HVCMxVC.c
			$CC $COPTS $INC $DEF HVCMxVC.c

$LIB/HVCMxVs.o:		HVCMxVs.c
			$CC $COPTS $INC $DEF HVCMxVs.c

$LIB/HVCWpAj.o:		HVCWpAj.c
			$CC $COPTS $INC $DEF HVCWpAj.c

$LIB/UNDEFINED.o:	UNDEFINED.c
			$CC $COPTS $INC $DEF UNDEFINED.c

$LIB/AddDIC.o:		AddDIC.c
			$CC $COPTS $INC $DEF AddDIC.c

$LIB/AddSF.o:		AddSF.c
			$CC $COPTS $INC $DEF AddSF.c

$LIB/cmsAllNCol.o:	cmsAllNCol.c
			$CC $COPTS $INC $DEF cmsAllNCol.c

$LIB/cmsAllCol.o:	cmsAllCol.c
			$CC $COPTS $INC $DEF cmsAllCol.c

$LIB/CCC.o:		CCC.c
			$CC $COPTS $INC $DEF CCC.c

$LIB/cmsCmap.o:		cmsCmap.c
			$CC $COPTS $INC $DEF cmsCmap.c

$LIB/cmsColNm.o:	cmsColNm.c
			$CC $COPTS $INC $DEF		\
			-D XCMSDB=\"/usr/xwin/lib/X11/Xcms.txt\" cmsColNm.c

$LIB/CvColW.o:		CvColW.c
			$CC $COPTS $INC $DEF CvColW.c

$LIB/CvCols.o:		CvCols.c
			$CC $COPTS $INC $DEF CvCols.c

$LIB/cmsGlobls.o:	cmsGlobls.c
			$CC $COPTS $INC $DEF cmsGlobls.c

$LIB/IdOfPr.o:		IdOfPr.c
			$CC $COPTS $INC $DEF IdOfPr.c

$LIB/cmsInt.o:		cmsInt.c
			$CC $COPTS $INC $DEF cmsInt.c

$LIB/LRGB.o:		LRGB.c
			$CC $COPTS $INC $DEF LRGB.c

$LIB/cmsLkCol.o:	cmsLkCol.c
			$CC $COPTS $INC $DEF cmsLkCol.c

$LIB/cmsMath.o:		cmsMath.c
			$CC $COPTS $INC $DEF cmsMath.c

$LIB/OfCCC.o:		OfCCC.c
			$CC $COPTS $INC $DEF OfCCC.c

$LIB/PrOfId.o:		PrOfId.c
			$CC $COPTS $INC $DEF PrOfId.c

$LIB/cmsProp.o:		cmsProp.c
			$CC $COPTS $INC $DEF cmsProp.c

$LIB/QBlack.o:		QBlack.c
			$CC $COPTS $INC $DEF QBlack.c

$LIB/QBlue.o:		QBlue.c
			$CC $COPTS $INC $DEF QBlue.c

$LIB/QGreen.o:		QGreen.c
			$CC $COPTS $INC $DEF QGreen.c

$LIB/QRed.o:		QRed.c
			$CC $COPTS $INC $DEF QRed.c

$LIB/QWhite.o:		QWhite.c
			$CC $COPTS $INC $DEF QWhite.c

$LIB/QuCol.o:		QuCol.c
			$CC $COPTS $INC $DEF QuCol.c

$LIB/QuCols.o:		QuCols.c
			$CC $COPTS $INC $DEF QuCols.c

$LIB/SetCCC.o:		SetCCC.c
			$CC $COPTS $INC $DEF SetCCC.c

$LIB/SetGetCols.o:	SetGetCols.c
			$CC $COPTS $INC $DEF SetGetCols.c

$LIB/StCol.o:		StCol.c
			$CC $COPTS $INC $DEF StCol.c

$LIB/StCols.o:		StCols.c
			$CC $COPTS $INC $DEF StCols.c

$LIB/cmsTrig.o:		cmsTrig.c
			$CC $COPTS $INC $DEF cmsTrig.c

$LIB/XRGB.o:		XRGB.c
			$CC $COPTS $INC $DEF XRGB.c

$LIB/FSWrap.o:		FSWrap.c
			$CC $COPTS $INC $DEF FSWrap.c

$LIB/ICWrap.o:		ICWrap.c
			$CC $COPTS $INC $DEF ICWrap.c

$LIB/IMWrap.o:		IMWrap.c
			$CC $COPTS $INC $DEF IMWrap.c

$LIB/OCWrap.o:		OCWrap.c
			$CC $COPTS $INC $DEF OCWrap.c

$LIB/OMWrap.o:		OMWrap.c
			$CC $COPTS $INC $DEF OMWrap.c

$LIB/lcWrap.o:		lcWrap.c
			$CC $COPTS $INC $DEF lcWrap.c

$LIB/mbWMProps.o:	mbWMProps.c
			$CC $COPTS $INC $DEF mbWMProps.c

$LIB/utf8WMProps.o:	utf8WMProps.c
			$CC $COPTS $INC $DEF utf8WMProps.c

$LIB/mbWrap.o:		mbWrap.c
			$CC $COPTS $INC $DEF mbWrap.c

$LIB/wcWrap.o:		wcWrap.c
			$CC $COPTS $INC $DEF wcWrap.c

$LIB/utf8Wrap.o:	utf8Wrap.c
			$CC $COPTS $INC $DEF utf8Wrap.c

$LIB/FilterEv.o:	FilterEv.c
			$CC $COPTS $INC $DEF FilterEv.c

$LIB/RegstFlt.o:	RegstFlt.c
			$CC $COPTS $INC $DEF RegstFlt.c

$LIB/locking.o:		locking.c
			$CC $COPTS $INC $DEF locking.c

$LIB/LockDis.o:		LockDis.c
			$CC $COPTS $INC $DEF LockDis.c

$LIB/x11trans.o:	../xtrans/xtrans.c
			$CC $COPTS $INC -I ../xtrans	\
			$DEF -D TRANS_CLIENT -D X11_t	\
			../xtrans/xtrans.c

$LIB/ximtrans.o:	../xtrans/xtrans.c
			$CC $COPTS $INC -I ../xtrans	\
			$DEF -D TRANS_CLIENT -D XIM_t	\
			../xtrans/xtrans.c

# => rm -f ximtrans.o unshared/ximtrans.o
# => cc -c -ansi -pedantic -Dasm=__asm -Wall -Wpointer-arith    -I../.. 
# -I../../exports/include  -DCSRG_BASED   -DFUNCPROTO=15 -DNARROWPROTO   
# -DMALLOC_0_RETURNS_NULL  -DHAS_SNPRINTF -DLIBX11 -O2  
# -I../../lib/xtrans -DTCPCONN -DUNIXCONN -DHAS_STICKY_DIR_BIT 
# -DHAS_FCHOWN -DBSD44SOCKETS -DXIM_t -DTRANS_CLIENT  ximtrans.c -o 
# unshared/ximtrans.o
# => rm -f ximtrans.o
# => cc -c -O2  -ansi -pedantic -Dasm=__asm -Wall -Wpointer-arith    -I../.. 
# -I../../exports/include  -DCSRG_BASED   -DFUNCPROTO=15 -DNARROWPROTO   
# -DMALLOC_0_RETURNS_NULL  -DHAS_SNPRINTF -DLIBX11 -I../../lib/xtrans 
# -DTCPCONN -DUNIXCONN -DHAS_STICKY_DIR_BIT -DHAS_FCHOWN -DBSD44SOCKETS 
# -DXIM_t -DTRANS_CLIENT    -fPIC ximtrans.c

$LIB/XKB.o:		XKB.c
			$CC $COPTS $INC $DEF XKB.c

#$LIB/XKBBind.o:	XKBBind.c
#			$CC $COPTS $INC $DEF -D XKB XKBBind.c

$LIB/XKBCompat.o:	XKBCompat.c
			$CC $COPTS $INC $DEF XKBCompat.c

$LIB/XKBCtrls.o:	XKBCtrls.c
			$CC $COPTS $INC $DEF XKBCtrls.c

$LIB/XKBCvt.o:		XKBCvt.c
			$CC $COPTS $INC $DEF XKBCvt.c

$LIB/XKBGetMap.o:	XKBGetMap.c
			$CC $COPTS $INC $DEF XKBGetMap.c

$LIB/XKBGetByName.o:	XKBGetByName.c
			$CC $COPTS $INC $DEF XKBGetByName.c

$LIB/XKBNames.o:	XKBNames.c
			$CC $COPTS $INC $DEF XKBNames.c

$LIB/XKBRdBuf.o:	XKBRdBuf.c
			$CC $COPTS $INC $DEF XKBRdBuf.c

$LIB/XKBSetMap.o:	XKBSetMap.c
			$CC $COPTS $INC $DEF XKBSetMap.c

$LIB/XKBUse.o:		XKBUse.c
			$CC $COPTS $INC $DEF XKBUse.c

$LIB/XKBleds.o:		XKBleds.c
			$CC $COPTS $INC $DEF XKBleds.c

$LIB/XKBBell.o:		XKBBell.c
			$CC $COPTS $INC $DEF XKBBell.c

$LIB/XKBGeom.o:		XKBGeom.c
			$CC $COPTS $INC $DEF XKBGeom.c

$LIB/XKBSetGeom.o:	XKBSetGeom.c
			$CC $COPTS $INC $DEF XKBSetGeom.c

$LIB/XKBExtDev.o:	XKBExtDev.c
			$CC $COPTS $INC $DEF XKBExtDev.c

$LIB/XKBList.o:		XKBList.c
			$CC $COPTS $INC $DEF XKBList.c

$LIB/XKBMisc.o:		XKBMisc.c
			$CC $COPTS $INC $DEF XKBMisc.c

$LIB/XKBMAlloc.o:	XKBMAlloc.c
			$CC $COPTS $INC $DEF XKBMAlloc.c

$LIB/XKBGAlloc.o:	XKBGAlloc.c
			$CC $COPTS $INC $DEF XKBGAlloc.c

$LIB/XKBAlloc.o:	XKBAlloc.c
			$CC $COPTS $INC $DEF XKBAlloc.c

$LIB/SetLocale.o:	SetLocale.c
			$CC $COPTS $INC $DEF SetLocale.c

$LIB/imCallbk.o:	imCallbk.c
			$CC $COPTS $INC $DEF imCallbk.c

$LIB/imConv.o:		imConv.c
			$CC $COPTS $INC $DEF imConv.c

$LIB/imDefFlt.o:	imDefFlt.c
			$CC $COPTS $INC $DEF imDefFlt.c

$LIB/imDefIc.o:		imDefIc.c
			$CC $COPTS $INC $DEF imDefIc.c

$LIB/imDefIm.o:		imDefIm.c
			$CC $COPTS $INC $DEF imDefIm.c

$LIB/imDefLkup.o:	imDefLkup.c
			$CC $COPTS $INC $DEF imDefLkup.c

$LIB/imDispch.o:	imDispch.c
			$CC $COPTS $INC $DEF imDispch.c

$LIB/imEvToWire.o:	imEvToWire.c
			$CC $COPTS $INC $DEF imEvToWire.c

$LIB/imExten.o:		imExten.c
			$CC $COPTS $INC $DEF imExten.c

$LIB/imImSw.o:		imImSw.c
			$CC $COPTS $INC $DEF imImSw.c

$LIB/imInsClbk.o:	imInsClbk.c
			$CC $COPTS $INC $DEF imInsClbk.c

$LIB/imInt.o:		imInt.c
			$CC $COPTS $INC $DEF imInt.c

$LIB/imKStoUCS.o:	imKStoUCS.c
			$CC $COPTS $INC $DEF imKStoUCS.c

$LIB/imLcFlt.o:		imLcFlt.c
			$CC $COPTS $INC $DEF imLcFlt.c

$LIB/imLcGIc.o:		imLcGIc.c
			$CC $COPTS $INC $DEF imLcGIc.c

$LIB/imLcIc.o:		imLcIc.c
			$CC $COPTS $INC $DEF imLcIc.c

$LIB/imLcIm.o:		imLcIm.c
			$CC $COPTS $INC $DEF imLcIm.c

$LIB/imLcLkup.o:	imLcLkup.c
			$CC $COPTS $INC $DEF imLcLkup.c

$LIB/imLcPrs.o:		imLcPrs.c
			$CC $COPTS $INC $DEF imLcPrs.c

$LIB/imLcSIc.o:		imLcSIc.c
			$CC $COPTS $INC $DEF imLcSIc.c

$LIB/imRm.o:		imRm.c
			$CC $COPTS $INC $DEF imRm.c

$LIB/imRmAttr.o:	imRmAttr.c
			$CC $COPTS $INC $DEF imRmAttr.c

$LIB/imThaiFlt.o:	imThaiFlt.c
			$CC $COPTS $INC $DEF imThaiFlt.c

$LIB/imThaiIc.o:	imThaiIc.c
			$CC $COPTS $INC $DEF imThaiIc.c

$LIB/imThaiIm.o:	imThaiIm.c
			$CC $COPTS $INC $DEF imThaiIm.c

$LIB/imTrX.o:		imTrX.c
			$CC $COPTS $INC $DEF imTrX.c

$LIB/imTransR.o:	imTransR.c
			$CC $COPTS $INC $DEF -D XIM_t -D TRANS_CLIENT imTransR.c

# => rm -f imTransR.o unshared/imTransR.o
# => cc -c -ansi -pedantic -Dasm=__asm -Wall -Wpointer-arith    -I../.. 
# -I../../exports/include  -DCSRG_BASED   -DFUNCPROTO=15 -DNARROWPROTO   
# -DMALLOC_0_RETURNS_NULL  -DHAS_SNPRINTF -DLIBX11 -O2  -DTCPCONN 
# -DUNIXCONN -DHAS_STICKY_DIR_BIT -DHAS_FCHOWN -DXIM_t -DTRANS_CLIENT 
# imTransR.c -o unshared/imTransR.o
# => rm -f imTransR.o
# => cc -c -O2  -ansi -pedantic -Dasm=__asm -Wall -Wpointer-arith    -I../.. 
# -I../../exports/include  -DCSRG_BASED   -DFUNCPROTO=15 -DNARROWPROTO   
# -DMALLOC_0_RETURNS_NULL  -DHAS_SNPRINTF -DLIBX11 -DTCPCONN -DUNIXCONN 
# -DHAS_STICKY_DIR_BIT -DHAS_FCHOWN -DXIM_t -DTRANS_CLIENT   -fPIC 
# imTransR.c

# => rm -f imTrans.o unshared/imTrans.o
# => cc -c -ansi -pedantic -Dasm=__asm -Wall -Wpointer-arith    -I../.. 
# -I../../exports/include  -DCSRG_BASED   -DFUNCPROTO=15 -DNARROWPROTO   
# -DMALLOC_0_RETURNS_NULL  -DHAS_SNPRINTF -DLIBX11 -O2  -DTCPCONN 
# -DUNIXCONN -DHAS_STICKY_DIR_BIT -DHAS_FCHOWN -DXIM_t -DTRANS_CLIENT 
# imTrans.c -o unshared/imTrans.o
# => rm -f imTrans.o
# => cc -c -O2  -ansi -pedantic -Dasm=__asm -Wall -Wpointer-arith    -I../.. 
# -I../../exports/include  -DCSRG_BASED   -DFUNCPROTO=15 -DNARROWPROTO   
# -DMALLOC_0_RETURNS_NULL  -DHAS_SNPRINTF -DLIBX11 -DTCPCONN -DUNIXCONN 
# -DHAS_STICKY_DIR_BIT -DHAS_FCHOWN -DXIM_t -DTRANS_CLIENT   -fPIC 
# imTrans.c

$LIB/lcCT.o:		lcCT.c
			$CC $COPTS $INC $DEF lcCT.c

$LIB/lcCharSet.o:	lcCharSet.c
			$CC $COPTS $INC $DEF lcCharSet.c

$LIB/lcConv.o:		lcConv.c
			$CC $COPTS $INC $DEF lcConv.c

$LIB/lcDB.o:		lcDB.c
			$CC $COPTS $INC $DEF lcDB.c

$LIB/lcDefConv.o:	lcDefConv.c
			$CC $COPTS $INC $DEF lcDefConv.c

$LIB/lcEuc.o:		lcEuc.c
			$CC $COPTS $INC $DEF lcEuc.c

$LIB/lcFile.o:		lcFile.c
			$CC $COPTS $INC $DEF	\
			-D XLOCALEDIR=\"/usr/xwin/lib/X11/locale\" lcFile.c

$LIB/lcGenConv.o:	lcGenConv.c
			$CC $COPTS $INC $DEF lcGenConv.c

$LIB/lcGeneric.o:	lcGeneric.c
			$CC $COPTS $INC $DEF lcGeneric.c

$LIB/lcInit.o:		lcInit.c
			$CC $COPTS $INC $DEF lcInit.c

$LIB/lcJis.o:		lcJis.c
			$CC $COPTS $INC $DEF lcJis.c

$LIB/lcPrTxt.o:		lcPrTxt.c
			$CC $COPTS $INC $DEF lcPrTxt.c

$LIB/lcPubWrap.o:	lcPubWrap.c
			$CC $COPTS $INC $DEF lcPubWrap.c

$LIB/lcPublic.o:	lcPublic.c
			$CC $COPTS $INC $DEF lcPublic.c

$LIB/lcRM.o:		lcRM.c
			$CC $COPTS $INC $DEF lcRM.c

$LIB/lcSjis.o:		lcSjis.c
			$CC $COPTS $INC $DEF lcSjis.c

$LIB/lcStd.o:		lcStd.c
			$CC $COPTS $INC $DEF lcStd.c

$LIB/lcTxtPr.o:		lcTxtPr.c
			$CC $COPTS $INC $DEF lcTxtPr.c

$LIB/lcUTF8.o:		lcUTF8.c
			$CC $COPTS $INC $DEF lcUTF8.c

$LIB/lcUTF8Load.o:	lcUTF8Load.c
			$CC $COPTS $INC $DEF lcUTF8Load.c

$LIB/lcUtil.o:		lcUtil.c
			$CC $COPTS $INC $DEF lcUtil.c

$LIB/omDefault.o:	omDefault.c
			$CC $COPTS $INC $DEF omDefault.c

$LIB/omGeneric.o:	omGeneric.c
			$CC $COPTS $INC $DEF omGeneric.c

$LIB/omImText.o:	omImText.c
			$CC $COPTS $INC $DEF omImText.c

$LIB/omText.o:		omText.c
			$CC $COPTS $INC $DEF omText.c

$LIB/omTextEsc.o:	omTextEsc.c
			$CC $COPTS $INC $DEF omTextEsc.c

$LIB/omTextExt.o:	omTextExt.c
			$CC $COPTS $INC $DEF omTextExt.c

$LIB/omTextPer.o:	omTextPer.c
			$CC $COPTS $INC $DEF omTextPer.c

$LIB/omXChar.o:		omXChar.c
			$CC $COPTS $INC $DEF omXChar.c

ks_tables.h:		util/makekeys
			util/makekeys < ../../include/X11/keysymdef.h > $@

util/makekeys:		util/makekeys.c
			$CC -o $@ $INC $DEF util/makekeys.c
