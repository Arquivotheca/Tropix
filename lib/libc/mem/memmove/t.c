#include <stdio.h>
#include <string.h>

#define	N	12

char	bufa[N] = "";
char	buf[]  	 = "0123456789";
char	bufd[N] = "";

char	super1[] = "0123456789";
char	super2[] = "0123456789";
char	super3[] = "0123456789";
char	super4[] = "0123456789";

main ()
{
	register char	*cp;
	register	n;

	/*
	 *	Destino < Fonte
	 */
	printf ("Destino < Fonte\n");

	n = 8;
	strncpy (bufa, "", N);
	cp = memmove (bufa, buf, n);
	printf ("fonte = %P, destino = %P, copia = %P, n = %d: \"%s\"\n",
		buf, bufa, cp, n, cp );

	n = 8;
	strncpy (bufa, "", N);
	cp = memmove (bufa+1, buf, n);
	printf ("fonte = %P, destino = %P, copia = %P, n = %d: \"%s\"\n",
		buf, bufa+1, cp, n, cp );

	n = 8;
	strncpy (bufa, "", N);
	cp = memmove (bufa, buf+1, n);
	printf ("fonte = %P, destino = %P, copia = %P, n = %d: \"%s\"\n",
		buf+1, bufa, cp, n, cp );

	n = 8;
	strncpy (bufa, "", N);
	cp = memmove (bufa+1, buf+1, n);
	printf ("fonte = %P, destino = %P, copia = %P, n = %d: \"%s\"\n",
		buf+1, bufa+1, cp, n, cp );

	n = 7;
	strncpy (bufa, "", N);
	cp = memmove (bufa, buf, n);
	printf ("fonte = %P, destino = %P, copia = %P, n = %d: \"%s\"\n",
		buf, bufa, cp, n, cp );

	n = 7;
	strncpy (bufa, "", N);
	cp = memmove (bufa+1, buf, n);
	printf ("fonte = %P, destino = %P, copia = %P, n = %d: \"%s\"\n",
		buf, bufa+1, cp, n, cp );

	n = 7;
	strncpy (bufa, "", N);
	cp = memmove (bufa, buf+1, n);
	printf ("fonte = %P, destino = %P, copia = %P, n = %d: \"%s\"\n",
		buf+1, bufa, cp, n, cp );

	n = 7;
	strncpy (bufa, "", N);
	cp = memmove (bufa+1, buf+1, n);
	printf ("fonte = %P, destino = %P, copia = %P, n = %d: \"%s\"\n",
		buf+1, bufa+1, cp, n, cp );

	/*
	 *	Destino > Fonte
	 */
	printf ("\nDestino > Fonte\n");

	n = 8;
	strncpy (bufd, "", N);
	cp = memmove (bufd, buf, n);
	printf ("fonte = %P, destino = %P, copia = %P, n = %d: \"%s\"\n",
		buf, bufd, cp, n, cp );

	n = 8;
	strncpy (bufd, "", N);
	cp = memmove (bufd+1, buf, n);
	printf ("fonte = %P, destino = %P, copia = %P, n = %d: \"%s\"\n",
		buf, bufd+1, cp, n, cp );

	n = 8;
	strncpy (bufd, "", N);
	cp = memmove (bufd, buf+1, n);
	printf ("fonte = %P, destino = %P, copia = %P, n = %d: \"%s\"\n",
		buf+1, bufd, cp, n, cp );

	n = 8;
	strncpy (bufd, "", N);
	cp = memmove (bufd+1, buf+1, n);
	printf ("fonte = %P, destino = %P, copia = %P, n = %d: \"%s\"\n",
		buf+1, bufd+1, cp, n, cp );

	n = 7;
	strncpy (bufd, "", N);
	cp = memmove (bufd, buf, n);
	printf ("fonte = %P, destino = %P, copia = %P, n = %d: \"%s\"\n",
		buf, bufd, cp, n, cp );

	n = 7;
	strncpy (bufd, "", N);
	cp = memmove (bufd+1, buf, n);
	printf ("fonte = %P, destino = %P, copia = %P, n = %d: \"%s\"\n",
		buf, bufd+1, cp, n, cp );

	n = 7;
	strncpy (bufd, "", N);
	cp = memmove (bufd, buf+1, n);
	printf ("fonte = %P, destino = %P, copia = %P, n = %d: \"%s\"\n",
		buf+1, bufd, cp, n, cp );

	n = 7;
	strncpy (bufd, "", N);
	cp = memmove (bufd+1, buf+1, n);
	printf ("fonte = %P, destino = %P, copia = %P, n = %d: \"%s\"\n",
		buf+1, bufd+1, cp, n, cp );

	/*
	 *	Superposicao
	 */
	printf ("\nSuperposicao\n");

	n = 7;
	cp = memmove (super1, super1+1, n);
	printf ("fonte = %P, destino = %P, copia = %P, n = %d: \"%s\"\n",
		super1+1, super1, cp, n, super1 );

	n = 7;
	cp = memmove (super2+1, super2, n);
	printf ("fonte = %P, destino = %P, copia = %P, n = %d: \"%s\"\n",
		super2, super2+1, cp, n, super2 );

	n = 8;
	cp = memmove (super3, super3+2, n);
	printf ("fonte = %P, destino = %P, copia = %P, n = %d: \"%s\"\n",
		super3+2, super3, cp, n, super3 );

	n = 8;
	cp = memmove (super4+2, super4, n);
	printf ("fonte = %P, destino = %P, copia = %P, n = %d: \"%s\"\n",
		super4, super4+2, cp, n, super4 );

}
