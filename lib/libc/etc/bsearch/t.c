#include <stdio.h>
#include <stdlib.h>

typedef struct tipo
{
	int	t_key, t_info;

}	TIPO;

entry TIPO	tipo[] =
{
	/* 0 */		3,	1003,
	/* 1 */		9,	1009,
	/* 2 */		15,	1015,
	/* 3 */		24,	1024,
	/* 4 */		32,	1032,
	/* 5 */		40,	1040,
	/* 6 */		54,	1054,
	/* 7 */		63,	1063
};

int compar (const int *, const TIPO *);

main (int argc, register char *argv[])
{
	int		key;
	register TIPO	*tp;

	for (key = -1; key < 70; key++)
	{
		tp = bsearch
			(	&key,
				tipo,
				sizeof (tipo) / sizeof (TIPO),
				sizeof (TIPO),
				compar
			);
	
		if (tp)
		{
			printf
			(	"Chave dada = %d, chave = %d, valor = %d, "
				"i = %d",
				key, tp->t_key, tp->t_info, tp-tipo
			);

			if (key != tp->t_key)
				printf ("******** ERRO \n");
			else
				printf ("\n");
		}
		else
		{
			printf ("Chave dada = %d, NÃO ENCONTRADA\n", key);
		}
	}
}

int
compar (const int *k, const TIPO *tp)
{
	if (*k == tp->t_key)
		return (0);
	if (*k < tp->t_key)
		return (-1);
	return (+1);
}
