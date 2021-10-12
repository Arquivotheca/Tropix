/*
 ****************************************************************
 *								*
 *			aux.c					*
 *								*
 *	Funções auxiliares					*
 *								*
 *	Versão	3.0.0, de 04.11.93				*
 *		3.0.7, de 04.08.97				*
 *								*
 *	Módulo: DOSMP						*
 *		Utilitários Sofisticados			*
 *		Categoria B					*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 2000 NCE/UFRJ - tecle "man licença"	*
 * 		Inspirado no PLDOS de Norival Ribeiro Figueira	*
 * 								*
 ****************************************************************
 */

#include <sys/common.h>
#include <sys/stat.h>
#include <sys/syscall.h>

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include <errno.h>

#include "../h/common.h"
#include "../h/dos.h"
#include "../h/proto.h"

#ifdef	BIG_ENDIAN
/*
 ****************************************************************
 *	Converte um "short little-endian"			*
 ****************************************************************
 */
int
get_bad_short (const char *cp)
{
	return (cp[0] | cp[1] << 8);

}	/* end get_bad_short */

/*
 ****************************************************************
 *	Converte um "long little-endian"			*
 ****************************************************************
 */
long
get_bad_long (const char *cp)
{
	return (cp[0] | cp[1] << 8 | cp[2] << 16 | cp[3] << 24);

}	/* end get_bad_long */

/*
 ****************************************************************
 *	Atribui um "short little-endian"			*
 ****************************************************************
 */
void
put_bad_short (int n, char *cp)
{
	cp[0] = n; cp[1] = n >> 8;

}	/* end put_bad_short */

/*
 ****************************************************************
 *	Atribui um "long little-endian"				*
 ****************************************************************
 */
void
put_bad_long (long n, char *cp)
{
	cp[0] = n;	 	cp[1] = (n >>= 8);
	cp[2] = (n >>= 8); 	cp[3] = (n >>= 8);

}	/* end put_bad_long */
#endif	BIG_ENDIAN

/*
 ****************************************************************
 *	Converte uma cadeia para o formato com brancos		*
 ****************************************************************
 */
void
put_str_with_blanks (const char *cp, char *area, int count)
{
	const char	*end_area;

	for (end_area = area + count; area < end_area; /* abaixo */)
	{
		if (*cp != '\0')
			*area++ = *cp++;
		else
			*area++ = ' ';
	}

}	/* end put_str_with_blanks */

/*
 ****************************************************************
 *	Normaliza um caminho					*
 ****************************************************************
 */
char *
path_can (char *path)
{
	char		*cp;

	for (cp = strend (path) - 1; cp[0] == '/' && cp > path; cp--)
		*cp = '\0';

	while (path[0] == '.' && path[1] == '/')
		path += 2;

	return (path);

}	/* end path_can */

/*
 ****************************************************************
 *	Obtém o último nome de um caminho			*
 ****************************************************************
 */
const char *
last_nm (const char *path)
{
	const char	*cp;

	if (cp = strrchr (path, '/'))
		return (cp + 1);
	else
		return (path);

} 	/* end last_nm */

/*
 ****************************************************************
 *	Retorna o tipo do arquivo				*
 ****************************************************************
 */
int
file_type_edit (int mode)
{
	switch (mode & Z_IFMT)
	{
	    case Z_VOL:
		return ('V');

	    case Z_DIR:
		return ('D');

	    case Z_REG:
		return ('R');

	    default:
		return ('?');
	}

}	/* end file_type_edit */

/*
 ****************************************************************
 *	Edita os indicadores de um arquivo			*
 ****************************************************************
 */
char *
file_mode_edit (int mode)
{
	int		i;
	static char	buf[12];
	char		*cp = buf;

	strcpy (cp, "21mdvshr");

	for (i = 8; i > 0; i--)
	{
		if ((mode & 0x80) == 0)
			*cp = '-';

		mode <<= 1; cp++;
	}

	return (buf);

}	/* end file_mode_edit */

/*
 ****************************************************************
 *	Retorna o tipo equivalente (para a cor)			*
 ****************************************************************
 */
int
file_color_convert (int mode)
{
	switch (mode & Z_IFMT)
	{
	    case Z_VOL:
		return (S_IFBLK >> 12);

	    case Z_DIR:
		return (S_IFDIR >> 12);

	    case Z_REG:
		return (S_IFREG >> 12);

	    default:
		return (16);
	}

}	/* end file_color_convert */

/*
 ****************************************************************
 *	Zera uma entrada de diretório				*
 ****************************************************************
 */
void
clr_dir_ent (DOSDIR *dp)
{
	memsetl (dp, 0, sizeof (DOSDIR) / sizeof (long));

}	/* end clr_dir_ent */

/*
 ****************************************************************
 *	Converte o tempo					*
 ****************************************************************
 */
time_t
get_dos_file_time (const DOSDIR *dp)
{
	TM		tm;

	/*
	 *	Se o tempo não está disponível, retorna -1
	 */
	if ((dp->d_mtime[0]|dp->d_mtime[1]|dp->d_mdate[0]|dp->d_mdate[1]) == 0)
		return (-1);

	/*
	 *	Completa a estrutura
	 */
	tm.tm_sec   = 2 * (dp->d_mtime[0] & 0x1F);
	tm.tm_min   = ((dp->d_mtime[1] & 7) << 3) + (dp->d_mtime[0] >> 5);
	tm.tm_hour  = dp->d_mtime[1] >> 3;
	tm.tm_mday  = dp->d_mdate[0] & 0x1F;
	tm.tm_mon   = ((dp->d_mdate[1] << 3) & 0x08) + (dp->d_mdate[0] >> 5) - 1;
	tm.tm_year  = (dp->d_mdate[1]  >> 1) + (1980 - 1900);
	tm.tm_wday  = 0;
	tm.tm_yday  = 0;
	tm.tm_isdst = 0;

#define	USE_GM_TIME
#ifdef	USE_GM_TIME
	return (mktime (&tm));
#else
	return (mktime (&tm) - gettzmin () * 60);
#endif	USE_GM_TIME

}	/* end get_dos_file_time */

/*
 ****************************************************************
 *	Imprime uma DATA					*
 ****************************************************************
 */
void
print_time (time_t Time)
{
	char		*tp;

	/*
	 *	Se Time == -1, o tempo não está disponível
	 */
	if (Time == -1)
		{ printf ("                   "); return; }

	/*
	 *	Converte e imprime o tempo
	 */
	tp = btime (&Time);

	printf (" %-5.5s ", tp);		/* hora */

	if (Time < SEMANA || Time >= AMANHA)
	{
		printf ("%4.4s ",  tp+20);
		printf ("%-6.6s ", tp+13);
	}
	else if (Time >= HOJE)
	{
		printf ("HOJE ");
		printf ("%-6.6s ", tp+13);
	}
	else
	{
		printf (" %-10.10s ", tp+9);
	}

}	/* end print_time */

/*
 ****************************************************************
 *	Converte o tempo para DOS				*
 ****************************************************************
 */
void
put_dos_file_time (time_t t, DOSDIR *dp)
{
	TM		*tp;

	/*
	 *	Se o tempo não está disponível, coloca ZEROs
	 */
	if (t == -1)
	{
		dp->d_mtime[0] = 0; dp->d_mtime[1] = 0;
		dp->d_mdate[0] = 0; dp->d_mdate[1] = 0;
		return;
	}

	/*
	 *	Converte usando primeiro a estrutura TM 
	 */
#ifdef	USE_GM_TIME
	tp = gmtime (&t);
#else
	tp = localtime (&t);
#endif	USE_GM_TIME

	dp->d_mtime[0] = ((tp->tm_min & 7) << 5) + (tp->tm_sec / 2);
	dp->d_mtime[1] = ((tp->tm_min >> 3) + (tp->tm_hour << 3));

	dp->d_mdate[0] = ((tp->tm_mon + 1) << 5) + (tp->tm_mday);
	dp->d_mdate[1] = ((tp->tm_year - (1980 - 1900)) << 1) + (tp->tm_mon >> 3);

}	/* end put_dos_file_time */

/*
 ****************************************************************
 *	Executa a opção "-s" (padrões de "C")			*
 ****************************************************************
 */
void
put_s_option (void)
{
	put_p_option ("*.[cshryv]", inc_pat, &inc_pati);
	put_p_option ("*[Mm]akefile", inc_pat, &inc_pati);

}	/* end put_s_option */

/*
 ****************************************************************
 *	Executa a opção "-p" (outros padrões)			*
 ****************************************************************
 */
int
put_p_option (const char *cp, const char *pat_vec[], int *pati_ptr)
{
	char		c;

	for (EVER)
	{
		while ((c = *cp) == ' ' || c == '\t' )
			cp++;

		if (c == '\0')
			return (0);

		if (*pati_ptr >= NPAT)
		{
			printf
			(	"%s: No momento somente aceitamos "
				XSTR (NPAT) " padrões\n\n",
				pgname
			);
			return (-1);
		}

		pat_vec[(*pati_ptr)++] = cp++;

		while ((c = *cp) != '\0' && c != ' ' && c != '\t')
			cp++;

		if (c == '\0')
			return (0);

		*(char *)cp++ = '\0';

	}	/* end for (EVER) */

}	/* end put_p_option */

/*
 ****************************************************************
 *	Verifica se o nome do arquivo satisfaz um dos padrões	*
 ****************************************************************
 */
int
pattern_accept (const char *file_nm)
{
	const char	**pp;

	/*
	 *	Testa em primeiro lugar os pontos
	 */
	if (!cmd_dotflag)
	{
		if (patmatch (file_nm, ".!.*") || patmatch (file_nm, "*/.!.*"))
			return (0);
	}

	/*
	 *	Agora testa os padrões de inclusão
	 */
	if ((pp = inc_pat)[0] != NOSTR)
	{
		for (/* vazio */; /* vazio */; pp++)
		{
			if (*pp == NOSTR)
				return (0);

			if (patmatch (file_nm, *pp))
				break;
		}
	}

	/*
	 *	Finalmente testa os padrões de exclusão
	 */
	if ((pp = exc_pat)[0] != NOSTR)
	{
		for (/* vazio */; *pp != NOSTR; pp++)
		{
			if (patmatch (file_nm, *pp))
				return (0);
		}
	}

	return (1);

}	/* end pattern_accept */

/*
 ****************************************************************
 *	Imprime o plural ou singular				*
 ****************************************************************
 */
const char *
plural (int n)
{
	if (n == 1)
		return ("");
	else
		return ("s");

}	/* end plural */

/*
 ****************************************************************
 *	Calcula uma percentagem					*
 ****************************************************************
 */
int
getpercent (long numer, long denom)
{
	int		quoc;

	/*
	 *	Tenta evitar overflow
	 */
	if (numer > (0x7FFFFFFF / 10000))
		quoc = numer / (denom / 10000);
	else
		quoc = 10000 * numer / denom;

	return (quoc);

}	/* end percent */

/*
 ****************************************************************
 *	Edita uma percentagem					*
 ****************************************************************
 */
const char *
editpercent (int quoc)
{
	static char	buf[8];
	char		*sinal;

	if (quoc < 0)
		{ sinal = "-"; quoc = -quoc; }
	else
		sinal = "";


	sprintf (buf, "%s%d.%02d", sinal, quoc / 100, quoc % 100);
	
	return (buf);

}	/* end editpercent */

/*
 ****************************************************************
 *	Converte blocos em KB, MB ou GB				*
 ****************************************************************
 */
char *
cv_blocks_to_KB_MB_or_GB (long bl_cnt)
{
	static char	area[32];

	if   (bl_cnt < (10*MBSZ/BLSZ))		/* Menor do que 10 MB */
	{
		bl_cnt += (KBSZ/BLSZ/2);	/* Arredonda 0.5 KB */

		sprintf
		(	area,
			"%d KB",
			(unsigned)bl_cnt >> (KBSHIFT-BLSHIFT)
		);
	}
	elif (bl_cnt < (GBSZ/BLSZ))		/* De 10 MB até 1024 MB */
	{
		bl_cnt += (MBSZ/BLSZ/(2 * 100)); /* Arredonda 0.005 MB */

		sprintf
		(	area,
			"%d.%02d MB",
			(unsigned)bl_cnt >> (MBSHIFT-BLSHIFT),
			(100 * ((unsigned)bl_cnt & (MBSZ/BLSZ - 1)))
					>> (MBSHIFT-BLSHIFT)
		);
	}
	else	/* 1024 MB ou acima */
	{
		bl_cnt += (GBSZ/BLSZ/(2 * 100)); /* Arredonda 0.005 GB */

		sprintf
		(	area,
			"%d.%02d GB",
			(unsigned)bl_cnt >> (GBSHIFT-BLSHIFT),
			(100 * ((unsigned)bl_cnt & (GBSZ/BLSZ - 1)))
					>> (GBSHIFT-BLSHIFT)
		);
	}

	return (area);

}	/* end cv_blocks_to_KB_MB_or_GB */

/*
 ****************************************************************
 *	Converte blocos em MB					*
 ****************************************************************
 */
char *
cv_blocks_to_MB (long bl_cnt)
{
	static char	area[32];

	bl_cnt += (MBSZ/BLSZ/(2 * 100)); /* Arredonda 0.005 MB */

	sprintf
	(	area,
		"%d.%02d MB",
		(unsigned)bl_cnt >> (MBSHIFT-BLSHIFT),
		(100 * ((unsigned)bl_cnt & (MBSZ/BLSZ - 1)))
				>> (MBSHIFT-BLSHIFT)
	);

	return (area);

}	/* end cv_blocks_to_MB */

/*
 ****************************************************************
 *	Imprime uma mensagem de erro				*
 ****************************************************************
 */
void
error (const char *format, ...)
{
	va_list		args;
	const char	*fmt = format;
	char		err, abrt;

	va_start (args, format);

	if ((abrt = *fmt) == '$')
		fmt++;

	if ((err = *fmt) == '*')
		fmt++;

	fprintf  (stderr, "%s: (Erro interno) ", pgname);
	vfprintf (stderr, fmt, args);

	if (err == '*' && errno != 0)
		fprintf (stderr, " (%s)", strerror (errno));

	fprintf (stderr, "\n");

	if (abrt == '$')
		quit (1);

	va_end (args);

}	/* end error */



#if (0)	/*******************************************************/
/*
 ****************************************************************
 *	Converte blocos em MB (Parte inteira)			*
 ****************************************************************
 */
int
cv_blocks_to_MB (long bl_cnt)
{
	return ((unsigned)bl_cnt >> (MBSHIFT-BLSHIFT));

}	/* end cv_blocks_to_MB */

/*
 ****************************************************************
 *	Converte blocos em MB (Décimos)				*
 ****************************************************************
 */
int
cv_blocks_to_MB_tenths (long bl_cnt)
{
	return (10 * ((unsigned)bl_cnt & (MBSZ/BLSZ - 1))) >> (MBSHIFT-BLSHIFT);

}	/* end cv_blocks_to_MB_tenths */

/*
 ****************************************************************
 *	Converte uma cadeia em maiúscula			*
 ****************************************************************
 */
char *
strupr (char *area)
{
	char		*bp;

	for (bp = area; *bp != '\0'; bp++)
		*bp = toupper (*bp);

	return (area);

}	/* end strupr */

/*
 ****************************************************************
 *	Converte uma cadeia em minúscula			*
 ****************************************************************
 */
char *
strlwr (char *area)
{
	char		*bp;

	for (bp = area; *bp != '\0'; bp++)
		*bp = tolower (*bp);

	return (area);

}	/* end strlwr */

/*
 ******	Protótipos de funções ***********************************
 */
void		put_dos_file_nm (const char *, DOSDIR *);

/*
 ****************************************************************
 *	Converte o nome de um arquivo de DOS para UNIX		*
 ****************************************************************
 */
char *
get_dos_file_nm (const DOSDIR *dp, const char **dos_nm_p)
{
	static char	sofix_nm[24], dos_nm[16];
	const char	*np, *rp = NOSTR;
	char		*bp;
	const char	*end_np, *end_rp = NOSTR;
	int		c;

	/*
	 *	Compõe o nome completo TROPIX
	 */
	bp = sofix_nm;

	if (dp->d_reser0[0] == 0x9E)
		{ rp = dp->d_reser0 + 1; end_rp = rp + 10 - 1; }

	while (rp < end_rp && (c = *rp++) != 0x9E)	/* Antes do nome */
		*bp++ = tolower (c);

	for (np = dp->d_name, end_np = np + 8; np < end_np && *np != ' '; /* abaixo */)
		{ c = *np++; *bp++ = tolower (c); }

	while (rp < end_rp && (c = *rp++) != 0x9E)	/* Final do nome */
		*bp++ = tolower (c);

	if (dp->d_ext[0] == ' ') 
		{ *bp = '\0'; goto do_dos_nm; }

	*bp++ = '.';

	for (np = dp->d_ext, end_np = np + 3; np < end_np && *np != ' '; /* abaixo */)
		{ c = *np++; *bp++ = tolower (c); }

	while (rp < end_rp && (c = *rp++) != '\0')	/* Final da extensão */
		*bp++ = tolower (c);

	*bp = '\0';

	/*
	 *	Compõe o nome (possivelmente truncado) DOS
	 */
    do_dos_nm:
	sofix_nm[IDSZ] = '\0';

	if (dos_nm_p == (const char **)NULL)
		return (sofix_nm);

	if (rp == NOSTR)
		{ *dos_nm_p = NOSTR; return (sofix_nm); }

	*dos_nm_p = bp = dos_nm;

	for (np = dp->d_name, end_np = np + 8; np < end_np && *np != ' '; /* abaixo */)
		{ c = *np++; *bp++ = tolower (c); }

	if (dp->d_ext[0] == ' ') 
		{ *bp = '\0'; return (sofix_nm); }

	*bp++ = '.';

	for (np = dp->d_ext, end_np = np + 3; np < end_np && *np != ' '; /* abaixo */)
		{ c = *np++; *bp++ = tolower (c); }

	*bp = '\0';

	return (sofix_nm);

}	/* end get_dos_file_nm */

/*
 ****************************************************************
 *	Verifica se o nome estilo UNIX é de agrado do DOS	*
 ****************************************************************
 */
const char *
dos_file_nm_can (const char *nm, DOSDIR *dp)
{
	int		dot_sz, base_sz, ext_sz;
	const char	*dot_place, *base_end;
	static char	new_nm[16];
#if (0)	/*******************************************************/
	static char	buf[32];
	int		new_nm_given = 0;
#endif	/*******************************************************/

	if (strchr (nm, '/'))
	{
		error ("dos_file_nm_can: Nome \"%s\" é um caminho", nm);
		return (nm);
	}

	memset (dp->d_reser0, '\0', sizeof (dp->d_reser0));

	for (EVER)
	{
		memset (new_nm, '\0', sizeof (new_nm));

		dot_sz = 0;

		if   ((dot_place = strchr (nm, '.')) == NOSTR)
		{
		   /***	dot_sz = 0; ***/	/* Estilo "alpha" */
			base_end = strend (nm);
			ext_sz = 0;
			dot_place = "";
		}
		elif (dot_place == nm)		/* Estilo ".new" */
		{
			dot_sz = 1;

			if ((dot_place = strrchr (nm + 1, '.')) == NOSTR)
			{
				base_end = strend (nm);
				ext_sz = 0;
				dot_place = "";
			}
			else			/* Estilo ".new.new" */
			{
				base_end = dot_place;
				ext_sz = strend (nm) - dot_place - 1;
			}
		}
		else				/* Estilo alpha.c */
		{
		   /***	dot_sz = 0; ***/
			base_end = dot_place;
			ext_sz = strend (nm) - dot_place - 1;
		}

		base_sz = base_end - nm - dot_sz;

		/*
		 *	Verifica se está dentro dos limites
		 */
		if (dot_sz == 0 && base_sz <= 8 && ext_sz <= 3)
		{
			put_dos_file_nm (nm, dp);
			return (nm);
		}

		/*
		 *	Não coube; tenta truncar
		 */
		if (base_sz <= 8)
			strncpy (new_nm, nm + dot_sz, base_sz);
		else
			strncpy (new_nm, nm + dot_sz, 8);

		strncat (new_nm, dot_place, 4);

#if (0)	/*******************************************************/
		/*
		 *	Verifica se o usuário concorda
		 */
		fprintf
		(	stderr,
			"%s: O nome \"%s\" não é próprio para o DOS\n"
			"%s: Será truncado para \"%s\" - Concorda? (s): ",
			cmd_nm, nm, cmd_nm, new_nm
		);

		if (askyesno () >= 0)
		{
#endif 	/*******************************************************/
			put_dos_file_nm (new_nm, dp);

#if (0)	/*******************************************************/
			if
			(	new_nm_given ||
				dot_sz + base_sz - 8 + ext_sz - 3 > 10 - 4
			)
#endif 	/*******************************************************/
				return (new_nm);

#if (0)	/*******************************************************/
			dp->d_reser0[0] = 0x9E;	/* No. mágico */

			if (dot_sz > 0)
				dp->d_reser0[1] = '.';

			strcat (dp->d_reser0, "\x9E");

			if (base_sz > 8)
				strncat (dp->d_reser0, nm + dot_sz + 8, base_sz - 8);

			strcat (dp->d_reser0, "\x9E");

			if (ext_sz > 3)
				strcat (dp->d_reser0, dot_place + 3 + 1);

#if (0)	/*************************************/
			strupr (dp->d_reser0);
#endif	/*************************************/

			return (new_nm);
		}

		/*
		 *	Lê um novo nome do teclado
		 */
		for (EVER)
		{
			do
			{
				printf ("%s: Dê um nome alternativo: ", cmd_nm);
			}
			while (gets (buf) == NOSTR);

			new_nm_given++;

			if (strchr (buf, '/') == NOSTR)
				break;

			printf ("%s: O nome não pode conter \"/\"\n", cmd_nm);
		}

		nm = buf;
#endif 	/*******************************************************/

	}	/* end for (EVER) */

}	/* end dos_file_nm_can */

/*
 ****************************************************************
 *	Converte o nome de um arquivo de UNIX para DOS		*
 ****************************************************************
 */
void
put_dos_file_nm (const char *nm, DOSDIR *dp)
{
	char		*bp, *end_bp;
	const char	*cp;
	int		c;

	/*
	 *	Inicializa
	 */
	memset (dp->d_name, ' ', sizeof (dp->d_name));
	memset (dp->d_ext,  ' ', sizeof (dp->d_ext));

	/*
	 *	Processa os casos particulares "." e ".."
	 */
	if (streq (nm, "."))
		{ dp->d_name[0] = '.'; return; }

	if (streq (nm, ".."))
		{ dp->d_name[0] = '.'; dp->d_name[1] = '.'; return; }

	/*
	 *	Processa o nome
	 */
	for
	(	cp = nm, bp = dp->d_name, end_bp = bp + sizeof (dp->d_name);
		/* abaixo */;
		/* abaixo */
	)
	{
		if ((c = *cp++) == '\0')
			return;

		if (c == '.')
			{ break; }

		if (bp >= end_bp)
			{ error ("Nome canônico (%s) inválido", nm); return; }

		*bp++ = toupper (c);
	}

	/*
	 *	Processa a extensão
	 */
	for
	(	bp = dp->d_ext, end_bp = bp + sizeof (dp->d_ext);
		/* abaixo */;
		/* abaixo */
	)
	{
		if ((c = *cp++) == '\0')
			return;

		if (bp >= end_bp)
			{ error ("Nome canônico (%s) inválido", nm); return; }

		*bp++ = toupper (c);
	}

}	/* end put_dos_file_nm */

/*
 ****************************************************************
 *	Copia um nome DOS					*
 ****************************************************************
 */
void
copy_dos_file_nm (DOSDIR *dst_dp, const DOSDIR *src_dp)
{
	memmove (dst_dp->d_name,   src_dp->d_name,   sizeof (src_dp->d_name));
	memmove (dst_dp->d_ext,    src_dp->d_ext,    sizeof (src_dp->d_ext));
	memmove (dst_dp->d_reser0, src_dp->d_reser0, sizeof (src_dp->d_reser0));

}	/* end copy_dos_file_nm */
#endif 	/*******************************************************/
