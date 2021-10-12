#*
#****************************************************************
#	Makefile para a biblioteca "lib/font"			*
#****************************************************************
#*
xc	=		../..

INC	=		-I .					\
			-I include				\
			-I $xc/include				\
			-I $xc/include/X11			\
			-I $xc/include/X11/fonts		\
			-I $xc/lib/xtrans			\
			-I $xc/sys/include

STDDEF	=		-D __i386__				\
			-D CSRG_BASED				\
			-D SHAPE				\
			-D XINPUT				\
			-D XF86BIGFONT 				\
			-D DPMSExtension			\
			-D PIXPRIV				\
			-D RENDER				\
			-D GCCUSESGAS				\
			-D AVOID_GLYPHBLT			\
			-D SINGLEDEPTH				\
			-D XvExtension 				\
			-D XFree86Server			\
#			-D BUILDDEBUG				\
			-D X_BYTE_ORDER=X_LITTLE_ENDIAN		\
			-D NDEBUG				\
			-D NO_INLINE				\
			-D TROPIX

#			-D asm=__asm				\
#			-D XKB					\
#			-D LBX					\
#			-D XAPPGROUP				\
#			-D XCSECURITY				\
#			-D TOGCUP				\
#			-D PANORAMIX				\
#			-D STATIC_COLOR				\
#			-D XFreeXDGA				\
#			-D XFree86LOADER			\
#			-D XF86VIDMODE				\
#			-D SMART_SCHEDULE 			\

CC	=		cc
COPTS	=		-c -O -o $@
LIB	=		lib

$LIB/.LIBSYM:		$LIB					\
			$LIB/bitmap				\
			$LIB/fontfile				\
			$LIB/fc					\
			$LIB/fontcache				\
#			$LIB/speedo				\
#			$LIB/Type1				\
#			$LIB/FreeType				\
			$LIB/util				\
			$LIB/bitmap/bdfread.o			\
			$LIB/bitmap/bdfutils.o			\
			$LIB/bitmap/bitmap.o			\
			$LIB/bitmap/bitmaputil.o		\
			$LIB/bitmap/bitscale.o			\
			$LIB/bitmap/bitmapfunc.o		\
			$LIB/bitmap/pcfread.o			\
			$LIB/bitmap/pcfwrite.o			\
			$LIB/bitmap/snfread.o			\
			$LIB/bitmap/fontink.o			\
			$LIB/fontfile/dirfile.o			\
			$LIB/fontfile/fontdir.o			\
			$LIB/fontfile/fontfile.o		\
			$LIB/fontfile/fileio.o			\
			$LIB/fontfile/fontscale.o		\
			$LIB/fontfile/defaults.o		\
			$LIB/fontfile/bitsource.o		\
			$LIB/fontfile/register.o		\
			$LIB/fontfile/renderers.o		\
			$LIB/fontfile/bufio.o			\
			$LIB/fontfile/decompress.o		\
			$LIB/fontfile/filewr.o			\
			$LIB/fontfile/printerfont.o		\
			$LIB/fontfile/ffcheck.o			\
			$LIB/fontfile/fontenc.o			\
			$LIB/fontfile/encparse.o		\
			$LIB/fontfile/gunzip.o			\
			$LIB/fontcache/fontcache.o		\
#			$LIB/speedo/spfuncs.o			\
#			$LIB/speedo/spfile.o			\
#			$LIB/speedo/spinfo.o			\
#			$LIB/speedo/sperr.o			\
#			$LIB/speedo/spfont.o			\
#			$LIB/speedo/spglyph.o			\
#			$LIB/speedo/spencode.o			\
#			$LIB/speedo/bics-unicode.o		\
#			$LIB/speedo/do_char.o			\
#			$LIB/speedo/do_trns.o			\
#			$LIB/speedo/out_bl2d.o			\
#			$LIB/speedo/out_blk.o			\
#			$LIB/speedo/out_scrn.o			\
#			$LIB/speedo/out_util.o			\
#			$LIB/speedo/reset.o			\
#			$LIB/speedo/set_spcs.o			\
#			$LIB/speedo/set_trns.o			\
#			$LIB/Type1/arith.o			\
#			$LIB/Type1/curves.o			\
#			$LIB/Type1/fontfcn.o			\
#			$LIB/Type1/hints.o			\
#			$LIB/Type1/lines.o			\
#			$LIB/Type1/objects.o			\
#			$LIB/Type1/paths.o			\
#			$LIB/Type1/regions.o			\
#			$LIB/Type1/scanfont.o			\
#			$LIB/Type1/spaces.o			\
#			$LIB/Type1/t1funcs.o			\
#			$LIB/Type1/t1info.o			\
#			$LIB/Type1/t1io.o			\
#			$LIB/Type1/t1malloc.o			\
#			$LIB/Type1/t1snap.o			\
#			$LIB/Type1/t1stub.o			\
#			$LIB/Type1/token.o			\
#			$LIB/Type1/type1.o			\
#			$LIB/Type1/util.o			\
#			$LIB/Type1/cidchar.o			\
#			$LIB/Type1/afm.o			\
#			$LIB/Type1/t1unicode.o			\
#			$LIB/FreeType/ftfuncs.o			\
#			$LIB/FreeType/ftenc.o			\
#			$LIB/FreeType/ftinit.o			\
#			$LIB/FreeType/fttools.o			\
#			$LIB/FreeType/ftmac.o			\
#			$LIB/FreeType/ftutil.o			\
#			$LIB/FreeType/freetype.o		\
#			$LIB/FreeType/ftxcmap.o			\
			$LIB/util/utilbitmap.o			\
			$LIB/util/fontnames.o			\
			$LIB/util/fontutil.o			\
			$LIB/util/fontxlfd.o			\
			$LIB/util/format.o			\
			$LIB/util/fontaccel.o			\
			$LIB/util/private.o			\
			$LIB/util/patcache.o			\
			$LIB/fc/fsconvert.o			\
			$LIB/fc/fserve.o			\
			$LIB/fc/fsio.o				\
			$LIB/fc/transport.o
			mklib $LIB

$LIB:			.exist
			mkdir -f $LIB

#****************************************************************
#	Compilação do diretório "bitmap"			*
#****************************************************************

DEF	=		-D CSRG_BASED				\
#			-D FUNCPROTO=15				\
#			-D NARROWPROTO				\
			-D BDFFORMAT				\
			-D PCFFORMAT				\
			-D SNFFORMAT				\
			-D X_GZIP_FONT_COMPRESSION		\
			-D TROPIX				\
			-D entry=__entry

$LIB/bitmap:		.exist
			mkdir -f $LIB/bitmap

$LIB/bitmap/bdfread.o:	bitmap/bdfread.c
			$CC $COPTS $INC $DEF bitmap/bdfread.c

$LIB/bitmap/bdfutils.o:	bitmap/bdfutils.c
			$CC $COPTS $INC $DEF bitmap/bdfutils.c

$LIB/bitmap/bitmap.o:	bitmap/bitmap.c
			$CC $COPTS $INC $DEF bitmap/bitmap.c

$LIB/bitmap/bitmaputil.o:bitmap/bitmaputil.c
			$CC $COPTS $INC $DEF bitmap/bitmaputil.c

$LIB/bitmap/bitscale.o:	bitmap/bitscale.c
			$CC $COPTS $INC $DEF bitmap/bitscale.c

$LIB/bitmap/bitmapfunc.o:bitmap/bitmapfunc.c
			$CC $COPTS $INC $DEF bitmap/bitmapfunc.c

$LIB/bitmap/pcfread.o:	bitmap/pcfread.c
			$CC $COPTS $INC $DEF bitmap/pcfread.c

$LIB/bitmap/pcfwrite.o:	bitmap/pcfwrite.c
			$CC $COPTS $INC $DEF bitmap/pcfwrite.c

$LIB/bitmap/snfread.o:	bitmap/snfread.c
			$CC $COPTS $INC $DEF bitmap/snfread.c

$LIB/bitmap/fontink.o:	bitmap/fontink.c
			$CC $COPTS $INC $DEF bitmap/fontink.c

#****************************************************************
#	Compilação do diretório "fontfile"			*
#****************************************************************

$LIB/fontfile:		.exist
			mkdir -f $LIB/fontfile

DEF1	=		-D CSRG_BASED				\
			-D FONTDIRATTRIB			\
#			-D BUILD_SPEEDO 			\
#			-D BUILD_TYPE1				\
#			-D BUILD_CID				\
#			-D BUILD_FREETYPE  			\
			-D X_GZIP_FONT_COMPRESSION		\
			-D TROPIX				\
			-D FONT_ENCODINGS_DIRECTORY='""'	\
			-D entry=__entry

$LIB/fontfile/dirfile.o:fontfile/dirfile.c
			$CC $COPTS $INC $DEF1 fontfile/dirfile.c

$LIB/fontfile/fontdir.o:fontfile/fontdir.c
			$CC $COPTS $INC $DEF1 fontfile/fontdir.c

$LIB/fontfile/fontfile.o:fontfile/fontfile.c
			$CC $COPTS $INC $DEF1 fontfile/fontfile.c

$LIB/fontfile/fileio.o:	fontfile/fileio.c
			$CC $COPTS $INC $DEF1 fontfile/fileio.c

$LIB/fontfile/fontscale.o:fontfile/fontscale.c
			$CC $COPTS $INC $DEF1 fontfile/fontscale.c

$LIB/fontfile/defaults.o:fontfile/defaults.c
			$CC $COPTS $INC $DEF1 fontfile/defaults.c

$LIB/fontfile/bitsource.o:fontfile/bitsource.c
			$CC $COPTS $INC $DEF1 fontfile/bitsource.c

$LIB/fontfile/register.o:fontfile/register.c
			$CC $COPTS $INC $DEF1 fontfile/register.c

$LIB/fontfile/renderers.o:fontfile/renderers.c
			$CC $COPTS $INC $DEF1 fontfile/renderers.c

$LIB/fontfile/bufio.o:	fontfile/bufio.c
			$CC $COPTS $INC $DEF1 fontfile/bufio.c

$LIB/fontfile/decompress.o:	fontfile/decompress.c
			$CC $COPTS $INC $DEF1 fontfile/decompress.c

$LIB/fontfile/filewr.o:	fontfile/filewr.c
			$CC $COPTS $INC $DEF1 fontfile/filewr.c

$LIB/fontfile/printerfont.o:	fontfile/printerfont.c
			$CC $COPTS $INC $DEF1 fontfile/printerfont.c

$LIB/fontfile/ffcheck.o:fontfile/ffcheck.c
			$CC $COPTS $INC $DEF1 fontfile/ffcheck.c

$LIB/fontfile/fontenc.o:fontfile/fontenc.c
			$CC $COPTS $INC $DEF1 fontfile/fontenc.c

$LIB/fontfile/encparse.o:fontfile/encparse.c
			$CC $COPTS $INC $DEF1 fontfile/encparse.c

$LIB/fontfile/gunzip.o:	fontfile/gunzip.c
			$CC $COPTS $INC -I ../zlib $DEF1 fontfile/gunzip.c

#****************************************************************
#	Compilação do diretório "fc"				*
#****************************************************************

DEF2	=		-D CSRG_BASED				\
#			-D FUNCPROTO=15				\
#			-D NARROWPROTO				\
			-D FONT_t 				\
			-D TCPCONN				\
			-D TRANS_CLIENT				\
			-D TRANS_SERVER				\
			-D TRANS_REOPEN				\
			-D TROPIX

$LIB/fc:		.exist
			mkdir -f $LIB/fc

# -DTCPCONN -DUNIXCONN -DHAS_STICKY_DIR_BIT -DHAS_FCHOWN -DFONT_t
# -DTRANS_CLIENT -DTRANS_SERVER -DTRANS_REOPEN -DBSD44SOCKETS

$LIB/fc/fsconvert.o:	fc/fsconvert.c
			$CC $COPTS $INC $DEF2 fc/fsconvert.c

$LIB/fc/fserve.o:	fc/fserve.c
			$CC $COPTS $INC $DEF2 fc/fserve.c

$LIB/fc/fsio.o:		fc/fsio.c
			$CC $COPTS $INC $DEF2 fc/fsio.c

$LIB/fc/transport.o:	../xtrans/xtrans.c
			$CC $COPTS $INC $DEF2 ../xtrans/xtrans.c

#****************************************************************
#	Compilação do diretório "fontcache"			*
#****************************************************************

DEF3	=		-D CSRG_BASED				\
#			-D FUNCPROTO=15				\
#			-D NARROWPROTO				\
			-D FONTCACHE				\
			-D TROPIX				\
			-D entry=__entry

$LIB/fontcache:		.exist
			mkdir -f $LIB/fontcache

$LIB/fontcache/fontcache.o:	fontcache/fontcache.c
			$CC $COPTS $INC $DEF3 fontcache/fontcache.c

#****************************************************************
#	Compilação do diretório "Speedo"			*
#****************************************************************

DEF4	=		-D CSRG_BASED				\
#			-D FUNCPROTO=15				\
#			-D NARROWPROTO				\
			-D FONTCACHE				\
			-D TROPIX				\
			-D entry=__entry

$LIB/speedo:		.exist
			mkdir -f $LIB/speedo

$LIB/speedo/spfuncs.o:	Speedo/spfuncs.c
			$CC $COPTS $INC $DEF4 Speedo/spfuncs.c

$LIB/speedo/spfile.o:	Speedo/spfile.c
			$CC $COPTS $INC $DEF4 Speedo/spfile.c

$LIB/speedo/spinfo.o:	Speedo/spinfo.c
			$CC $COPTS $INC $DEF4 -U index Speedo/spinfo.c

$LIB/speedo/sperr.o:	Speedo/sperr.c
			$CC $COPTS $INC $DEF4 Speedo/sperr.c

$LIB/speedo/spfont.o:	Speedo/spfont.c
			$CC $COPTS $INC $DEF4 Speedo/spfont.c

$LIB/speedo/spglyph.o:	Speedo/spglyph.c
			$CC $COPTS $INC $DEF4 Speedo/spglyph.c

$LIB/speedo/spencode.o:	Speedo/spencode.c
			$CC $COPTS $INC $DEF4 Speedo/spencode.c

$LIB/speedo/bics-unicode.o:	Speedo/bics-unicode.c
			$CC $COPTS $INC $DEF4 Speedo/bics-unicode.c

$LIB/speedo/do_char.o:	Speedo/do_char.c
			$CC $COPTS $INC $DEF4 Speedo/do_char.c

$LIB/speedo/do_trns.o:	Speedo/do_trns.c
			$CC $COPTS $INC $DEF4 Speedo/do_trns.c

$LIB/speedo/out_bl2d.o:	Speedo/out_bl2d.c
			$CC $COPTS $INC $DEF4 Speedo/out_bl2d.c

$LIB/speedo/out_blk.o:	Speedo/out_blk.c
			$CC $COPTS $INC $DEF4 Speedo/out_blk.c

$LIB/speedo/out_scrn.o:	Speedo/out_scrn.c
			$CC $COPTS $INC $DEF4 Speedo/out_scrn.c

$LIB/speedo/out_util.o:	Speedo/out_util.c
			$CC $COPTS $INC $DEF4 Speedo/out_util.c

$LIB/speedo/reset.o:	Speedo/reset.c
			$CC $COPTS $INC $DEF4 Speedo/reset.c

$LIB/speedo/set_spcs.o:	Speedo/set_spcs.c
			$CC $COPTS $INC $DEF4 Speedo/set_spcs.c

$LIB/speedo/set_trns.o:	Speedo/set_trns.c
			$CC $COPTS $INC $DEF4 Speedo/set_trns.c

#****************************************************************
#	Compilação do diretório "Type1"				*
#****************************************************************

DEF5	=		-D CSRG_BASED				\
#			-D FUNCPROTO=15				\
#			-D NARROWPROTO				\
#			-D BUILDCID				\
			-D HAVE_CFM				\
			-D TROPIX				\
			-D entry=__entry

$LIB/Type1:		.exist
			mkdir -f $LIB/Type1

$LIB/Type1/arith.o:	Type1/arith.c
			$CC $COPTS $INC $DEF5 Type1/arith.c

$LIB/Type1/curves.o:	Type1/curves.c
			$CC $COPTS $INC $DEF5 Type1/curves.c

$LIB/Type1/fontfcn.o:	Type1/fontfcn.c
			$CC $COPTS $INC $DEF5 Type1/fontfcn.c

$LIB/Type1/hints.o:	Type1/hints.c
			$CC $COPTS $INC $DEF5 Type1/hints.c

$LIB/Type1/lines.o:	Type1/lines.c
			$CC $COPTS $INC $DEF5 Type1/lines.c

$LIB/Type1/objects.o:	Type1/objects.c
			$CC $COPTS $INC $DEF5 Type1/objects.c

$LIB/Type1/paths.o:	Type1/paths.c
			$CC $COPTS $INC $DEF5 Type1/paths.c

$LIB/Type1/regions.o:	Type1/regions.c
			$CC $COPTS $INC $DEF5 Type1/regions.c

$LIB/Type1/scanfont.o:	Type1/scanfont.c
			$CC $COPTS $INC $DEF5 Type1/scanfont.c

$LIB/Type1/spaces.o:	Type1/spaces.c
			$CC $COPTS $INC $DEF5 Type1/spaces.c

$LIB/Type1/t1funcs.o:	Type1/t1funcs.c
			$CC $COPTS $INC $DEF5 Type1/t1funcs.c

$LIB/Type1/t1info.o:	Type1/t1info.c
			$CC $COPTS $INC $DEF5 Type1/t1info.c

$LIB/Type1/t1io.o:	Type1/t1io.c
			$CC $COPTS $INC $DEF5 Type1/t1io.c

$LIB/Type1/t1malloc.o:	Type1/t1malloc.c
			$CC $COPTS $INC $DEF5 Type1/t1malloc.c

$LIB/Type1/t1snap.o:	Type1/t1snap.c
			$CC $COPTS $INC $DEF5 Type1/t1snap.c

$LIB/Type1/t1stub.o:	Type1/t1stub.c
			$CC $COPTS $INC $DEF5 Type1/t1stub.c

$LIB/Type1/token.o:	Type1/token.c
			$CC $COPTS $INC $DEF5 Type1/token.c

$LIB/Type1/type1.o:	Type1/type1.c
			$CC $COPTS $INC $DEF5 Type1/type1.c

$LIB/Type1/util.o:	Type1/util.c
			$CC $COPTS $INC $DEF5 Type1/util.c

$LIB/Type1/cidchar.o:	Type1/cidchar.c
			$CC $COPTS $INC $DEF5 Type1/cidchar.c

$LIB/Type1/afm.o:	Type1/afm.c
			$CC $COPTS $INC $DEF5 Type1/afm.c

$LIB/Type1/t1unicode.o:	Type1/t1unicode.c
			$CC $COPTS $INC $DEF5 Type1/t1unicode.c

#****************************************************************
#	Compilação do diretório "FreeType"			*
#****************************************************************

INC6	=		$INC					\
			-I FreeType				\
			-I $xc/extras/FreeType/lib		\
			-I $xc/extras/FreeType/lib/extend	\
			-I $xc/extras/FreeType/lib/arch/unix	\
			-I $xc/extras/freetype2/include		\
			-I $xc/extras/freetype2/src

DEF6	=		$STDDEF					\
			-D TT_MAKE_OPTION_SINGLE_OBJECT=1	\
			-D entry=__entry
# -DHAVE_MMAP=1

$LIB/FreeType:		.exist
			mkdir -f $LIB/FreeType

$LIB/FreeType/ftfuncs.o:FreeType/ftfuncs.c
			$CC $COPTS $INC6 $DEF6 FreeType/ftfuncs.c

$LIB/FreeType/ftenc.o:	FreeType/ftenc.c
			$CC $COPTS $INC6 $DEF6 FreeType/ftenc.c

$LIB/FreeType/fttools.o:FreeType/fttools.c
			$CC $COPTS $INC6 $DEF6 FreeType/fttools.c

$LIB/FreeType/freetype.o: $xc/extras/FreeType/lib/arch/unix/freetype.c
			$CC $COPTS $INC6 $DEF6 $xc/extras/FreeType/lib/arch/unix/freetype.c

$LIB/FreeType/ftxcmap.o:$xc/extras/FreeType/lib/extend/ftxcmap.c
			$CC $COPTS $INC6 $DEF6 $xc/extras/FreeType/lib/extend/ftxcmap.c

$LIB/FreeType/ftinit.o:	$xc/extras/freetype2/src/base/ftinit.c
			$CC $COPTS $INC6 $DEF6 $xc/extras/freetype2/src/base/ftinit.c

$LIB/FreeType/ftmac.o:	$xc/extras/freetype2/src/base/ftmac.c
			$CC $COPTS $INC6 $DEF6 $xc/extras/freetype2/src/base/ftmac.c

#****************************************************************
#	Compilação do diretório "util"				*
#****************************************************************

DEF7	=		-D CSRG_BASED				\
#			-D FUNCPROTO=15				\
#			-D NARROWPROTO				\
			-D TROPIX				\
			-D entry=__entry

$LIB/util:		.exist
			mkdir -f $LIB/util

$LIB/util/utilbitmap.o:	util/utilbitmap.c
			$CC $COPTS $INC $DEF7 util/utilbitmap.c

$LIB/util/fontnames.o:	util/fontnames.c
			$CC $COPTS $INC $DEF7 util/fontnames.c

$LIB/util/fontutil.o:	util/fontutil.c
			$CC $COPTS $INC $DEF7 util/fontutil.c

$LIB/util/fontxlfd.o:	util/fontxlfd.c
			$CC $COPTS $INC $DEF7 util/fontxlfd.c

$LIB/util/format.o:	util/format.c
			$CC $COPTS $INC $DEF7 util/format.c

$LIB/util/fontaccel.o:	util/fontaccel.c
			$CC $COPTS $INC $DEF7 util/fontaccel.c

$LIB/util/atom.o:	util/atom.c
			$CC $COPTS $INC $DEF7 util/atom.c

$LIB/util/private.o:	util/private.c
			$CC $COPTS $INC $DEF7 util/private.c

$LIB/util/patcache.o:	util/patcache.c
			$CC $COPTS $INC $DEF7 util/patcache.c
