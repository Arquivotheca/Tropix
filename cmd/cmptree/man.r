.bp
.he 'CMPTREE (cmd)'TROPIX: Manual de Referência'CMPTREE (cmd)'
.fo 'Atualizado em 28.10.03'Versão 4.5.0'Pag. %'

.b NOME
.in 5
.wo "cmptree -"
compara árvores
.br

.in
.sp
.b SINTAXE
.in 5
.(l
cmptree [-ufglrqmL.] <PAD> <fonte> <destino>

Valores de <PAD>: [-s] [-p "<p>"] [-P "<P>"]
.)l

.in
.sp
.b DESCRIÇÃO
.in 5
O comando "cmptree" compara a árvore <fonte> com a árvore <destino>.
Isto é realizado caminhando a árvore <fonte>,
e comparando todos os arquivos da árvore <fonte>
com os arquivos correspondentes da árvore <destino>.

.sp
Se for encontrada alguma diferença entre as árvores, ela é indicada,
e de acordo com a opção "-u" (veja abaixo), a árvore <destino> pode
ser atualizada.

.sp
O comando verifica/atualiza as relações de elos físicos da árvore <destino>
encontradas na árvore <fonte>.

.sp
Se os arquivos a serem comparados forem especiais ou FIFO,
são comparados/copiados os nós-índice ao invés dos conteúdos dos arquivos.
Para o caso dos elos simbólicos, são comparados/copiados os seus conteúdos sem modificações, o que
pode criar referências inexistentes.


.sp
As opções do comando são:

.in +3
.ip -u
Atualiza a árvore destino:
remove/reescreve todos os arquivos necessários da árvore <destino>,
para que ela fique igual à árvore <fonte>. 
Normalmente, é pedida a confirmação do usuário para cada atualização
a realizar na árvore <destino> (veja a opção "-f", abaixo).
.bc	/*************************************/
Se for dada uma letra maiúscula na confirmação ("S", "Y" ou "N"),
tôdas as perguntas seguintes serão respondidas automaticamente
(veja "askyesno" (libc)).
.ec	/*************************************/
Se a confirmação da atualização de um arquivo (exceto diretório)
for negada (e não foi dada a opção "-f"),
o usuário é consultado se deseja
remover o arquivo da árvore <fonte>.
Se a confirmação da atualização de um diretório for negada, será pulada toda
a sub-árvore definida pelo diretório.

.ip -f
"Força": Normalmente, as atualizações/cópias são realizadas
interativamente: o usuário é consultado sobre
cada atualização/cópia a realizar.
Com a opção "-f", o usuário permite com que todas as
atualizações/cópias sejam realizadas sem consultas posteriores.

.sp
Um outro método de evitar as consultas é a de responder com letras
maíusculas: com a resposta "S" ou "Y" todas as perguntas seguintes
serão automaticamente respondidas afirmativamente, e (analogamente)
com a resposta "N" serão respondidadas negativamente
(veja "askyesno" (libc)).

.bc	/*************************************/
Permissão explícita:
com esta opção (juntamente com a opção "-u"), o usuário dá ao
comando "cmptree" a permissão de realizar todas as modificações
necessárias na árvore <destino> para que ela fique igual
à árvore <fonte>. 
.ec	/*************************************/

.ip -g
"garfield":
cria o arquivo "garfield" (no diretório
corrente), contendo os nomes dos arquivos diferentes.
Os nomes são referentes à árvore <fonte>.
Este arquivo pode ser usado como documentação (uma lista dos arquivos
diferentes), como entrada para "gar" (cmd) criar uma coleção com
estes arquivos, ou ...

.ip -l
Ao encontrar um caractere diferente entre os arquivos,
lista as linhas correspondente de cada arquivo.
Isto só é realizado se nenhum dos arquivos for um módulo objeto.

.ip -r
Modo rigoroso:
normalmente, o dono, grupo, modo/proteção e datas dos arquivos
não são comparados.
Com esta opção, estes itens são comparados/atualizados
(esta opção implica também a opção "-m").
Isto só é inteiramente possível se o usuário do comando for superusuário. 

.ip -q
Comparação rápida para módulos objeto:
se os dois arquivos forem módulos objetos, os seus tamanhos totais
e as datas contidas nos seus cabeçalhos forem iguais, os arquivos
são considerados iguais.
Esta comparação é muito mais rápida, e a probabilidade de indicar
uma igualdade falsa é extremamente baixa.

.ip -m
Em caso de atualização, normalmente o dono e o grupo dos arquivos
da árvore <destino> são conservados caso êstes já existam,
e são usados os do usuário do comando em caso contrário. 
Com a opção "-m", são copiados o dono, grupo, modo/proteção e datas dos arquivos
(exceto os elos simbólicos)
da árvore <fonte> para o arquivos correspondentes da árvore <destino>.
Isto só é inteiramente possível se o usuário do comando for superusuário. 

.ip -L
Compara apenas os arquivos do diretório <fonte> raiz;
não entra nos subdiretórios.

.ip -d
Compara apenas os arquivos contidos no dispositivo da árvore <fonte>;
não entra nos subdiretórios contidos em outros dispositivos montados.
.bc	/*************************************/
não entra nos subdiretórios montados em outros dispositivos.
.ec	/*************************************/

.ip "-."
compara também os arquivos ocultos:
arquivos "ocultos" são aqueles cujos nomes contém algum componente que
começa com um ponto (exceto ".."), como por exemplo "/usr/src/sys/.old/rot.c".
Normalmente, arquivos ocultos são ignorados - esta opção faz com que
estes arquivos sejam também considerados na comparação/atualização.
Esta opção é implícita se o nome da árvore <fonte> ou <destino>
tiver um componente começando com um ponto.

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

.ep
.in -3

.in
.sp
.b
VALOR DE RETORNO
.r
.in 5
O comando "cmptree" retorna 0 quando as duas árvores são iguais ou ficaram
iguais após a atualização (opção "-u"). Em caso contrário, retorna o
número de arquivos diferentes. Repare que neste cálculo só são levados
em conta os arquivos selecionados pelos padrões dados. 

.in
.sp
.b OBSERVAÇÕES
.in 5
O único caso em que "cmptree" não consegue atualizar inteiramente a árvore
<destino>, é quando no ponto correspondente a um arquivo normal
da árvore <fonte> está um diretório (possivelmente uma subárvore)
da árvore <destino>.
Isto exigiria a remoção de toda esta subárvore, tarefa que "cmptree"
ainda não efetua.

.sp
Ao atualizar uma árvore utilizando a opção "-r", é possível que
sejam necessários vários passos, pois a criação de um arquivo
implica na escrita em um diretório, alterando o seu tempo de
modificação.

.in
.sp
.b
VEJA TAMBÉM
.r
.in 5
.wo "(cmd): "
sh, cmp, cmpobj, cptree, gar
.br
.wo "(libc):"
askyesno
.br

.in
.sp
.b ESTADO
.in 5
Efetivo.
.in
