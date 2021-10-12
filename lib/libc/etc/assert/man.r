.bp
.he 'ASSERT (libc)'TROPIX: Manual de Referência'ASSERT (libc)'
.fo 'Atualizado em 08.06.95'Versão 3.0'Pag. %'

.b NOME
.in 5
.wo "assert -"
verifica o valor de uma expressão e emite diagnóstico
.br

.in
.sp
.b SINTAXE
.in 5
.(l
#include <assert.h>

void	assert (int expr);
.)l

.in
.sp
.b DESCRIÇÃO
.in 5
A macro "assert" insere testes e mensagens de diagnósticos no programa para
certificar-se de que o valor de uma expressão é não-NULO.
Durante a execução, se "expr" for falsa (isto é, seu valor for ZERO),
a macro "assert" imprime uma mensagem de erro da forma

.sp
.(l
	Assertiva falsa: "expr", arquivo "t.c", linha 6
.)l

.sp
onde "expr" é o próprio texto da expressão sendo testada,
"t.c" é o nome do arquivo e "6" o número da linha onde
ocorreu o erro.
Em seguida, "assert" chama a função "abort" (libc).

.sp
Se a macro "NDEBUG" estiver definida no programa antes
da inclusão de <assert.h>, as chamadas de "assert" não serão expandidas.
Isto é útil para retirar os testes e mensagens de um programa
já depurado. 

.in
.sp
.b
VEJA TAMBÉM
.r
.in 5
.wo "(libc):"
abort
.br

.in
.sp
.b ESTADO
.in 5
Efetivo.

.in
