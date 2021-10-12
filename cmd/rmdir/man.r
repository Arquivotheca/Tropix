.bp
.he 'RMDIR (cmd)'TROPIX: Manual de Referência'RMDIR (cmd)'
.fo 'Atualizado em 08.05.02'Versão 4.2.0'Pag. %'

.b NOME
.in 5
.wo "rmdir -"
remove diretórios
.br

.in
.sp
.b SINTAXE
.in 5
.(l
rmdir [-vN] <diretório> ...
.)l

.in
.sp
.b DESCRIÇÃO
.in 5
O comando "rmdir" remove os diretório dados, que devem estar vazios.

.sp
As opções do comando são:

.in +3

.ip -v
Lista os nomes dos <diretório>s sendo removidos.
Isto é útil em conjunto com a opção "-N".

.ip -N
Lê os nomes dos arquivos da entrada padrão.
Esta opção é implícita se não forem dados <arquivo>s.

.ep
.in -3

.sp
Se algum dos diretórios não estiver vazio, utilize o comando "rm" (cmd)
para remover os arquivos nele contidos ou então o comando
"rmtree" (cmd) para remover a árvore inteira. 

.in
.sp
.b
VEJA TAMBÉM
.r
.in 5
.wo "(sys): "
rmdir
.br
.wo "(cmd): "
rm, rmtree, emptydir
.br

.in
.sp
.b ESTADO
.in 5
Efetivo.

.in
