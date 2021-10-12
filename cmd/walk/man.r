.bp
.he 'WALK (cmd)'TROPIX: Manual de Referência'WALK (cmd)'
.fo 'Atualizado em 25.04.02'Versão 4.2.0'Pag. %'

.b NOME
.in 5
.wo "walk -"
caminha em árvores do sistema de arquivos
.br

.in
.sp
.b SINTAXE
.in 5
.(l
walk [rdbclfamL.N] [<árvore> ...]

Valores de <PAD>: [-s] [-p "<p>"] [-P "<P>"]
.)l

.in
.sp
.b DESCRIÇÃO
.in 5
O programa "walk" caminha em cada <árvore> dada do sistema de arquivos
e escreve os nomes completos dos arquivos selecionados (ver as opções)
na saída padrão, um por linha.

.sp
As opções do comando são:

.in +3
.ip "--"
Sem nenhuma das opções "-rdbclfa" (veja abaixo) seleciona todos os tipos 
de arquivos exceto diretórios.

.ip "-r"
Seleciona arquivos regulares.

.ip "-d"
Seleciona diretórios.

.ip "-b"
Seleciona arquivos especiais de bloco (estruturados).

.ip "-c"
Seleciona arquivos especiais de caracteres (não estruturados).

.ip "-l"
Seleciona elos simbólicos.

.ip "-f"
Seleciona FIFOs.

.ip "-a"
Seleciona todos os tipos de arquivos (equivalente a "-rdbcf").

.ip "-m"
Imprime um prefixo contendo o tipo do arquivo e o seu tamanho.

.ip "-L"
Não seleciona nomes que correspondem a arquivos cujos conteúdos
já foram selecionados anteriormente. 
Isto pode ocorrer quando um arquivo possui vários elos físicos.

.ip "-."
Seleciona também nomes de arquivos ocultos.
Normalmente, arquivos cujos nomes contém algum componente que
começa com um ponto (exceto "..") não são selecionados
(como por exemplo "/usr/src/sys/.old/rot.c").
Esta opção faz com que estes arquivos sejam também considerados.

.ip "-N"
Lê os nomes das <árvore>s da entrada padrão.
Esta opção é implícita se não forem dadas <árvore>s.

.ip "-s"
Esta opção é a abreviatura de
.sp
.nf
			-p "*[Mm]akefile *.[cshryv]"
.fi
.sp
e é útil para selecionar arquivos relacionados com a linguagem "C".
Padrões adicionais podem ser dados com a opção "-p" (veja abaixo).

.bc	/*************************************/
.ip -p
Padrões de inclusão:
tanto na entrada como na saída, podemos dar <padrões>
(isto é, expressões regulares, como os
do "sh", formados com "*", "?", "!", "[...]") 
para a seleção dos nomes dos arquivos desejados.
Repare que os <padrões> são aplicados ao nome completo do arquivo,
de tal modo que podemos utilizar (por exemplo) um padrão da forma "*/etc/*.c".
No momento podemos dar até 20 <padrões>, entre aspas, separados
por brancos ou tabulações.
.ec	/*************************************/

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
VEJA TAMBÉM
.r
.in 5
.wo "(cmd): "
sh
.br
.wo "(libc):"
ftw, patmatch
.br

.(t
.in
.sp
.b EXEMPLO
.in 5
O comando
.sp
.nf
		walk -p "*env.c *man.r" /src/lib/libc
.fi

.sp
procura arquivos cujos nomes terminam em "env.c ou "man.r" na árvore
cuja raiz é "/src/lib/libc" e produz (por exemplo)

.sp
.nf
		/src/lib/libc/etc/getenv/getenv.c
		/src/lib/libc/etc/putenv/putenv.c
		/src/lib/libc/etc/putenv/man.r
.fi
.)t

.in
.sp
.b ESTADO
.in 5
Efetivo.
