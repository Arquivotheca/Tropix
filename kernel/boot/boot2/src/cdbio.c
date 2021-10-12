/*
 ****************************************************************
 *								*
 *			cdbio.c					*
 *								*
 *	Funções para leitura do sistema de arquivos ISO9660	*
 *								*
 *	Versão	4.6.0, de 15.04.04				*
 *		4.6.0, de 21.10.04				*
 *								*
 *	Módulo: Boot2						*
 *		NÚCLEO do TROPIX para PC			*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 2004 NCE/UFRJ - tecle "man licença"	*
 *								*
 ****************************************************************
 */

#include <common.h>
#include <sync.h>

#include <cdfs.h>
#include <cdio.h>
#include <stat.h>
#include <disktb.h>
#include <bhead.h>
#include <devmajor.h>

#include "../h/common.h"
#include "../h/extern.h"
#include "../h/proto.h"

/*
 ****************************************************************
 *	Definições globais					*
 ****************************************************************
 */
entry const DISKTB	*pp_cache;		/* Cache da última sessão */
entry int		offset_cache;

/* Deve ter lugar para 12 MB */

entry daddr_t	daddrvec[12 * MBSZ / ISO_BLSZ];

const char	lowertoupper[];

/*
 ******	Protótipos de funções ***********************************
 */
int		cd_get_last_session (const DISKTB *pp);
int		cd_name_match (const char *name, const CDDIR *dp);

/*
 ****************************************************************
 *	Procura o Arquivo Desejado				*
 ****************************************************************
 */
CDDIR *
cd_iname (const DISKTB *pp, const char *path)
{
	daddr_t		lbn, pbn, end_pbn;
	int		len;
	const CDDIR	*dp, *end_dp;
	char		*cp;
	char		area[ISO_BLSZ];
	char		path_vec[128];
	static CDDIR	d;

	/*
	 *	Converte o caminho (para facilitar a comparação)
	 */
	while (*path == '/')
		path++;

	strcpy (path_vec, path);

	for (cp = path_vec; *cp != '\0'; cp++)
	{
		if   (*cp == '/')
			*cp = '\0';
	}

	*++cp = '\0';	/* NULO extra */

	/*
	 *	Obtém a última sessão, se for o caso
	 */
	if (pp_cache != pp)
		{ offset_cache = cd_get_last_session (pp); pp_cache = pp; }

	/*
	 *	Obtém o diretório da RAIZ
	 */
	bread (pp, ISOBL_TO_BL (offset_cache + ISO_SBNO), area, ISO_BLSZ);

	memmove (&d, &((CDVOL *)area)->cd_root_dir, CDDIRSZ);

	/*
	 *	Percorre os diretórios, procurando o arquivo pedido
	 */
	for (cp = path_vec; *cp != '\0'; cp = strend (cp) + 1)
	{
		if ((d.d_flags & ISO_IFMT) != ISO_DIR)
			goto bad;

		pbn	= ISO_GET_LONG (d.d_first_block) + d.d_ext_attr_len;
		end_pbn = pbn + BYTOISOBL (ISO_GET_LONG (d.d_size));

		for (/* acima */; /* abaixo */; pbn++)
		{
			if (pbn >= end_pbn)
				goto bad;

			bread (pp, ISOBL_TO_BL (pbn), area, ISO_BLSZ);

			dp = (CDDIR *)area; end_dp = (CDDIR *)((int)dp + ISO_BLSZ);

			for (/* acima */; dp < end_dp; dp = (CDDIR *)((int)dp + len))
			{
				if ((len = dp->d_len) == 0)
					{ dp = end_dp; continue; }

				/* Pula "." e ".." */

				if (dp->d_name_len == 1 && dp->d_name[0] <= 1)
					continue;

				if (cd_name_match (cp, dp))
					{ memmove (&d, dp, CDDIRSZ); goto next_nm; }
	
			}	/* end for (dp = ...) */

		}	/* end for (pbn = ...) */

	    next_nm:
		/* vazio */;
	}

	/*
	 *	Guarda os endereços dos blocos
	 */
	pbn	= ISO_GET_LONG (d.d_first_block) + d.d_ext_attr_len;
	end_pbn = pbn + BYTOISOBL (ISO_GET_LONG (d.d_size));

	if (end_pbn - pbn > sizeof (daddrvec) / sizeof (daddrvec[0]))
		{ printf ("\nestouro em \"daddrvec\"\n"); return (NOCDDIR); }

	for (lbn = 0; pbn < end_pbn; pbn++)
		daddrvec[lbn++] = pbn;

	return (&d);

	/*
	 *	Em caso de erro, ...
	 */
    bad:
	printf ("\nArquivo inexistente\n");
	return (NOCDDIR);

}	/* end cd_iname */

/*
 ****************************************************************
 *	x			*
 ****************************************************************
 */
int
cd_get_last_session (const DISKTB *pp)
{
	int		nentries, i, offset;
	const TOC_ENTRY	*tep;
	TOC		toc;

	/*
	 *	Em primeiro lugar, lê a TOC
	 */
	if ((nentries = bctl (pp, CD_READ_TOC, (int)&toc)) <= 0)
		return (-1);

	/*
	 *	Procura a última entrada de dados
	 */
	for (i = nentries - 1, tep = &toc.tab[i]; /* abaixo */; i--, tep--)
	{
		if (i < 0)
			return (-1);

		if (tep->control & 4)
			break;
	}

	offset = tep->addr.lba;

#undef	DEBUG
#ifdef	DEBUG
	printf ("offset = %d\n", offset);
#endif	DEBUG

	return (offset);

}	/* end cd_get_last_session */

/*
 ****************************************************************
 *   Verifica se o nome procurado confere com o do diretório	*
 ****************************************************************
 */
int
cd_name_match (const char *name, const CDDIR *dp)
{
	char		c, cd_c;
	int		i, len;

	/*
	 *	Compara os caracteres com muito cuidado!
	 */
	for (len = dp->d_name_len, i = 0; /* abaixo */; i++)
	{
		c = name[i];

		if (i >= len)
			return (c == '\0');

		cd_c = dp->d_name[i];

		if (c == '\0')
		{
			if (cd_c == ';')
				return (1);

			if (cd_c == '.' && dp->d_name[i+1] == ';')
				return (1);

			return (0);
		}

		if (cd_c == c)
			continue;

		if (cd_c == '_' && c == '.')
			continue;

		if (lowertoupper[cd_c] != lowertoupper[c])
			return (0);
	}

}	/* end cd_name_match */

/*
 ****************************************************************
 *	Conversão minúscula => maiúscula			*
 ****************************************************************
 */
entry const char	lowertoupper[] =
{
/*	NUL	SOH	STX	ETX	EOT	ENQ	ACK	BE	*/

	0x00,  	0x01,	0x02,	0x03,	0x04,	0x05,	0x06,	0x07,


/*	BS	HT	LF	VT	FF	CR	SO	SI	*/

	0x08,	0x09,	0x0A,	0x0B,	0x0C,	0x0D,	0x0E,	0x0F,


/*	DLE	DC1	DC2	DC3	DC4	NAK	SYN	ETB	*/

	0x10,	0x11,	0x12,	0x13,	0x14,	0x15,	0x16,	0x17,


/*	CAN	EM	SUB	ESC	FS	GS	RS	US	*/

	0x18,	0x19,	0x1A,	0x1B,	0x1C,	0x1D,	0x1E,	0x1F,


/*	blank	!	"	#	$	%	&	'	*/

	' ',	'!',	'"',	'#',	'$',	'%',	'&',	'\'',


/*	(	)	*	+	,	-	.	/	*/

	'(',	')',	'*',	'+',	',',	'-',	'.',	'/',


/*	0	1	2	3	4	5	6	7	*/

	'0',	'1',	'2',	'3',	'4',	'5',	'6',	'7',


/*	8	9	:	;	<	=	>	?	*/

	'8',	'9',	':',	';',	'<',	'=',	'>',	'?',


/*	@	A	B	C	D	E	F	G	*/

	'@',	'A',	'B',	'C',	'D',	'E',	'F',	'G',


/*	H	I	J	K	L	M	N	O	*/

	'H',	'I',	'J',	'K',	'L',	'M',	'N',	'O',


/*	P	Q	R	S	T	U	V	W	*/

	'P',	'Q',	'R',	'S',	'T',	'U',	'V',	'W',


/*	X	Y	Z	[	\	]	^	_	*/

	'X',	'Y',	'Z',	'[',	'\\',	']',	'^',	'_',


/*	`	a	b	c	d	e	f	g	*/

	'`',	'A',	'B',	'C',	'D',	'E',	'F',	'G',


/*	h	i	j	k	l	m	n	o	*/

	'H',	'I',	'J',	'K',	'L',	'M',	'N',	'O',


/*	p	q	r	s	t	u	v	w	*/

	'P',	'Q',	'R',	'S',	'T',	'U',	'V',	'W',


/*	x	y	z	{	|	}	~	(del)	*/

	'X',	'Y',	'Z',	'{',	'|',	'}',	'~',	0x7F,


/*	80	81	82	83	84	85	86	87	*/

	0x80,	0x81,	0x82,	0x83,	0x84,	0x85,	0x86,	0x87,


/*	88	89	8A	8B	8C	8D	8E	8F	*/

	0x88,	0x89,	0x8A,	0x8B,	0x8C,	0x8D,	0x8E,	0x8F,


/*	90	91	92	93	94	95	96	97	*/

	0x90,	0x91,	0x92,	0x93,	0x94,	0x95,	0x96,	0x97,


/*	98	99	9A	9B	9C	9D	9E	9F	*/

	0x98,	0x99,	0x9A,	0x9B,	0x9C,	0x9D,	0x9E,	0x9F,

/*	A0 nbs  A1  ¡   A2  ¢   A3  £   A4  ¤   A5  ¥   A6  ¦   A7  §   */

	0xA0,	'¡',	'¢',	'£',	'¤',	'¥',	'¦',	'§',

/*	A8  ¨   A9  ©   AA  ª   AB  «   AC  ¬   AD  ­   AE  ®   AF  ¯   */

	'¨',	'©',	'ª',	'«',	'¬',	'­',	'®',	'¯',

/*	B0  °   B1  ±   B2  ²   B3  ³   B4  ´   B5  µ   B6  ¶   B7  ·   */ 

	'°',	'±',	'²',	'³',	'´',	'µ',	'¶',	'·',

/*	B8  ¸   B9  ¹   BA  º   BB  »   BC  ¼   BD  ½   BE  ¾   BF  ¿   */

	'¸',	'¹',	'º',	'»',	'¼',	'½',	'¾',	'¿',

/*	C0  À   C1  Á   C2  Â   C3  Ã   C4  Ä   C5  Å   C6  Æ   C7  Ç  	*/   

	'À',	'Á',	'Â',	'Ã',	'Ä',	'Å',	'Æ',	'Ç',

/*	C8  È   C9  É   CA  Ê   CB  Ë   CC  Ì   CD  Í   CE  Î   CF  Ï   */

	'È',	'É',	'Ê',	'Ë',	'Ì',	'Í',	'Î',	'Ï',

/*	D0  Ð   D1  Ñ   D2  Ò   D3  Ó   D4  Ô   D5  Õ   D6  Ö   D7  ×   */

	'Ð',	'Ñ',	'Ò',	'Ó',	'Ô',	'Õ',	'Ö',	'×',

/*	D8  Ø   D9  Ù   DA  Ú   DB  Û   DC  Ü   DD  Ý   DE  Þ   DF  ß   */ 

	'Ø',	'Ù',	'Ú',	'Û',	'Ü',	'ý',	'þ',	'ß',

/*	E0  à   E1  á   E2  â   E3  ã   E4  ä   E5  å   E6  æ   E7  ç   */

	'À',	'Á',	'Â',	'Ã',	'Ä',	'Å',	'Æ',	'Ç',

/*	E8  è   E9  é   EA  ê   EB  ë   EC  ì   ED  í   EE  î   EF  ï   */

	'È',	'É',	'Ê',	'Ë',	'Ì',	'Í',	'Î',	'Ï',

/*	F0  ð   F1  ñ   F2  ò   F3  ó   F4  ô   F5  õ   F6  ö   F7  ÷   */ 

	'ð',	'Ñ',	'Ò',	'Ó',	'Ô',	'Õ',	'Ö',	'÷',

/*	F8  ø   F9  ù   FA  ú   FB  û   FC  ü   FD  ý   FE  þ   FF  ÿ   */

	'Ø',	'Ù',	'Ú',	'Û',	'Ü',	'ý',	'þ',	'ÿ',

}	/* end lowertoupper */;
