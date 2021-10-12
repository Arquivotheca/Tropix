.bp
.he 'LASTTIME (cmd)'TROPIX: Manual de Referência'LASTTIME (cmd)'
.fo 'Atualizado em 04.08.97'Versão 3.0.4'Pag. %'

.b NOME
.in 5
.wo "lasttime -"
lista os nomes de arquivos modificados recentemente
.br

.in
.sp
.b SINTAXE
.in 5
.(l
lasttime [-g.] [-r <n>] <PAD> <árvore> ...

Valores de <PAD>: [-s] [-p "<p>"] [-P "<P>"]
.)l

.bc	/*************************************/
.(l
lasttime [-g.s] [-r <n>] [-n <max>] [-p <padrões>] <árvore> ...
.)l
.ec	/*************************************/

.in
.sp
.b DESCRIÇÃO
.in 5
O comando "lasttime" caminha as diversas <árvore>s, listando
os nomes dos arquivos modificados hoje (isto é, desde a última meia-noite,
veja a opção "-r"), 
juntamente com a data da sua última modificação.

.sp
As opções do comando são:

.in +3
.ip -g
"garfield": cria o arquivo "garfield" (no diretório corrente),
contendo os nomes dos arquivos modificados.
Este arquivo pode ser usado como documentação (uma lista dos arquivos
modificados), como entrada para "gar" (cmd) criar uma coleção com
estes arquivos, ou ...

.ip "-."
compara também os arquivos ocultos:
arquivos "ocultos" são aqueles cujos nomes contém algum componente que
começa com um ponto (exceto ".."), como por exemplo "/usr/src/sys/.old/rot.c".
Normalmente, arquivos ocultos são ignorados - esta opção faz com que
estes arquivos sejam também considerados.

.ip -r
Imprime os arquivos modificados nos últimos <n> dias ao invés
de um dia (hoje).

.ip "-s"
Esta opção é a abreviatura de
.sp
.nf
			-p "*[Mm]akefile *.[cshryv]"
.fi
.sp
e é útil para selecionar arquivos relacionados com a linguagem "C".
Padrões adicionais podem ser dados com a opção "-p" (veja abaixo).

.ip "-p"
Padrões de inclusão:
Seleciona apenas arquivos cujos nomes são aceitos por um dos <padrões> dados.
Cada padrão é uma expressão regular, como as do "sh" (cmd),
formada com "*", "?", "!", "[...]" (ver patmatch (libc)).
Os padrões são aplicados ao nome completo do arquivo,
sem considerar o caractere "/" como especial.
Um caractere branco ou <ht> indica o final de um padrão, de tal modo que
uma única opção pode conter vários padrões
(desde que venham entre aspas ou apóstrofes).
Ao total podemos ter até 20 <padrões>.

.ip -P
Padrões de exclusão:
Esta opção é análoga à opção "-p", exceto que os padrões são usados
para a exclusão de arquivos (ao invés de inclusão).
Todos os arquivos cujos nomes são
aceitos por estes padrões não são selecionados, mesmo que tenham sido
aceitos pelos padrões das opções "-p" ou "-s".

.bc	/*************************************/
.ip -n
Normalmente, "lasttime" lista um máximo de 100 arquivos.
Com esta opção, lista até <max> arquivos.
.ec	/*************************************/

.ep
.in -3

.in
.sp
.b
VEJA TAMBÉM
.r
.in 5
.wo "(cmd): "
gar
.br

.in
.sp
.b ESTADO
.in 5
Efetivo.

.in
