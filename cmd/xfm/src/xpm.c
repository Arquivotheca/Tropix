/*
 ****************************************************************
 *								*
 *			xpm.c					*
 *								*
 *	Definições e conversões de ícones			*
 *								*
 *	Versão	4.5.0, de 24.06.03				*
 *		4.5.0, de 09.09.03				*
 *								*
 *	Módulo: xfm						*
 *		Utilitários Básicos				*
 *		Categoria B					*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 2003 NCE/UFRJ - tecle "man licença"	*
 * 								*
 ****************************************************************
 */

#include <sys/types.h>

#include <stdio.h>
#include <stdlib.h>
#include <stat.h>

#include <X11/xpm.h>

#include "../h/xfm.h"

/*
 ****************************************************************
 *	Variáveis e Definições globais				*
 ****************************************************************
 */
#undef	BIG

entry char	background_line[32];

const char	*small_dir_xpm[];
const char	*big_dir_xpm[];
const char	*small_mount_xpm[];
const char	*small_disk_xpm[];
const char	*big_disk_xpm[];
const char	*small_executable_xpm[];
const char	*big_executable_xpm[];
const char	*small_text_xpm[];
const char	*big_text_xpm[];
const char	*big_script_xpm[];
const char	*small_symlink_xpm[];
const char	*small_exclam_xpm[];

entry XImage	*small_dir_ximage;
entry XImage	*small_mount_ximage;
entry XImage	*small_disk_ximage;
entry XImage	*small_executable_ximage;
entry XImage	*small_text_ximage;
entry XImage	*small_symlink_ximage;
entry XImage	*small_exclam_ximage;

#ifdef	BIG
entry XImage	*big_dir_ximage;
entry XImage	*big_disk_ximage;
entry XImage	*big_executable_ximage;
entry XImage	*big_text_ximage;
entry XImage	*big_script_ximage;
#endif	BIG

typedef struct
{
	const char	*xpm_nm;	/* Nome do XPM */
	const char	**xpm_xpm;	/* Fonte em caracteres */
	XImage		**xpm_ximage;	/* Convertido */

}	XPMICON;

const XPMICON	xpmicon[] =
{
	"small_dir",		small_dir_xpm,		&small_dir_ximage,
	"small_mount",		small_mount_xpm,	&small_mount_ximage,
	"small_disk",		small_disk_xpm,		&small_disk_ximage,
	"small_executable",	small_executable_xpm,	&small_executable_ximage,
	"small_text",		small_text_xpm,		&small_text_ximage,
	"small_symlink",	small_symlink_xpm,	&small_symlink_ximage,
	"small_exclam",		small_exclam_xpm,	&small_exclam_ximage,

#ifdef	BIG
	"big_dir",		big_dir_xpm,		&big_dir_ximage,
	"big_disk",		big_disk_xpm,		&big_disk_ximage,
	"big_executable",	big_executable_xpm,	&big_executable_ximage,
	"big_text",		big_text_xpm,		&big_text_ximage,
	"big_script",		big_script_xpm,		&big_script_ximage,
#endif	BIG

	NOSTR
};

/*
 ****** Protótipos de funções ***********************************
 */
void		convert_one_xpm_image (XpmAttributes attr, const XPMICON *xpm);

/*
 ****************************************************************
 *	Prepara as imagens XPM					*
 ****************************************************************
 */
void
convert_xpm_images (void)
{
	const XPMICON		*xpm;
	Window			root;
	XWindowAttributes	root_attr;
	XpmAttributes		attr;

	/*
	 *	Prepara a conversão
	 */
	root = RootWindow (display, DefaultScreen (display));

	XGetWindowAttributes (display, root, &root_attr);

	attr.colormap  = root_attr.colormap;
	attr.valuemask = XpmInfos | XpmReturnPixels | XpmColormap;

	/*
	 *	Converte os XPMs em XImages
	 */
	for (xpm = xpmicon; xpm->xpm_nm != NOSTR; xpm++)
		convert_one_xpm_image (attr, xpm);

}	/* end convert_xpm_images */

/*
 ****************************************************************
 *	Converte uma imagem XPM em Ximage			*
 ****************************************************************
 */
void
convert_one_xpm_image (XpmAttributes attr, const XPMICON *xpm)
{
	/*
	 *	Converte um XPM em XImage
	 */
	switch (XpmCreateImageFromData (display, (char **)xpm->xpm_xpm, xpm->xpm_ximage, NULL, &attr))
	{
	    case XpmSuccess:
		return;

	    case XpmColorFailed:
		error ("$%s: Impossível alocar as cores pedidas", xpm->xpm_nm);

	    case XpmFileInvalid:
		error ("$Formato inválido do arquivo XPM \"%s\"", xpm->xpm_nm);

	    case XpmColorError:
		error ("$Especificação de cor inválida no XPM \"%s\"", xpm->xpm_nm);

	    case XpmNoMemory:
		error ("$%s: Memória insuficiente", xpm->xpm_nm);

	    default:
		error ("$%s: Erro desconhecido", xpm->xpm_nm);
	}

}	/* end convert_one_xpm_image */

/*
 ****************************************************************
 *	Ícone pequeno do Diretório				*
 ****************************************************************
 */
const char	*small_dir_xpm[] =
{
	"16 16 9 1",

	background_line,
	".	c Black",
	"B	c #8F8F00",
	"D	c #EFEFEF",
	"E	c #FEF78F",
	"F	c #FEF7EE",
	"G	c #CEC75F",
	"H	c #FEC78F",
	"I	c #2E2F00",

	"                ",
	"  BBBB.         ",
	" BFEEEB.        ",
	"BGGGGGGGBBBBBB  ",
	"BDDDDDDDDDDDDD. ",
	"BDEEEEEEEEHEHEG.",
	"BDEEEEEEEHEHHEGI",
	"BDEEEEEEHEEHEHGI",
	"BDEEEEHEEHEEHEGI",
	"BDEEEHEHEEHHEHGI",
	"BDEHEEEHEHHEHHGI",
	"BDEEHHEHEHHHEHGI",
	"BDEHEHEHHEHHHHGI",
	"BGGGGGGGGGGGGGGI",
	" IIIIIIIIIIIIII ",
	"                "
};

#ifdef	BIG
/*
 ****************************************************************
 *	Ícone grande do Diretório				*
 ****************************************************************
 */
const char	*big_dir_xpm[] =
{
	"32 32 9 1",

	background_line,
	".	c Black",
	"B	c #8F8F00",
	"D	c #EFEFEF",
	"E	c #FEF78F",
	"F	c #FEF7EE",
	"G	c #CEC75F",
	"H	c #FEC78F",
	"I	c #2E2F00",

	"                                ",
	"                                ",
	"    BBBBBBBBB.                  ",
	"   BDEEEEEEEEB.                 ",
	"  BDEEEEEEEEEEB.                ",
	" BFEEEEEEEEEEEEB.               ",
	"BGGGGGGGGGGGGGGGBBBBBBBBBBBBBB  ",
	"BDDDDDDDDDDDDDDDDDDDDDDDDDDDDD. ",
	"BDEEEEEEEEEEEEEEEEEEEHEEHEEEHEG.",
	"BDEEEEEEEEEEEEEHEHEHEEEHEEEHEHGI",
	"BDEEEEEEEEEEEEEEEEEEEEHEEHHEEEGI",
	"BDEEEEEEEEEEEEEEEEHEHEEEHEEEEHGI",
	"BDEEEEEEEEEEHEHEHEEEEEEEEHEHEHGI",
	"BDEEEEEEEEEEEEEEEEHEHEHEHEEEHEGI",
	"BDEEEEEEEEHEEEEHEEEEEEEHEEHHEHGI",
	"BDEEEEEEEEEEEHEEHEHEEHEEHEEEHEGI",
	"BDEEEEEEHEEEEEEEEEEEHEHEEHHEEHGI",
	"BDEEEEEEEEHEHEHEHHEHEEEEHEEHHEGI",
	"BDEEEEHEEEEEEEEEEEEEHEHHEHHHEHGI",
	"BDEEEEEEHEEEHEHEHEHEEHEEHEEHHHGI",
	"BDEEEEEEEEEHEEEEEHEHHEHHEHHEHEGI",
	"BDEHEHEEHEHEEHEHEEEEEHHHHEHHHHGI",
	"BDEEEEEEEEEHEEHEHHEHHEEEHHHHEHGI",
	"BDEEHEHEEHEEEHEHEEHEEHHHEHEEHHGI",
	"BDHEEEEEHEEHHEEHEHEEHHHEHHHHHHGI",
	"BDEEEHEHEEHEEHHEHEHHEEHHHEHHHHGI",
	"BDHEHEHEEHEHEEEHEHEEHHEHEHHHEHGI",
	"BGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGI",
	" IIIIIIIIIIIIIIIIIIIIIIIIIIIIII ",
	"                                ",
	"                                ",
	"                                ",
};
#endif	BIG

/*
 ****************************************************************
 *	Ícone pequeno do Diretório Montável			*
 ****************************************************************
 */
const char	*small_mount_xpm[] =
{
	"16 16 10 1",

	background_line,
	".	c Black",
	"B	c #8F8F00",
	"D	c #EFEFEF",
	"E	c #FEF78F",
	"F	c #FEF7EE",
	"G	c #CEC75F",
	"H	c #FEC78F",
	"I	c #2E2F00",
	"R	c orangered",

	"                ",
	"  BBBB.         ",
	" BFEEEB.        ",
	"BGGGGGGGBBBBBB  ",
	"BDDDDDDDDDDDDD. ",
	"BDEEEEEEEEHEHEG.",
	"BDEEEEEREHEHHEGI",
	"BDEEEERRREEHEHGI",
	"BDEEERRRRREEHEGI",
	"BDEEEHRRREHHEHGI",
	"BDEHEEEREHHEHHGI",
	"BDEEHHEHEHHHEHGI",
	"BDEHEHEHHEHHHHGI",
	"BGGGGGGGGGGGGGGI",
	" IIIIIIIIIIIIII ",
	"                "
};

/*
 ****************************************************************
 *	Ícone pequeno do Disco rígido				*
 ****************************************************************
 */
const char	*small_disk_xpm[] =
{
	"16 16 18 1",

	background_line,
	"B	c #6F6F6F",
	"C	c #8F8F8F",
	"D	c #5F5F5F",
	".	c Black",
	"F	c #7F7F7F",
	"G	c #AEAFAE",
	"H	c #5E575E",
	"I	c #FEF7FE",
	"J	c #DEDFDE",
	"K	c #DED7DE",
	"L	c #CFCFCF",
	"M	c #CEC7CE",
	"N	c #BEB7BE",
	"O	c #9E979F",
	"P	c #00F700",
	"Q	c #007700",
	"R	c #2E5F00",

	"                ",
	"                ",
	"                ",
	"  BBBBBBBBBBBBB.",
	" BCCCCCCCCCCCCD.",
	"FGGGGGGGGGGGGBH.",
	"FIIIIIIIIQIICBH.",
	"FIJKLMNGQPQCCBH.",
	"FIKBBBBBBQCOCBH.",
	"FIKIIIIIIICOCBH.",
	"FIOOOOOOOOOOCB. ",
	"FFFFFFFFFFFFB.  ",
	".............   ",
	"                ",
	"                ",
	"                ",
};

#ifdef	BIG
/*
 ****************************************************************
 *	Ícone grande do Disco rígido				*
 ****************************************************************
 */
const char	*big_disk_xpm[] =
{
	"32 32 18 1",

	"A	c #BFBFBF",
	"B	c #6F6F6F",
	"C	c #8F8F8F",
	"D	c #5F5F5F",
	".	c Black",
	"F	c #7F7F7F",
	"G	c #AEAFAE",
	"H	c #5E575E",
	"I	c #FEF7FE",
	"J	c #DEDFDE",
	"K	c #DED7DE",
	"L	c #CFCFCF",
	"M	c #CEC7CE",
	"N	c #BEB7BE",
	"O	c #9E979F",
	"P	c #00F700",
	"Q	c #007700",
	"R	c #2E5F00",

	"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA",
	"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA",
	"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA",
	"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA",
	"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA",
	"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA",
	"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA",
	"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA",
	"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA",
	"AAAABBBBBBBBBBBBBBBBBBBBBBBBBBAA",
	"AAABCCCCCCCCCCCCCCCCCCCCCCCCCD.A",
	"AAFGGGGGGGGGGGGGGGGGGGGGGGGGBH.A",
	"AFIIIIIIIIIIIIIIIIIIIIIIIIICBH.A",
	"AFIJKLLLMMMNNNGGGGGOOOPPQCOCBH.A",
	"AFIKLLMMMMNNNGNGGOGGOOQQRCOCBH.A",
	"AFIKLLLMMMNNNNGGGGGOOOOCCCOCBH.A",
	"AFIKBBBBBBBBBBBBBBBBBBBBBCOCBH.A",
	"AFIKIIIIIIIIIIIIIIIIIIIIICOCBH.A",
	"AFIKBBBBBBBBBBBBBBBBBBBBBCOCBH.A",
	"AFIKIIIIIIIIIIIIIIIIIIIIICOCBH.A",
	"AFIOOOOOOOOOOOOOOOOOOOOOOOOCB.AA",
	"AFFFFFFFFFFFFFFFFFFFFFFFFFFB.AAA",
	"AA..........................AAAA",
	"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA",
	"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA",
	"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA",
	"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA",
	"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA",
	"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA",
	"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA",
	"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA",
	"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
};
#endif	BIG

/*
 ****************************************************************
 *	Ícone pequeno do Programa executável			*
 ****************************************************************
 */
const char	*small_executable_xpm[] =
{
	"16 16 8 1",

	background_line,
	"B	c #7E777E",
	".	c Black",
	"D	c #BEB7BE",
	"E	c #00007E",
	"F	c #FEF7FE",
	"G	c #7E7700",
	"H	c #FEF700",

	"                ",
	"       BBB      ",
	"      BFFD.     ",
	"   GGBBDBB.BGB  ",
	"  GFHHBFDB.GFH. ",
	"  .GFFBFBB.FHG. ",
	"GGGGFBBFDB.BHD..",
	"GFGFHBBFBB.GFHFH",
	".GGGFH.....DH.GG",
	".GG.GFFHHFHHG.GG",
	"B..HH.GGDG.GFD..",
	"  GH.GGGF.HGGG. ",
	"  GH.G.GG...GG. ",
	"  D..DF.H.FD..  ",
	"       ...      ",
	"                ",
	"                "
};

#ifdef	BIG
/*
 ****************************************************************
 *	Ícone grande do Programa executável			*
 ****************************************************************
 */
const char	*big_executable_xpm[] =
{
	"32 32 8 1",

	"A	c #BFBFBF",
	"B	c #7E777E",
	".	c Black",
	"D	c #BEB7BE",
	"E	c #00007E",
	"F	c #FEF7FE",
	"G	c #7E7700",
	"H	c #FEF700",

	"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA",
	"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA",
	"BBBBBBBBBBBBBBBBBBBBBBBBBBBBBBB.",
	"BDDDDDDDDDDDDDDDDDDDDDDDDDDDDDB.",
	"BDEEEEEEEEEEEEEEEEEEEEEEEEEEEEB.",
	"BDEEEEEEEEEEEEEEEEEEEDD.DD.DD.B.",
	"BDEEEEEEEEEEEEEEEEEEEDD.DD.DD.B.",
	"BDEEEEEEEEEEEEEEEEEEE.........B.",
	"BDDDDDDDDDDDDDDDDDDDDDDDDDDDDDB.",
	"BDBBBBBBBBBBBBBBBBBBBBBBBBBBBDB.",
	"BDBFFFFFFFFFFFFFFFFFFFFFFFFFFDB.",
	"BDBFFFFFFFFFFFFFFFFFFFFFFFFFFDB.",
	"BDBFFFFFFFFFFFFBBBFFFFFFFFFFFDB.",
	"BDBFFFFFFFFFFFBFFD.FFFFFFFFFFDB.",
	"BDBFFFFFFFFGGBBDBB.BGBFFFFFFFDB.",
	"BDBFFFFFFFGFHHBFDB.GFH.FFFFFFDB.",
	"BDBFFFFFFF.GFFBFBB.FHG.FFFFFFDB.",
	"BDBFFFFFGGGGFBBFDB.BHD..FFFFFDB.",
	"BDBFFFFFGFGFHBBFBB.GFHFH.FFFFDB.",
	"BDBFFFFF.GGGFH.....DH.GG.FFFFDB.",
	"BDBFFFFF.GG.GFFHHFHHG.GG.FFFFDB.",
	"BDBFFFFFB..HH.GGDG.GFD..DFFFFDB.",
	"BDBFFFFFFFGH.GGGF.HGGG.FFFFFFDB.",
	"BDBFFFFFFFGH.G.GG...GG.FFFFFFDB.",
	"BDBFFFFFFFD..DF.H.FD..FFFFFFFDB.",
	"BDBFFFFFFFFFFFF...FFFFFFFFFFFDB.",
	"BDBFFFFFFFFFFFFFFFFFFFFFFFFFFDB.",
	"BDBFFFFFFFFFFFFFFFFFFFFFFFFFFDB.",
	"BDDDDDDDDDDDDDDDDDDDDDDDDDDDDDB.",
	"BBBBBBBBBBBBBBBBBBBBBBBBBBBBBBB.",
	"................................",
	"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
};
#endif	BIG

/*
 ****************************************************************
 *	Ícone do Texto						*
 ****************************************************************
 */
const char	*small_text_xpm[] =
{
	"16 16 6 1",

	background_line,
	"B	c #7E777E",
	"C	c #FEF7FE",
	".	c Black",
	"E	c #BEB7BE",
	"F	c #FEFEFE",

	" BBBBBBBB.      ",
	" BCCCCCCCB.     ",
	" BCCCCCCCBB.    ",
	" BCCCCCCCBCB.   ",
	" BCCCCCCCBCEB.  ",
	" BCCCCCCCB..... ",
	" BCCCCCCCCCCCE. ",
	" BCFFFFFFFFFCE. ",
	" BCFFFFFFFFFCE. ",
	" BCFFFFFFFFFCE. ",
	" BCFFFFFFFFFCE. ",
	" BCFFFFFFFFFCE. ",
	" BCFFFFFFFFFCE. ",
	" BCCCCCCCCCCCE. ",
	" BEEEEEEEEEEEE. ",
	" .............. "
};

#ifdef	BIG
/*
 ****************************************************************
 *	Ícone do Texto						*
 ****************************************************************
 */
const char	*big_text_xpm[] =
{
	"32 32 6 1",

	"A	c #BFBFBF",
	"B	c #7E777E",
	"C	c #FEF7FE",
	".	c Black",
	"E	c #BEB7BE",
	"F	c #FEFEFE",

	"AAABBBBBBBBBBBBBBBBBBBBAAAAAAAAA",
	"AAABCCCCCCCCCCCCCCCCCCB.AAAAAAAA",
	"AAABCCCCCCCCCCCCCCCCCCBB.AAAAAAA",
	"AAABCCCCCCCCCCCCCCCCCCBEB.AAAAAA",
	"AAABCCCCCCCCCCCCCCCCCCBCEB.AAAAA",
	"AAABCCCCCCCCCCCCCCCCCCBCCEB.AAAA",
	"AAABCCCCCCCCCCCCCCCCCCB......AAA",
	"AAABCCCCCCCCCCCCCCCCCCCCCCCE.AAA",
	"AAABCCCCCCCCCCCCCCCCCCCCCCCE.AAA",
	"AAABCCCCFFFFFFFFFFFFFFFFFCCE.AAA",
	"AAABCCCCFFFFFFFFFFFFFFFFFCCE.AAA",
	"AAABCCCCFFFFFFFFFFFFFFFFFCCE.AAA",
	"AAABCCCCFFFFFFFFFFFFFFFFFCCE.AAA",
	"AAABCCCCFFFFFFFFFFFFFFFFFCCE.AAA",
	"AAABCCCCFFFFFFFFFFFFFFFFFCCE.AAA",
	"AAABCCCCFFFFFFFFFFFFFFFFFCCE.AAA",
	"AAABCCCCFFFFFFFFFFFFFFFFFCCE.AAA",
	"AAABCCCCFFFFFFFFFFFFFFFFFCCE.AAA",
	"AAABCCCCFFFFFFFFFFFFFFFFFCCE.AAA",
	"AAABCCCCFFFFFFFFFFFFFFFFFCCE.AAA",
	"AAABCCCCFFFFFFFFFFFFFFFFFCCE.AAA",
	"AAABCCCCFFFFFFFFFFFFFFFFFCCE.AAA",
	"AAABCCCCFFFFFFFFFFFFFFFFFCCE.AAA",
	"AAABCCCCFFFFFFFFFFFFFFFFFCCE.AAA",
	"AAABCCCCFFFFFFFFFFFFFFFFFCCE.AAA",
	"AAABCCCCFFFFFFFFFFFFFFFFFCCE.AAA",
	"AAABCCCCFFFFFFFFFFFFFFFFFCCE.AAA",
	"AAABCCCCFFFFFFFFFFFFFFFFFCCE.AAA",
	"AAABCCCCCCCCCCCCCCCCCCCCCCCE.AAA",
	"AAABCCCCCCCCCCCCCCCCCCCCCCCE.AAA",
	"AAABEEEEEEEEEEEEEEEEEEEEEEEE.AAA",
	"AAA..........................AAA"
};

/*
 ****************************************************************
 *	Ícone do "script"					*
 ****************************************************************
 */
const char	*big_script_xpm[] =
{
	"32 32 7 1",

	"A	c #BFBFBF",
	"B	c #7E777E",
	"C	c #FEF7FE",
	".	c Black",
	"E	c #BEB7BE",
	"F	c #7E7700",
	"G	c #FEF700",

	"AAABBBBBBBBBBBBBBBBBBBBAAAAAAAAA",
	"AAABCCCCCCCCCCCCCCCCCCB.AAAAAAAA",
	"AAABCCCCCCCCCCCCCCCCCCBB.AAAAAAA",
	"AAABCCCCCCCCCCCCCCCCCCBEB.AAAAAA",
	"AAABCCCCCCCCCCCCCCCCCCBCEB.AAAAA",
	"AAABCCCCCCCCCCCCCCCCCCBCCEB.AAAA",
	"AAABCCCCCCCCCCCCCCCCCCB......AAA",
	"AAABCCCCCCCCCCCCCCCCCCCCCCCE.AAA",
	"AAABCCCCCCCCCCCCCCCCCCCCCCCE.AAA",
	"AAABCCCCCCCCCBCCCCCCCCCCCCCE.AAA",
	"AAABCCCCCCBC.C.C.CCCCCCCCCCE.AAA",
	"AAABCCCCCBE.CB..C.CCCCCCCCCE.AAA",
	"AAABCCCCC.CC.C.CE.CCCCCCCCCE.AAA",
	"AAABCCCCB.B..C..B..CCCCCCCCE.AAA",
	"AAABCCCC.ECC...CCC.CCCCCCCCE.AAA",
	"AAABCCCC.B.CBCC.CEBCC.CCCCCE.AAA",
	"AAABCCCCC.EBBC.F..C..G.CCCCE.AAA",
	"AAABCCCC.CB..B.GFBC.GF.CCCCE.AAA",
	"AAABCCCCC..C...F..C..C..CCCE.AAA",
	"AAABCCCCCCCCC.FGC...CGF.CCCE.AAA",
	"AAABCCCCCCCCC.F.GCGCG.F.CCCE.AAA",
	"AAABCCCCCCCCCC.GFFC.FG.CCCCE.AAA",
	"AAABCCCCCCCCC.GF..G..FF.CCCE.AAA",
	"AAABCCCCCCCCCC..C.F.C..CCCCE.AAA",
	"AAABCCCCCCCCCCCCC...CCCCCCCE.AAA",
	"AAABCCCCCCCCCCCCCCCCCCCCCCCE.AAA",
	"AAABCCCCCCCCCCCCCCCCCCCCCCCE.AAA",
	"AAABCCCCCCCCCCCCCCCCCCCCCCCE.AAA",
	"AAABCCCCCCCCCCCCCCCCCCCCCCCE.AAA",
	"AAABCCCCCCCCCCCCCCCCCCCCCCCE.AAA",
	"AAABEEEEEEEEEEEEEEEEEEEEEEEE.AAA",
	"AAA..........................AAA"
};
#endif	BIG

/*
 ****************************************************************
 *	Ícone pequeno do Elo simbólico				*
 ****************************************************************
 */
const char	*small_symlink_xpm[] =
{
	"16 16 4 1",

	background_line,
	".	c Black",
	"E	c orangered",
	"D	c dodgerblue",

	"                ",
	"                ",
	"                ",
	"                ",
	"                ",
	"  EEEEEDDDDDD.  ",
	" EEEEEDDDDDDDD. ",
	"EE   DDDEE   DD.",
	"EE   DD EE   DD.",
	"EE   DDEEE   DD.",
	" EEEEEEEEDDDDD. ",
	"  EEEEEEDDDDD.  ",
	"         ....   ",
	"                ",
	"                ",
	"                ",
	"                ",
	"                "
};

/*
 ****************************************************************
 *	Ícone pequeno do Ponto de exclamação			*
 ****************************************************************
 */
const char	*small_exclam_xpm[] =
{
	"16 16 5 1",

	background_line,
	".	c #808080",
	"X	c orangered",
	"o	c white",
	"O	c black",

	"        .X      ",
	"       .oXX.    ",
	"      .oXXXO    ",
	"      XXXXXO    ",
	"     .XXXXO.    ",
	"     XXXXXO     ",
	"     XXXXO      ",
	"     XXXO       ",
	"     XXO        ",
	"     XO         ",
	"     O          ",
	"   .oXX         ",
	"   XXXX.        ",
	"    XXO.        ",
	"    .O.         ",
	"                "
};
