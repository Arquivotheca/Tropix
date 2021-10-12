.bp
.he 'DU (cmd)'TROPIX: Manual de Referência'DU (cmd)'
.fo 'Atualizado em 27.10.04'Versão 4.6.0'Pag. %'

.b NOME
.in 5
.wo "du -"
informa sobre o uso de disco
.br

.in
.sp
.b SINTAXE
.in 5
.(l
du [-dam.sN] [-b <sz>] [-p "<p>"] [-P "<P>"] [<árvore> ...]
.)l

.bc	/*************************************/
Opções:	-d: Informa também para cada subárvore
	-a: Informa também para cada arquivo
	-m: Escreve os valores em MB (ao invés de KB)
	-.: Lista também arquivos ocultos (".old" ...)
	-s: Adiciona os padrões dos fontes em "C"
	-N: Lê os nomes das <árvore>s de "stdin"
	-b: Use <sz> como o tamanho do bloco
	-p: Seleciona arquivos através dos padrões <p> de inclusão
	-P: Seleciona arquivos através dos padrões <P> de exclusão
.ec	/*************************************/

.in
.sp
.b DESCRIÇÃO
.in 5
O comando "du" informa sobre o espaço ocupado pelos arquivos e diretórios
contidos em cada <árvore> dada, no sistema de arquivos (em disco).

.sp
O espaço é dado normalmente em KB (veja a opção "-m" abaixo),
e NÃO considera os arquivos "ocultos" (arquivos cujos "caminhos" contém
algum componente que começa com um ponto - veja a opção "-.").

.sp
Normalmente, <árvore> é o nome de um diretório (a raiz) cujos arquivos
e subdiretórios (recursivamente) definem a árvore.
Se <árvore> for o nome de um arquivo (e não de um diretório),
a árvore consiste em apenas este arquivo.
Se não for dada nenhuma <árvore>, será considerada a árvore cuja raiz é
o diretório corrente.

.sp
Os tamanhos dos arquivos são arredondados para o tamanho do bloco
do sistema de arquivos nos quais estão contidos, para dar um valor
representativo do espaço efetivamente ocupado no meio (veja a opção "-b" abaixo).

.sp
Sem opções, o comando imprime uma linha para cada <árvore> dada.
.bc	/*************************************/
e para cada um de seus subdiretórios.
.ec	/*************************************/

.sp
As opções do comando são:

.in +3
.ip -d
Imprime uma linha também para cada subárvore.

.ip -a
Imprime uma linha também para cada arquivo.

.ip -m
Imprime os resultados em MB (ao invés de KB).

.ip "-."
Seleciona também nomes de arquivos ocultos.
Normalmente, arquivos cujos nomes contém algum componente que
começa com um ponto (exceto "..") não são selecionados
(como por exemplo "/usr/src/sys/.old/rot.c").
Esta opção faz com que estes arquivos sejam também considerados.


.ip "-s"
Esta opção é a abreviatura de
.sp
.nf
			-p "*[Mm]akefile *.[cshryv]"
.fi
.sp
e é útil para selecionar arquivos relacionados com a linguagem "C".
Padrões adicionais podem ser dados com a opção "-p" (veja abaixo).

.ip -N
Lê os nomes das árvores de "stdin".

.ip "-b"
Use <sz> como o tamanho do bloco para o arredondamento dos arquivos.
Podem ser usados os sufixos "c" ou "C" para especificar multiplicação
por 2048, "k" ou "K" para especificar multiplicação por 1024,
"b" ou "B" para especificar multiplicação por 512.

.ip "-p"
Padrões de inclusão:
Seleciona apenas arquivos cujos nomes são aceitos por um dos <padrões> dados.
Cada padrão é uma expressão regular, como as do "sh" (cmd),
formada com "*", "?", "!", "[...]" (ver "patmatch" (libc)).
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
.b OBSERVAÇÕES
.in 5
Um arquivo com mais de um elo físico só é considerado uma única vez.

.sp
No caso de elos simbólicos, são computados os espaços ocupados
pelos próprios elos, e não pelos arquivos referenciados.

.sp
Não são computados os blocos indiretos dos arquivos.

.sp
Arquivos com "buracos" (ver "lseek" (sys)) são computados incorretamente.

.in
.sp
.(t
.b
VEJA TAMBÉM
.r
.in 5
.wo "(cmd): "
df
.br
.wo "(sys): "
lseek
.br
.wo "(libc):"
patmatch
.br
.)t

.in
.sp
.b ESTADO
.in 5
Efetivo.
.in
