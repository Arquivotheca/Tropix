/*
 ****************************************************************
 *								*
 *			base64.c				*
 *								*
 *	Tabela e funções da codificação "base64"		*
 *								*
 *	Versão	3.2.2, de 12.08.99				*
 *		4.0.0, de 04.10.00				*
 *								*
 *	Módulo: MAIL						*
 *		Utilitários de comunicação			*
 *		Categoria B					*
 *								*
 *	TROPIX: Sistema Operacional Multiprocessado		*
 *		Copyright © 2000 NCE/UFRJ - tecle "man licença"	*
 *								*
 ****************************************************************
 */

#include <sys/types.h>
#include <sys/syscall.h>

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <termio.h>
#include <terminfo.h>
#include <time.h>
#include <errno.h>

#include "../h/mail.h"

/*
 ****************************************************************
 *	Variáveis globais					*
 ****************************************************************
 */
extern TERMIO	new_termio, old_termio; /* Estado do vídeo */

/*
 ****************************************************************
 *	Vetor do alfabeto Base 64 (codificação)			*
 ****************************************************************
 */
const char	base_64_vec[64] =
{
	'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H',
	'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P',
	'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X',
	'Y', 'Z', 'a', 'b', 'c', 'd', 'e', 'f',
	'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n',
	'o', 'p', 'q', 'r', 's', 't', 'u', 'v',
	'w', 'x', 'y', 'z', '0', '1', '2', '3',
	'4', '5', '6', '7', '8', '9', '+', '/'

}	/* end base_64_vec[] */;

/*
 ****************************************************************
 *	Tabela do alfabeto Base 64 (decodificação)		*
 ****************************************************************
 */
const char	base_64_tb[256] =
{

/*	NUL	SOH	STX	ETX	EOT	ENQ	ACK	BE	*/

	0xFF,	0xFF,	0xFF,	0xFF,	0xFF,	0xFF,	0xFF,	0xFF,


/*	BS	HT	LF	VT	FF	CR	SO	SI	*/

	0xFF,	0xFF,	0xFF,	0xFF,	0xFF,	0xFF,	0xFF,	0xFF,


/*	DLE	DC1	DC2	DC3	DC4	NAK	SYN	ETB	*/

	0xFF,	0xFF,	0xFF,	0xFF,	0xFF,	0xFF,	0xFF,	0xFF,


/*	CAN	EM	SUB	ESC	FS	GS	RS	US	*/

	0xFF,	0xFF,	0xFF,	0xFF,	0xFF,	0xFF,	0xFF,	0xFF,


/*	blank	!	"	#	$	%	&	'	*/

	0xFF,	0xFF,	0xFF,	0xFF,	0xFF,	0xFF,	0xFF,	0xFF,


/*	(	)	*	+	,	-	.	/	*/

	0xFF,	0xFF,	0xFF,	62,	0xFF,	0xFF,	0xFF,	63,


/*	0	1	2	3	4	5	6	7	*/

	52,	53,	54,	55,	56,	57,	58,	59,


/*	8	9	:	;	<	=	>	?	*/

	60,	61,	0xFF,	0xFF,	0xFF,	0xFF,	0xFF,	0xFF,


/*	@	A	B	C	D	E	F	G	*/

	0xFF,	0,	1,	2,	3,	4,	5,	6,


/*	H	I	J	K	L	M	N	O	*/

	7,	8,	9,	10,	11,	12,	13,	14,


/*	P	Q	R	S	T	U	V	W	*/

	15,	16,	17,	18,	19,	20,	21,	22,


/*	X	Y	Z	[	\	]	^	_	*/

	23,	24,	25,	0xFF,	0xFF,	0xFF,	0xFF,	0xFF,


/*	`	a	b	c	d	e	f	g	*/

	0xFF,	26,	27,	28,	29,	30,	31,	32,


/*	h	i	j	k	l	m	n	o	*/

	33,	34,	35,	36,	37,	38,	39,	40,


/*	p	q	r	s	t	u	v	w	*/

	41,	42,	43,	44,	45,	46,	47,	48,


/*	x	y	z	{	|	}	~	(del)	*/

	49,	50,	51,	0xFF,	0xFF,	0xFF,	0xFF,	0xFF,


/*	80	81	82	83	84	85	86	87	*/

	0xFF,	0xFF,	0xFF,	0xFF,	0xFF,	0xFF,	0xFF,	0xFF,


/*	88	89	8A	8B	8C	8D	8E	8F	*/

	0xFF,	0xFF,	0xFF,	0xFF,	0xFF,	0xFF,	0xFF,	0xFF,


/*	90	91	92	93	94	95	96	97	*/

	0xFF,	0xFF,	0xFF,	0xFF,	0xFF,	0xFF,	0xFF,	0xFF,


/*	98	99	9A	9B	9C	9D	9E	9F	*/

	0xFF,	0xFF,	0xFF,	0xFF,	0xFF,	0xFF,	0xFF,	0xFF,


/*	A0	A1	A2	A3	A4	A5	A6	A7	*/

	0xFF,	0xFF,	0xFF,	0xFF,	0xFF,	0xFF,	0xFF,	0xFF,


/*	A8	A9	AA	AB	AC	AD	AE	AF	*/

	0xFF,	0xFF,	0xFF,	0xFF,	0xFF,	0xFF,	0xFF,	0xFF,


/*	B0	B1	B2	B3	B4	B5	B6	B7	*/

	0xFF,	0xFF,	0xFF,	0xFF,	0xFF,	0xFF,	0xFF,	0xFF,


/*	B8	B9	BA	BB	BC	BD	BE	BF	*/

	0xFF,	0xFF,	0xFF,	0xFF,	0xFF,	0xFF,	0xFF,	0xFF,


/*	C0	C1	C2	C3	C4	C5	C6	C7	*/

	0xFF,	0xFF,	0xFF,	0xFF,	0xFF,	0xFF,	0xFF,	0xFF,


/*	C8	C9	CA	CB	CC	CD	CE	CF	*/

	0xFF,	0xFF,	0xFF,	0xFF,	0xFF,	0xFF,	0xFF,	0xFF,


/*	D0	D1	D2	D3	D4	D5	D6	D7	*/

	0xFF,	0xFF,	0xFF,	0xFF,	0xFF,	0xFF,	0xFF,	0xFF,


/*	D8	D9	DA	DB	DC	DD	DE	DF	*/

	0xFF,	0xFF,	0xFF,	0xFF,	0xFF,	0xFF,	0xFF,	0xFF,


/*	E0	E1	E2	E3	E4	E5	E6	E7	*/

	0xFF,	0xFF,	0xFF,	0xFF,	0xFF,	0xFF,	0xFF,	0xFF,


/*	E8	E9	EA	EB	EC	ED	EE	EF	*/

	0xFF,	0xFF,	0xFF,	0xFF,	0xFF,	0xFF,	0xFF,	0xFF,


/*	F0	F1	F2	F3	F4	F5	F6	F7	*/

	0xFF,	0xFF,	0xFF,	0xFF,	0xFF,	0xFF,	0xFF,	0xFF,


/*	F8	F9	FA	FB	FC	FD	FE	FF	*/

	0xFF,	0xFF,	0xFF,	0xFF,	0xFF,	0xFF,	0xFF,	0xFF

}	/* end base_64_tb[] */;

/*
 ****************************************************************
 *	Obtém o tamanho da parte codificada em "base 64"	*
 ****************************************************************
 */
int
get_base_64_sz (const LINE *lp)
{
	const char	*cp;
	int		total_bits = 0;
	int		c;
	char		t;

	/*
	 *	Primeiro, pula as linhas de cabeçalho
	 */
	for (/* vazio */; /* abaixo */; lp = lp->l_forw)
	{
		if (lp->l_head != 0)
			return (-1);

		if (lp->l_text[0] == '\0')
			break;
	}

	/*
	 *	Agora, pula as linhas em branco
	 */
	for (/* acima */; /* abaixo */; lp = lp->l_forw)
	{
		if (lp->l_head != 0)
			return (-2);

		if (lp->l_text[0] != '\0')
			break;
	}

	/*
	 *	Examina as linhas
	 */
	for (/* acima */; lp->l_head == 0; lp = lp->l_forw)
	{
		cp = lp->l_text;

		if (*cp == '\0')
			return (total_bits / 8);

		while ((c = *cp++) != 0)
		{
			if (c == '=')
				return (total_bits / 8);

			if ((t = base_64_tb[c]) == 0xFF)
				return (-3);

			total_bits += 6;
		}
	}

	return (total_bits / 8);

}	/* end get_base_64_sz */

/*
 ****************************************************************
 *	Salva o arquivo codificado em "base 64"			*
 ****************************************************************
 */
int
write_base_64_file (PARTBOX *cb)
{
	const PART	*part_head;
	char		*file_nm;
	const LINE	*lp;
	FILE		*fp;
	const char	*cp;
	int		c;
	char		t;
	int		total_bits;
	unsigned long	area;
	char		file_nm_vec[80];

	/*
	 *	Prólogo
	 */
	part_head = cb->b_cursor_mp;
	file_nm	  = part_head->p_name;

	/*
	 *	Verifica se há um nome de arquivo/se tem tamanho excessivo
	 */
	if (file_nm == NOSTR || file_nm[0] == '\0')
	{
		fprintf
		(	fp_out, "%sDê o nome do arquivo: %s",
			parmexec (c_cup, LASTLINE, 0), c_el
		);

		fflush (fp_out);

		ioctl (fileno (fp_tty), TCSETNR, &old_termio);
		file_nm = fnrgets (file_nm_vec, sizeof (file_nm_vec), fp_tty);
		ioctl (fileno (fp_tty), TCSETNR, &new_termio);

		if (file_nm == NOSTR || file_nm[0] == '\0')
			return (-1);
	}
#if (0)	/*******************************************************/
	elif ((len = strlen (file_nm)) > IDSZ)
	{
		if (file_nm[len-4] == '.')	/* Examina a terminação */
			strcpy (&file_nm[IDSZ-4], &file_nm[len-4]);

		file_nm[IDSZ] = '\0';
	}
#endif	/*******************************************************/

	/*
	 *	Verifica se o arquivo já existe/Confirma a escrita
	 */
	if (access (file_nm, F_OK) >= 0)
		cp = "\a%sO arquivo \"%s\" já existe! Reescreve? (n): %s";
	else
		cp = "%sCria o arquivo \"%s\"? (n): %s";

	fprintf
	(	fp_out, cp,
		parmexec (c_cup, LASTLINE, 0), file_nm, c_el
	);

	fflush (fp_out); /*** cb->b_msg++; ***/

	if ((cb->b_cmd = getc (fp_tty)) != 's' && cb->b_cmd != 'S' && cb->b_cmd != 'y' && cb->b_cmd != 'Y')
		{ cb->b_msg++; cb->b_next_cmd = ' '; return (-1); }

	/*
	 *	Cria o arquivo
	 */
	if ((fp = fopen (file_nm, "w")) == NOFILE)
	{
		fprintf
		(	fp_out, "\a\r%s: "
			"Não conseguir criar o arquivo \"%s\" (%s)",
			pgname, file_nm, strerror (errno)
		);
		goto out;
	}

	/*
	 *	Primeiro, pula as linhas de cabeçalho
	 */
	for (lp = part_head->p_line_head.l_forw; /* abaixo */; lp = lp->l_forw)
	{
		if (lp->l_head != 0)
			goto bad;

		if (lp->l_text[0] == '\0')
			break;
	}

	/*
	 *	Agora, pula as linhas em branco
	 */
	for (lp = lp->l_forw; /* abaixo */; lp = lp->l_forw)
	{
		if (lp->l_head != 0)
			goto bad;

		if (lp->l_text[0] != '\0')
			break;
	}

	/*
	 *	Examina as linhas
	 */
	total_bits = area = 0;

	for (/* acima */; /* abaixo */; lp = lp->l_forw)
	{
		if (lp->l_head != 0)
			break;

		cp = lp->l_text;

		if (*cp == '\0')
			break;

		while ((c = *cp++) != 0)
		{
			if (c == '=')
				break;

			if ((t = base_64_tb[c]) == 0xFF)
				goto bad;

			area <<= 6; area |= t;

			if ((total_bits += 6) >= 24)
			{
				putc (area >> 16, fp);
				putc (area >> 8,  fp);
				putc (area,	  fp);

				total_bits -= 24;
				area = 0;
			}
		}
	}

	/*
	 *	Emite os bytes residuais
	 */
	if   (total_bits >= 18)
	{
		putc (area >> 10, fp);
		putc (area >> 2,  fp);
	}
	elif (total_bits >= 12)
	{
		putc (area >> 4,  fp);
	}

	fclose (fp);

	/*
	 *	Confirma a criação
	 */
	fprintf
	(	fp_out, "%sO arquivo \"%s\" foi criado. %s",
		parmexec (c_cup, LASTLINE, 0), file_nm, c_el
	);

	fflush (fp_out); /*** cb->b_msg++; ***/

	getc (fp_tty);

	cb->b_msg++; cb->b_next_cmd = ' ';

	return (0);

	/*
	 *	Termina o campo e retorna
	 */
    bad:
	fprintf
	(	fp_out, "\a%s: "
		"Erro na decodificação do arquivo \"%s\"",
		pgname, file_nm
	);
	fclose (fp);
    out:
	fputs (c_ed, fp_out); fputs (c_sgr0, fp_out);
	fflush (fp_out); getc (fp_tty);
	return (-1);

}	/* end write_base_64_file */

/*
 ****************************************************************
 *	Mostra um texto codificado em "base 64"			*
 ****************************************************************
 */
int
show_base_64_text (const MDIREC *mp, PARTBOX *cb)
{
	const PART	*part_head;
	PART		new_part_head;
	const LINE	*lp;
	LINE		*np, *bp;
	const char	*cp = NOSTR;
	int		c, len, index = 0;
	char		t, eof = 0;
	int		total_bits;
	char		area[DATASZ], *ap;
	char		*new_line;
	unsigned int	word;

	/*
	 *	Prólogo
	 */
	part_head = cb->b_cursor_mp;

	/*
	 *	Primeiro, pula as linhas de cabeçalho
	 */
	for (lp = part_head->p_line_head.l_forw; /* abaixo */; lp = lp->l_forw)
	{
		if (lp->l_head != 0)
			goto bad;

		if (lp->l_text[0] == '\0')
			break;
	}

	/*
	 *	Agora, pula as linhas em branco
	 */
	for (lp = lp->l_forw; /* abaixo */; lp = lp->l_forw)
	{
		if (lp->l_head != 0)
			goto bad;

		if (lp->l_text[0] != '\0')
			break;
	}

	/*
	 *	Prepara a cabeça da lista
	 */
	new_part_head.p_line_head.l_forw  = &new_part_head.p_line_head;
	new_part_head.p_line_head.l_back  = &new_part_head.p_line_head;
	new_part_head.p_line_head.l_head  = 1;

	/*
	 *	Examina as linhas
	 */
	total_bits = word = 0; ap = area;

	for (/* acima */; /* abaixo */; lp = lp->l_forw)
	{
		if (lp->l_head != 0)
			goto eof_seen;

		cp = lp->l_text;

		if (*cp == '\0')
			goto eof_seen;

		while ((c = *cp++) != 0)
		{
			if (c == '=')
			{
			    eof_seen:
				word <<= 8; word |= '\n'; total_bits += 8;
				eof++; goto do_last_bytes;
			}

			if ((t = base_64_tb[c]) == 0xFF)
				goto bad;

			word <<= 6; word |= t; total_bits += 6;

		    do_last_bytes:
			for (/* acima */; total_bits >= 8; total_bits -= 8)
			{
				c = (word >> (total_bits - 8)) & 0xFF;

				if (c == '\r')
					continue;

				if (c != '\n' && !eof)
					{ *ap++ = c; continue; }

				*ap++ = '\0';

				len = strlen (area); new_line = mem_alloc (len + 1);
				strcpy (new_line, area);

				np = mem_alloc (sizeof (LINE)); np->l_head = 0;

				np->l_text  = new_line;
				np->l_len   = len;
				np->l_index = ++index;

				/*
				 *	Atualiza a lista duplamente encadeada
				 */
				bp = new_part_head.p_line_head.l_back;

				bp->l_forw = np; np->l_back = bp;

				np->l_forw = &new_part_head.p_line_head;
				new_part_head.p_line_head.l_back = np;

				ap = area;

			}	/* for (caracteres da saída (8 bits) */

			if (eof)
				goto out;

		}	/* for (caracteres da entrada (6 bits) */
	
	}	/* for (linhas da entrada) */

	/*
	 *	Converte as linhas compridas, e mostra o texto
	 */
    out:
	new_part_head.p_quoted = 0;

	convert_line_text (&new_part_head);

	return (show_line (mp, &new_part_head.p_line_head));

	/*
	 *	Erro: Termina o campo e retorna
	 */
    bad:
	fprintf
	(	fp_out, "\a%s: "
		"Erro na decodificação de um texto",
		pgname
	);

	fputs (c_ed, fp_out); fputs (c_sgr0, fp_out);
	fflush (fp_out); getc (fp_tty);
	return (-1);

}	/* end show_base_64_text */
