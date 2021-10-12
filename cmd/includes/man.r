.bp
.he 'INCLUDES (cmd)'TROPIX: Manual de Referência'INCLUDES (cmd)'
.fo 'Atualizado em 03.05.97'Versão 1.0.0'Pag. %'

.b NOME
.in 5
.wo "includes -"
descobre arquivos incluídos
.br

.in
.sp
.b SINTAXE
.in 5
.(l
includes [-vuN] [-I <dir>] ... [<caminho> ...]
.)l

.in
.sp
.b DESCRIÇÃO
.in 5
O comando "includes" lista, na saída padrão, todos os arquivos incluídos
através da diretiva "#include" do pré-processador C nos módulos
especificados como argumentos.
.sp
Podem ser especificados arquivos regulares ou diretórios como argumentos
do programa.
No segundo caso, é realizada uma busca em árvore.
.sp
O formato peculiar da listagem produzida destina-se a facilitar o
preenchimento de um "Makefile" que esteja sendo construído para os
módulos em questão.

.sp
As opções do comando são:

.in +3
.ip -I
inclui o diretório <dir> na lista de diretórios de busca.

.ip -v
Modo verboso.

.ip -N
Lê os nomes dos <arquivo>s da entrada padrão. Neste caso, não
devem ser dados argumentos.

.ip -u
Lista também os arquivos incluídos que forem encontrados no
diretório "/usr/include".

.ep
.in -3

.in
.sp
.b OBSERVAÇÕES
.in 5
Apenas arquivos com extensões ".c" e ".h" são varridos à procura de
"includes".

.in
.sp
.b
VEJA TAMBÉM
.r
.in 5
.wo "(cmd): "
trcpp
.br

.in
.sp
.b ESTADO
.in 5
Efetivo.

.in
