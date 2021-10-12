#include "./pcntl.h"

main ()
{

	int	pri, npri;
	int i;
	char	rtflag;

	printf ("Tipo original = %d\n", pcntl (P_SETRTPROC, 0));

	printf ("Real Time s ou n? ");
	scanf ("%c", &rtflag);

	if (rtflag == 's')
	{
		printf ("Real Time\n");
		pcntl (P_SETRTPROC, 1);
	}
	else
	{
		printf ("Normal\n");
		pcntl (P_SETRTPROC, -1);
	}

	printf ("Tipo modificado = %d\n", pcntl (P_SETRTPROC, 0));

	pri = pcntl (P_GETPRI);
	printf ("Prioridade original = %d\n", pri);
	printf ("Forçando prioridade 550\n");
	pcntl (P_SETPRI, 550);
	pri = pcntl (P_GETPRI);
	printf ("Prioridade alterada = %d\n", pri);

	for (i = 0; i < 10000; i++)
	pri = pcntl (P_GETPRI);
	printf ("Prioridade alterada depois do loop 1 = %d\n", pri);

	for (i = 0; i < 10000; i++)
	pri = pcntl (P_GETPRI);
	printf ("Prioridade alterada depois do loop 2 = %d\n", pri);

	sleep (2);
	pri = pcntl (P_GETPRI);
	printf ("Prioridade alterada depois do sleep 1 = %d\n", pri);

	sleep (2);
	pri = pcntl (P_GETPRI);
	printf ("Prioridade alterada depois do sleep 2 = %d\n", pri);

	pri = pcntl (P_GETPRI);
	printf ("Prioridade original = %d\n", pri);
	printf ("Forçando prioridade 1910\n");
	pcntl (P_SETPRI, 1910);
	pri = pcntl (P_GETPRI);
	printf ("Prioridade alterada = %d\n", pri);

	for (i = 0; i < 10000; i++)
	pri = pcntl (P_GETPRI);
	printf ("Prioridade alterada depois do loop 1 = %d\n", pri);

	for (i = 0; i < 10000; i++)
	pri = pcntl (P_GETPRI);
	printf ("Prioridade alterada depois do loop 2 = %d\n", pri);

	sleep (2);
	pri = pcntl (P_GETPRI);
	printf ("Prioridade alterada depois do sleep 1 = %d\n", pri);

	sleep (2);
	pri = pcntl (P_GETPRI);
	printf ("Prioridade alterada depois do sleep 2 = %d\n", pri);

	printf ("Tecle uma tecla\n");
	do
		scanf ("%c", &rtflag);
	while (rtflag == '\n');
	pri = pcntl (P_GETPRI);
	printf ("Prioridade alterada depois do input 1 = %d\n", pri);

	printf ("Tecle uma tecla\n");
	do
		scanf ("%c", &rtflag);
	while (rtflag == '\n');
	pri = pcntl (P_GETPRI);
	printf ("Prioridade alterada depois do input 2 = %d\n", pri);

	printf ("\nRetorno ao tipo normal\n");
	pcntl (P_SETRTPROC, -1);
	pri = pcntl (P_GETPRI);
	printf ("Prioridade original = %d\n", pri);
	printf ("Forçando prioridade 550\n");
	pcntl (P_SETPRI, 550);
	pri = pcntl (P_GETPRI);
	printf ("Prioridade alterada = %d\n", pri);

	for (i = 0; i < 10000; i++)
	pri = pcntl (P_GETPRI);
	printf ("Prioridade alterada depois do loop 1 = %d\n", pri);

	for (i = 0; i < 10000; i++)
	pri = pcntl (P_GETPRI);
	printf ("Prioridade alterada depois do loop 2 = %d\n", pri);

	sleep (2);
	pri = pcntl (P_GETPRI);
	printf ("Prioridade alterada depois do sleep 1 = %d\n", pri);

	sleep (2);
	pri = pcntl (P_GETPRI);
	printf ("Prioridade alterada depois do sleep 2 = %d\n", pri);
}
