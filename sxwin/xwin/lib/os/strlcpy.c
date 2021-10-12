/*
 ****************************************************************
 *								*
 *			strlcpy.c				*
 *								*
 *	Compara cadeias, desprezando maiúsculas/minúsculas	*
 *								*
 *	Versão	1.0.0, de 16.06.97				*
 *		1.0.1, de 28.03.98				*
 *								*
 *	Módulo: xc/lib/aux					*
 *		X Windows - Bibiotecas				*
 *		Baseado no software homônimo do XFree86		*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright (c) 1998 TROPIX Technologies Ltd.	*
 * 								*
 ****************************************************************
 */

#include <string.h>

/*
 * Copy src to string dst of size siz.  At most siz-1 characters
 * will be copied.  Always NUL terminates (unless siz == 0).
 * Returns strlen(src); if retval >= siz, truncation occurred.
 */
int
strlcpy (char *dst, const char *src, int siz)
{
	char		*d = dst;
	const char	*s = src;
	int		n = siz;

	/* Copy as many bytes as will fit */

	if (n != 0 && --n != 0)
	{
		do
		{
			if ((*d++ = *s++) == '\0')
				break;
		}
		while (--n != 0);
	}

	/* Not enough room in dst, add NUL and traverse rest of src */

	if (n == 0)
	{
		if (siz != 0)
			*d = '\0';		/* NUL-terminate dst */

		while (*s++)
			/* vazio */;
	}

	return (s - src - 1);	/* count does not include NUL */

}	/* end strlcpy */
