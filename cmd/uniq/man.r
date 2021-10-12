.bp
.he 'UNIQ (cmd)'TROPIX: Manual de Referência'UNIQ (cmd)'
.fo 'Atualizado em 13.07.97'Versão 3.0.0'Pag. %'

.b NOME
.in 5
.wo "uniq -"
indica linhas repetidas de um arquivo
.br

.in
.sp
.b SINTAXE
.in 5
.(l
uniq [-nudc] [-f <f>] [-b <b>] [<entrada> [<saída>]]
.)l

.in
.sp
.b DESCRIÇÃO
.in 5
O comando "uniq" lê o arquivo de <entrada> comparando linhas adjacentes.
No modo normal (sem opções), "uniq" escreve o arquivo na <saída>,
mantendo apenas um exemplar de cada linha repetida.

.sp
Note que as linhas repetidas devem estar adjacentes para serem encontradas
(veja "sort" (cmd)).

.sp
Se <saída> não for dado, "uniq" escreve na saída padrão.
Se <entrada> não for dado ou for "-", "uniq" lê da entrada padrão.



.sp
As opções do comando são:

.in +3
.ip -n
Mantém apenas um exemplar de cada linha repetida (modo normal por omissão).

.ip -u
Escreve na <saída> apenas as linhas não repetidas.

.ip -d
Escreve na <saída> apenas um exemplar somente das linhas repetidas.

.ip -c
Escreve cada linha precedida de uma contagem de freqüência.

.ip -f
Na comparação, ignora os <f> campos iniciais, juntamente com os brancos
(<sp> ou <ht>) que os delimitam. Um campo é definido como uma cadeia
de caracteres não-brancos delimitada por brancos.

.ip -b
Na comparação, ignora os <b> caracteres iniciais.
Se for dada também a opção "-f", os caracteres são pulados após os campos.

.ep
.in -3

.in
.sp
.b OBSERVAÇÃO
.in 5
Repare que saída do modo normal é a união das saídas dos modos
das opções "-u" e "-d".

.in
.sp
.b
VEJA TAMBÉM
.r
.in 5
.wo "(cmd): "
sort, comm
.br

.in
.sp
.b ESTADO
.in 5
Efetivo.

.in
