.bp
.he 'NETTIME (cmd)'TROPIX: Manual de Referência'NETTIME (cmd)'
.fo 'Atualizado em 23.08.97'Versão 3.0.1'Pag. %'

.b NOME
.in 5
.wo "nettime -"
obtém a data/hora corrente de um nó remoto
.br

.in
.sp
.b SINTAXE
.in 5
.(l
nettime [-scdv] <nó_remoto>
.)l

.in
.sp
.b DESCRIÇÃO
.in 5
O comando "nettime" obtém a data/hora corrente do <nó_remoto> dado,
e imprime-o no horário local (levando em conta o fuso horário, conforme
os comandos "data" ou "date" (cmd)).

.sp
As opções do comando são:

.in +3
.ip -s
Atualiza o tempo local a partir do tempo remoto obtido
(somente para superusuários).

.ip -c
Compara os tempos local e remoto, indicando a diferença de dias, horas,
minutos e segundos entre os nós.

.ip -d
Imprime também o tempo obtido sem conversão, em decimal/hexadecimal.

.ip -v
Verboso.

.ep
.in -3

.in
.sp
.b
VEJA TAMBÉM
.r
.in 5
.wo "(cmd): "
data, date
.br

.in
.sp
.b
REFERÊNCIAS
.r
.in 5
"Time Protocol", J. Postel, Request for Comments 868.

.in
.sp
.b ESTADO
.in 5
Efetivo.

.in
