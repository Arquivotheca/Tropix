.bp
.he 'BRK (sys)'TROPIX: Manual de Referência'BRK (sys)'
.fo 'Atualizado em 11.08.95'Versão 3.0'Pag. %'

.b NOME
.in 5
Muda o tamanho do segmento de dados do processo:
.sp
.wo "brk  -"
dado o novo tamanho
.br
.wo "sbrk -"
dado um incremento/decremento
.br

.in
.sp
.b SINTAXE
.in 5
.(l
#include <sys/syscall.h>

int	brk (const void *end_data);
void	*sbrk (int inc);

extern void	*_cbreak;
.)l

.in
.sp
.b DESCRIÇÃO
.in 5
As chamadas ao sistema "brk" e "sbrk" são usadas para mudar dinâmicamente
a quantidade de memória alocada ao segmento de dados do processo. 
Por definição, o valor de "break" é o primeiro endereço
após o final do segmento de dados.

.sp
A chamada "brk" altera a alocação de memória de tal modo que o valor de "break"
seja igual a  "end_data".

.sp
A chamada "sbrk" adiciona "inc" bytes ao segmento de dados.
Como "inc" pode ser negativo, a quantidade de memória alocada
pode ser reduzida.

.sp
A variável "_cbreak" contém sempre o valor atual de "break".

.in
.sp
.b
VALOR DE RETORNO
.r
.in 5
Em caso de sucesso, a chamada ao sistema "brk" devolve zero,
e "sbrk" devolve o valor antigo de "break".
No caso contrário, elas devolvem -1 e indicam em "errno" a causa do erro.

.in
.sp
.b OBSERVAÇÕES
.in 5
Quando o segmento de dados aumenta, a nova região alocada ao
processo é zerada.

.sp
Quando um processo é iniciado, o valor de "break" e "&end" tem o mesmo
valor, o que não ocorre depois da chamada de uma destas chamadas
ao sistema.

.sp
Recomenda-se utilizar a função "malloc" (libc) para alocar
memória. Repare no entanto, que programas usando "malloc"
ou a "stdio" não devem utilizar "brk" nem "sbrk".

.in
.sp
.b
VEJA TAMBÉM
.r
.in 5
.wo "(sys): "
exec
.br
.wo "(libc):"
end
.br

.in
.sp
.b ESTADO
.in 5
Efetivo.
