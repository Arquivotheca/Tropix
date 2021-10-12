.bp
.he 'C (cmd)'TROPIX: Manual de Referência'C (cmd)'
.fo 'Atualizado em 12.07.02'Versão 4.3.0'Pag. %'

.b NOME
.in 5
.wo "c -"
escreve as linhas de um arquivo em várias colunas
.br

.in
.sp
.b SINTAXE
.in 5
.(l
c [-l <largura>] [-b <brancos>] [<arquivo>]
.)l

.in
.sp
.b DESCRIÇÃO
.in 5
O comando "c" lê as linhas do arquivo de entrada <arquivo>
e as escreve alinhadas em várias colunas na saída padrão. 
Se o <arquivo> não for dado, "c" lê da entrada padrão.

.sp
O comando "c" usa como largura o número de colunas do terminal/vídeo
usado para a sua saída. Se não conseguir este valor, usa
o valor padrão de 80 caracteres.
Normalmente, as colunas são separadas por 2 brancos (veja as opções abaixo).

.sp
O comando "c" tenta utilizar o maior possível número de colunas;
se o <arquivo> de entrada contiver linhas muito compridas,
pode ocorrer que o arquivo de saída
contenha apenas uma coluna
(isto é, seja igual ao arquivo de entrada).

.sp
As opções do comando são:

.in +3
.ip -l
Escreve as linhas de saída com <largura> caracteres.

.ip -b
Separa as colunas da saída com <brancos> brancos ao invés de 2.

.ep
.in -3

.in
.sp
.b
VEJA TAMBÉM
.r
.in 5
.wo "(cmd): "
cat
.br

.in
.sp
.b ESTADO
.in 5
Efetivo.

.in
