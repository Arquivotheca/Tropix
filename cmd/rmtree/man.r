.bp
.he 'RMTREE (cmd)'TROPIX: Manual de Referência'RMTREE (cmd)'
.fo 'Atualizado em 26.04.02'Versão 4.2.0'Pag. %'

.b NOME
.in 5
.wo "rmtree -"
remove árvores
.br

.in
.sp
.b SINTAXE
.in 5
.(l
rmtree [-vfs] [-p "<padrões>"] <árvore> ...
.)l

.in
.sp
.b DESCRIÇÃO
.in 5
O comando "rmtree" caminha cada uma das <árvore>s dadas e remove
seletivamente cada arquivo encontrado, de acordo com as opções e/ou
confirmação do usuário.

.sp
As opções do comando são:

.in +3
.ip --
Se não for dada a opcão "-f", "rmtree" funciona interativamente:
é pedida a confirmação do usuário para cada diretório a percorrer e
para cada arquivo/diretório a remover.
Se for negada a confirmação para percorrer certo diretório,
será pulada toda a sub-árvore definida por este diretório.

.ip -v
No caso de uso não interativo, esta opção pode ser usada
para imprimir os nomes do arquivos/diretórios sendo removidos.

.ip -f
Normalmente o comando é interativo. Com esta opção ele deixa de ser interativo,
percorrendo os diretórios e removendo os arquivos/diretórios
sem consultar o usuário.

.sp
Um outro método de evitar as consultas é a de responder com letras
maíusculas: com a resposta "S" ou "Y" todas as perguntas seguintes
serão automaticamente respondidas afirmativamente, e (analogamente)
com a resposta "N" serão respondidadas negativamente
(veja "askyesno" (libc)).

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
.b OBSERVAÇÃO
.in 5
Como é desejável, o comando só remove um diretório se todos os arquivos
nele contidos forem tambem removidos.

.in
.sp
.b
VEJA TAMBÉM
.r
.in 5
.wo "(cmd): "
rm, rmdir
.br
.wo "(libc): "
askyesno
.br

.in
.sp
.(t
.b EXEMPLOS
.in 5
O comando
.ce
rmtree -f /alpha/beta
.sp
irá remover completamente a árvore "/alpha/beta", enquanto que
.sp
.ce
rmtree -f -p "*.o" /alpha/beta
.sp
irá remover todos os módulos objeto nela contidos, e mais os diretórios
que eventualmente ficaram vazios com a remoção dos módulos objeto. 
.)t

.in
.sp
.b ESTADO
.in 5
Efetivo.

.in
