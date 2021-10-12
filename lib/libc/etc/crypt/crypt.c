/*
 ****************************************************************
 *								*
 *			crypt.c					*
 *								*
 *	Rotina para gerar a senha encryptada			*
 *								*
 *	Versão	1.0.0, de 15.09.86				*
 *		3.2.3, de 05.01.00				*
 *								*
 *	Módulo: crypt						*
 *		libc/etc					*
 *		Categoria B					*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 2000 NCE/UFRJ - tecle "man licença"	*
 * 								*
 ****************************************************************
 */

#include <stdlib.h>

/*
 ****************************************************************
 *	Definições básicas					*
 ****************************************************************
 */
#define	elif	else if

#define REPET	113
#define MULT	3183098861
#define INCR	907633385

/*
 ******	Variáveis globais ***************************************
 */
static	char	cryptpw[14];	/* O resultado */

/*
 ******	Protótipos de funções ***********************************
 */
static void		gettrans (short [], long);
static char		getcsalt (int);

/*
 ****************************************************************
 *	Rotina de encriptação da senha				*
 ****************************************************************
 */
char *
crypt (const char *key, const char *salt)
{
	register int		c;
	register int		i, j ,k;
	char			pw[16];
	long			n[8];
	char			ent[66], sai[66];
	short			t0[66], t1[66], t2[66], t3[66], t4[66], t5[66], t6[66];

	cryptpw[0] = *salt++;
	cryptpw[1] = *salt;

	pw[4]  = *salt--;
	pw[10] = *salt;

	for (i = 0; i < 16; i++)
	{
		if ( (i == 4) || (i == 10))
			continue;

		if (*key == '\0')
			pw[i] = '\0';
		else
			pw[i] = *key++;
	}

	for (i = 0; i < 14; i += 2)
		n[i>>1] = (((((pw[i] << 8) | pw[i+1]) << 8) | pw[i+2]) << 8) | pw[i+3];

#undef	DEBUG
#ifdef	DEBUG
	printf
	(	"Ns = %P %P %P %P %P %P %P %P\n",
		n[0], n[1], n[2], n[3], n[4], n[5], n[6], n[7]
	);
#endif	DEBUG

	for (i = 0; i < 16; i++)
		pw[i] = '\0';

	for (i = 0, j = 33; i < 33; i++, j++)
		{ ent[i] = 0; ent[j] = 1; }

	gettrans (t0, n[0]);
	gettrans (t1, n[1]);
	gettrans (t2, n[2]);
	gettrans (t3, n[3]);
	gettrans (t4, n[4]);
	gettrans (t5, n[5]);
	gettrans (t6, n[6]);

	for (i = 0; i < 7; i++)
		n[i] = 0;

	for (i = 0; i < 66; i++)
		sai[i] = ent[t6[t5[t4[t3[t2[t1[t0[i]]]]]]]];

	for (i = 0; i < 66; i++)
	{
		t0[i] = 0;
		t1[i] = 0;
		t2[i] = 0;
		t3[i] = 0;
		t4[i] = 0;
		t5[i] = 0;
		t6[i] = 0;
	}

	k = 0;

	for (i = 2; i < 13; i++)
	{
		c = 0;

		for (j = 0; j < 6; j++)
		{
			c = c << 1;

			if (sai[k++] == 1)
				c = c | 1;

		}

		cryptpw[i] = getcsalt (c);

	}

	cryptpw[i] = '\0';

	if (cryptpw[0] == '\0' || cryptpw[1] == '\0')
		{ cryptpw[0] = '0'; cryptpw[1] = '0'; }

	return (cryptpw);
			
}	/* end crypt */

/*
 ****************************************************************
 *	Rotina geradora das transformações			*
 ****************************************************************
 */
static void
gettrans (short t[], long n)
{
	register int	i, k;
	register long	x;

	/*
	 *	Inicialização do gerador
	 */
	setrand (MULT, INCR, REPET);

	x = n;

	for (i = 0; i < 66; i++)
		t[i] = -1;

	for (i = 0; i < 66; i++)
	{
		x = getrand (x);
		k = (x >> 26) & 0x3F;

		while (t[k] != -1)
		{
			k++;

			if (k == 66)
				k = 0;
		}

		t[k] = i;
	}

}	/* end gettrans */

/*
 ****************************************************************
 *	Rotina que gera o carac. do salt a partir da ordem	*
 ****************************************************************
 */
static char
getcsalt (int o)
{
	if   (o < 12)
		return (o + '.');
	elif (o < 38)
		return (o - 12 + 'A');
	else
		return (o - 38 + 'a');

}	/* end getcsalt */
