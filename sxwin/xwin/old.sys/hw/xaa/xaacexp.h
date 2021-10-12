/* $XFree86: xc/programs/Xserver/hw/xfree86/xaa/xaacexp.h,v 1.8 2005/10/14 15:17:11 tsi Exp $ */


#include <X11/Xarch.h>

#ifndef FIXEDBASE
#define CHECKRETURN(b) if(width <= ((b) * 32)) return(base + (b))
#else
#define CHECKRETURN(b) if(width <= ((b) * 32)) return(base)
#endif

#if X_BYTE_ORDER == X_BIG_ENDIAN
# define SHIFT_L(value, shift) ((value) >> (shift))
# define SHIFT_R(value, shift) ((value) << (shift))
#else
# define SHIFT_L(value, shift) ((value) << (shift))
# define SHIFT_R(value, shift) ((value) >> (shift))
#endif

#ifndef MSBFIRST
# define BYTE_EXPAND3 byte_expand3
# ifdef FIXEDBASE
#   define WRITE_IN_BITORDER(dest, offset, data) \
	*(dest) = data;
# else
#   define WRITE_IN_BITORDER(dest, offset, data) \
	*(dest + offset) = data;
# endif
#else
# define BYTE_EXPAND3 byte_reversed_expand3
# ifdef FIXEDBASE
#   define WRITE_IN_BITORDER(dest, offset, data) \
	*(dest) = SWAP_BITS_IN_BYTES(data);
# else
#   define WRITE_IN_BITORDER(dest, offset, data) \
	*(dest + offset) = SWAP_BITS_IN_BYTES(data)
# endif
#endif

#ifdef FIXEDBASE
# ifdef MSBFIRST
#  define WRITE_BITS(b)   *base = SWAP_BITS_IN_BYTES(b)
# else
#  define WRITE_BITS(b)   *base = (b)
# endif
# if X_BYTE_ORDER == X_BIG_ENDIAN
#  define WRITE_BITS1(b) { \
	CARD32 tmp = (b); \
	*base = BYTE_EXPAND3[*(CARD8 *)(&tmp)] << 8 | \
		BYTE_EXPAND3[*((CARD8 *)(&tmp) + 1)] >> 16; }
#  define WRITE_BITS2(b) { \
	CARD32 tmp = (b); \
	*base = BYTE_EXPAND3[*(CARD8 *)(&tmp)] << 8 | \
		BYTE_EXPAND3[*((CARD8 *)(&tmp) + 1)] >> 16; \
	*base = BYTE_EXPAND3[*((CARD8 *)(&tmp) + 1)] << 16 | \
		BYTE_EXPAND3[*((CARD8 *)(&tmp) + 2)] >> 8; }
#  define WRITE_BITS3(b) { \
	CARD32 tmp = (b); \
	*base = BYTE_EXPAND3[*(CARD8 *)(&tmp)] << 8 | \
		BYTE_EXPAND3[*((CARD8 *)(&tmp) + 1)] >> 16; \
	*base = BYTE_EXPAND3[*((CARD8 *)(&tmp) + 1)] << 16 | \
		BYTE_EXPAND3[*((CARD8 *)(&tmp) + 2)] >> 8; \
	*base = BYTE_EXPAND3[*((CARD8 *)(&tmp) + 2)] << 24 | \
		BYTE_EXPAND3[*((CARD8 *)(&tmp) + 3)]; }
# else
#  define WRITE_BITS1(b) { \
	CARD32 tmp = (b); \
	*base = BYTE_EXPAND3[*(CARD8 *)(&tmp)] | \
		BYTE_EXPAND3[*((CARD8 *)(&tmp) + 1)] << 24; }
#  define WRITE_BITS2(b) { \
	CARD32 tmp = (b); \
	*base = BYTE_EXPAND3[*(CARD8 *)(&tmp)] | \
		BYTE_EXPAND3[*((CARD8 *)(&tmp) + 1)] << 24; \
	*base = BYTE_EXPAND3[*((CARD8 *)(&tmp) + 1)] >> 8 | \
		BYTE_EXPAND3[*((CARD8 *)(&tmp) + 2)] << 16; }
#  define WRITE_BITS3(b) { \
	CARD32 tmp = (b); \
	*base = BYTE_EXPAND3[*(CARD8 *)(&tmp)] | \
		BYTE_EXPAND3[*((CARD8 *)(&tmp) + 1)] << 24; \
	*base = BYTE_EXPAND3[*((CARD8 *)(&tmp) + 1)] >> 8 | \
		BYTE_EXPAND3[*((CARD8 *)(&tmp) + 2)] << 16; \
	*base = BYTE_EXPAND3[*((CARD8 *)(&tmp) + 2)] >> 16 | \
		BYTE_EXPAND3[*((CARD8 *)(&tmp) + 3)] << 8; }
# endif
#else
# ifdef MSBFIRST
#  define WRITE_BITS(b)   *(base++) = SWAP_BITS_IN_BYTES(b)
# else
#  define WRITE_BITS(b)   *(base++) = (b)
# endif
# if X_BYTE_ORDER == X_BIG_ENDIAN
#  define WRITE_BITS1(b) { \
	CARD32 tmp = (b); \
	*(base++)   = BYTE_EXPAND3[*(CARD8 *)(&tmp)] << 8 | \
		      BYTE_EXPAND3[*((CARD8 *)(&tmp) + 1)] >> 16; }
#  define WRITE_BITS2(b) { \
	CARD32 tmp = (b); \
	*(base)     = BYTE_EXPAND3[*(CARD8 *)(&tmp)] << 8 | \
		      BYTE_EXPAND3[*((CARD8 *)(&tmp) + 1)] >> 16; \
	*(base + 1) = BYTE_EXPAND3[*((CARD8 *)(&tmp) + 1)] << 16 | \
		      BYTE_EXPAND3[*((CARD8 *)(&tmp) + 2)] >> 8; \
	base += 2; }
#  define WRITE_BITS3(b) { \
	CARD32 tmp = (b); \
	*(base)     = BYTE_EXPAND3[*(CARD8 *)(&tmp)] << 8 | \
		      BYTE_EXPAND3[*((CARD8 *)(&tmp) + 1)] >> 16; \
	*(base + 1) = BYTE_EXPAND3[*((CARD8 *)(&tmp) + 1)] << 16 | \
		      BYTE_EXPAND3[*((CARD8 *)(&tmp) + 2)] >> 8; \
	*(base + 2) = BYTE_EXPAND3[*((CARD8 *)(&tmp) + 2)] << 24 | \
		      BYTE_EXPAND3[*((CARD8 *)(&tmp) + 3)]; \
	base += 3; }
# else
#  define WRITE_BITS1(b) { \
	CARD32 tmp = (b); \
	*(base++)   = BYTE_EXPAND3[*(CARD8 *)(&tmp)] | \
		      BYTE_EXPAND3[*((CARD8 *)(&tmp) + 1)] << 24; }
#  define WRITE_BITS2(b) { \
	CARD32 tmp = (b); \
	*(base)     = BYTE_EXPAND3[*(CARD8 *)(&tmp)] | \
		      BYTE_EXPAND3[*((CARD8 *)(&tmp) + 1)] << 24; \
	*(base + 1) = BYTE_EXPAND3[*((CARD8 *)(&tmp) + 1)] >> 8 | \
		      BYTE_EXPAND3[*((CARD8 *)(&tmp) + 2)] << 16; \
	base += 2; }
#  define WRITE_BITS3(b) { \
	CARD32 tmp = (b); \
	*(base)     = BYTE_EXPAND3[*(CARD8 *)(&tmp)] | \
		      BYTE_EXPAND3[*((CARD8 *)(&tmp) + 1)] << 24; \
	*(base + 1) = BYTE_EXPAND3[*((CARD8 *)(&tmp) + 1)] >> 8 | \
		      BYTE_EXPAND3[*((CARD8 *)(&tmp) + 2)] << 16; \
	*(base + 2) = BYTE_EXPAND3[*((CARD8 *)(&tmp) + 2)] >> 16 | \
		      BYTE_EXPAND3[*((CARD8 *)(&tmp) + 3)] << 8; \
	base += 3; }
# endif
#endif

#ifdef FIXEDBASE
# ifdef MSBFIRST
#  define EXPNAME(x) x##MSBFirstFixedBase
# else
#  define EXPNAME(x) x##LSBFirstFixedBase
# endif
#else
# ifdef MSBFIRST
#  define EXPNAME(x) x##MSBFirst
# else
#  define EXPNAME(x) x##LSBFirst
# endif
#endif
