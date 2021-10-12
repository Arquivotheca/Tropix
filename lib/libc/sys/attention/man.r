.bp
.he 'ATTENTION (sys)'TROPIX: Manual de Referência'ATTENTION (sys)'
.fo 'Atualizado em 16.04.02'Versão 4.2.0'Pag. %'

.b NOME
.in 5
.wo "attention -"
multiplexação de entrada síncrona
.br

.in
.sp
.b SINTAXE
.in 5
.(l
#include <sys/syscall.h>

int	attention (int n, const int vector[],
					int index, int timeout);
extern int	_attention_index;
.)l

.in
.sp
.b DESCRIÇÃO
.in 5
A chamada ao sistema "attention" examina os "n" descritores de arquivos
do vetor "vector" para ver se algum deles tem dados disponíveis
(e portanto está pronto para ser lido).
Cada descritor é obtido por uma das chamadas "creat", "open",
"dup", "fcntl", "pipe" (sys) ou t_open (xti).

.sp
O vetor "vector" dado deve conter "n" descritores
(cujos índices são "0" a "n - 1").
Os descritores são examinados ascendentemente e circularmente
a partir do descritor SEGUINTE ao de índice "index".
O valor de "index" dado deve ser de "0" a "n - 1",
ou ainda "-1" (que é equivalente a "n - 1").

.sp
Se no momento da chamada ao sistema um ou mais dos descritores já tiver dados
disponíveis, "attention" retorna imediatamente com o índice do primeiro
descritor com dados disponíveis
(encontrado de acordo com a busca circular descrita acima).
A variável "_attention_index" retorna o mesmo índice, exceto nos casos
de erro (veja abaixo em VALOR DE RETORNO).

.sp
Em caso contrário (isto é, se no momento da chamada ao sistema NENHUM
dos descritores tiver dados disponíveis), a ação de "attention" depende
do valor de "timeout", de acordo com os seguintes casos:

.in +3
.ip 1. 3
"timeout" < 0 ("polling"): a chamada ao sistema retorna imediatamente
com o valor "-1", indicando o erro "EINTR" em "errno";

.ip 2. 3
"timeout" == 0 ("espera ilimitada"): a chamada ao sistema retorna
apenas quando um dos descritores passar a ter dados disponíveis,
e retorna o seu índice;

.ip 3. 3
"timeout" > 0 ("espera limitada"): a chamada ao sistema
espera no máximo "timeout" segundos - se durante este intervalo de tempo
algum dos descritores passar a ter dados disponíveis, retorna o seu índice.
Em caso contrário retorna o valor "-1" indicando o erro "EINTR".

.ep
.in -3

.sp
De acordo com os valores de "index" fornecidos, podemos construir várias
estrategias de prioridades.
Assim, por exemplo, se o valor de "index" for:

.in +3
.ip 1. 3
o valor de retorno da última chamada a "attention", daremos prioridades
iguais aos dispositivos, pois a busca será iniciada a partir do
descritor seguinte ao que acabou de ser lido;

.ip 2. 3
sempre "-1", daremos prioridade máxima para o primeiro descritor do vetor,
com prioridades descrescendo à medida que caminhamos para o seu final.

.ep
.in -3

.in
.sp
.b
VALOR DE RETORNO
.r
.in 5
Em caso de dados disponíveis, a chamada ao sistema "attention" devolve
o índice "i" do vetor (de "0" a "n - 1"), de tal modo que "vector[i]"
contém o descritor de arquivos com dados disponíveis.
Este índice também é atribuído à variável "_attention_index".

.sp
Em caso de um erro associado a um certo descritor de arquivos,
"attention" devolve o valor "-1" e atribui à variável "_attention_index"
o índice do vetor contendo o descritor no qual ocorreu o erro.
Em "errno" é indicada a causa do erro.

.sp
Em caso de erros que não são associados a nenhum descritor de arquivos em
particular, "attention" devolve o valor "-1" e também
atribui este valor à variável "_attention_index".
Como é usual, em "errno" é indicada a causa do erro.

.sp
No caso de dados não disponíveis imediatamente ("polling") ou após o tempo
esgotado ("espera limitada"), "attention" devolve o valor "-1"
(e atribui este valor à variável "_attention_index")
indicando o erro "EINTR".

.in
.sp
.b OBSERVAÇÕES
.in 5
A chamada ao sistema "attention" NÃO está disponível para todos os dispositivos.
No momento, só está implementada para terminais (linhas de comunicações,
dispositivos "/dev/console", "/dev/tty*", "/dev/video*"),
pseudo-terminais (tanto do lado do cliente como do servidor, 
dispositivos "/dev/ptyc*" e "/dev/ptys*"), na internet (dispositivos
"/dev/udp", "/dev/tcp", "/dev/raw"), arquivos "pipe"
e na espera por um processo filho
terminado (dispositivo "/dev/grave").

.in
.sp
.b
VEJA TAMBÉM
.r
.in 5
.wo "(sys): "
select, open, read, wait, pipe
.br
.wo "(fmt): "
termio
.br
.wo "(xti): "
t_open
.br

.in
.sp
.(t
.b EXEMPLO
.in 5
O exemplo abaixo implementa prioridades iguais, e espera indefinida:
.nf

	.................

	#define	N	...	/* Número de dispositivos */

	main (....)
	{
		int		fd_vec[N];
		int		fd;
		int		i;
		int		r;

		i = -1;		/* Para o primeiro "attention" */
		...........................

		if ((fd_vec[0] = open (disp0, O_RDONLY)) < 0)
			......................

		if ((fd_vec[1] = open (disp1, O_RDONLY)) < 0)
			......................

		...........................

		for (;;)
		{
			if ((i = attention (N, fd_vec, i, 0)) < 0)
			{
				......................
				fd = fd_vec[_attention_index];
				......................
			}

			fd = fd_vec[i];

			if ((r = read (fd, ..., ...)) < 0)
				......................
		}
	}
.fi
.)t


.in
.sp
.b ESTADO
.in 5
Efetivo.

.in
