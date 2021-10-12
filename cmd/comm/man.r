.bp
.he 'COMM (cmd)'TROPIX: Manual de Referência'COMM (cmd)'
.fo 'Atualizado em 10.04.97'Versão 3.0.0'Pag. %'

.b NOME
.in 5
.wo "comm -"
seleciona linhas comuns a dois arquivos ordenados
.br

.in
.sp
.b SINTAXE
.in 5
.(l
comm [-123] <arquivo1> <arquivo2>
.)l

.in
.sp
.b DESCRIÇÃO
.in 5
O utilitário "comm" compara, linha a linha, os arquivos de entrada
<arquivo1> e <arquivo2>, que devem estar ordenados lexicograficamente
de acordo com o código ISO (veja "sort" (cmd)).
O nome de um dos <arquivo>s pode ser "-" para indicar a entrada padrão.

.sp
O utilitário produz, na saída padrão,
linhas começando em três colunas distintas: 

.sp
.in +3
- coluna 1: para linhas somente no <arquivo1>;
.sp
- coluna 2: para linhas somente no <arquivo2>;
.sp
- coluna 3: para linhas em ambos os arquivos.
.in -3

.sp 2
As opções do comando são:

.in +3
.ip -1
Suprime a coluna 1;

.ip -2
Suprime a coluna 2;

.ip -3
Suprime a coluna 3;

.ep
.in +3

.in
.sp
.b OBSERVAÇÃO
.in 5
Se forem dados as 3 opções simultaneamente  ("comm -123 ...."),
o comando não irá produzir nenhuma saída. 

.in
.sp
.b
VEJA TAMBÉM
.r
.in 5
.wo "(cmd): "
sort, cmp, diff, uniq
.br

.in
.sp
.b ESTADO
.in 5
Efetivo.

.in
