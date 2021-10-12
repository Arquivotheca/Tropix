.bp 1
.he 'GETRAND (libc)'TROPIX: Manual de Referência'GETRAND (libc)'
.fo 'Atualizado em 19.06.95'Versão 3.0'Pag. %'

.b NOME
.in 5
Geração de números pseudo-aleatórios:
.sp
.wo "getrand -"
gera um número
.br
.wo "setrand -"
inicializa os parâmetros do gerador
.br

.in
.sp
.b SINTAXE
.in 5
.(l
#include <stdlib.h>

unsigned long getrand (unsigned long num);
void setrand (unsigned long mult, unsigned long incr, int iter);
.)l

.in
.sp
.b DESCRIÇÃO
.in 5
A função "getrand" gera números pseudo-aleatórios entre 0 e 4294967295
.wo "(2 ** 32 - 1)."
A cada chamada de "getrand" deve ser fornecido como argumento o
número pseudo-aleatório obtido na última chamada. Na primeira
vez pode ser dado um número qualquer (como por exemplo o valor "1").

.sp
A função utiliza o conhecido método congruencial linear,
que utiliza 3 parametros: o multiplicador, o incremento e
o número de iterações.
Normalmente, os valores utilizados são "314159269" para o multiplicador,
"453806245" para o incremento e "1" para o número de iterações.

.sp
Estes parametros podem ser alterados através da função "setrand".
Os seus 3 argumentos correspondem aos 3 parametros do método:
"mult" para o multiplicador, "incr" para o incremento e "iter" para o
número de iterações.

.in
.sp
.b
VALOR DE RETORNO
.r
.in 5
A cada chamada, "getrand" retorna o próximo número pseudo-aleatório.

.in
.sp
.b OBSERVAÇÃO
.in 5
Estas funções fazem a multiplicação, soma, truncamento
e controle do número das iterações em tempo mínimo.

.in
.sp
.b
VEJA TAMBÉM
.r
.in 5
.wo "(cmd): "
crypt
.br
.wo "(libc):"
rand, crypt
.br

.(t
.in
.b EXEMPLO
.in 7
.sp
A função "rand" da "libc" é aproximadamente a seguinte:
.sp
.nf
	#define	MULT	314159269
	#define	INCR	453806245

	static long	seed;

	void
	srand (long a)
	{
		setrand (MULT, INCR, 1);
		seed = a;
	}

	long
	rand (void)
	{
		seed = getrand (seed);
		return (seed & 0x7FFFFFFF);
	}
.fi
.)t

.in
.sp
.b ESTADO
.in 5
Efetivo.

.in
