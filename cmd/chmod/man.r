.bp 1
.he 'CHMOD (cmd)'TROPIX: Manual de Referência'CHMOD (cmd)'
.fo 'Atualizado em 08.05.02'Versão 4.2.0'Pag. %'

.b NOME
.in 5
.wo "chmod -"
modifica a permissão de acesso de arquivos

.in
.sp
.b SINTAXE
.in 5
.(l
chmod [-vN] <modo> [<arquivo> ...]
.)l

.in
.sp
.b DESCRIÇÃO
.in 5
O modo de cada <arquivo> dado é alterado segundo o <modo> especificado,
que pode ser dado na forma absoluta ou simbólica.
A filosofia de proteção de acesso a arquivos pode ser vista em "intro" (sys).

.sp
As opções do comando são:

.in +3
.ip -v
Lista os modos antigo e novo de cada <arquivo>.

.ip -N
Le os nomes dos arquivos da entrada padrão.
Esta opção é implícita se não forem dados <arquivo>s.

.ep
.in -3

.in
.sp
.b 'FORMA ABSOLUTA'
.in 5
A forma absoluta de um modo consiste de um número inteiro,
representando o outório dos bites individuais de permissão
de acesso do arquivo, como descritos no arquivo <stat.h>. 
Em "chmod" (sys) temos uma descrição mais detalhada de
cada bite.

.sp
O número inteiro pode ser dado em decimal, hexadecimal ou octal
conforme as convenções da linguagem "C" (veja "strtol" (libc)).

.in
.sp
.b 'FORMA SIMBÓLICA'
.in 5
A forma simbólico de um modo tem a forma:

.sp
.nf
		[<quem>] <op> [<permissão>]
.fi
.sp
O argumento <quem> é especificado através da combinação das letras
"u" (para as permissões do dono), "g" (do grupo) e "o" (outros).
A letra "a" equivale a todas as letras ("ugo").
Se "quem" for omitido, é assumido "a".

.sp
O argumento <op> pode ser "+" para adicionar permissões ao modo do arquivo,
"-" para retirar permissões e "=" para atribuir permissões absolutamente
(todas as outras permissões serão negadas).

.sp
O argumento <permissão> é dado através da combinação
das letras "r" (permissão de leitura), "w" (de escrita),
"x" (execução/pesquisa), "s" ("set user/group ID"),
"t" (mantenha o texto no swap) e "m" (mantenha o texto na memória).
O argumento <quem> especifica a quem as
permissões se referem na modificação do modo atual. A omissão de
permissões é útil apenas em conjunção com "=" para retirar todas as
permissões.

.sp
Múltiplos modos simbólicos podem ser dados,
desde que estejam separados por vírgulas.
Neste caso, as operações são efetuadas seqüencialmente
(onde uma operação eventualmente pode alterar uma ação anterior).

.sp
A letra "s" significa "set user ID" em conjunto com "u",
e "set group ID" em conjunto com "g".

.sp
Somente o dono do arquivo (ou o superusuário) pode trocar o modo de um arquivo.
Somente o superusuário pode adicionar as permissões "t" e "m".

.in
.sp
.b OBSERVAÇÃO
.in 5
Repare que usando a forma absoluta do modo, se o número NÃO começar
por 0, ele será interpretado como decimal, e NÃO octal.
 
.in
.sp
.b
VEJA TAMBÉM
.r
.in 5
.wo "(cmd): "
ls
.br
.wo "(sys): "
intro, chmod, stat
.br
.wo "(libc):"
strtol
.br

.in
.sp
.b ARQUIVOS
.in 5
/usr/include/stat.h

.in
.sp
.b EXEMPLOS
.in 5
O comando
.sp
.nf
		chmod 0751 arquivo
.fi
.sp
altera o modo do arquivo para: permissão
para leitura, escrita e execução/pesquisa (0400 + 0200 + 0100) para o dono,
leitura e execução/pesquisa (0040 + 0010) para os usuários do mesmo grupo,
e somente permissão de execução para os demais usuários.

.sp 2
O comando
.sp
.nf
		chmod ug=r,u+w,a+x arquivo
.fi
.sp
é equivalente ao anterior.

.sp 2
O comando
.sp
.nf
		chmod =rwx,g+s arquivo
.fi
.sp
permite a todos ler, executar e executar/pesquisar o arquivo;
além disto, liga o bite de "set group ID".

.in
.sp
.b ESTADO
.in 5
Efetivo.
.in
