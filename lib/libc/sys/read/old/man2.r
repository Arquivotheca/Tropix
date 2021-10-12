.bp
.he 'SIGREAD (sys)'TROPIX: Manual de Referência'SIGREAD (sys)'
.fo 'Escrito em 25.09.90'Versão 2.3'Pag. %'

.b NOME
.in 5
sigread - leitura assíncrona
.sp
.br

.in
.sp
.b DESCRIÇÃO
.in 5
Através do sinal SIGREAD (veja "signal" (sys)) é possível
ler assincronamente de várias linhas de comunicação simultaneamente.

.sp
Isto é feito através dos seguintes passos (acompanhe no exemplo):

.in +3
.ip 1. 3
O programa usa a chamada ao sistema "signal" para indicar uma função
interceptadora para o sinal SIGREAD.

.ip 2. 3
Em seguida, ele abre as linhas com o indicador "O_SIGREAD" (veja "open" (sys)).

.ip 3. 3
O programa pode agora esperar através de "pause" (sys),
"sleep" (libc) ou executar outras tarefas.

.ip 4. 3
Assim que houverem dados disponíveis em uma ou mais linhas,
é enviado o sinal SIGREAD para o programa, quando então
será chamada a função interceptadora com dois argumentos.
O primeiro argumento é o próprio número do sinal SIGREAD
e o segundo é o descritor de arquivos referente à linha de comunicações
com os dados disponíveis.

.ip 5. 3
A função inteceptadora lê a linha (cujo descritor de arquivos foi dado).

.ip 6. 3
A função interceptadora, antes de retornar, deve reinicializar a
interceptação através de um novo "signal".

.ip 7. 3
Após o sinal, o programa retorna ao ponto onde estava; 
se estava suspenso em virtude de uma chamada ao sistema "pause" ou "sleep",
ele retorna desta chamada.

.ep
.in -3

.in
.sp
.b OBSERVAÇÕES
.in 5
Mesmo que várias linhas recebam dados simultaneamente,  
a função interceptadora será chamada uma vez para cada linha com
dados disponíveis.

.sp
Uma vez aberta uma linha com o indicador "O_SIGREAD", será enviado
um sinal toda vez que ela receber novos dados.
Se isto não for mais desejado, pode-se usar "signal" para ignorar
o sinal, ou então fechar a linha.
Lembre-se que um sinal não ignorado nem interceptado causa o término
do programa.

.in
.sp
.b
VEJA TAMBÉM
.r
.in 5
.wo "(sys): "
open, read, signal, pause
.br
.wo "(libc):"
sleep
.br

.in
.sp
.(t
.b EXEMPLO
.nf
	#include <signal.h>
	#include <fcntl.h>
	
	void	readfunction ();
	
	main (....)
	{
		int	fd1, fd2, ...;
	
		...........................

		signal (SIGREAD, readfunction);
	
		if ((fd1 = open (linha1, O_RDONLY|O_SIGREAD)) < 0)
			......................
	
		if ((fd2 = open (linha2, O_RDONLY|O_SIGREAD)) < 0)
			......................
	
		...........................
	
		for (;;)
		{
			pause ();
		}
	}
	
	void
	readfunction (signo, fd)
	int signo, fd;
	{
		char	area[128];
		int	n;
	
		if ((n = read (fd, area, 128)) < 0)
			......................
	
		printf ("Mensagem da linha do descritor \%d:", fd);
		write (1, area, n);
	
		signal (SIGREAD, readfunction);
	}
.fi
.)t

.in
.sp
.b ESTADO
.in 5
Efetivo.

.in
