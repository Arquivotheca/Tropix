# $XFree86: xc/programs/Xserver/hw/xfree86/os-support/bsd/drm/kernel/sis/Makefile,v 1.6 2006/01/09 15:00:20 dawes Exp $

.PATH:	${.CURDIR}/..
KMOD= 	sis
NOMAN=	YES
SRCS=	sis_drv.c sis_ds.c sis_mm.c
SRCS+=	device_if.h bus_if.h pci_if.h opt_drm.h
CFLAGS+=	${DEBUG_FLAGS} -I. -I..

.if defined(DRM_DEBUG)
DRM_DEBUG_OPT= "\#define DRM_DEBUG 1"
.endif

.if !defined(DRM_NOLINUX)
DRM_LINUX_OPT= "\#define DRM_LINUX 1"
.endif

opt_drm.h:
	touch opt_drm.h
	echo $(DRM_DEBUG_OPT) >> opt_drm.h
	echo $(DRM_LINUX_OPT) >> opt_drm.h

.include <bsd.kmod.mk>
