#************************************************************************
#*	Makefile para o servidor X					*
#************************************************************************

PGM	=		xserver
DIR	=		/usr/xwin/bin
PROT	=		u=srxw,g+srx,o+rx

xc	=		..

#************************************************************************
#*	Modificações no Ambiente					*
#************************************************************************

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
			-D XFreeXDGA				\
			-D XvExtension 				\
			-D XFree86Server			\
			-D BUILDDEBUG				\
			-D X_BYTE_ORDER=X_LITTLE_ENDIAN		\
			-D NDEBUG				\
			-D NO_INLINE				\
			-D TROPIX

#			-D FUNCPROTO=15				\
#			-D NARROWPROTO			    	\
#			-D asm=__asm				\
#			-D XKB					\
#			-D LBX					\
#			-D XAPPGROUP				\
#			-D XCSECURITY				\
#			-D TOGCUP				\
#			-D PANORAMIX				\
#			-D STATIC_COLOR				\
#			-D XFree86LOADER			\
#			-D XF86VIDMODE				\
#			-D SMART_SCHEDULE 			\

EXTDEF =		-D MITMISC				\
			-D XTEST				\
			-D XTESTEXT1				\
			-D XSYNC				\
			-D XCMISC				\
			-D BIGREQS				\
			-D XF86VIDMODE				\
			-D XF86MISC				\
			-D SCREENSAVER				\
			-D EVI					\
			-D XV					\
			-D FONTCACHE

#			-D XRECORD				\
#			-D PEXEXT				\
#			-D MITSHM				\
#			-D XIE					\
#			-D DBE					\
#			-D GLXEXT				\
#			-D XF86DRI				\
#			-D GLX_DIRECT_RENDERING			\
#			-D GLX_USE_DLOPEN			\
#			-D GLX_USE_MESA				\

.putenv STDDEF EXTDEF

#************************************************************************
#*	Bibliotecas							*
#************************************************************************

LIBSYMs =	lib/ati/.LIBSYM			\
		lib/bus/.LIBSYM			\
		lib/cfb/.LIBSYM			\
		lib/cfb16/.LIBSYM		\
		lib/cfb24/.LIBSYM		\
		lib/cfb32/.LIBSYM		\
		lib/common/.LIBSYM		\
		lib/ddc/.LIBSYM			\
		lib/dix/.LIBSYM			\
		lib/fb/.LIBSYM			\
		lib/fbdev/.LIBSYM		\
		lib/fbdevhw/.LIBSYM		\
		lib/i2c/.LIBSYM			\
		lib/input/.LIBSYM		\
		lib/int10/.LIBSYM		\
		lib/mfb/.LIBSYM			\
		lib/mi/.LIBSYM			\
		lib/miextshadow/.LIBSYM		\
		lib/nv/.LIBSYM			\
		lib/os/.LIBSYM			\
		lib/rac/.LIBSYM			\
		lib/ramdac/.LIBSYM		\
		lib/render/.LIBSYM		\
		lib/s3virge/.LIBSYM		\
		lib/scanpci/.LIBSYM		\
		lib/shadowfb/.LIBSYM		\
#		lib/sis/.LIBSYM			\
		lib/trident/.LIBSYM		\
		lib/tropix/.LIBSYM		\
		lib/vga/.LIBSYM			\
		lib/vgahw/.LIBSYM		\
		lib/Xext/.LIBSYM		\
		lib/Xi/.LIBSYM			\
		lib/xaa/.LIBSYM			\
		lib/xf1bpp/.LIBSYM		\
		lib/xf4bpp/.LIBSYM		\
		lib/xf24_32bpp/.LIBSYM		\
		lib/xf8_16bpp/.LIBSYM		\
		lib/xf8_32bpp/.LIBSYM		\
		lib/xf86config/.LIBSYM		\
		lib/xpstubs/.LIBSYM		\
		$xc/lib/os/lib/.LIBSYM		\
#		$xc/lib/Xau/lib/.LIBSYM		\
		$xc/lib/font/lib/.LIBSYM	\
		$xc/lib/zlib/lib/.LIBSYM

ALLLIBs =	lib/ati				\
		lib/bus				\
		lib/cfb				\
		lib/cfb16			\
		lib/cfb24			\
		lib/cfb32			\
		lib/common			\
		lib/ddc				\
		lib/dix				\
		lib/fb				\
		lib/fbdev			\
		lib/fbdevhw			\
		lib/i2c				\
		lib/input			\
		lib/int10			\
		lib/mfb				\
		lib/mi				\
		lib/miextshadow			\
		lib/nv				\
		lib/os				\
		lib/rac				\
		lib/ramdac			\
		lib/render			\
		lib/s3virge			\
		lib/scanpci			\
		lib/shadowfb			\
#		lib/sis				\
		lib/trident			\
		lib/tropix			\
		lib/vga				\
		lib/vgahw			\
		lib/Xext			\
		lib/Xi				\
		lib/xaa				\
		lib/xf1bpp			\
		lib/xf4bpp			\
		lib/xf24_32bpp			\
		lib/xf8_16bpp			\
		lib/xf8_32bpp			\
		lib/xf86config			\
		lib/xpstubs			\
		$xc/lib/os/lib			\
#		$xc/lib/Xau/lib			\
		$xc/lib/font/lib		\
		$xc/lib/zlib/lib

#************************************************************************
#*	Compilação do Servidor X					*
#************************************************************************

$PGM:		lib $LIBSYMs
		ld -usn -o $PGM /lib/crt0.o $ALLLIBs -lt.o
		size $PGM

lib:		.exist
		mkdir lib

#************************************************************************
#*	Cópia/Comparação						*
#************************************************************************

.virtual	cp cpq cmp cmpq cmpqcp unld clean rmobj

cp:		$DIR $PGM
		cp -f $PGM $DIR
		chmod $PROT $DIR/$PGM
		ls -L $DIR/$PGM

cpq:
		cp -f $PGM $DIR
		chmod $PROT $DIR/$PGM
		ls -L $DIR/$PGM

$DIR:		.exist
		mkdir -f $DIR

cmp:
		-cmpobj $PGM $DIR

cmpq:
		-cmpobj -q $PGM $DIR

cmpqcp:
		-cmpobj -q $PGM $DIR  ||  make cpq

unld:
		-rm -f $PGM

clean \
rmobj:
		walk -. -p "*.o" -p "*/.LIBSYM" . | rm

#************************************************************************
#*	Compilação das bibliotecas					*
#************************************************************************

lib/ati/.LIBSYM:
		cd hw/drivers/ati	&& make

lib/bus/.LIBSYM:
		cd hw/os-support/bus	&& make

lib/cfb/.LIBSYM:
		cd cfb			&& make

lib/cfb16/.LIBSYM:
		cd cfb16		&& make

lib/cfb24/.LIBSYM:
		cd cfb24		&& make

lib/cfb32/.LIBSYM:
		cd cfb32		&& make

lib/common/.LIBSYM:
		cd hw/common		&& make

lib/dix/.LIBSYM		\
lib/xpstubs/.LIBSYM:
		cd dix			&& make

lib/ddc/.LIBSYM:
		cd hw/ddc		&& make

lib/fb/.LIBSYM:
		cd fb			&& make

lib/fbdev/.LIBSYM:
		cd hw/drivers/fbdev	&& make

lib/fbdevhw/.LIBSYM:
		cd hw/fbdevhw		&& make

lib/i2c/.LIBSYM:
		cd hw/i2c		&& make

lib/int10/.LIBSYM:
		cd hw/int10		&& make

lib/input/.LIBSYM:
		cd hw/input/mouse 	&& make

lib/mfb/.LIBSYM:
		cd mfb			&& make

lib/mi/.LIBSYM:
		cd mi			&& make

lib/miextshadow/.LIBSYM:
		cd miext/shadow		&& make

lib/nv/.LIBSYM:
		cd hw/drivers/nv	&& make

lib/os/.LIBSYM:
		cd os			&& make

lib/rac/.LIBSYM:
		cd hw/rac		&& make

lib/ramdac/.LIBSYM:
		cd hw/ramdac		&& make

lib/render/.LIBSYM:
		cd render		&& make

lib/scanpci/.LIBSYM:
		cd hw/scanpci		&& make

lib/s3virge/.LIBSYM:
		cd hw/drivers/s3virge	&& make

lib/shadowfb/.LIBSYM:
		cd hw/shadowfb		&& make

#lib/sis/.LIBSYM:
#		cd hw/drivers/sis	&& make

lib/trident/.LIBSYM:
		cd hw/drivers/trident	&& make

lib/tropix/.LIBSYM:
		cd hw/os-support/tropix	&& make

lib/vga/.LIBSYM:
		cd hw/drivers/vga	&& make

lib/vgahw/.LIBSYM:
		cd hw/vgahw		&& make

lib/Xext/.LIBSYM:
		cd Xext			&& make

lib/Xi/.LIBSYM:
		cd Xi			&& make

lib/xaa/.LIBSYM:
		cd hw/xaa		&& make

lib/xf1bpp/.LIBSYM:
		cd hw/xf1bpp		&& make

lib/xf4bpp/.LIBSYM:
		cd hw/xf4bpp		&& make

lib/xf24_32bpp/.LIBSYM:
		cd hw/xf24_32bpp 	&& make

lib/xf8_16bpp/.LIBSYM:
		cd hw/xf8_16bpp 	&& make

lib/xf8_32bpp/.LIBSYM:
		cd hw/xf8_32bpp 	&& make

lib/xf86config/.LIBSYM:
		cd hw/parser		&& make

$xc/lib/os/lib/.LIBSYM:
		cd $xc/lib/os		&& make

#$xc/lib/Xau/lib/.LIBSYM:
#		cd $xc/lib/Xau		&& make

$xc/lib/font/lib/.LIBSYM:
		cd $xc/lib/font		&& make

$xc/lib/zlib/lib/.LIBSYM:
		cd $xc/lib/zlib		&& make
