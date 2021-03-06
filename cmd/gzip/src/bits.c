/*
 ****************************************************************
 *								*
 *			bits.c					*
 *								*
 *	Output variable-length bit strings			*
 *								*
 *	Vers?o	3.0.0, de 05.06.93				*
 *		3.0.0, de 09.06.93				*
 *								*
 *	M?dulo: GZIP						*
 *		Utilit?rios de compress?o/descompress?o		*
 *		Categoria B					*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright ? 2000 NCE/UFRJ - tecle "man licen?a"	*
 *		Baseado em software hom?nimo do GNU		*
 *								*
 ****************************************************************
 */

#include <sys/types.h>

#include <stdio.h>

#include "../h/gzip.h"

/*
 ****************************************************************
 *	Vari?veis globais					*
 ****************************************************************
 */
local file_t zfile; /* output gzip file */

/*
 *	Output buffer. bits are inserted starting at the bottom
 *	(least significant bits).
 */
local unsigned short	bi_buf;

/*
 *	Number of bits used within bi_buf. (bi_buf might be implemented on
 *	more than 16 bits on some systems.)
 */
#define Buf_size (8 * 2 * sizeof (char))

/*
 *	Number of valid bits in bi_buf.  All bits above the last valid bit
 *	are always zero.
 */
local int	bi_valid;

/*
 ******	Tabela de invers?o de um byte ***************************
 */
const char	inv_byte[256] =
{
    /*	 0	 1	 2	 3	 4	 5	 6	 7	*/
    /*	 8	 9	 A	 B	 C	 D	 E	 F	*/

/* 0.*/	0x00,	0x80,	0x40,	0xC0,	0x20,	0xA0,	0x60,	0xE0,
	0x10,	0x90,	0x50,	0xD0,	0x30,	0xB0,	0x70,	0xF0,

/* 1.*/	0x08,	0x88,	0x48,	0xC8,	0x28,	0xA8,	0x68,	0xE8,
	0x18,	0x98,	0x58,	0xD8,	0x38,	0xB8,	0x78,	0xF8,

/* 2.*/	0x04,	0x84,	0x44,	0xC4,	0x24,	0xA4,	0x64,	0xE4,
	0x14,	0x94,	0x54,	0xD4,	0x34,	0xB4,	0x74,	0xF4,

/* 3.*/	0x0C,	0x8C,	0x4C,	0xCC,	0x2C,	0xAC,	0x6C,	0xEC,
	0x1C,	0x9C,	0x5C,	0xDC,	0x3C,	0xBC,	0x7C,	0xFC,

/* 4.*/	0x02,	0x82,	0x42,	0xC2,	0x22,	0xA2,	0x62,	0xE2,
	0x12,	0x92,	0x52,	0xD2,	0x32,	0xB2,	0x72,	0xF2,

/* 5.*/	0x0A,	0x8A,	0x4A,	0xCA,	0x2A,	0xAA,	0x6A,	0xEA,
	0x1A,	0x9A,	0x5A,	0xDA,	0x3A,	0xBA,	0x7A,	0xFA,

/* 6.*/	0x06,	0x86,	0x46,	0xC6,	0x26,	0xA6,	0x66,	0xE6,
	0x16,	0x96,	0x56,	0xD6,	0x36,	0xB6,	0x76,	0xF6,

/* 7.*/	0x0E,	0x8E,	0x4E,	0xCE,	0x2E,	0xAE,	0x6E,	0xEE,
	0x1E,	0x9E,	0x5E,	0xDE,	0x3E,	0xBE,	0x7E,	0xFE,

/* 8.*/	0x01,	0x81,	0x41,	0xC1,	0x21,	0xA1,	0x61,	0xE1,
	0x11,	0x91,	0x51,	0xD1,	0x31,	0xB1,	0x71,	0xF1,

/* 9.*/	0x09,	0x89,	0x49,	0xC9,	0x29,	0xA9,	0x69,	0xE9,
	0x19,	0x99,	0x59,	0xD9,	0x39,	0xB9,	0x79,	0xF9,

/* A.*/	0x05,	0x85,	0x45,	0xC5,	0x25,	0xA5,	0x65,	0xE5,
	0x15,	0x95,	0x55,	0xD5,	0x35,	0xB5,	0x75,	0xF5,

/* B.*/	0x0D,	0x8D,	0x4D,	0xCD,	0x2D,	0xAD,	0x6D,	0xED,
	0x1D,	0x9D,	0x5D,	0xDD,	0x3D,	0xBD,	0x7D,	0xFD,

/* C.*/	0x03,	0x83,	0x43,	0xC3,	0x23,	0xA3,	0x63,	0xE3,
	0x13,	0x93,	0x53,	0xD3,	0x33,	0xB3,	0x73,	0xF3,

/* D.*/	0x0B,	0x8B,	0x4B,	0xCB,	0x2B,	0xAB,	0x6B,	0xEB,
	0x1B,	0x9B,	0x5B,	0xDB,	0x3B,	0xBB,	0x7B,	0xFB,

/* E.*/	0x07,	0x87,	0x47,	0xC7,	0x27,	0xA7,	0x67,	0xE7,
	0x17,	0x97,	0x57,	0xD7,	0x37,	0xB7,	0x77,	0xF7,

/* F.*/	0x0F,	0x8F,	0x4F,	0xCF,	0x2F,	0xAF,	0x6F,	0xEF,
	0x1F,	0x9F,	0x5F,	0xDF,	0x3F,	0xBF,	0x7F,	0xFF

};

/*
 ******	Sa?da ZIP ***********************************************
 */
extern ZIPFILE	zipfile;	/* Estrutura para a sa?da ZIP */

/*
 ****************************************************************
 *	Initialize the bit string routines			*
 ****************************************************************
 */
void
bi_init (file_t zp)
{
	zfile  = zp;
	bi_buf = 0;
	bi_valid = 0;

}	/* end bi_init */

/*
 ****************************************************************
 *	Send a value on a given number of bits			*
 ****************************************************************
 */
void
send_bits (register unsigned value, register int length)
{
	register ZIPFILE *zp1 = &zipfile;
	register ulong	u;

	/*
	 *	If not enough room in bi_buf, use (valid) bits
	 *	from bi_buf and (16 - bi_valid) bits from value,
	 *	leaving (width - (16-bi_valid)) unused bits in value.
	 */
	if (bi_valid > Buf_size - length)
	{
		u = bi_buf | (value << bi_valid);
		PUTZIP (u); 	PUTZIP (u >> 8);

		bi_buf = value >> (Buf_size - bi_valid);
		bi_valid += length - Buf_size;
	}
	else
	{
		bi_buf |= value << bi_valid;
		bi_valid += length;
	}

}	/* end send_bits */

/*
 ****************************************************************
 *	Reverse the first len bits of a code			*
 ****************************************************************
 */
unsigned int
bi_reverse (register unsigned code, register int len)
{
	register unsigned int	left, right;

	if   (len < 8)
	{
		return (inv_byte[code] >> (8 - len));
	}
	elif (len == 8)
	{
		return (inv_byte[code]);
	}
	elif (len <= 16)
	{
		left  = inv_byte[code >> 8];
		right = inv_byte[code & 0xFF];

		right <<= (len - 8);
		left  >>= (16 - len);

		return	(left | right);
	}
	else	/* len > 16 */
	{
		fprintf (stderr, "bi_reverse: len = %d\n", len);
		return (0);
	}

#if (0)	/*************************************/
	register unsigned	res = 0;

	do
	{
		res |= code & 1;
		code >>= 1, res <<= 1;
	}
	while (--len > 0);

	return res >> 1;
#endif	/*************************************/

}	/* end bi_reverse */

/*
 ****************************************************************
 *	Write out any remaining bits in an incomplete byte	*
 ****************************************************************
 */
void
bi_windup (void)
{
	register ZIPFILE *zp1 = &zipfile;
	register ulong	u;

	if   (bi_valid > 8)
	{
		u = bi_buf; 	PUTZIP (u); 	PUTZIP (u >> 8);
	}
	elif (bi_valid > 0)
	{
		PUTZIP (bi_buf);
	}

	bi_buf = 0;
	bi_valid = 0;

}	/* end bi_windup */

/*
 ****************************************************************
 *	Copy a stored block to the zip file			*
 ****************************************************************
 */
void
copy_block (register char *buf, register unsigned len, int header)
{
	register ZIPFILE *zp1 = &zipfile;
	register unsigned	not_len;

	bi_windup ();			  /* align on byte boundary */

	if (header)
	{
		PUTZIP (len);		PUTZIP (len >> 8);
		not_len = ~len;
		PUTZIP (not_len);	PUTZIP (not_len >> 8);
	}

	while (len--)
	{
		PUTZIP (*buf++);
	}

}	/* end copy_block */
