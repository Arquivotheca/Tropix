.bp
.he 'ALLOCA (libc)'TROPIX: Manual de Referência'ALLOCA (libc)'
.fo 'Atualizado em 05.07.95'Versão 3.0'Pag. %'

.b NOME
.in 5
.wo "alloca  -"
alocação de memória na pilha de execução do processo
.br

.in
.sp
.b SINTAXE
.in 5
.(l
#include <stdlib.h>

void	*alloca (size_t size);
.)l

.in
.sp
.b DESCRIÇÃO
.in 5
A função "alloca" retorna um ponteiro para uma área de pelo menos
"size" bytes, já adequadamente alinhada para qualquer uso.
A área é alocada na pilha de execução do processo, e é automaticamente
desalocada quando a função que chamou "alloca" retorna.
Esta área é funcionalmente equivalente a uma variável
local (automática) da função que chamou "alloca". 

.sp
Este modo de alocação de memória (na pilha de execução do processo)
é diferente do usado nas diversas funções
da série "malloc" (libc), que obtém memória
na área de dados do processo através da chamada ao sistema "sbrk" (sys).

.sp
A alocação de memória através de "alloca" é extremamente rápida,
e como a área é automaticamente
desalocada quando a função que chamou "alloca" retorna,
não é necessária uma função do tipo "free" (libc).
Dependendo da aplicação, esta desalocação automática pode 
ser vantajosa: não há necessidade de se preocupar com a desalocação -
ou desvantajosa: não é possível colocar um resultado na área alocada
para a utilização global por todas as outras funções
(pois a área alocada só é disponível para a função
que chamou "alloca" e as funções chamadas por ela).

.in
.sp
.b DIAGNÓSTICOS
.in 5
Se não houver espaço disponível na pilha de execução para a área solicitada,
o processo é cancelado ainda durante a execução da função "alloca".

.in
.sp
.b OBSERVAÇÕES
.in 5
A função "alloca" JAMAIS deve ser utilizada em um
argumento de outra função, pois neste caso ela altera os ponteiros da
pilha de execução justamente durante o empilhamento
dos argumentos. No exemplo abaixo

.sp
.nf
		ret = ftn (alloca (256), 123, 456);
.fi
.sp
os ponteiros da pilha são alterados depois que os argumentos
"123" e "456" já foram empilhados, e portanto a função "ftn"
será chamada com argumentos inválidos.

.sp
Nunca é demais relembrar que o acesso a itens fora da região
alocada pode causar resultados imprevisíveis, principalmente
tratando-se da pilha de execução.

.in
.sp
.(t
.b
VEJA TAMBÉM
.r
.in 5
.wo "(sys): "
sbrk
.br
.wo "(libc):"
malloc, free
.br
.)t

.in
.sp
.(t
.b EXEMPLO
.in 5
No exemplo abaixo, mostramos como "alloca" pode ser usada para
a alocação de um vetor de tamanho variável, "vetor_var", de "n"
elementos. Repare que o seu uso e desalocação são similares ao do
vetor de tamanho fixo, "vetor_fixo".
.sp
.nf
	int
	func (int n /* Número de elementos do vetor */)
	{
		int	vetor_fixo[10];
		int	*vetor_var;

		vetor_var = alloca (n * sizeof (int));

		......

		vetor_fixo[3] = 5;
		vetor_var[3] = 5;

		......

		return (...);	/* Desaloca os dois vetores */ 

	}	/* end func */
.fi
.)t
.sp

.in
.sp
.b ESTADO
.in 5
Efetivo.

.in
