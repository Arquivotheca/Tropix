.bp
.he 'TNET (cmd)'TROPIX: Manual de Referência'TNET (cmd)'
.fo 'Atualizado em 05.05.02'Versão 4.2.0'Pag. %'

.b NOME
.in 5
Comparação/atualização/cópia de/para árvores remotas:
.sp
.wo "tcmpto   -"
comparação/atualização caminhando árvores locais
.br
.wo "tcmpfrom -"
comparação/atualização caminhando árvores remotas
.br
.wo "tcpto    -"
cópia caminhando árvores locais
.br
.wo "tcpfrom  -"
cópia caminhando árvores remotas
.br
.wo "tpipe    -"
escreve arquivos remotos na saída padrão
.br

.in
.sp
.b SINTAXE
.in 5
.(l
tcmpto   [-tfdgLFvm.N] <ENV> <PAD> [<árvore> ...]
tcmpfrom [-tfdgLFvm.N] <ENV> <PAD> [<árvore> ...]
tcpto    [-LFvm.N] <ENV> <PAD> [<árvore> ...]
tcpfrom  [-LFvm.N] <ENV> <PAD> [<árvore> ...]
tpipe    [-N] <ENV> [<árvore> ...]

Valores de <ENV>: [-C] [-n <nó>] [-u <usuário>] [-D <rdir>]

Valores de <PAD>: [-s] [-p "<p>"] [-P "<P>"]
.)l

.in
.sp
.b DESCRIÇÃO
.in 5
A família TNET de comandos destina-se à comparação/atualização ou
cópia entre/de árvores locais e árvores correspondentes de um <nó>
remoto de uma rede. A família compõe-se
dos utilitários "tcmpto", "tcmpfrom", "tcpto", "tcpfrom" e "tpipe".

.sp
O utilitário "tcmpto" caminha as <árvore>s locais dadas, e compara
cada arquivo local com o arquivo correspondente do <nó> remoto.
Opcionalmente pode atualizar os arquivos da árvore desejada
(local ou remota).

.sp
O utilitário "tcmpfrom" caminha as <árvore>s remotas dadas, e compara
cada arquivo do <nó> remoto com o arquivo local correspondente.
Opcionalmente pode atualizar os arquivos da árvore desejada
(local ou remota).

.sp
Repare que a diferença entre os utilitários "tcmpto" e "tcmpfrom" é
apenas o local das <árvore>s que são caminhadas; quais das <árvore>s
serão (opcionalmente) atualizadas é decidido por uma das opções
"-t", "-f" ou "-d" (elas são mutuamente exclusivas, veja abaixo);
sem alguma destas opções NÃO são realizadas atualizações.

.sp
O utilitário "tcpto" caminha as <árvore>s locais dadas, copiando
cada arquivo local para o <nó> remoto (deste modo reproduzindo as
<árvore>s no <nó> remoto).

.sp
O utilitário "tcpfrom" caminha as <árvore>s do <nó> remoto dadas,
copiando cada arquivo destas <árvore>s remotas para o local correspondente
das <árvore>s locais (deste modo reproduzindo as
<árvore>s no <nó> local).

.sp
O utilitário "tpipe" caminha as <árvore>s remotas dadas, escrevendo
todos os arquivos do <nó> remoto na saída padrão.

.sp
.(t
As opções do comando são:

.in +3
.ip -t
"To": Atualiza as árvores do <nó> remoto:
remove/reescreve todos os arquivos necessários das árvores do <nó> remoto,
para que elas fiquem iguais às árvores locais. 
Normalmente, é pedida a confirmação do usuário para cada atualização
a realizar (veja a opção "-F", abaixo).
.)t

.ip -f
"From": Atualiza as árvores locais:
remove/reescreve todos os arquivos necessários das árvores locais,
para que elas fiquem iguais às árvores do <nó> remoto. 
Normalmente, é pedida a confirmação do usuário para cada atualização
a realizar (veja a opção "-F", abaixo).

.ip -d
Data: Atualiza os arquivos diferentes das árvores locais ou do
no <nó> remoto, dependendo das datas de modificação dos arquivos
(atualizando os arquivos mais antigos).
Normalmente, é pedida a confirmação do usuário para cada atualização
a realizar (veja a opção "-F", abaixo).

.ip -g
"Garfield": Cria o arquivo "garfield" (no diretório corrente),
contendo os nomes dos arquivos diferentes.
Os nomes são referentes às árvores locais.
Este arquivo pode ser usado como documentação (uma lista dos arquivos
diferentes), como entrada para "tcpto/tcpfrom" atualizar a árvore
posteriormente, como entrada para "gar" (cmd) criar uma coleção com
estes arquivos, ou ainda ...

.ip -L
Compara apenas os arquivos do diretório raiz;
não entra nos subdiretórios.

.ip -F
"Força": Normalmente, as atualizações/cópias são realizadas
interativamente: o usuário é consultado sobre
cada operação de atualização/cópia a realizar.
Com a opção "-F", o usuário permite com que todas as
atualizações/cópias sejam realizadas sem consultas posteriores.

.sp
Um outro método de evitar as consultas é a de responder com letras
maíusculas: com a resposta "S" ou "Y" todas as perguntas seguintes
serão automaticamente respondidas afirmativamente, e (analogamente)
com a resposta "N" serão respondidadas negativamente
(veja "askyesno" (libc)).

.bc	/*************************************/
.ip -i
"interativo": Normalmente, as atualizações são realizadas sem
consultar o usuário; com a opção "-i", o usuário é consultado sobre
cada operação de atualização a realizar.
Se a atualização de um diretório for negada, será pulada toda
a sub-árvore definida pelo diretório.
.ec	/*************************************/

.ip -v
Verboso: Imprime estatísticas sobre as transferências dos arquivos
além de outras informações.

.ip -m
Em caso de atualizações, normalmente o dono e o grupo dos arquivos
da árvore <destino> são conservados caso êstes já existam,
e são usados os do usuário do comando em caso contrário. 
Com a opção "-m", são copiados o dono, grupo, modo/proteção e datas dos arquivos
da árvore <fonte> para o arquivos correspondentes da árvore <destino>.
Isto só é inteiramente possível se o usuário do comando for superusuário. 

.ip "-."
Compara também os arquivos ocultos:
arquivos "ocultos" são aqueles cujos nomes contém algum componente que
começa com um ponto (exceto ".."), como por exemplo "/usr/src/sys/.old/rot.c".
Normalmente, arquivos ocultos são ignorados - esta opção faz com que
estes arquivos sejam também considerados na comparação/atualização/cópia.

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
.ip -p
Padrões: Podemos dar <padrões> (isto é, expressões regulares, como os
do "sh" (cmd), formados com "*", "?", "!", "[...]") 
para a seleção dos arquivos desejados.
Repare que os <padrões> são aplicados ao nome completo
do arquivo, de tal modo que
podemos utilizar (por exemplo) um padrão da forma "*/etc/*.c".
Se forem dados vários padrões, devem vir separados
pelos caracteres <sp> ou <ht>.

.ip -P
Padrões de exclusão:
Esta opção é análoga à opção "-p", exceto que os padrões são usados
para a exclusão de arquivos. Todos os arquivos cujos nomes são
aceitos por estes padrões não são processados, mesmo que tenham sido
aceitos pelos padrões da opção "-p" ou "-s".

.ip -s
Fontes em "C":
Esta opção é útil para o processamento de programas fontes
na linguagem "C", e é uma abreviatura para "-p '*[Mm]akefile *.[cshryv]'".  
Padrões adicionais podem ser dados com a opção "-p".
.ec	/*************************************/

.ip -C
Compressão: Com esta opção é utilizada a compressão LZW para a transferência
dos conteúdos dos arquivos, aumentando a sua velocidade.
Esta opção pode também ser dada no ambiente (fmt), veja abaixo.

.ip -n
Nó remoto: Define o <nó> remoto com o qual as comparações/atualizações/cópias
serão realizadas.
Esta opção pode também ser dada no ambiente (fmt), veja abaixo.

.ip -u
Usuário do nó remoto: Define o nome do <usuário> no <nó> remoto. Se esta
opção não for dada, o nome do <usuário> remoto é igual ao nome do usuário
local.
Se for necessário, é pedida a senha do <usuário> no <nó> remoto.
Esta opção pode também ser dada no ambiente (fmt), veja abaixo.

.ip -D
.bc	/*************************************/
Nome para compor o diretório remoto:
.ec	/*************************************/
Define <rdir>, um nome
usado para compor o caminho completo das <árvore>s remotas
.bc	/*************************************/
Se dado, define o "deslocamento" entre os diretórios local e do
<nó> remoto
.ec	/*************************************/
(veja detalhes abaixo).
Esta opção pode também ser dada no ambiente (fmt), veja abaixo.

.ip -N
Le os nomes das <árvore>s da entrada padrão, ao invés de obtê-los
como argumentos.
Esta opção é implícita, se não forem dadas <árvore>s.

.ep
.in -3

.in
.sp
.b 'RELAÇÃO ENTRE O CAMINHOS DE UMA ÁRVORE LOCAL E DO <NÓ> REMOTO'
.in 5
A relação entre o caminho de uma árvore local e o caminho da árvore
correspondente do <nó> remoto depende do caminho da <árvore> local e de
<rdir> dado através da opção "-D":

.in +3
.(t
.ip 1. 3
Se o caminho da <árvore> local for absoluto (isto é, começa por '/'),
o caminho da árvore remota será idêntico ao caminho da <árvore> local
(independente de <rdir>).  
.)t

.ip 2. 3
Se o caminho da <árvore> local for relativo (isto é, NÃO começa por '/'),
o caminho da árvore remota depende de <rdir> do seguinte modo:

.in +3
.ip a. 3
Se <rdir> NÃO for dado, o caminho da árvore remota
será formado pela concatenação do diretório "HOME"
(do <usuário> no <nó> remoto)
com o caminho da <árvore> local.

.ip b. 3
Se <rdir> for um caminho absoluto, o caminho da árvore remota
será formado pela concatenação de <rdir> com o caminho da <árvore> local.

.ip c. 3
Se <rdir> for um caminho relativo,
o caminho da árvore remota
será formado pela concatenação do diretório corrente (local)
com <rdir> e com o caminho da <árvore> local.

.ep
.in -6

.sp 2
Na tabela abaixo, <rhome> significa o diretório "HOME" do <usuário>
no <nó> remoto e <lcwd> representa o diretório corrente (local):

.sp
.(t
.nf
	+---------------+---------------+-----------------------+
	| Caminho local	| <rdir>	| Caminho remoto	|
	+---------------+---------------+-----------------------+
	|		|		|			|
	| /gamma	| (não importa)	| /gamma		|
	|		|		|			|
	| gamma		| (não dado)	| <rhome>/gamma		|
	|		|		|			|
	| .		| (não dado)	| <rhome>		|
	|		|		|			|
	| gamma		| /beta		| /beta/gamma		|
	|		|		|			|
	| .		| /beta		| /beta			|
	|		|		|			|
	| gamma		| beta		| <lcwd>/beta/gamma	|
	|		|		|			|
	| .		| beta		| <lcwd>/beta		|
	|		|		|			|
	| gamma		| .		| <lcwd>/gamma		|
	|		|		|			|
	| .		| .		| <lcwd>		|
	|		|		|			|
	+---------------+---------------+-----------------------+
.fi
.)t


.in
.sp
.(t
.b 'OPÇÕES NO AMBIENTE'
.in 5
Para simplificar o uso dos utilitários, algumas das opções podem
ser colocadas no ambiente (fmt).
A idéia é a de colocar certos parametros (que irão permanecer constantes
nos próximos usos dos utilitários, tal como o nome do <nó> remoto) no
ambiente apenas uma vez, ao invés de repetí-los em cada uso do utilitário.

.sp
A cadeia do ambiente deve ter a forma

.sp
.nf
		TNET=<argumento>, ...
.fi
.)t
.sp
.(t
onde os <argumento>s podem ser:

.in +3
.ip reduce 13
Compressão: Equivalente a dar a opção "-C".
.)t

.ip compress 13
Compressão: Equivalente a dar a opção "-C".

.ip node=<nó> 13
Define o nó remoto: Equivalente à opção
.wo '"-n <nó>".'

.ip user=<user> 13
Define o usuário remoto: Equivalente à opção "-u <user>".

.ip rdir=<rdir> 13
Define o nome usado para compor o caminho completo das <árvore>s remotas:
Equivalente à opção "-D <rdir>".

.ep
.in -3

.bc	/*************************************/
Usando a cadeia "TNET" no ambiente, podemos definir globalmente certas
opções e usá-las automaticamente nos comandos s
.ec	/*************************************/

.bc	/*************************************/
Valores de <ENV>: [-C] [-n <nó>] [-u <usuário>] [-D <rdir>]
TNET=node=<node>,user=<user>,rdir=<rdir>,reduce,compreess,<n>
.ec	/*************************************/

.in
.sp
.b
VEJA TAMBÉM
.r
.in 5
.wo "(cmd):  "
sh, gar
.br
.wo "(libc): "
askyesno
.br
.wo "(fmt):  "
ambiente
.br

.in
.sp
.b ARQUIVOS
.in 5
garfield

.in
.sp
.b EXEMPLOS
.in 5
Compara os arquivos da árvore cuja raiz é o diretório corrente
com os arquivos correspondentes da árvore cuja raiz é o diretório
"HOME" do nó "abacate":
.sp
.nf
		tcmpto -n abacate .
.fi

.sp 2
Compara os arquivos da árvore cuja raiz é o diretório "HOME"
do nó "abacate" com os arquivos correspondentes da árvore
cuja raiz é o diretório corrente, copiando (interativamente)
para o nó local os arquivos diferentes:
.sp
.nf
		tcmpfrom -fvC -n abacate .
.fi

.sp 2
Copia os arquivos da árvore cuja raiz é "/usr/etc/local" para
a árvore de mesma raiz do nó "abacate" (sem consultas ao usuário):
.sp
.nf
		tcpto -vFC -n abacate /usr/etc/local
.fi
.sp 2
Supondo que o diretório corrente é "alpha",
compara os arquivos da árvore cuja raiz é "gamma"
com os arquivos correspondentes da árvore cuja raiz é o diretório
"alpha/gamma" do nó "abacate":
.sp
.nf
		tcmpto -vC -n abacate -D . gamma
.fi
.sp 2
Supondo que o diretório corrente é "alpha",
compara os arquivos da árvore cuja raiz é "gamma"
com os arquivos correspondentes da árvore cuja raiz é o diretório
"alpha/beta/gamma" do nó "abacate":
.sp
.nf
		tcmpto -vC -n abacate -D beta gamma
.fi
.sp 2
Analisando os dois últimos exemplos, podemos ver como <rdir>
pode ser usado como um deslocamento entre os diretórios
local e do <nó> remoto.

.sp 2
No último exemplo, as opções "-n", "-D" e "-C" podem ser definidas no
ambiente, e deste modo não precisam ser dadas nos comandos subseqüêntes:
.sp
.nf
		TNET=node=abacate,rdir=beta,reduce
		export TNET

		tcmpto -v gamma
.fi

.sp 2
Podemos fazer comparações entre arquivos local e de um <nó> remoto através
de "tpipe":
.sp
.nf
	     tpipe -n abacate -C prog.c | diff -c - prog.c | show
.fi


.in
.sp
.b ESTADO
.in 5
Efetivo.

.in
