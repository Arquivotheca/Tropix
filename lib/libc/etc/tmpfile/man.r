.bp
.he 'TMPFILE (libc)'TROPIX: Manual de Referência'TMPFILE (libc)'
.fo 'Atualizado em 26.06.95'Versão 3.0'Pag. %'

.b NOME
.in 5
.wo "tmpfile -"
cria um arquivo temporário
.br

.in
.sp
.b SINTAXE
.in 5
.(l
#include <stdio.h>

FILE	*tmpfile (void);
.)l

.in
.sp
.b DESCRIÇÃO
.in 5
A função "tmpfile" cria um arquivo temporário que é automaticamente
removido quando for fechado ou quando o programa terminar.

.sp
O arquivo é aberto para atualizações (isto é, para leituras/escritas)
com o modo "w+" (ver "fopen" (libc)).

.in
.sp
.b
VALOR DE RETORNO
.r
.in 5
Em caso de sucesso, a função devolve um ponteiro para a estrutura "FILE"
correspondente ao arquivo.
No caso contrário, devolve um ponteiro NULO.

.in
.sp
.b OBSERVAÇÃO
.in 5
O arquivo é removido mesmo que o programa termine anormalmente. 

.in
.sp
.b
VEJA TAMBÉM
.r
.in 5
.wo "(libc):"
mktemp, tmpnam, fopen 
.br

.in
.sp
.b ESTADO
.in 5
Efetivo.

.in
