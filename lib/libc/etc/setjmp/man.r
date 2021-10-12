.bp 1
.he 'SETJMP (libc)'TROPIX: Manual de Referência'SETJMP (libc)'
.fo 'Atualizado em 26.06.95'Versão 3.0'Pag. %'

.b NOME
.in 5
Desvios não-locais:
.sp
.wo "setjmp  -"
prepara o desvio
.br
.wo "longjmp -"
executa o desvio
.br

.in
.sp
.b SINTAXE
.in 5
.(l
#include <setjmp.h>

int	setjmp (jmp_buf env);
void	longjmp (jmp_buf env, int val);
.)l

.in
.sp
.b DESCRIÇÃO
.in 5
Estas funções são utilizadas para o tratamento de erros e interrupções
em funções de baixo nível de programas. 

.sp
A função "setjmp" guarda os valores dos registros e o estado da
pilha de execução em "env" para posterior uso de "longjmp".
Se o retorno é de uma chamada direta a "setjmp", ela retorna o
valor zero. Se o retorno é de uma chamada à função "longjmp"
(ver adiante), a função "setjmp" retorna um valor não-nulo.

.sp
A função "longjmp" restaura os valores dos registros e o estado da
pilha de execução, que foram salvos pela última chamada a "setjmp" com o
mesmo argumento "env".
Se não houve esta chamada a "setjmp", ou se a função contendo a chamada
a "setjmp" já retornou neste interim, o resultado será imprevisível.
Após a execução de "longjmp", o programa
continua como se o "setjmp" correspondente tivesse retornado o valor "val".

.sp
A função "longjmp" não pode causar "setjmp" retornar zero.
Se "longjmp" for chamado com "val" igual a zero, "setjmp" retornará o valor um.

.in
.sp
.b OBSERVAÇÕES
.in 5
No instante do segundo retorno de "setjmp", todas as variáveis terão os
valores do momento em que "longjmp" foi chamada, exceto as variáveis
automáticas alocadas em registros. Estas variáveis terão os valores
do momento em que "setjmp" foi chamada (veja o exemplo).

.sp
Um modo de impedir que uma variável automática seja alocada em um
registro é através do uso do atributo "volatile".
Repare que o compilador não garante que variáveis com o atributo
"register" seja efetivamente alocadas em registros.

.in
.sp
.b
VEJA TAMBÉM
.r
.nf
     (sys): signal
.fi

.in
.(t
.sp
.b EXEMPLO
.br
.in 5
Estamos supondo que a presença/ausência do atributo "register"
seja levada em conta:
.sp
.nf
	#include <setjmp.h>

	jmp_buf		env;

	int		g = 0;

	main (...)
	{
		int		s = 0;
		register int 	r = 0;

		if (setjmp (env) != 0)
		{
			printf
			(	"Segundo retorno, "
				"g = \%d, s = \%d, r = \%d\n",
				g, s, r
			);
			exit (0);
		}

		printf
		(	"Primeiro retorno, "
			"g = \%d, s = \%d, r = \%d\n",
			g, s, r
		);

		g = s = r = 1;

		sub ();

	}	/* end main */

	sub ()
	{
		longjmp (env, 1);

	}	/* end sub */


   O programa irá escrever:


	Primeiro retorno, g = 0, s = 0, r = 0
	Segundo retorno,  g = 1, s = 1, r = 0
.fi
.)t


.in
.sp
.b ESTADO
.in 5
Efetivo.

.in
