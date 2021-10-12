.bp 1
.he 'NLIST (libc)'TROPIX: Manual de Referência'NLIST (libc)'
.fo 'Atualizado em 22.08.01'Versão 4.0.0'Pag. %'

.b NOME
.in 5
nlist - obtém entradas da tabela de símbolos de um mod. objeto

.in
.lp
.b SINTAXE
.in 5
.(l
#include  <nlist.h>  

int	nlist (const char *mod, NLIST *nl);
.)l

.in
.sp
.b DESCRIÇÃO
.in 5
A rotina "nlist" consulta a tabela de símbolos do
módulo objeto "mod" e completa as informações
do vetor de estruturas NLIST apontado por "nl".

.sp
Para cada estrutura do vetor,
"nlist" preenche o tipo e o valor do símbolo dado.
Se o símbolo não for encontrado na tabela,
o tipo e valor são zerados.

.sp
O vetor deve ser terminado por uma estrutura contendo um ponteiro
"n_name" NULO.

.in
.sp
.b
VALOR DE RETORNO
.r
.in 5
Normalmente, a rotina retorna ZERO. Nos seguintes casos, ela
devolve um valor negativo: não conseguiu abrir o módulo objeto "mod";
"mod" não é um módulo objeto; "mod" não possui a
tabela de símbolos. 

.in
.sp
.b OBSERVAÇÃO
.in 5
Para se obter informações sobre certos símbolos do núcleo do TROPIX,
pode ser mais eficiente utilizar a chamada ao sistema "kcntl" ou "getscb"
(sys). 

.in
.sp
.b
VEJA TAMBÉM
.r
.in 5
.wo "(sys): "
knctl, getscb
.br
.wo "(cmd): "
nm
.br
.wo "(fmt): "
a.out.h
.br

.in
.sp
.b ESTADO
.in 5
Efetivo.

.in
