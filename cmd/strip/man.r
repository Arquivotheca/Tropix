.bp
.he 'STRIP (cmd)'TROPIX: Manual de Referência'STRIP (cmd)'
.fo 'Atualizado em 06.06.02'Versão 4.3.0'Pag. %'

.b NOME
.in 5
.wo "strip -"
remove as tabelas de símbolos e depuração de módulos
.br

.in
.sp
.b SINTAXE
.in 5
.(l
strip [-wN] [<módulo> ...] 
.)l

.in
.sp
.b DESCRIÇÃO
.in 5
O comando "strip" remove a tabela de símbolos, a informação de relocação,
a tabela de linhas e a tabela de depuração dos <módulo>s-objeto dados.
O objetivo desta remoção é a redução do espaço usado pelo <módulo> em disco.

.sp
Uma vez removidas estas informações de um módulo executável,
não podemos mais depurá-lo através de "wsdb" (cmd),
nem consultar a sua tabela de símbolos através de "nm" (cmd)
(veja a opção "-w").

.sp
No caso de uma biblioteca compartilhada, são removidas as tabelas de símbolos
e de módulos. Após esta remoção, não é mais possível usar a biblioteca para
"link-edições" (ver "ld" (cmd)), nem para a geração de versões mais novas
da biblioteca  (ver "mkshlib" (cmd)).

.sp
Se não for dado nenhum <módulo> (nem a opção "-N"),
será considerado o módulo "a.out".

.sp
O efeito de "strip" é o mesmo da opção "-s" de "cc" ou "ld" (cmd).

.sp
As opções do comando são:

.in +3
.ip -w
Remove apenas as tabelas de linhas e de depuração.
A tabela de símbolos e as informações de relocação são mantidas,
o que significa que continuaremos podendo consultar a sua tabela de
símbolos através de "nm".

.ip -N
Lê os nomes dos módulos de "stdin".

.ep
.in -3

.in
.sp
.b OBSERVAÇÃO
.in 5
Um programa compilado com a opção "-w" de "cc" (cmd) para conter
as tabelas de depuração, seguido da remoção destas tabelas através
de "strip" não será idêntico ao programa compilado sem a opção "-w".
Isto ocorre, porque o compilador "C" realiza outras modificações além
de gerar a tabela (como por exemplo, não considerar a declaração "register").

.in
.sp
.b
VEJA TAMBÉM
.r
.in 5
.wo "(cmd): "
cc, ld, wsdb, nm, mkshlib
.br
.wo "(fmt): "
a.out
.br

.in
.sp
.b ESTADO
.in 5
Efetivo.

.in
