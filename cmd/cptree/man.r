.bp 1
.he 'CPTREE (cmd)'TROPIX: Manual de Referência'CPTREE (cmd)'
.fo 'Atualizado em 22.09.04'Versão 4.6.0'Pag. %'

.b NOME
.in 5
.wo "cptree -"
copia uma árvore
.br

.in
.sp
.b SINTAXE
.in 5
.(l
cptree [-ifvmlLe.] <PAD> <fonte> <destino>

Valores de <PAD>: [-s] [-p "<p>"] [-P "<P>"]
.)l

.in
.sp
.b DESCRIÇÃO
.in 5
O comando "cptree" copia a árvore <fonte> para a árvore <destino>.
Isto é realizado caminhando a árvore <fonte> e copiando todos
os arquivos para a árvore <destino> nos locais correspondentes.

.sp
O comando tenta reproduzir as relações dos elos físicos na árvore <destino>
encontradas na árvore <fonte>. No entanto, isto só é inteiramente possível
quando é realizada uma cópia completa.

.sp
Se o arquivo a ser copiado for especial ou FIFO,
é copiado o nó-índice ao invés do conteúdo do arquivo.
Para o caso do elo simbólico, é copiado o seu conteúdo sem modificações, o que
pode criar referências inexistentes.

.sp
As opções do comando são:

.in +3
.ip -i
Modo interativo:
é pedida a confirmação do usuário para cada arquivo a copiar.
Se a confirmação de um diretório for negada, será pulada toda
a sub-árvore definida pelo diretório.

.ip -f
Se algum arquivo correspondente na árvore <destino> já existir,
"cptree" normalmente pede permissão ao usuário para removê-lo.
Com esta opção, o arquivo é removido silenciosamente.

.sp
Um outro método de evitar as consultas é a de responder com letras
maíusculas: com a resposta "S" ou "Y" todas as perguntas seguintes
serão automaticamente respondidas afirmativamente, e (analogamente)
com a resposta "N" serão respondidadas negativamente
(veja "askyesno" (libc)).

.ip -v
Lista os nomes dos arquivos que estão sendo copiados.

.ip -m
Copia os estados dos arquivos:
normalmente, o dono e o grupo dos arquivos da árvore <destino> são conservados
caso êstes já existam, e são usados os do usuário do comando em caso contrário. 
Com a opção "-m", são copiados o dono, grupo, proteção e datas dos arquivos
da árvore <fonte> para o arquivos correspondentes da árvore <destino>
(exceto elos simbólicos).
Isto só é inteiramente possível se o usuário do comando for superusuário. 

.ip -l
Copia apenas os arquivos do dispositivo da árvore <fonte>;
não entra nos subdiretórios montados.

.ip -L
Copia apenas os arquivos do diretório raiz;
não entra nos subdiretórios.

.ip -e
Copia apenas os arquivos não existentes na árvore <destino>.

.ip "-."
Copia também os arquivos ocultos.
Normalmente, arquivos cujos nomes contém algum componente que
começa com um ponto (exceto "..") não são copiados
(como por exemplo "/usr/src/sys/.old/rot.c").
Esta opção faz com que estes arquivos sejam também copiados.

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
Todos os arquivos cujos nomes são
aceitos por estes padrões não são selecionados, mesmo que tenham sido
aceitos pelos padrões das opções "-p" ou "-s".

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
Ao copiar uma árvore utilizando a opção "-m", é possível que
os tempos de modificação dos diretórios não fiquem iguais,
pois a criação de um arquivo implica na escrita em um diretório,
alterando o seu tempo de modificação.

.in
.sp
.b
VEJA TAMBÉM
.r
.in 5
.wo "(cmd):  "
sh, cp
.br
.wo "(libc): "
askyesno
.br

.in
.sp
.b ESTADO
.in 5
Efetivo.
.in
