.bp
.he 'VA_LIST (libc)'TROPIX: Manual de Referência'VA_LIST (libc)'
.fo 'Atualizado em 18.07.95'Versão 3.0'Pag. %'

.b NOME
.in 5
lista variável de argumentos:
.sp
.wo "va_start -"
inicializa a lista de argumentos
.br
.wo "va_arg   -"
obtém o argumento seguinte
.br
.wo "va_end   -"
encerra a utilização da lista de argumentos
.br
.wo "va_first -"
obtém o endereço do primeiro argumento
.br

.in
.sp
.b SINTAXE
.in 5
.(l
#include <stdarg.h>

void	va_start (va_list ap, parmN);
type	va_arg (va_list ap, type);
void	va_end (va_list ap);
void	*va_first (parmN);
.)l

.in
.sp
.b DESCRIÇÃO
.in 5
As macros "va_start", "va_arg", "va_end" e "va_first"
(definidas em <stdarg.h>) possibilitam a elaboração de uma
função com um número e tipo variável de parâmetros.

.sp
Uma função deste tipo tem "N" parâmetros antes das reticências ("..."),
que indica o começo da parte variável. O último parâmetro tem um
papel importante no mecanismo de acesso aos argumentos, e será chamado
de "parmN" neste descrição.

.sp
A macro "va_start" deve ser invocada antes do acesso aos argumentos
da parte variável. Ela inicializa "ap" para o uso subseqüênte por
"va_arg" e "va_end".

.sp
A macro "va_arg" expande em uma expressão que tem o tipo e o valor
do próximo argumento na chamada.
Cada invocação de "va_arg" modifica "ap" de tal modo, que os sucessivos
argumentos são devolvidos um a um.

.sp
A macro "va_end" executa operações para possibilitar o retorno normal
da função que utilizou as macros anteriores.

.sp
Em certos casos, necessitamos apenas o endereço do primeiro argumento
após "parmN", que é fornecido pela macro "va_first". Neste caso,
não precisamos utilizar as macros anteriores.

.in
.sp
.b OBSERVAÇÕES
.in 5
A macro "va_arg" supõe que possa ser criado um tipo que é o ponteiro
para o "tipo" dado, apenas acrescentando-se o caractere "*" ao final.
Isto é válido para muitos tipos simples (tal como "int" em "int *"),
mas não no caso geral. 

.in
.sp
.b
VEJA TAMBÉM
.r
.in 5
.wo "(libc):"
vprintf
.br

.in
.sp
.(t
.b EXEMPLO
.in 5
A função "f1" reúne em um vetor uma lista de argumentos que são
ponteiros para cadeias, e em seguida passa o vetor como um único
argumento para  função "f2". O número de ponteiros é dado como o
primeiro argumento de "f1". 
.(l

	#include <stdarg.h>

	#define	MAXARGS 31

	void f1 (int n_ptrs, ...)
	{
		register va_list ap;
		register int	ptr_no = 0;
		char		*array[MAXARGS];

		if (n_ptrs > MAXARGS)
			n_ptrs = MAXARGS;

		/* Inicializa "ap" */

		va_start (ap, n_ptrs);

		/* Copia os argumentos no vetor */

		while (ptr_no < n_ptrs)
			array[ptr_no++] = va_arg (ap, char *);

		/* Encerra a operação de acesso aos argumentos */ 

		va_end (ap);

		f2 (n_ptrs, array);
	} 
.)l
.)t

.in
.sp
.b ESTADO
.in 5
Efetivo.

.in
